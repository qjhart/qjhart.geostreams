package geostreams.ptolemy;

import geostreams.image.IImage;
import geostreams.workflow.RemoteClient;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentLinkedQueue;

import ptolemy.actor.IOPort;
import ptolemy.actor.PortEvent;
import ptolemy.actor.PortEventListener;
import ptolemy.data.ObjectToken;
import ptolemy.data.Token;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * A stream server.
 * 
 * @author Carlos Rueda
 * @version $Id: StreamServer.java,v 1.5 2007/09/09 08:19:55 crueda Exp $
 */
class StreamServer implements PortEventListener { //TokenSentListener {
	
	private String streamName;
	private IOPort port;

	private NetInterface netInterface;
	
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
    public StreamServer(String streamName, IOPort port) {
    	this.streamName = streamName;
    	this.port = port;
//		port.addTokenSentListener(this);
        port.addPortEventListener(this);
		System.out.println("StreamServer for stream '" +this.streamName+ "' created.");
    }
	
    private void _updateIcon(final int numberOfClients) {
		// TODO updateIcon
    }

    void setNetInterface(NetInterface view) {
		this.netInterface = view;
	}

	void addRemoteClient(RemoteClient remoteClient) throws IOException {
		System.out.println("\n---\n---StreamServer.addRemoteClient: stream: " +remoteClient.getRequestedStream()+ "\n---");
		_activeClients.add(remoteClient);
		remoteClient.send("OK-added");
		_printStatus();
	}
	
	private void _printStatus() {
		System.out.println("printStatus: " +this);
		_updateIcon(_activeClients.size());
	}

	void closeClients() {
		for ( RemoteClient remoteClient : _activeClients ) {
			remoteClient.close();
		}
	}

	
	public String toString() {
		return getClass().getSimpleName()+ " activeClients = " +_activeClients.size();
	}

    
	/**
	 * @param remoteClient
	 */
	public void removeRemoteClient(RemoteClient remoteClient) {
		System.out.println("(((((((( StreamServer#removeRemoteClient: " +remoteClient);
		_activeClients.remove(remoteClient);
		_printStatus();
	}

	/** Sends the image in the event to active clients. 
	 */
    public void portEvent(PortEvent event) {
//	public void tokenSentEvent(TokenSentEvent event) {
        if ( _activeClients.size() == 0 ) {
			return;
		}
        Token token = event.getToken();
		
		ObjectToken objToken = (ObjectToken) token;
		IImage img = (IImage) objToken.getValue();
		
//		System.out.println(this+ "--[[ tokenSentEvent: " +getStreamName()+ "]]--" +
//				" img.isNull() = " +img.isNull());
		
		if ( img.isNull() ) {
			// DO NOT send null images.
			return;
		}
		
		_deadClients.clear();		
		for ( RemoteClient rclient : _activeClients ) {
			if ( rclient.isActive() ) {
				rclient.send(img);
			}
			
			if ( !rclient.isActive() ) {
				System.out.println("StreamServer: client is no longer active");
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
		
		// No clients left active?, then notify netInterface
		if ( _activeClients.size() == 0 ) {
			netInterface.streamServerIdle(this);
		}
	}

	public IOPort getPort() {
		return port;
	}

	public String getStreamName() {
		return streamName;
	}

}
