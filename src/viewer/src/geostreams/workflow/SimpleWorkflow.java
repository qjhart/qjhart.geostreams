package geostreams.workflow;

import geostreams.image.IConnectionObserver;
import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.image.ReferenceSpace;
import geostreams.image.ServerConnections;
import geostreams.image.StreamDef;
import geostreams.image.VectorInfo;

import java.net.ConnectException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * A command-line, non-interactive implementation of IWorkflow backed on a ServerConnection.
 * 
 * <p>
 * Main parameter is the name of the xml file defining a geostreams spec as used in other
 * parts of this project. This spec is handled by a ServerConnection object.
 * 
 * <p>
 * Only the streams (aka channels) in the geostreams spec will be externally available.
 * 
 * <p>
 * Implemented by following the scheme in Workflow, and StreamServerActor.
 * 
 * @author Carlos Rueda
 * @version $Id: SimpleWorkflow.java,v 1.23 2007/06/12 16:51:55 crueda Exp $
 */
public class SimpleWorkflow implements IWorkflow {
	private static final String DEFAULT_GS_SPEC = "file:/home/carueda/goesdata/local/local.xml";
	private static final int DEFAULT_PORT = 35813;
	private static final int DEFAULT_SERVER_TIMEOUT = 2000;
	private static final int DEFAULT_CLIENT_TIMEOUT = 0;

	private IWorkflowObserver viewObserver;
	private IServerConnection gscon;

	private List<String> streamNames;

	
	private Map<String, StreamServer> streamServers = new HashMap<String, StreamServer>();
	
	/** Defines the method to dispatch the notification of new images */
	private class ConnectionStreamObserver extends IConnectionObserver.Adapter {
		// no default log messages
		protected void _log(String msg) {}
		
	    /** notifies the new image to the StreamServer corresponding to the channel in the image */
		public void newImage(IImage iimg) {
			iimg.setServerConnection(gscon);
			_notifyNewImage(iimg);
		}
	}
	
	public SimpleWorkflow(String serverSpec, IWorkflowObserver viewObserver,
			int port, int serverTimeout, int clientTimeout) 
	throws Exception {
		assert viewObserver != null;
		this.viewObserver = viewObserver;
		
		gscon = ServerConnections.openConnection(serverSpec);
		
		streamNames = new ArrayList<String>();
		for ( StreamDef streamDef : gscon.getStreamDefs() ) {
			streamNames.add(streamDef.getStreamID());
		}
		
//		start listening for client connections.
		this.viewObserver.executionStarted(this, port, serverTimeout, clientTimeout);		
	}

	
	/**
	 * notifies the new image to the StreamServer corresponding to the channel in the image.
	 * @param iimg
	 */
	private void _notifyNewImage(IImage iimg) {
		String streamName = iimg.getChannelID();
		StreamServer streamServer = streamServers.get(streamName);
		if ( streamServer != null ) {
			boolean active = streamServer.notifyNewImage(iimg);
			if ( !active ) {
				streamServers.remove(streamName);
				gscon.interruptStream(streamName);
			}
		}
	}

	/**
	 * Adds a remote client to the workflow.
	 * If no StreamServer yet exists handling the requested stream, then
	 * one is created and inserted in the workflow.
	 */
	public void addRemoteClient(RemoteClient remoteClient) throws Exception {
		final String streamName = remoteClient.getRequestedStream();
		if ( !streamNames.contains(streamName) ) {
			remoteClient.sendNoSuchStream(streamName);
			return;
		}
		
		// look for a SSA already dispatching the requested stream:
		StreamServer ssa = streamServers.get(streamName);
		if ( ssa == null ) {
			// doesn't exist yet; so, create it:
			System.out.println("CREATING StreamServer");
			ssa = new StreamServer(streamName);
			streamServers.put(streamName, ssa);
		}
		gscon.addObserver(new ConnectionStreamObserver(), streamName);
		gscon.openStream(streamName, null);
		ssa.addRemoteClient(remoteClient);

		System.out.println("ADDED");
	}

	public void removeRemoteClient(RemoteClient remoteClient) {
		System.out.println("removeRemoteClient: " +remoteClient);
		String streamName = remoteClient.getRequestedStream();
		if ( streamName != null ) {
			StreamServer ssa = streamServers.get(streamName);
			if ( ssa != null ) {
				ssa.removeRemoteClient(remoteClient);
			}
			gscon.interruptStream(streamName);
		}
	}

	public ReferenceSpace getReferenceSpace() {
		return gscon.getReferenceSpace();
	}

	public List<StreamDef> getStreamDefs() {
		return new ArrayList<StreamDef>(gscon.getStreamDefs());
	}
	
