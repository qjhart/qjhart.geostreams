/**
 * 
 */
package geostreams.workflow;

import geostreams.image.StreamDef;
import geostreams.image.VectorInfo;

import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

/**
 * @author Carlos Rueda
 * @version $Id: NIManager.java,v 1.14 2007/06/12 16:51:55 crueda Exp $
 */
class NIManager {

	private WorkflowObserver viewObserver;
	
	/** The server */
	private NIServer _server = null;
	
	private List<RemoteClient> remoteClients = new ArrayList<RemoteClient>();

	NIManager(WorkflowObserver viewObserver, int port, int serverTimeout, int clientTimeout) throws Exception {
		System.out.println("\n<<<<<<<<<<<<<<<< create NIManager");
		assert _server == null;
		
		this.viewObserver = viewObserver;
		
        // create the server
		try {
			_server = new NIServer(this, port, serverTimeout, clientTimeout);
			startThread(_server);
		}
		catch(Exception ex) {
			throw new Exception("NI: Error creating server: " +ex.getMessage());
		}
	}
	
    public void executionFinished() {
    	System.out.println("################ NIManager.shutdown");
    	if ( _server != null ) {
    		_server.interrupt();
    	}
    	_server = null;
    	interruptStartedThreads();
    	remoteClients.clear();
    }

    /** starts sending the requested stream to a client */
	public void sendStreamToRemoteClient(RemoteClient remoteClient) throws Exception {
		getWorkflow().addRemoteClient(remoteClient);
		remoteClients.add(remoteClient);
	}

	/** starts sending the requested stream to a client */
	public void sendVectorToRemoteClient(RemoteClient remoteClient, String vectorName) throws Exception {
		VectorInfo vectorInfo = getWorkflow().getVectorInfo(vectorName);
		remoteClient.send(vectorInfo);
	}
	
	/**
	 * @param remoteClient
	 */
	public synchronized void removeClient(RemoteClient remoteClient) {
		getWorkflow().removeRemoteClient(remoteClient);
		remoteClients.remove(remoteClient);
	}

	/**
	 * @return the associates workflow.
	 */
	IWorkflow getWorkflow() {
		return viewObserver.getWorkflow();
	}
	
	public void clientAccepted(Socket clientSocket) throws Exception {
		getWorkflow().incrClients(1);
		RemoteClient.IObserver observer = new RemoteClient.IObserver() {
			public void clientDied(RemoteClient client) {
				getWorkflow().incrClients(-1);
				removeClient(client);
			}
		};

		RemoteClient remoteClient = new RemoteClient(observer, clientSocket);
		startThread(new DispatchRemoteClient(this, remoteClient));
	}

	public boolean existsStream(String streamName) {
		for ( StreamDef def : getWorkflow().getStreamDefs() ) {
			if ( def.getStreamID().equals(streamName) ) {
				return true;
			}
		}
		return false;
	}
	
	public boolean existsVector(String vectorName) {
		for (VectorInfo vectorInfo : getWorkflow().getVectorInfos() ) {
			if ( vectorName.equals(vectorInfo.getName()) ) {
				return true;
			}
		}
		return false;
	}
	
	private List<Thread> startedThreads = new ArrayList<Thread>();
	void startThread(Thread thread) {
		thread.start();
		startedThreads.add(thread);
	}
	void interruptStartedThreads() {
		for ( Thread thread : startedThreads ) {
			if ( thread.isAlive() ) {
				System.out.println(" ***** I n t e r r u p t i n g " +thread);
				thread.interrupt();
			}
		}
		startedThreads.clear();
	}

	/**
	 * 
	 */
	public void executionPaused() {
		// TODO executionPaused
	}
}
