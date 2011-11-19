/**
 * 
 */
package geostreams.ptolemy;

import geostreams.image.IImage;
import geostreams.ptolemy.actor.ActorUtilities;
import geostreams.workflow.RemoteClient;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ObjectToken;
import ptolemy.data.Token;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.ChangeRequest;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * @author Carlos Rueda
 * @version $Id: StreamServerActor.java,v 1.8 2007/01/08 10:38:52 crueda Exp $
 */
public class StreamServerActor extends TypedAtomicActor {
	/** The output port. */
	public TypedIOPort input = null;

	private Map<String, Object> props;

	/** The view in the workflow */
	private Workflow workflow;
	
	/** List of active clients */
	private ConcurrentLinkedQueue<RemoteClient> _activeClients = new ConcurrentLinkedQueue<RemoteClient>();

	/** aux list to deregister dead clients */
	private List<RemoteClient> _deadClients = new ArrayList<RemoteClient>();
 	
	
    /** Construct an actor with the given container and name.
     *  @param container The container.
     *  @param name The name of this actor.
     *  @exception IllegalActionException If the actor cannot be contained
     *   by the proposed container.
     *  @exception NameDuplicationException If the container already has an
     *   actor with this name.
     */
    public StreamServerActor(CompositeEntity container, String name)
	throws IllegalActionException, NameDuplicationException {
        super(container, name);

        input = new TypedIOPort(this, "input", true, false);
        input.setTypeEquals(BaseType.OBJECT);
		input.setMultiport(false);
		
		_activeClients = new ConcurrentLinkedQueue<RemoteClient>();
		
        input.setTypeEquals(BaseType.OBJECT);

        props = new HashMap<String,Object>();
        props.put("name", "SS");
        props.put("numClients", 0);
        props.put("fillColor", "0xffcccc");
        
		_attachText("_iconDescription", ActorUtilities.getIconText(props));
    }
	
    private void _updateIcon(final int numberOfClients) {
		ChangeRequest changeRequest = new ChangeRequest(this, "_updateIcon") {
			protected void _execute() throws Exception {
				props.put("numClients", numberOfClients);
				_attachText("_iconDescription", ActorUtilities.getIconText(props));
			}
		};
		workflow._requestChange(changeRequest);
    }
    @SuppressWarnings("unused")
	private void _updateIcon(final boolean active) {
    	ChangeRequest changeRequest = new ChangeRequest(this, "_updateIcon") {
    		protected void _execute() throws Exception {
    			props.put("fillColor", active ? "0xd57777" : "0xffcccc");
    			_attachText("_iconDescription", ActorUtilities.getIconText(props));
    		}
    	};
    	workflow._requestChange(changeRequest);
    }

    void setWorkflow(Workflow view) {
		this.workflow = view;
	}

	void addRemoteClient(RemoteClient remoteClient) throws IOException {
		System.out.println("\n---\n---StreamServerActor.addRemoteClient: stream: " +remoteClient.getRequestedStream()+ "\n---");
		_activeClients.add(remoteClient);
		remoteClient.send("OK-added");
		_printStatus();
	}
	
	private void _printStatus() {
		System.out.println("printStatus: " +this);
		_updateIcon(_activeClients.size());
	}

    public void stop() {
    	System.out.println("\n\n____ S T O P -- StreamServerActor\n" +
    			"_activeClients = " +_activeClients
    	);
    	
    	super.stop();
    	closeClients();
    }
    
    /** If this component is removed from the model, close clients */
    public void setContainer(CompositeEntity container) throws IllegalActionException, NameDuplicationException {
		try {
			if (container == null) {
				closeClients();
			}
		}
		finally {
			super.setContainer(container);
		}
	}

	void closeClients() {
		for ( RemoteClient remoteClient : _activeClients ) {
			remoteClient.close();
		}
	}

	public boolean prefire() {
    	if ( _stopRequested ) {
    		closeClients();
    		_activeClients.clear();
    	}

    	return !_stopRequested;
	}
	
	/** 
	  */
    public void fire() throws IllegalActionException {
    	
        if ( !input.hasToken(0) )
			return;   // will never happen under PN

        
//        _updateIcon(true);
        
        // take the token
        Token token = input.get(0);

//        _updateIcon(false);

        if ( _activeClients.size() == 0 ) {
			return;
		}
		
		ObjectToken objToken = (ObjectToken) token;
		IImage img = (IImage) objToken.getValue();
		
		_deadClients.clear();		
		for ( RemoteClient rclient : _activeClients ) {
			if ( rclient.isActive() ) {
				rclient.send(img);
			}
			
			if ( !rclient.isActive() ) {
				System.out.println("StreamServerActor: client is no longer active");
				Throwable ex = rclient.getLastException();
				if ( ex != null ) { 
					System.out.println("An exception was thrown: " +ex.getClass()+ ": " +ex.getMessage());
					ex.printStackTrace();
				}
				_deadClients.add(rclient);
			}
		}
	
		if ( _deadClients.size() > 0 ) {
			// remove dead clients from list of active clients
			System.out.println("\n---\n---_deadClients: " +_deadClients.size()+ "\n---");
			_activeClients.removeAll(_deadClients);  
			_printStatus();
		}
		
		// No clients left active?, then notify NI
		if ( _activeClients.size() == 0 ) {
			workflow.streamServerActorIdle(this);
		}
	}
	
    /**
     * just returns super.postfire(), that is, the value of !_stopRequested.
     * This is because we need this actor to keep being fired while the workflow
     * is executing, even if no clients are active to this stream server. 
     */
    public boolean postfire() throws IllegalActionException {
    	return super.postfire();
    }
    
    
	public String toString() {
		return getClass().getSimpleName()+ " activeClients = " +_activeClients.size();
	}

    public void wrapup() throws IllegalActionException {
    	System.out.println("StreamServerActor#wrapup");
    }

    public void stopFire() {
    	System.out.println("StreamServerActor#stopFire");
    }
    
	/**
	 * @param remoteClient
	 */
	public void removeRemoteClient(RemoteClient remoteClient) {
		System.out.println("(((((((( StreamServerActor#removeRemoteClient: " +remoteClient);
		_activeClients.remove(remoteClient);
		_printStatus();
	}

}