	public List<String> getStreamIDs() {
		List<String> list = new ArrayList<String>(); 
		for ( StreamDef streamDef : gscon.getStreamDefs() ) {
			list.add(streamDef.getStreamID());
		}
		return list;
	}

	public List<VectorInfo> getVectorInfos() {
		return gscon.getVectors();
	}
	
	public VectorInfo getVectorInfo(String vectorName) {
		return gscon.getVectorInfo(vectorName);
	}

	public String getSavedPattern() {
		return gscon.getSavedPattern();
	}

	public String toString() {
		StringBuffer sb = new StringBuffer();
		sb.append(
				"\nserver='" +gscon.getServerShortDescription()+ "'" + "\n" +
				"reference space: " +gscon.getReferenceSpace()+ "\n" +
				"streams:"+ "\n"
		);
		for ( StreamDef streamDef : getStreamDefs() ) {
			sb.append("    " +streamDef+ "\n");
		}
		sb.append("overviewStreamID: " +gscon.getOverviewStreamDef().getStreamID()+ "\n");
		sb.append("vectors:"+ "\n");
		for ( VectorInfo v :gscon.getVectors() ) {
			sb.append(
					"  vector: name='" +v.getName()+ "'" + "\n"+
				    "           url='" +v.getURL()+ "'" + "\n"
			);
		}
		return sb.toString();
	}
	
	public void incrClients(int incr) {
		System.out.println("    incrClients called with " +incr);
	}

	/**
	 * SimpleWorkflow main program.
	 * 
	 * @param args  
	 * 		--gsSpec &lt;URI of geostreams spec file>
	 * 		--port &lt;server port >
	 * 		--serverTimeout &lt;value>
	 * 		--clientTimeout &lt;value>
	 */
	public static void main(String[] args) throws Exception {
		String serverSpec = DEFAULT_GS_SPEC;
		int port = DEFAULT_PORT;
		int serverTimeout = DEFAULT_SERVER_TIMEOUT;
		int clientTimeout = DEFAULT_CLIENT_TIMEOUT;
		IWorkflowObserver viewObserver = new WorkflowObserver();

		for (int i = 0; i < args.length; i++) {
			String arg = args[i];
			if ( arg.equals("--gsSpec") ) {
				serverSpec = args[++i];
			}
			else if ( arg.equals("--port") ) {
				port = Integer.parseInt(args[++i]);
			}
			else if ( arg.equals("--serverTimeout") ) {
				serverTimeout = Integer.parseInt(args[++i]);
			}
			else if ( arg.equals("--clientTimeout") ) {
				clientTimeout = Integer.parseInt(args[++i]);
			}
		}

		Thread.setDefaultUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
			public void uncaughtException(Thread t, Throwable e) {
				System.out.println("SimpleWorkflow::UncaughtExceptionHandler:\n  " +
						"Thread " +t+ " terminated with exception:");
				e.printStackTrace();
			}
		});
		
		try {
			SimpleWorkflow simpleWorkflow = new SimpleWorkflow(serverSpec, viewObserver, port, serverTimeout, clientTimeout);
			System.out.println(simpleWorkflow);
		}
		catch(ConnectException ex) {
			System.out.println("SimpleWorkflow: error connecting: " +ex.getClass().getCanonicalName()+ ": " +ex.getMessage());
			// no stacktrace
			return;
		}
	}


}

/**
 *  Similar to StreamServerActor, this class can dispatch multiple clients 
 */ 
class StreamServer {
	private final String streamName;
	
	/** List of active clients */
	private ConcurrentLinkedQueue<RemoteClient> _activeClients = new ConcurrentLinkedQueue<RemoteClient>();
	
	/** aux list to deregister dead clients */
	private List<RemoteClient> _deadClients = new ArrayList<RemoteClient>();

	/**
	 * @param streamName
	 */
	StreamServer(String streamName) {
		this.streamName = streamName;
	}
	
	/**
	 * @param remoteClient
	 */
	void removeRemoteClient(RemoteClient remoteClient) {
		_activeClients.remove(remoteClient);
		_printStatus();
	}
	
	/**
	 * @param iimg
	 * @return true iff there are active clients 
	 */
	boolean notifyNewImage(IImage img) {
		if ( _activeClients.size() == 0 ) {
			return false;
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
			_activeClients.removeAll(_deadClients);
			
			_printStatus();
		}
		
		return _activeClients.size() > 0;
	}
	
	/**
	 * @param remoteClient
	 */
	void addRemoteClient(RemoteClient remoteClient) {
		_activeClients.add(remoteClient);
		remoteClient.send("OK-added");
		_printStatus();
	}
	
	private void _printStatus() {
		System.out.println("  :::::::" +this);
	}
	
	public String toString() {
		return "StreamServer(stream: " +streamName+ ")  activeClients = " +_activeClients.size();
	}	
}
