package geostreams.vizstreams;

import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionAdapter;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import javax.imageio.ImageIO;
import javax.swing.JPanel;

/**
  * 
  * An image panel to render java.awt.Image's and some other geometries
  * according to a given spatial Euclidean reference system.
  * 
  * Before images are displayed (addImage method), two pieces of
  * information should be provided. First one is the size of the complete
  * displayable canvas image. When zoom factor is 100, this is the size
  * of the complete image that is displayed in this panel. Secondly,
  * this panel needs to know the actual dimension of the reference
  * system in real world. For convenience, the actual dimension of the
  * reference system is initialized to the same size as the canvas
  * image, but normally setReferenceSpace will be called to set this
  * information.
  * 
  * @author Carlos Rueda-Velasquez
  * @version $Id: ImagePanel.java,v 1.9 2007/06/02 16:55:07 crueda Exp $
  */
public class ImagePanel extends JPanel {
	/** Modifier to dispatch ROI definition */
	private final static int roiModifier = MouseEvent.SHIFT_DOWN_MASK;

	//static Stroke stroke = new BasicStroke(3f);	
	/** The size of the canvas. */
	Dimension canvasSize;
	
	/** The canvasImage being displayed. */
	private CanvasImage canvasImage;
	
	/** The real reference system. */
	private Rectangle referenceSpace;

	/** current zoom factor.
	  * zoom/100 = fraction of canvas size.
	  */
	private int zoom = 100;
	
	/** prefSize = (zoom * canvasImage.getWidth|getHeight) / 100 */
	private final Dimension prefSize = new Dimension();
	
	/** to fill the final graphics */
	private final Color bgColor = Color.white;//black;
	
	
	private boolean zoomChanged = true;
	private boolean contentsChanged = true;
	private boolean showFrames = true;
	private boolean showRasterImage = true;

	/** Is ROI definition enabled? */
	private boolean roiDefinitionEnabled = false;
	
	/** Rectangle defining a ROI. Coordinates in reference space. */	
	private Rectangle roiRect = null;

	/** Panel to reference space coordinate conversion */
	private final int _toRefX(int px) { 
		double x = 1 + px;
		double scaleX = canvasImage.getScaleX();
		
		if ( zoom == 100 ) {
			x /= scaleX;
		}
		else {
			x *= 100 / scaleX / zoom;
		}
		return (int) Math.round(x);
	}
	/** Panel to reference space coordinate conversion */
	private final int _toRefY(int py) { 
		double y = 1 + py;
		double scaleY = canvasImage.getScaleY();
		
		if ( zoom == 100 ) {
			y /= scaleY;
		}
		else {
			y *= 100 / scaleY / zoom;
		}
		return (int) Math.round(y);
	}

	
	/** If ROI definition is enabled then draging the mouse with roiModifier
	  * on will define a ROI on this panel.
	  * By default, ROI definition is NOT enabled.
	  */
	public synchronized void setROIDefinitionEnabled(boolean b) {
		roiDefinitionEnabled = b;
	}
	
	/** Returns a reference to the rectangle defining the current ROI.
	  * Coordinates of this rectangle are relative to the reference space.
	  * null if no ROI is currently being defined.
	  */
	public Rectangle getROI() {
		return roiRect;
	}
	
	/** This mouse-listener is used to call my mouseMoved and mouseDragged
	  * methods.
	  */
	private MouseMotionListener mouseMotionListener = new MouseMotionAdapter() {
		public void mouseDragged(MouseEvent ev) { 
			ImagePanel.this.mouseDragged(ev);
		}
			
		public void mouseMoved(MouseEvent ev) {
			ImagePanel.this.mouseMoved(_toRefX(ev.getX()), _toRefY(ev.getY()), ev);
		}
	};
	/** mousePressed defined to initialize the ROI rectangle only if roiModifier is down. */
	private MouseListener mouseListener = new MouseAdapter() {
		public void mousePressed(MouseEvent ev) {
			if ( roiDefinitionEnabled
			&& (ev.getModifiersEx() & roiModifier) == roiModifier ) {
				roiRect = null;
				refresh();
			}
		}
		public void mouseReleased(MouseEvent ev) {
			setCursor(Cursor.getDefaultCursor());
		}
	};

	/** This mouse-listener is used to call my mouseMoved and mouseDragged
	  * methods.
	  */
	private MouseWheelListener mouseWheelListener = new MouseWheelListener() {
		public void mouseWheelMoved(MouseWheelEvent ev) {
			ImagePanel.this.mouseWheelMoved(ev);
		}
	};
	
