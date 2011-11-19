package geostreams.vizstreams;

import geostreams.image.ChannelDef;
import geostreams.image.FrameDef;
import geostreams.image.FrameStatus;
import geostreams.image.IImage;
import geostreams.image.IStream;
import geostreams.image.StreamDef;
import geostreams.image.StreamStatus;
import geostreams.image.VectorInfo;
import geostreams.util.Utils;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.Date;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

/**
 * TODO : 0 istream
 * 
 * Will replace ImageStreamViewer
 * 
 * Panel with some GUI controls for visualization of an image stream
 * using an ImagePanel.
 * Includes functionality related with rectangle drawing.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamViewer.java,v 1.6 2007/06/13 00:29:26 crueda Exp $
 */
public class StreamViewer extends JPanel {
	private IStream istream;

	protected JPanel _buttonsPanel = new JPanel(new GridLayout(0, 1));

	private JPanel _statusPanel = new JPanel(new GridLayout(1, 0));

	private final JLabel channelNameLabel = new JLabel("", JLabel.LEFT);
	
	private final JLabel connectionStatusLabel = new JLabel("| Connection status: ", JLabel.RIGHT);
	private final JLabel connectionStatus = new JLabel("", JLabel.LEFT);

	private final JLabel status = new JLabel("Received: " + 0 + " row(s) in "
			+ 0 + " frame(s)", JLabel.LEFT);

	private final JLabel mouseLabel = new JLabel("mouse location", JLabel.RIGHT);

	private StreamViewerControlPanel viewControlsPanel;

	boolean eventFromSlider;

	protected ImagePanel imgPanel;

	private JScrollPane scrollPane;

	/** Map originalName -> VectorInfo 
	 * package private so ViewControlsPanel can access it 
	 */
	final Map<String,VectorInfo> vectors = new LinkedHashMap<String,VectorInfo>();
	
	
	// accessible by VizStreams
	// updated while the viewer is running:	
	/** has the background image already attempted to be painted? */ 
	boolean saved_already_attempted;
	
	/** resets the runtime attributes associated to the stream */
	void resetInfo() {
		saved_already_attempted = false;
	}

	Rectangle roi;
	String getShortName() {
		int index = channelDef.getName().indexOf(",");
		if ( index > 0 ) {
			return channelDef.getName().substring(0, index);
		}
		else {
			return channelDef.getName();
		}
	}

	
	
	
	/** If true, the whole image is saved when a frame is finished */
	boolean autoSaveFrames;

	/** Visualized stream */
	StreamDef streamDef;
	
	/** Associated channel */
	ChannelDef channelDef;

	/** is this viewer used as an overview? */
	private boolean isOverview = false;

	private String streamID;

	private StreamStatus streamStatus;

	/**
	 * Creates an image stream viewer with default attributes.
	 */
	public StreamViewer() {
		super(new BorderLayout());

		imgPanel = new MyImagePanel();
		imgPanel.setROIDefinitionEnabled(true);

		scrollPane = new JScrollPane(imgPanel
//				,
//				JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
//				JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS
		);
		scrollPane.getVerticalScrollBar().setUnitIncrement(10);
		scrollPane.getHorizontalScrollBar().setUnitIncrement(10);
		scrollPane.setWheelScrollingEnabled(false);

		this.add(scrollPane);

		viewControlsPanel = new StreamViewerControlPanel();
		viewControlsPanel.setStreamViewer(this);


		this.add(_buttonsPanel, "North");
		this.add(_statusPanel, "South");
		this.add(viewControlsPanel, java.awt.BorderLayout.WEST);
		
		_addButtons();
		_addStatusFields();

		// initial zoom factor: (requires scrollPane and viewControlsPanel to already exist)
		imgPanel.setZoom(40);
	}

	/**
	 * 
	 */
	public void setROI(Rectangle roi) {
		this.roi = roi;
	}

	public void setZoom(int zoom) {
		imgPanel.setZoom(zoom);
	}

	/**
	 * Returns a reference to the rectangle defining the current ROI.
	 * Coordinates of this rectangle are relative to the reference space. null
	 * if no ROI is currently being defined.
	 */
	public Rectangle getDefROI() {
		return imgPanel.getROI();
	}

	class MyImagePanel extends ImagePanel {
		// info for mouseDragged:
		int d_prev_x = -1;

		int d_prev_y = -1;

		final int DRAG_STATE_NOTHING = 0;

		final int DRAG_STATE_ZOOMING = 1;

		final int DRAG_STATE_DRAGGING = 2;

		int drag_state = DRAG_STATE_NOTHING;

		final Rectangle vprect = new Rectangle(0, 0, 1, 1);

