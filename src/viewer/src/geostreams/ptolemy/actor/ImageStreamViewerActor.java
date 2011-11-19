package geostreams.ptolemy.actor;

import geostreams.goes.GOESConstants;
import geostreams.image.FrameStatus;
import geostreams.image.IConnectionObserver;
import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.ptolemy.icon.ViewerIcon;
import geostreams.vizstreams.ImageStreamViewer;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Image;
import java.awt.Point;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Map;

import javax.swing.JFrame;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.AWTImageToken;
import ptolemy.data.ObjectToken;
import ptolemy.data.Token;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * @deprecated Use {@link StreamViewerActor}
 * 
  * Displays located images using a {@link geostreams.vizstreams.ImageStreamViewer}.
  * 
  * The internal image with the current state of the visualization is
  * made available via AWTImageTokens through an output port.
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: ImageStreamViewerActor.java,v 1.12 2007/06/04 09:07:52 crueda Exp $
  */
public class ImageStreamViewerActor extends TypedAtomicActor {
	public TypedIOPort input;
	/** to broadcast the internal CanvasImage in AWTImageTokens */
	public TypedIOPort output = null;
	
	Map<String, Object> props;

	public StringParameter frameLocationParam;
	public StringParameter zoomFactorParam;
	public StringParameter bgSizeParam;
	private JFrame jframe;
	
//	private ViewerIcon _icon;
	
	
	/** the viewer used to display the incoming images. */
	private volatile ImageStreamViewer imgStreamViewer = null;

    /** Construct an actor with the given container and name.
     *  @param container The container.
     *  @param name The name of this actor.
     *  @exception IllegalActionException If the actor cannot be contained
     *   by the proposed container.
     *  @exception NameDuplicationException If the container already has an
     *   actor with this name.
     */
    public ImageStreamViewerActor(CompositeEntity container, String name)
	throws IllegalActionException, NameDuplicationException {

        super(container, name);
//        _icon = 
        new ViewerIcon(this, "_icon");

		//
		// ports
		//
        input = new TypedIOPort(this, "input", true, false);
        input.setMultiport(true);
        input.setTypeEquals(BaseType.OBJECT);
        
        output = new TypedIOPort(this, "output", false, true);
        output.setTypeEquals(BaseType.OBJECT);
        
        //
        // parameters
        //
        frameLocationParam = new StringParameter(this, "frameLocationParam");
        frameLocationParam.setExpression("0 0 200 200");
        frameLocationParam.setDisplayName("Frame location");

        zoomFactorParam = new StringParameter(this, "zoomFactorParam");
        zoomFactorParam.setExpression("10");
        zoomFactorParam.setDisplayName("Zoom factor");
        
        bgSizeParam = new StringParameter(this, "bgSizeParam");
        bgSizeParam.setExpression("1000");
        bgSizeParam.setDisplayName("Background width");
        
//        props = new HashMap<String,Object>();
//        props.put("name", "Viewer");
//        
//		_attachText("_iconDescription", ActorUtilities.getIconText(props));
    }

	/** Creates the window and the ImageStreamViewer.
	  */
    public synchronized void initialize() throws IllegalActionException {
//		System.out.println("ImageStreamViewerActor.initialize");
        super.initialize();
        
        Point frameLoc = new Point(300, 0);
        Dimension frameDim = new Dimension(235, 250);
        try {
        	String[] toks = frameLocationParam.stringValue().split("\\s+");
        	int[] vals = new int[toks.length];
        	for (int i = 0; i < vals.length; i++) {
				vals[i] = Integer.parseInt(toks[i]);
			}
        	frameLoc.x = vals[0];
        	frameLoc.y = vals[1];
        	frameDim.width = vals[2];
        	frameDim.height= vals[3];
        }
        catch (Exception e) {
			System.out.println("Warning: ignoring invalid frame location specification");
		}

        if ( jframe == null) {
			jframe = new JFrame("Image Stream Viewer");
        }
        jframe.setLocation(frameLoc);
        jframe.setSize(frameDim);

        int zoomFactor = 10;
        try {
        	zoomFactor = Integer.parseInt(zoomFactorParam.stringValue());
        }
        catch (Exception e) {
			System.out.println("Warning: ignoring invalid zoom factor specification");
		}

        int bgWidth = 2000;
        try {
        	bgWidth = Integer.parseInt(bgSizeParam.stringValue());
        }
        catch (Exception e) {
			System.out.println("Warning: ignoring invalid background width specification");
		}

        if ( imgStreamViewer == null ) {
			int bgHeight = (int) (GOESConstants.aspect * bgWidth);
			Dimension backgroundSize = new Dimension(bgWidth, bgHeight);
			imgStreamViewer = new ImageStreamViewer();
			imgStreamViewer.setCanvasSize(backgroundSize);
			imgStreamViewer.setZoom(zoomFactor);
			jframe.getContentPane().removeAll();
			jframe.getContentPane().add(imgStreamViewer);
		}
        else {
        	imgStreamViewer.setZoom(zoomFactor);
        }
        jframe.setVisible(true);
	}
    
    private synchronized void setImageStreamViewer(ImageStreamViewer imgStreamViewer) {
    	this.imgStreamViewer = imgStreamViewer;
    }
    