	private abstract class DrawableElem {
		Color color;
		abstract void drawOnFinalGraphics(Graphics graphics);
	}
	Map<String,DrawableElem> drawables = new HashMap<String,DrawableElem>();

	private class RectElem extends DrawableElem {
		Rectangle rect;
		String tip;

		void drawOnFinalGraphics(Graphics graphics) {
			if ( showFrames ) {	
				int x = rect.x;
				int y = rect.y;
				int w = rect.width;
				int h = rect.height;
				_drawRectOnFinalGraphics(graphics, x, y, w, h, color);
			}
		}
	}
	
	private class LineElem extends DrawableElem {
		int x0, y0;
		int x1, y1;
		
		void drawOnFinalGraphics(Graphics graphics) {
			if ( showFrames ) {	
				_drawLineOnFinalGraphics(graphics, x0, y0, x1, y1, color);
			}
		}
	}
	
	private class VectorElem extends DrawableElem {
		List<List<Point>> vlines;
		
		void drawOnFinalGraphics(Graphics graphics) {
			_drawVLinesOnFinalGraphics(graphics, color, vlines);	
		}
	}
	

	/** Called when the mouse is moved over this panel. 
	  * The [x,y] location is relative to the reference dimension.
	  *  
	  * In this implementation a simple tooltip with the name of
	  * the smallest rectangle containing the location is set.
	  *
	  * @param x mouse x location relative to full dimension
	  * @param y mouse y location relative to full dimension
	  * @param ev The original mouse event
	  */
	protected void mouseMoved(int x, int y, MouseEvent ev) { 
		String ttext = null;   // tooltip text
		int min_area = 0;
		for ( DrawableElem drawable : drawables.values() ) {
			if ( drawable instanceof RectElem ) {
				RectElem re = (RectElem) drawable;
				if ( re.rect.contains(x, y) ) {
					int area = re.rect.width * re.rect.height;
					if ( ttext == null  ||  min_area > area ) {
						ttext = re.tip;
						min_area = area;
					}
				}
			}
		}
		setToolTipText(ttext);
	}

	/** Called when the mouse is dragged over this panel. 
	  * The original mouse event is passed (no translation as in mouseMoved).
	  *  
	  * If ROI definition is enabled and roiModifier is on, a rectangle for
	  * ROI is updated and drawn; the event is then consumed. The rectangle
	  * returned by getROI() is relative to the reference space.
	  *
	  * @param ev The original mouse event
	  */
	protected void mouseDragged(MouseEvent ev) {
		if ( roiDefinitionEnabled
		&& (ev.getModifiersEx() & roiModifier) == roiModifier ) {
			int x = _toRefX(ev.getX());             
			int y = _toRefY(ev.getY());
			if ( roiRect == null ) {
				roiRect = new Rectangle(x, y, 1, 1);
				setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
			}
			else {
				roiRect.width = x - roiRect.x + 1;
				roiRect.height = y - roiRect.y + 1;
			}
			
			ev.consume();
			refresh();
		}
	}
	/** Draws the ROI rectangle, if any.
	  */
	private synchronized void _drawROIRectOnFinalGraphics(Graphics graphics) {
		if ( roiRect != null ) {
			graphics.setColor(Color.YELLOW);
			int x = roiRect.x;
			int w = roiRect.width;
			int y = roiRect.y;
			int h = roiRect.height;
			if ( w < 0 ) {
				x += w;
				w = -w;
			}
			if ( h < 0 ) {
				y += h;
				h = -h;
			}
			// Now get coordinates in the panel:
			
			// first: translation:
			int x0 = x - referenceSpace.x;
			int x1 = x + w - 1 - referenceSpace.x;
			int y0 = y - referenceSpace.y;
			int y1 = y + h - 1 - referenceSpace.y;
			
			// we need our current preferred size:
			double final_scaleX = (double) prefSize.width / referenceSpace.width;
			double final_scaleY = (double) prefSize.height / referenceSpace.height;
			
			int vx0 = (int) Math.round(final_scaleX * x0);
			int vx1 = (int) Math.round(final_scaleX * x1);
			int vy0 = (int) Math.round(final_scaleY * y0);
			int vy1 = (int) Math.round(final_scaleY * y1);
			
			
			graphics.drawRoundRect(vx0-1, vy0-1, vx1-vx0, vy1-vy0, 7, 7);
			graphics.drawString(x+ ", " +y, vx0-8, vy0-8);
			graphics.drawString((x+w-1)+ ", " +(y+h-1), vx1+8, vy1+8);
		}
	}



	
	/** Called when the mouse wheel is rotated. 
	  * The original mouse event is passed (no translation as in mouseMoved).
	  *  
	  * In this class nothing is done.
	  *
	  * @param ev The original mouse event
	  */
	protected void mouseWheelMoved(MouseWheelEvent ev) {
	}

