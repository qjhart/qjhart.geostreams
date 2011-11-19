package geostreams.vizstreams;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;

/**
  * A canvas where multiple located images can be drawn
  * with respect to a spatial Euclidean reference system.
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: CanvasImage.java,v 1.5 2007/06/02 16:55:07 crueda Exp $
  */
public class CanvasImage extends BufferedImage {
	
	/** my associated graphics. */
	private Graphics2D graphics;

	/** The real reference system. */
	private Rectangle referenceSpace;

	/** (double) size.width / referenceSpace.width */
	private double scaleX;
	
	/** (double) size.height / referenceSpace.height */
	private double scaleY;
	
	/** to fill the final graphics */
	private static final Color bgColor = Color.black;

	/**
	  * Creates a canvas image.
	  *
	  * @param size 
	  *		The full size for this canvas image.
	  * @param referenceSpace
	  *		the reference system.
	  */
	public CanvasImage(Dimension size, Rectangle referenceSpace) {
		super(size.width, size.height,
//			TYPE_INT_RGB
			TYPE_4BYTE_ABGR   //<transparency>
		);
		setReferenceSpace(referenceSpace);

		// get associated graphics:
		graphics = this.createGraphics();
		graphics.setColor(bgColor);
		graphics.fillRect(0, 0, Integer.MAX_VALUE, Integer.MAX_VALUE);
	}

	/** sets the dimension of the reference system. */
	public void setReferenceSpace(Rectangle referenceSpace) {
		this.referenceSpace = (Rectangle) referenceSpace.clone();
		scaleX = (double) this.getWidth() / referenceSpace.width;
		scaleY = (double) this.getHeight() / referenceSpace.height;
	}
	
	/** gets the factor to project an x coordinate */
	public double getScaleX() { return scaleX; }

	/** gets the factor to project an y coordinate */	
	public double getScaleY() { return scaleY; }	
	
	/** Draws an image in this canvas. 
	  * @param x coordinate relative to reference system
	  * @param y coordinate relative to reference system
	  * @param psx Pixel size in x
	  * @param psy Pixel size in y
	  * @param img Image whose size is relative to reference system
	  */
	public synchronized void drawImage(int x, int y, int psx, int psy, Image img) {
		// first: translation:
		x -= referenceSpace.x;
		y -= referenceSpace.y;
		
		int w = psx * img.getWidth(null);
		int h = psy * img.getHeight(null);
		int vx0 = (int) Math.round(scaleX * x);
		int vx1 = (int) Math.round(scaleX * (x + w -1));
		
		int vy0 = (int) Math.round(scaleY * y);
		int vy1 = (int) Math.round(scaleY * (y + h -1));
		
		int vw = vx1 - vx0 + 1;
		int vh = vy1 - vy0 + 1;
		if ( vh < 1 )
			vh = 1;
		
		graphics.drawImage(img, vx0, vy0, vw, vh, null);
	}

	/** Fully redraws this canvas with the given image.
	  * The given image is scaled to the dimension of this
	  * canvas image.
	  */
	public void fillWithImage(Image img) {
		graphics.drawImage(img, 0, 0, getWidth(), getHeight(), null);
	}
	
	public void clear() {
		graphics.clearRect(0, 0, getWidth(), getHeight());
	}
	
}