		protected void mouseMoved(int x, int y, MouseEvent ev) {
			mouseLabel.setText("[" + x + ", " + y + "]");
			super.mouseMoved(x, y, ev);
			d_prev_x = d_prev_y = -1;
			drag_state = DRAG_STATE_NOTHING;
			setCursor(Cursor.getDefaultCursor());
		}

		protected void mouseDragged(MouseEvent ev) {
			super.mouseDragged(ev);
			if (ev.isConsumed()) {
				return;
			}
			int x = ev.getX();
			int y = ev.getY();
			int cursor = Cursor.DEFAULT_CURSOR;
			int next_state = DRAG_STATE_NOTHING;

			int zoommask = MouseEvent.SHIFT_DOWN_MASK;
			if ((ev.getModifiersEx() & zoommask) == zoommask) {
				next_state = DRAG_STATE_ZOOMING;
				cursor = Cursor.HAND_CURSOR;
			} else {
				next_state = DRAG_STATE_DRAGGING;
				cursor = Cursor.MOVE_CURSOR;
			}

			if (drag_state != next_state) {
				drag_state = next_state;
				d_prev_x = x;
				d_prev_y = y;
				setCursor(Cursor.getPredefinedCursor(cursor));
				return;
			}

			int delta_y = d_prev_y - y;

			if (drag_state == DRAG_STATE_ZOOMING) {
				// nothing: we are now using a JSlider
				/*
				 * int new_zoom = (int) Math.round((double) zoom -
				 * delta_y/20.0); if ( new_zoom != zoom ) { setZoom(new_zoom);
				 * imgPanel.refresh(); doLayout(); }
				 */
			} else if (drag_state == DRAG_STATE_DRAGGING) {
				int delta_x = d_prev_x - x;
				Rectangle vr = scrollPane.getViewport().getViewRect();

				// scroll by delta:
				vprect.x = vr.x;
				vprect.y = vr.y;
				vprect.width = vr.width;
				vprect.height = vr.height;
				vprect.translate(delta_x, delta_y);
				scrollRectToVisible(vprect);
			} else {
				assert false;
			}
		}

		protected void mouseWheelMoved(MouseWheelEvent ev) {
			super.mouseWheelMoved(ev);

			int zoommask = MouseEvent.SHIFT_DOWN_MASK;
			if ((ev.getModifiersEx() & zoommask) == zoommask) {
				int zoom = imgPanel.getZoom();
				int new_zoom = zoom + 5 * ev.getWheelRotation();
				if (new_zoom != zoom) {
					setZoom(new_zoom);
				}
			} else {
				Rectangle vr = scrollPane.getViewport().getViewRect();
				vprect.x = vr.x;
				vprect.y = vr.y;
				vprect.width = vr.width;
				vprect.height = vr.height;
				int d = 20 * ev.getWheelRotation();
				vprect.translate(0, d);
				scrollRectToVisible(vprect);
				imgPanel.refresh();
				doLayout();
			}
		}

		public void setZoom(int zoom) {
			int cur_zoom = getZoom();
			Rectangle vr = scrollPane.getViewport().getViewRect();
			vprect.x = vr.x;
			vprect.y = vr.y;
			vprect.width = vr.width;
			vprect.height = vr.height;
			super.setZoom(zoom);
			int new_zoom = getZoom();
			int d = 10 * (new_zoom - cur_zoom);
			vprect.translate(d, d);
			scrollRectToVisible(vprect);
			viewControlsPanel.updateZoomControls();
			imgPanel.refresh();
			// doLayout();
		}
	};

	public void setCanvasSize(Dimension canvasSize) {
		imgPanel.setCanvasSize(canvasSize);
	}

	public void setIsOverview(boolean b) {
		this.isOverview = b;
		if (isOverview) {
			_buttonsPanel.remove(channelNameLabel);
			_statusPanel.remove(status);
			connectionStatusLabel.setText("Connection status: ");
		}
		viewControlsPanel.setIsOverview(this.isOverview);
	}

	/** Adds components to the status panel */
	private void _addStatusFields() {
		_statusPanel.add(status);
		_statusPanel.add(connectionStatusLabel);
		_statusPanel.add(connectionStatus);
		connectionStatus.setOpaque(true);
	}

