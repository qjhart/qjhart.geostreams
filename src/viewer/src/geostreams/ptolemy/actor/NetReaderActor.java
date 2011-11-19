package geostreams.ptolemy.actor;

import java.util.HashMap;
import java.util.Map;

import javax.swing.JFrame;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ObjectToken;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;
import geostreams.Client;
import geostreams.Client.IChunkObserver;
import geostreams.image.IImage;
import geostreams.util.SyncFifo;


/**
  * Similar to Client but here as an actor.
  *
  * @deprecated Use {@link ImageStreamReaderActor}
  * 
  * @author Carlos Rueda-Velasquez
  * @version $Id: NetReaderActor.java,v 1.2 2007/01/09 21:15:27 crueda Exp $
  */
public class NetReaderActor extends TypedAtomicActor {
	public StringParameter host_p;            
	public StringParameter port_p;            
	
	/** The output port. */
	public TypedIOPort output = null;


	private Map<String,Object> props;

	
	private Client client;
	private JFrame jframe;
	

	private SyncFifo syncFifo;
	
	private IChunkObserver chunkObserver = new IChunkObserver() {
		public void newChunk(Object chunk) {
			if ( syncFifo != null ) {
				try {
					syncFifo.put(chunk);
				}
				catch(InterruptedException ex) {
					syncFifo = null;
				}
			}
		}

		public void exceptionThrown(Throwable ex) {
			System.out.println("exception: " +ex.getMessage());
			ex.printStackTrace();
		}

		public void end(String msg) {
		}
	}; 


	
	/** Construct an actor with the given container and name.
     *  @param container The container.
     *  @param name The name of this actor.
     *  @exception IllegalActionException If the actor cannot be contained
     *   by the proposed container.
     *  @exception NameDuplicationException If the container already has an
     *   actor with this name.
     */
    public NetReaderActor(CompositeEntity container, String name)
	throws IllegalActionException, NameDuplicationException {

        super(container, name);
        
		//
		// parameters
		//
        host_p = new StringParameter(this, "Host");
        host_p.setExpression("localhost");

        port_p = new StringParameter(this, "port");
        port_p.setExpression("35813");
        
		//
		// output
		//
		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(BaseType.OBJECT);
		output.setMultiport(true);

        props = new HashMap<String,Object>();
        props.put("name", "NReader");
        _attachText("_iconDescription", ActorUtilities.getIconText(props));
    }
    
	/**
	 */
    public void initialize() throws IllegalActionException {
		System.out.println("\n<<<<<<<<<<<<<<<< NReader.INITIALIZE");
        super.initialize();
        
        if ( jframe == null) {
			jframe = new JFrame("NetReader");
        }

		
		try {
			String host = host_p.stringValue();
			int port = Integer.parseInt(port_p.stringValue());
			client = new Client(jframe, host, port);
			syncFifo = new SyncFifo();
			client.setChunkObserver(chunkObserver);
		}
		catch(Exception ex) {
			throw new IllegalActionException("NI: Error creating server: " +ex.getMessage());
		}
	}
    
    public boolean prefire() throws IllegalActionException {
		if ( syncFifo == null ) {
			return false;
		}
		return true;
    }

    public void fire() throws IllegalActionException {
		if ( syncFifo == null ) {
			return;
		}
		Object obj = null;
    	try {
			obj = syncFifo.take();
    	}
		catch(InterruptedException ex) {
			syncFifo = null;
		}
		if ( obj == null ) {
			return;
		}

		super.fire();
		
		// only output IImage tokens:
		
		if ( obj instanceof IImage ) {
			output.broadcast(new ObjectToken(obj));
		}
		else {
			System.out.println("NetReader.fire: received (" +obj.getClass().getName()+ ") = " +obj);
		}
    }
	
    public void stop() {
    	System.out.println("\n\n____ S T O P -- NetReaderActor");
    	super.stop();
    	reset();
    }
    
    private void reset() {//throws IllegalActionException {
    	System.out.println("################ NetReaderActor.RESET");
    	if ( client != null ) {
    		client.destroy();
    		client = null;
    	}
    	if ( jframe != null ) {
    		jframe.dispose();
    		jframe = null;
    	}
    }
}

