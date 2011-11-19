package geostreams.ptolemy.actor;

import geostreams.image.IConnectionObserver;
import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.image.Images;
import geostreams.image.ServerConnections;
import geostreams.ptolemy.icon.ReaderIcon;

import java.awt.Color;
import java.awt.Rectangle;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import ptolemy.actor.Director;
import ptolemy.actor.ExecutionListener;
import ptolemy.actor.Manager;
import ptolemy.data.ObjectToken;
import ptolemy.data.expr.StringParameter;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;


/**
 * TODO Will replace ImageStreamReaderActor
 * 
  * This actor broadcasts an image stream by reading data in a given channel
  * from a GeoStreams server. Each broadcasted token is an IImage object.
  * 
  * <p>
  * Ther GeoStreams server can be specified in one of two ways:
  * <ul>
  * 	<li> URL indicating an XML specification ..
  * 	<li> host:port of the server serving a net-enabled workflow
  * </ul>
  * See {@link geostreams.image.ServerConnections#openConnection(String)}
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: StreamReaderActor.java,v 1.12 2007/09/09 08:09:21 crueda Exp $
  */ 
public class StreamReaderActor extends StreamGeneratorActor {
	private static final String[] GEOSTR_SERVERS = {
		"http://geostreams.ucdavis.edu/~carueda/applet/dev/geostreams2.xml",
		"geostreams.ucdavis.edu:35813",
		"comet.cs.ucdavis.edu:35813",
		"file:/home/carueda/goesdata/local/local.xml",
		"localhost:35813",
		
	};
	
	private IServerConnection gscon;
	
	
	
	

	/** Synchronizes image production-consumption threads.
	 * See newImage(IImage) and fire() methods below.
	 */
	private BlockingQueue<IImage> syncFifo;
	
	private Thread ptThread;
	
	/** the server parameter */
	public StringParameter serverParam;
	
	/** the channelID parameter */
	public StringParameter channelIdParam;
	
	
	private ReaderIcon _icon;
	
	private boolean null_img_sent;

	// current manager state:
	private volatile Manager.State state = null;

	/** Defines the method to dispatch the notification of new images */
	private IConnectionObserver conObs = new IConnectionObserver.Adapter() {
		// no default log messages
		protected void _log(String msg) {}
		
	    /** Puts the image in the synchronized container */
		public void newImage(IImage iimg) {
//			System.out.println("StreamReaderActor.NEW_IMAGE");
			if ( !iimg.isNull() ) {
				iimg.setServerConnection(gscon);
				iimg.setStream(_istream);
			}
			if ( syncFifo != null ) {
				// If the model is pausing, just discard the new images, except the null image:
				if ( state != Manager.PAUSED || iimg.isNull() ) {
					_icon.setStreamIsActive(true);
					try {
						syncFifo.put(iimg);
					}
					catch(InterruptedException ex) {
						Thread.currentThread().interrupt();
						_interruptPtThread();
					}
				}
			}
		}
	};
	
	/** To detect external failures, e.g. director is removed while the
	 * model is executing.
	 */
	private ExecutionListener exeListener = new ExecutionListener() {
		public void executionError(Manager manager, Throwable throwable) {
			reset();
			System.out.println(">>>StreamReaderActor executionError: " +throwable);
			throwable.printStackTrace();
		}
		
		public void executionFinished(Manager manager) {}

		public void managerStateChanged(Manager manager) {
			Manager.State newState = manager.getState();
			if ( state == Manager.PAUSED && newState != state ) {
				// some elements may have got the queue before being written to the output port.
				// However, instead of resuming on those elements, just discard them for a better visual effect:
				syncFifo.clear();
			}
			state = newState;
//			System.out.println(">>>StreamReaderActor managerStateChanged: " +state);
		}
	};
	private boolean executionListenerRegistered;
	
	/** Construct an actor with the given container and name.
	  *  @param container The container.
	  *  @param name The name of this actor.
	  *  @exception IllegalActionException If the actor cannot be contained
	  *   by the proposed container.
	  *  @exception NameDuplicationException If the container already has an
	  *   actor with this name.
	  */
    public StreamReaderActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
        _icon = new ReaderIcon(this, "_icon");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);

		//
		// parameters
		//
		serverParam = new StringParameter(this, "serverParam");
		serverParam.setExpression(GEOSTR_SERVERS[0]);
		for (int i = 0; i < GEOSTR_SERVERS.length; i++) {
			serverParam.addChoice(GEOSTR_SERVERS[i]);
		}
		serverParam.setDisplayName("Stream server");
		
		channelIdParam = new StringParameter(this, "channelIdParam");
		channelIdParam.setExpression("2");
		channelIdParam.setDisplayName("Desired channel");
		
