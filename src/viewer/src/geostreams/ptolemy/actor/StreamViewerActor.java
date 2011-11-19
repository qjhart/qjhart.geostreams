package geostreams.ptolemy.actor;

import geostreams.goes.GOESConstants;
import geostreams.image.FrameStatus;
import geostreams.image.IImage;
import geostreams.image.IStream;
import geostreams.ptolemy.icon.ViewerIcon;
import geostreams.vizstreams.StreamViewer;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashSet;
import java.util.Set;

import javax.swing.JFrame;
import javax.swing.Timer;

import ptolemy.actor.TypedIOPort;
import ptolemy.data.AWTImageToken;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * TODO : 0 istream
 * 
 * Will replace ImageStreamViewerActor
 *
 * Displays located images using a {@link geostreams.vizstreams.StreamViewer}.
 * 
 * The internal image with the current state of the visualization is
 * made available via AWTImageTokens through an output port.
 *
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamViewerActor.java,v 1.10 2007/06/26 06:20:48 crueda Exp $
 */
public class StreamViewerActor extends StreamSinkActor {
	/** to broadcast the internal CanvasImage in AWTImageTokens */
	public TypedIOPort output = null;
	
	public StringParameter frameLocationParam;
	public StringParameter zoomFactorParam;
	public StringParameter bgSizeParam;
	private JFrame jframe;
	
	/** to detect change in frame, so to update the color to use */
	private String oldFrameID;

	
	/** the viewer used to display the incoming images. */
	private volatile StreamViewer streamViewer = null;
	
	/** streamID's received so far */
	private final Set<String> _istreams = new HashSet<String>();
	
	private ViewerIcon _icon;