	/** Default size for an ImagePanel */
	static final Dimension DEFAULT_CANVAS_SIZE = new Dimension(512,400);
	
	/**
	  * Creates an image panel with default canvas and reference
	  * space dimensions. The reference space dimension takes the same
	  * canvas size. Normally, setCanvasSize() and setReferenceSpace()
	  * will be called next to set the desired sizes.
	  */
	public ImagePanel() {
		super(new GridLayout(1,1));
		
		this.canvasSize = DEFAULT_CANVAS_SIZE;
		// Assume the reference dimension is the same as the canvas dimension:
		this.referenceSpace = new Rectangle(canvasSize);
		
		canvasImage = new CanvasImage(canvasSize, referenceSpace);
		
		// start attending mouse events:
		addMouseMotionListener(mouseMotionListener);
		addMouseListener(mouseListener);
		addMouseWheelListener(mouseWheelListener);

		_updatePreferredSize();
		
		setToolTipText("MY TOOLTIP");
	}

	/**
	  * Set the size of the canvas for the complete image.
	  *
	  * @param canvasSize The size of the complete image that
	  *        is kept in memory. When zoom factor is 100,
	  *        this is the size of the complete image that
	  *        is displayed in this panel.
	  *        The reference dimension, if null, is also set to the value of
	  *        this argument.
	  */
	public void setCanvasSize(Dimension canvasSize) {
		this.canvasSize = (Dimension) canvasSize.clone();
		if ( referenceSpace == null ) {
			// Asssume the reference dimension is the same as the canvas dimension:
			referenceSpace = new Rectangle(canvasSize);
		}
		
		canvasImage = new CanvasImage(canvasSize, referenceSpace);

		_updatePreferredSize();
	}

	/** Updates the preferred size according to current zoom
	  * and canvas size factors.
	  */
	private void _updatePreferredSize() {
		prefSize.width =  (zoom * canvasSize.width ) / 100; 
		prefSize.height = (zoom * canvasSize.height) / 100;
	}
	
	/** sets the dimension of the reference system. */
	public void setReferenceSpace(Rectangle referenceSpace) {
		this.referenceSpace = (Rectangle) referenceSpace.clone();
		canvasImage.setReferenceSpace(referenceSpace);
	}
	
	/** gets the reference system. */
	public Rectangle getReferenceSpace() {
		return referenceSpace;
	}

	public Dimension getPreferredSize() {
		return prefSize;
	}
	
	/** Draws an image in the canvas. 
	 * 
	 * @param x coordinate relative to reference system
	 * @param y coordinate relative to reference system
	 * @param psx Pixel size in x
	 * @param psy Pixel size in y
	 * @param img Image whose size is relative to reference system
	 */
	public synchronized void drawImage(int x, int y, int psx, int psy, Image img) {
		canvasImage.drawImage(x, y, psx, psy, img);
		contentsChanged = true;
	}
	

	/**
	 * Draws a rectangle.
	 * @param key The key for the rectangle.
	 * @param tip
	 * @param rect
	 * @param color
	 */
	public synchronized void drawRect(String key, String tip, Rectangle rect, Color color) {
		RectElem re = new RectElem();
		drawables.put(key, re);
		re.rect = rect;
		re.color = color;
		re.tip = tip;
		contentsChanged = true;
	}
	
	/**
	 * Draws an object with a given color.
	 * Does nothing if the key does not resolve to a drawable object.
	 * @param key Key of the object.
	 * @param color Desired color.
	 */
	public synchronized void drawWithColor(String key, Color color) {
		DrawableElem drawable = drawables.get(key);
		if ( drawable != null ) {
			drawable.color = color;
			contentsChanged = true;
		}
	}
	public synchronized void drawAllStartingWith(String str, Color color) {
		for ( String key : drawables.keySet() ) {
			if ( key.startsWith(str) ) {
				drawables.get(key).color = color;
				contentsChanged = true;
			}
		}
	}