	/** Adds components to the buttons panel */
	private void _addButtons() {
		//_buttonsPanel.add(channelNameLabel);
		JPanel _controlsPanel = new JPanel(new GridLayout(1, 0));
		_buttonsPanel.add(_controlsPanel);
		_controlsPanel.add(channelNameLabel);
		_controlsPanel.add(mouseLabel);
		_controlsPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));

		imgPanel.setShowFrames(true);
		imgPanel.setShowRasterImage(true);
	}

	/**
	 * Draws a vector dataset on the image panel.
	 * Does nothing if the polylines cannot be obtained.
	 * @param vectorInfo
	 */
	private void _drawVector(VectorInfo vectorInfo) {
		List<List<Point>> vlines = null; 
		try {
			vlines = vectorInfo.getPolyLines();
		}
		catch(Exception ex) {
			// TODO log some warning but only once per vector dataset
		}
		if ( vlines != null ) {
			imgPanel.drawVector("vect:" +vectorInfo.getName(), vlines, vectorInfo.getColor());
		}
	}
	
	/**
	 * @param vectors
	 */
	public void setVectors(List<VectorInfo> vectors) {
		// put all given vectors in our map:
		this.vectors.clear();
		imgPanel.removeAllStartingWith("vect:");
		if ( vectors != null && vectors.size() >= 1 ) {
			for (VectorInfo vectorInfo : vectors) {
				this.vectors.put(vectorInfo.getName(), vectorInfo);
				if ( vectorInfo.isSelected() ) {
					_drawVector(vectorInfo);
				}
			}
		}
		else {
			System.out.println("StreamViewer: WARNING: No vector data!");
		}
		viewControlsPanel.refreshViewPopup();
	}
	
	/**
	 * @param vectorName
	 * @param show
	 */
	public void setShowVector(String vectorName, boolean show) {
		if ( show ) {
			VectorInfo vectorInfo = vectors.get(vectorName);
			if ( vectorInfo != null ) {
				_drawVector(vectorInfo);
			}
			else {
				System.out.println("StreamViewer: setShowVector: unexistent object: " +vectorName);
			}
		}
		else {
			imgPanel.removeElement("vect:" +vectorName);
		}
	}


	/** If true, the whole image is saved when a frame is finished */
	public void setAutoSaveFrames(boolean autoSaveFrames) {
		this.autoSaveFrames = autoSaveFrames;
	}

	/** Starts a thread for saving the current contents of the image. */
	public void save() {
		new Thread(new Runnable() {
			public void run() {
				_save();
			}
		}).start();
	}

	/** Saves the current contents of the image. */
	void _save() {
		if ( istream == null ) {
			System.out.println("StreamViewer._save: no IStream");
			return;
		}
		
		String saved = istream.getSavedURL();
		if (saved == null) {
			return;
		}
		try {
			URL url = new URL(saved);
			String filename = new File(url.getFile()).getName();
			System.out.println("[" + new Date() + "] saving " + filename);
			imgPanel.save(filename);
		} catch (Exception ex) {
			System.out.println("Error: " + ex.getMessage());
		}
	}

	// TODO check other methods for proper AWT event dispatch
	
	/** Writes a text in the connection status label. */
	public void writeConnectionStatus(final Color bgcolor, final String text) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				connectionStatus.setBackground(bgcolor);
				connectionStatus.setText(text);
			}			
		});
	}

	/** Writes a text in the status label. */
	public void writeStatus(final String text) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				status.setText(text);
			}			
		});
	}

	public synchronized void setReferenceSpace(Rectangle referenceSpace) {
		System.out.println("StreamViewer: Setting reference space = " + referenceSpace);
		imgPanel.setReferenceSpace(referenceSpace);
	}
	
	public synchronized void newFrame(FrameStatus frameStatus) {
		if (isOverview) {
			// we keep only the current frame
			imgPanel.removeAllStartingWith("rect:");
		}

//		Utils.log("StreamViewer: newFrame: " +frameStatus);
		String tip = _getFrameTip(frameStatus);
		FrameDef frameDef = frameStatus.getFrameDef();
		String frameID = frameDef.getID();
		imgPanel.drawRect("rect:" +frameID, tip, frameDef.getRectangle(), Color.GREEN);
	}

	public synchronized void drawFOV(Rectangle rect) {
		imgPanel.drawRect("rect:" +"FOV", "Field of View", rect, Color.CYAN);
	}

	/** 
	 * Displays one more image.
	 */
	public synchronized void addImage(IImage img) {
		istream = img.getStream();
		assert istream != null;

		streamID = istream.getStreamID();
		channelDef = istream.getChannelDef();
		streamStatus = istream.getStreamStatus();
		
		assert streamStatus != null || Utils.fail(
				"No StreamStatus for streamID=" +streamID+ "\n" +
				"istream=" +istream+ "\n" +
				"channelDef=" +istream.getChannelDef())
		;

		channelNameLabel.setText(streamID);
		
		if (!isOverview) {
			int psx = channelDef.getPixelSizeX();
			int psy = channelDef.getPixelSizeY();
			imgPanel.drawImage(img.getX(), img.getY(), psx, psy, img.getBufferedImage());
			
			writeStatus("Received: " + streamStatus.getImageCount() + " row(s) in "
					+ streamStatus.getFrameCount() + " frame(s)");
		}

		String frameID = img.getFrameID();
		FrameStatus frameStatus = istream.getFrameStatusMap().get(frameID);

		int psx = channelDef.getPixelSizeX();
		int line_width = img.getWidth();
		
		// for now
//		assert frameStatus != null;
		if (frameStatus == null ) {
			imgPanel.drawLine("line:" +frameID + "_first",
					img.getX(), img.getY(),
					img.getX() + psx * line_width - 1,
					img.getY(), Color.YELLOW);
			imgPanel.refresh();
			return;
		}
		
	
		// get frameDef and use it provided is known:
		FrameDef frameDef = frameStatus.getFrameDef();

		// management of img.getWidth() == 0 to drawLine with the
		// width of the corresponding frame:
		if (line_width == 0 && frameDef != null) {
			line_width = frameDef.getRectangle().width / psx;
		}

		// First line:
		if (!isOverview) {
			if (frameStatus.getNumberOfRows() == 1) {
				imgPanel.drawLine("line:" +frameID + "_first",
						img.getX(), frameStatus.getFirstRow(),
						img.getX() + psx * line_width - 1,
						frameStatus.getFirstRow(), Color.YELLOW);
			}
		}

		if (frameStatus.completed()) {
			// change the frame color:
			imgPanel.drawWithColor("rect:" +frameID, Color.RED);
			// and remove line indicators:
			imgPanel.removeElement("line:" +frameID + "_first");
			imgPanel.removeElement("line:" +frameID + "_last");

			// and save if so indicated:
			if (autoSaveFrames) {
				// call the non-threaded version:
				_save();
			}
		} else {
			// draw indicator for last row:
			
//			OLD
//			imgPanel.drawLine("line:" +frameID + "_last", 
//				img.getX(), frameStatus.getLastRow(), 
//				img.getX() + psx * line_width - 1, frameStatus.getLastRow(), 
//				Color.YELLOW);
			
//			should be:
			imgPanel.drawLine("line:" +frameID + "_last", 
					img.getX(), img.getY(),
					img.getX() + psx * line_width - 1,
					img.getY(), Color.YELLOW);
		}

		imgPanel.refresh();
	}

	public synchronized void loadBackgroundImage(URL url) {
		try {
			imgPanel.loadBackgroundImage(url);
		} catch (IOException ex) {
			System.out.println("StreamViewer: loadBackgroundImage: error: " + ex.getMessage());
		}
	}

	public synchronized void finishFrames() {
		// paint all frames red
		imgPanel.drawAllStartingWith("rect:", Color.RED);
		imgPanel.refresh();
	}


	public StreamDef getStreamDef() {
		return streamDef;
	}

	public ChannelDef getChannelDef() {
		return channelDef;
	}

	/**
	 * Returns the ROI associated to this stream viewer.
	 */
	public Rectangle getROI() {
		return roi;
	}

	/** ToolTip for a frame */
	private static String _getFrameTip(FrameStatus frameStatus) {
		StreamDef streamDef = frameStatus.getStreamDef();
		FrameDef frameDef = frameStatus.getFrameDef();
		String frameID = frameStatus.getFrameID();
		Rectangle rect = frameDef.getRectangle();
		String streamID = "unknown";
		String channelID = "unknown";
		if ( streamDef != null ) {
			channelID = streamDef.getChannelDef().getID();
			streamID = streamDef.getStreamID();
		}
		return "<html>" 
				+ "<b>Stream: ID = " +streamID + "</b><br>"
				+ "<b>Channel: ID = " +channelID + "</b><br>"
				+ "<b>Frame: ID = " +frameID+ "</b><br>"
				+ "<b>Origin: (x, y) = (" + rect.x + ", " + rect.y
				+ ")</b><br>" + "<b>Size: width x height = " + rect.width
				+ " x " + rect.height + "</b><br>" 
				+ " completed: " +frameStatus.completed()+ "</b><br>" 
				+ "</html>";
	}

	public boolean isOverview() {
		return isOverview;
	}

	/**
	 * @return
	 */
	public Image getCanvasImage() {
		return imgPanel.getCanvasImage();
	}

	public IStream getStream() {
		return istream;
	}
	
	public void clear() {
		imgPanel.removeAllStartingWith("");
		imgPanel.clear();
		imgPanel.refresh();
	}
}