    /** Construct an actor with the given container and name.
     *  @param container The container.
     *  @param name The name of this actor.
     *  @exception IllegalActionException If the actor cannot be contained
     *   by the proposed container.
     *  @exception NameDuplicationException If the container already has an
     *   actor with this name.
     */
    public StreamViewerActor(CompositeEntity container, String name)
	throws IllegalActionException, NameDuplicationException {

        super(container, name, true);
        _icon = new ViewerIcon(this, "_icon");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);

        
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
        
    }

	/** Creates the window and the ImageStreamViewer.
	  */
    public synchronized void initialize() throws IllegalActionException {
//		System.out.println("StreamViewerActor.initialize");
        super.initialize();
        
        _istreams.clear();
        
        oldFrameID = null;
        
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
        	
			jframe = new JFrame("StreamViewer");
			jframe.setLocation(frameLoc);
			jframe.setSize(frameDim);
        }
        
        int zoomFactor = 10;
        try {
        	zoomFactor = Integer.parseInt(zoomFactorParam.stringValue());
        }
        catch (Exception e) {
        	System.out.println("StreamViewerActor: Warning: ignoring invalid zoom factor specification");
        }

        int bgWidth = 2000;
        try {
        	bgWidth = Integer.parseInt(bgSizeParam.stringValue());
        }
        catch (Exception e) {
        	System.out.println("StreamViewerActor: Warning: ignoring invalid background width specification");
        }


        if ( streamViewer == null ) {
        	int bgHeight = (int) (GOESConstants.aspect * bgWidth);
        	Dimension backgroundSize = new Dimension(bgWidth, bgHeight);
        	streamViewer = new StreamViewer();
        	streamViewer.setCanvasSize(backgroundSize);
        	streamViewer.setZoom(zoomFactor);
        	jframe.getContentPane().removeAll();
        	jframe.getContentPane().add(streamViewer);
        }

        jframe.setVisible(true);
        
        setViewerIsActive(true);
	}
    
    private synchronized void setStreamViewer(StreamViewer streamViewer) {
    	if ( streamViewer != null ) {
        	streamViewer.clear();
    	}
    	this.streamViewer = streamViewer;
    }
    
    
    protected synchronized void _sink(IImage img) throws IllegalActionException {
    	setStreamIsActive(true);
    	
		if ( img.isNull() ) {
			return;
		}
		IStream istream = img.getStream();
		assert istream != null;						
		
		if ( streamViewer == null ) {
			return;
		}
		
		String streamID = istream.getStreamID();
		String frameID = img.getFrameID();
		boolean firstImage = !_istreams.contains(streamID);

		if ( firstImage ) {
			_istreams.add(streamID);
			
			oldFrameID = frameID;

			// If first image, then obtain
			// additional information and displayable data:
			//  - reference space
			//  - initial full image
			//  - vector datasets --> NO

			// reference space:
			streamViewer.setReferenceSpace(istream.getReferenceSpace().getRectangle());
			
			Rectangle fov = istream.getFOV();
			if ( fov != null ) {
				streamViewer.drawFOV(fov);
			}

			// initial full image:
			String saved = istream.getSavedURL();
			if ( saved != null ) {
				try {
					URL saved_url = new URL(saved);
					streamViewer.loadBackgroundImage(saved_url);
				}
				catch (MalformedURLException ex) {
					System.out.println("StreamViewerActor: MalformedException: " +ex.getMessage());
				}
			}

			oldFrameID = frameID;
			FrameStatus frameStatus = istream.getFrameStatusMap().get(frameID);
//			Utils.log("StreamViewerActor: newFrame: frameStatus=" +frameStatus);
			if ( frameStatus != null ) {
				streamViewer.newFrame(frameStatus);
			}

			// vectors:
			// TODO should vector info be given by a different means?
			streamViewer.setVectors(istream.getVectors());

			// TODO remove
//			gscon.addObserver(new ConnectionObserver(), channelID);
		}
		else if ( oldFrameID == null || !oldFrameID.equals(frameID) ) {
			oldFrameID = frameID;

			if (streamViewer != null ) {
				FrameStatus frameStatus = istream.getFrameStatusMap().get(frameID);
//				Utils.log("StreamViewerActor: newFrame: frameStatus=" +frameStatus);
				if ( frameStatus != null ) {
					streamViewer.newFrame(frameStatus);
				}
				// else: TODO how is IStream knowing about new status items?
			}
		}

		streamViewer.addImage(img);

		// and broadcast the internal AWT image
		Image wholeImage = streamViewer.getCanvasImage();
		output.broadcast(new AWTImageToken(wholeImage));
    }
	
    
    public void stop() {
//    	System.out.println("\n\n____ S T O P -- StreamViewerActor\n" +
//    	                   "     imgStreamViewer = " +imgStreamViewer);
    	super.stop();
    	reset();
    }
    
    private void reset() {
    	_istreams.clear();
    	oldFrameID = null;
    	setViewerIsActive(false);
    	lastRead = 0;
    	
		setStreamViewer(null);
//		if ( jframe != null ) {
//			jframe.dispose();
//			jframe = null;
//		}
    }

    public void wrapup() throws IllegalActionException {
    	super.wrapup();
		reset();
    }

    
    private boolean _streamIsActive;
    private Timer timer;
    private long lastRead = 0;
    /** 
     * Sets whether the input stream is active or not.
     * @param active
     */
	private synchronized void setStreamIsActive(boolean active) {
		_icon.setStreamIsActive(active);
		lastRead = System.currentTimeMillis();
		if ( streamViewer != null ) {
			if ( _streamIsActive != active ) {
				if ( active ) {
					streamViewer.writeConnectionStatus(Color.GREEN, "Reading data...");
				}
				else {
					streamViewer.writeConnectionStatus(Color.YELLOW, "Waiting...");
				}
				_streamIsActive = active;
			}
		}
	}
    /** 
     * Sets whether the viewer is active or not.
     * @param active
     */
	private synchronized void setViewerIsActive(boolean active) {
		_icon.setIconActive(active);
		if ( streamViewer != null ) {
			if ( active ) {
				streamViewer.writeConnectionStatus(Color.YELLOW, "Waiting...");
				if ( timer == null ) {
					timer = new Timer(300, new ActionListener() {
						public void actionPerformed(ActionEvent ev) {
							long current = System.currentTimeMillis();
							long delay = current - lastRead;
							if ( delay > 1000 ) {
								setStreamIsActive(false);
							}
						}
					});
					timer.start();
				}
			}
			else {
				if ( timer != null ) {
					timer.stop();
					timer = null;
				}
				streamViewer.writeConnectionStatus(null, "Idle.");
			}
		}
	}
}