	/** Draws a named line.
	  */
	public synchronized void drawLine(String name, int x0, int y0, int x1, int y1, Color color) {
		String key = name;
		LineElem le = new LineElem();
		drawables.put(key, le);
		le.x0 = x0;
		le.y0 = y0;
		le.x1 = x1;
		le.y1 = y1;
		le.color = color;
		contentsChanged = true;
	}

	/**
	 * Draw a vector dataset.
	 * @param key Key to associate to the dataset.
	 * @param vlines The dataset.
	 * @param color Desored color.
	 */
	public synchronized void drawVector(String key, List<List<Point>> vlines, Color color) {
		VectorElem ve = new VectorElem();
		drawables.put(key, ve);
		ve.vlines = vlines;
		ve.color = color;
		contentsChanged = true;
	}

	/**
	 * @param string
	 * @return
	 */
	public boolean isElementVisible(String key) {
		return drawables.get(key) != null;
	}

	/**
	 * Removes a drawn element
	 * @param key
	 */
	public synchronized void removeElement(String key) {
		drawables.remove(key);
	}
	
	/** Removes all objects whose key starts with the given string.
	  */
	public synchronized void removeAllStartingWith(String str) {
		List<String> keys = new ArrayList<String>();
		for ( String key : drawables.keySet() ) {
			if ( key.startsWith(str) ) {
				keys.add(key);
			}
		}
		for ( String key : keys ) {
			drawables.remove(key);
		}
	}
	
	/** Call this to refresh the contents of this panel. */
	public void refresh() {
		if ( zoomChanged ) {
			revalidate(); // get scrollbars updated
		}
		repaint();
	}
	
	
	public synchronized void update(Graphics g) {
		paint(g);
	}

	/** Draws a rectangle on the final graphics with coordinates 
	  * relative to the actual reference system.
	  */
	private void _drawRectOnFinalGraphics(Graphics graphics, int x, int y, int w, int h, Color color) {
		//((Graphics2D) graphics).setStroke(stroke);
		// first: translation:
		x -= referenceSpace.x;
		y -= referenceSpace.y;
		
		// we need our current preferred size:
		double final_scaleX = (double) prefSize.width / referenceSpace.width;
		double final_scaleY = (double) prefSize.height / referenceSpace.height;
		
		int vx0 = (int) Math.round(final_scaleX * x);
		int vx1 = (int) Math.round(final_scaleX * (x + w -1));
		
		int vy0 = (int) Math.round(final_scaleY * y);
		int vy1 = (int) Math.round(final_scaleY * (y + h -1));
		
		int vw = vx1 - vx0 + 1;
		int vh = vy1 - vy0 + 1;
		
		graphics.setColor(color);
		graphics.drawRect(vx0, vy0, vw, vh);
		contentsChanged = true;
	}

	/** Draws a line on the final graphics with coordinates 
	  * relative to the actual reference system.
	  */
	private synchronized void _drawLineOnFinalGraphics(Graphics graphics, int x0, int y0, int x1, int y1, Color color) {
		// first: translation:
		x0 -= referenceSpace.x;
		y0 -= referenceSpace.y;
		x1 -= referenceSpace.x;
		y1 -= referenceSpace.y;
		
		// we need our current preferred size:
		double final_scaleX = (double) prefSize.width / referenceSpace.width;
		double final_scaleY = (double) prefSize.height / referenceSpace.height;
		
		int vx0 = (int) Math.round(final_scaleX * x0);
		int vy0 = (int) Math.round(final_scaleY * y0);
		int vx1 = (int) Math.round(final_scaleX * x1);
		int vy1 = (int) Math.round(final_scaleY * y1);
		
		graphics.setColor(color);
		graphics.drawLine(vx0, vy0, vx1, vy1);
		contentsChanged = true;
	}

	/** Draws a point on the final graphics with coordinates 
	  * relative to the actual reference system.
	  */
	@SuppressWarnings("unused")
	private void _drawPointOnFinalGraphics(Graphics graphics, int x0, int y0, Color color) {
		// first: translation:
		x0 -= referenceSpace.x;
		y0 -= referenceSpace.y;
		
		// we need our current preferred size:
		double final_scaleX = (double) prefSize.width / referenceSpace.width;
		double final_scaleY = (double) prefSize.height / referenceSpace.height;
		
		int vx0 = (int) Math.round(final_scaleX * x0);
		int vy0 = (int) Math.round(final_scaleY * y0);
		
		graphics.setColor(color);
		graphics.fillOval(vx0, vy0, 1, 1);
		contentsChanged = true;
	}

	
	public void setShowFrames(boolean showFrames) {
		contentsChanged = this.showFrames != showFrames;
		this.showFrames = showFrames;
	}
	