    private long lastFireTime = System.currentTimeMillis();
    private void writeConnectionStatus(ImageStreamViewer isv, Color color, String msg) {
    	if ( isv != null ) {
    		isv.writeConnectionStatus(color, msg);
    	}
    	lastFireTime = System.currentTimeMillis();
    }
    private void updateConnectionStatus(ImageStreamViewer isv) {
    	long newFireTime = System.currentTimeMillis();
    	if ( isv != null && newFireTime - lastFireTime >= 2000 ) {
    		writeConnectionStatus(isv, Color.YELLOW, "Connected. Waiting for data...");
    	}
    	lastFireTime = newFireTime;
    }
	
	/** Gets the image and adds it to the viewer.
	  */
    public void fire() throws IllegalActionException {
    	if ( imgStreamViewer != null
		&& input.numberOfSources() > 0 
		&& input.hasToken(0) ) {
			for ( int i = 0; i < input.numberOfSources(); i++ ) {
				Token token = input.get(i);
				ObjectToken objToken = (ObjectToken) token;
				Object obj = objToken.getValue();
				
				if ( obj instanceof IImage ) {
					IImage img = (IImage) objToken.getValue();
					
					if ( !img.isNull() ) {
						assert img.getServerConnection() != null;						
						if ( imgStreamViewer != null ) {
							IServerConnection gscon = imgStreamViewer.getServerConnection();
							boolean firstImage = gscon == null;

							if ( firstImage ) {
								// If first image, then the connection is used to obtain
								// additional information and displayable data:
								//  - reference space
								//  - initial full image
								//  - vector datasets
								gscon = img.getServerConnection();
								
								// reference space:
								imgStreamViewer.setReferenceSpace(gscon.getReferenceSpace().getRectangle());
								
								// initial full image:
								String channelID = img.getChannelID();
								String saved = gscon.getSavedURL(channelID);
								try {
									URL saved_url = new URL(saved);
									imgStreamViewer.loadBackgroundImage(saved_url);
								}
								catch (MalformedURLException ex) {
									System.out.println("MalformedException: " +ex.getMessage());
								}
								
								// vectors:
								imgStreamViewer.setVectors(gscon.getVectors());
								
								gscon.addObserver(new ConnectionObserver(), channelID);
							}

							imgStreamViewer.addImage(img);
							imgStreamViewer.writeConnectionStatus(Color.GREEN, "Connected. Reading data...");
							
							// and broadcast the internal AWT image
							Image wholeImage = imgStreamViewer.getCanvasImage();
							output.broadcast(new AWTImageToken(wholeImage));
						}
						writeConnectionStatus(imgStreamViewer, Color.GREEN, "Connected. Reading data...");
					}
				}
				else {
					System.out.println("Warning: ignoring unexpected object in input token: " +
							obj+ (obj != null ? " of class " +obj.getClass().getName() : "")
					);
				}
			}
    	}

    	updateConnectionStatus(imgStreamViewer);
	}
    
    public void stop() {
//    	System.out.println("\n\n____ S T O P -- ImageStreamViewerActor\n" +
//    	                   "     imgStreamViewer = " +imgStreamViewer);
    	super.stop();
    	reset();
    }
    
    private void reset() {
		if ( imgStreamViewer != null ) {
			updateConnectionStatus(imgStreamViewer);
		}
		setImageStreamViewer(null);
    }

    public void wrapup() throws IllegalActionException {
    	super.wrapup();
		reset();
    }

    /**
     * Used mainly to update frame related event. 
     * This is copied/adapted from VizStreams.ConnectionObserver
     */
	class ConnectionObserver implements IConnectionObserver {
		public void newImage(IImage iimg) {
			// NOTHING - images come from the input port ;-)
		}
	
		public void frameStarts(FrameStatus frameStatus) {
			if (imgStreamViewer != null ) {
				imgStreamViewer.newFrame(frameStatus);
			}
//			System.out.println("ImageStreamViewerActor: frameStarts: " +frameStatus);
		}
		
		public void frameEnds(FrameStatus frameStatus) {
			// TODO frameEnds
		}
		
		/**
		 * Returns true.
		 * TODO should return false, but for now returning true to maintain original
		 * behavior while I get time to review this code
		 */
		public boolean connectionException(String streamID, Exception ex) {
			if (imgStreamViewer != null ) {
				imgStreamViewer.writeConnectionStatus(Color.RED, "Exception: " +ex.getMessage());
			}
			System.out.println("ImageStreamViewerActor: connectionException: " +ex.getMessage());
			return true;
		}
	
		public void connectedWaitingData(String streamID, String msg) {
			if (imgStreamViewer != null ) {
				imgStreamViewer.writeConnectionStatus(Color.YELLOW, msg);
			}
		}
		
		/**
		 * Returns true.
		 * TODO should return false, but for now returning true to maintain original
		 * behavior while I get time to review this code
		 */
		public boolean readerFinished(String streamID, final String msg) {
			EventQueue.invokeLater(new Runnable() {
				public void run() {
					if (imgStreamViewer != null ) {
						imgStreamViewer.finishFrames();
						imgStreamViewer.writeConnectionStatus(Color.RED, msg);
					}
				}
			});
			return true;
		}
	}
}