//        props.put("name", "Reader");
//        
//		_attachText("_iconDescription", ActorUtilities.getIconText(props));
	}

    public synchronized void initialize() throws IllegalActionException {
//		System.out.println("StreamReaderActor.initialize");
        super.initialize();
        _icon.setIconActive(true);
        
//        System.out.println("my director = " +getDirector());
//        System.out.println("my manager  = " +getManager());
        Manager manager = getManager();
        if ( manager != null ) {
    		if ( !executionListenerRegistered ) {
            	manager.addExecutionListener(exeListener);
	        	executionListenerRegistered = true;
    		}
        }
        
		null_img_sent = false;

		try {
			String server_s = serverParam.stringValue();
			String channelID_s = channelIdParam.stringValue();
			
			gscon = ServerConnections.openConnection(server_s);
			syncFifo = new LinkedBlockingQueue<IImage>();

			int simulationDelay = -1;
			if ( true ) {
				// a trick to specify a delay: channelID parameter given as: "ID  delay=###" 
				int idx = channelID_s.indexOf("delay=");
				if ( idx >= 0 ) {
					simulationDelay = Integer.parseInt(channelID_s.substring(idx + "delay=".length()).trim());
					System.out.println("StreamReaderActor.initialize: gscon.setSimulationDelay: " +simulationDelay);
					channelID_s = channelID_s.substring(0, idx).trim();
				}
			}
			
			// TODO: stream-naming
			String streamName = channelID_s;
			
			gscon.addObserver(conObs, streamName);
			
			gscon.openStream(streamName, null/*ROI*/);
			
			Rectangle fov = gscon.getReferenceSpace().getRectangle();
			
			_istream = Images.createStream(streamName, gscon);
			_istream.setFov(fov);
			
			_generatedStreamObtained();
			
			if ( simulationDelay >= 0 ) {
				gscon.setSimulationDelay(simulationDelay);
			}

//			TODO port-name - remove this -- we don't change it
//			output.setName(streamName);
		}
		catch(Exception ex) {
			System.out.println("StreamReaderActor.initialize: exception: " +ex.getMessage());
			ex.printStackTrace();
			if ( gscon != null ) {
				IServerConnection gscon2 = gscon;
				gscon = null;
				gscon2.close();
			}
			throw new IllegalActionException(this, ex, ex.getMessage());
		}
    }

   /** Waits for the next image to come.
     * @exception IllegalActionException If an IO error occurs.
     */
    public boolean prefire() throws IllegalActionException {
		ptThread = Thread.currentThread();
		if ( syncFifo == null || null_img_sent || gscon == null ) {
			return false;
		}
		return true;
    }

	/** 
	  * Waits for an image to be read and then broadcasts it.
	  * @exception IllegalActionException If there is no director.
	  */
	public void fire() throws IllegalActionException {
		if ( syncFifo == null ) {
			return;
		}
		IImage img = null;
		try {
			img = syncFifo.take();
			null_img_sent = img != null && img.isNull();
//			System.out.println("StreamReaderActor.FIRE");
		}
		catch(InterruptedException ex) {
			Thread.currentThread().interrupt();
		}
		if ( img != null ) {
			super.fire();
			output.broadcast(new ObjectToken(img));
		}
	}


    /** Returns true iff image just sent is valid.
     */
    public boolean postfire() throws IllegalActionException {
    	if ( !super.postfire() ) {
    		return false;
    	}
    	
    	// similar prefire condition:
		if ( syncFifo == null || gscon == null ) {
			return false;
		}
		
		boolean ret = null_img_sent;
		if ( null_img_sent ) {
			System.out.println("StreamReaderActor.postfire: null_image_sent");
			reset();
		}
		return !ret;
	}
    
    public void stop() {
    	System.out.println("\n\n____ S T O P -- StreamReaderActor");
    	System.out.println("    -- syncFifo      = " +syncFifo.size()+ " elements");
    	System.out.println("    -- _imgObserver         = " +gscon);
    	System.out.println("    -- null_img_sent = " +null_img_sent);
    	super.stop();
    	reset();
    	_interruptPtThread();
    }

    private void _interruptPtThread() {
    	if ( ptThread != null ) {
    		System.out.println("    \\\\\\\\ interrupting ptThread " +ptThread);
    		ptThread.interrupt();
    	}
    }
    
    private void reset() {
    	try {
    		if ( gscon != null) {
    			gscon.close();
    		}
	    	_icon.setIconActive(false);
	        if ( getManager() != null ) {
	        	getManager().removeExecutionListener(exeListener);
	        }
	        
	        Director director = getDirector();
	        if ( director != null ) {
	        	synchronized(director) {
	        		director.notifyAll();
	        	}
	        }
    	}
    	finally {
    		gscon = null;
    		null_img_sent = false;
    		System.gc();
    	}
    }

    public void wrapup() throws IllegalActionException {
    	super.wrapup();
    	System.out.println("StreamReaderActor#wrapup");
    	reset();
    } 
}