	public void setShowRasterImage(boolean showRasterImage) {
		contentsChanged = this.showRasterImage != showRasterImage;
		this.showRasterImage = showRasterImage;
	}
	
	private void _drawVLinesOnFinalGraphics(Graphics graphics, Color color, List<List<Point>> vlines) {
		for ( List<Point> vline : vlines ) {
			Iterator<Point> it2 = vline.iterator();
			if ( !it2.hasNext() )
				continue;
			Point p = it2.next();
			while ( it2.hasNext() ) {
				Point q = it2.next();
			
				int x0 = p.x;
				int y0 = p.y;
				int x1 = q.x;
				int y1 = q.y;
				_drawLineOnFinalGraphics(graphics, x0, y0, x1, y1, color);
				//_drawPointOnFinalGraphics(graphics, x0, y0, color);
				//_drawPointOnFinalGraphics(graphics, x1, y1, color);
				p = q;
			}
		}
	}
	
	public synchronized void paint(Graphics g) {
		if ( false && canvasImage == null ) {
			g.setColor(Color.gray);
			g.fillRect(0, 0, Integer.MAX_VALUE, Integer.MAX_VALUE);
			return;
		}
		int width = (int)  (canvasImage.getWidth()*zoom)/100; 
		int height = (int) (canvasImage.getHeight()*zoom)/100;
		
		// fill with bgColor if necessary:
		if ( contentsChanged || zoomChanged ) {
			g.setColor(bgColor);
			g.fillRect(0, 0, Integer.MAX_VALUE, Integer.MAX_VALUE);
			g.fillRect(0, height-1, Integer.MAX_VALUE, Integer.MAX_VALUE);
		}

		// draw the current contents of the canvas image:
		if ( showRasterImage ) {
			g.drawImage(canvasImage, 0, 0, width, height, null);
		}
		else {
			g.setColor(Color.BLACK);
			g.fillRect(0, 0, width, height);
		}

		
		// draw all drawables on final graphics:
		for ( DrawableElem drawable : drawables.values() ) {
			drawable.drawOnFinalGraphics(g);
		}
		
		_drawROIRectOnFinalGraphics(g);
		
		contentsChanged = zoomChanged = false;
	}
	
	/** Saves the current selected canvas image.
	  * The format for the file will be determined by the extension
	  * of the file name. 
	  */
	public void save(String filename) throws IOException {
		if ( canvasImage == null ) {
			System.out.println("ImagePanel: save: No canvasImage is currently set");
			return;
		}
		String format = filename.substring(1 + filename.lastIndexOf("."));
		File f = new File(filename);
		ImageIO.write(canvasImage, format, f);
	}
	
	/** Fills the canvas image with a given image.
	  * The given image is scaled to the dimension of the
	  * canvas image.
	  */
	public void loadBackgroundImage(URL url) throws IOException {
		BufferedImage img = ImageIO.read(url);
		// TODO: this is temporary:
//		System.out.println("ImagePanel: Getting subimage...");
		int x = (int) Math.round(canvasImage.getScaleX() * referenceSpace.x); 
		int y = (int) Math.round(canvasImage.getScaleY() * referenceSpace.y);
		img = img.getSubimage(x, y, img.getWidth() - x, img.getHeight() - y);
		
		canvasImage.fillWithImage(img);
		contentsChanged = true;
	}

	/** Gets the current zoom factor */
	public int getZoom() {
		return zoom;
	}
	
	/** Sets the current zoom factor and updates the display.
	  * The minimum and maximum final zoom factor are arbitrarily
	  * set to 10% and 200%.
	  */
	public void setZoom(int zoom) {
		if ( zoom < 10 )
			zoom = 10; 
		else if ( zoom > 200 )
			zoom = 200;
		this.zoom = zoom;
		_updatePreferredSize();
		zoomChanged = true;
	}
	
	public boolean getShowFrames() {
		return showFrames;
	}
	public boolean getShowRasterImage() {
		return showRasterImage;
	}
	public void clear() {
		canvasImage.clear();
		contentsChanged = true;
	}
	public CanvasImage getCanvasImage() {
		return canvasImage;
	}
}

