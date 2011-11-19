package geostreams.ptolemy;

import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.image.ReferenceSpace;
import geostreams.image.StreamDef;
import geostreams.image.VectorInfo;
import geostreams.ptolemy.actor.ImageStreamReaderActor;
import geostreams.workflow.IWorkflow;
import geostreams.workflow.IWorkflowObserver;
import geostreams.workflow.RemoteClient;

import java.io.IOException;
import java.lang.reflect.Constructor;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentLinkedQueue;

import ptolemy.actor.CompositeActor;
import ptolemy.actor.ExecutionListener;
import ptolemy.actor.IOPort;
import ptolemy.actor.Manager;
import ptolemy.actor.PortEvent;
import ptolemy.actor.PortEventListener;
import ptolemy.data.ObjectToken;
import ptolemy.data.Token;
import ptolemy.data.expr.StringParameter;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.ChangeRequest;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * deprecated Use {@link NetInterface}
 *
 * An implementation of IWorkflow to interact with the view of a ptolemy model.
 * 
 * @author Carlos Rueda
 * @version $Id: Workflow.java,v 1.22 2007/09/09 08:19:55 crueda Exp $
 */
public class Workflow extends AbstractWorkflow implements IWorkflow {
	
	private static final String DEFAULT_VIEW_OBSERVER = "geostreams.workflow.WorkflowObserver";

	private static final String DEFAULT_PORT = "35813";

	private static final String DEFAULT_SERVER_TIMEOUT = "2000";

	private static final String DEFAULT_CLIENT_TIMEOUT = "0";

	/** Class name of IWorkflowObserver implementation */
	public StringParameter viewObserverParam;    
	
	/** Port to listen to */
	public StringParameter portParam;            
	
	/** Server timeout */
	public StringParameter serverTimeoutParam;            
	
	/** Client timeout */
	public StringParameter clientTimeoutParam ;


	private boolean executionListenerRegistered;
	
	private IWorkflowObserver viewObserver;
	
	private int numberRemoteClients;


	private ExecutionListener executionListener = new ExecutionListener() {
		private void log(String msg) {
//			System.out.println("         " +msg);
		}

		public void executionError(Manager manager, Throwable throwable) {
			log(">>>>>>>>>>Workflow executionError: " +throwable+ "<<<<<<<<<<<<<<<");
			throwable.printStackTrace();
			_executionFinished(true);
		}

		public void executionFinished(Manager manager) {
			Manager.State state = manager.getState();
			log(">>>>>>>>>>Workflow executionFinished: " +state+ "<<<<<<<<<<<<<<<");
			_executionFinished(true);
		}

		public void managerStateChanged(Manager manager) {
			Manager.State state = manager.getState();
			log(">>>>>>>>>>Workflow managerStateChanged: " +state+ "<<<<<<<<<<<<<<<");
			if ( state == Manager.PREINITIALIZING ) {
				_icon.update(true, 0, 0);
			}
			else if ( state == Manager.PAUSED ) {
				_executionPaused();
			}
			else if ( state == Manager.IDLE ) {
				_executionFinished(true);
			}
		}
	};            
	
	public Workflow(CompositeEntity container, String name) throws IllegalActionException, NameDuplicationException {
		super(container, name);

		//
		// parameters
		//
		viewObserverParam = new StringParameter(this, "viewObserverParam");
		viewObserverParam.setExpression(DEFAULT_VIEW_OBSERVER);
		viewObserverParam.setDisplayName("View observer class");

        portParam = new StringParameter(this, "portParam");
        portParam.setExpression(DEFAULT_PORT);
        portParam.setDisplayName("Run server on port");
        
        serverTimeoutParam = new StringParameter(this, "serverTimeoutParam");
        serverTimeoutParam.setExpression(DEFAULT_SERVER_TIMEOUT);
        serverTimeoutParam.setDisplayName("Server timeout");
        
        clientTimeoutParam =  new StringParameter(this, "clientTimeoutParam");
        clientTimeoutParam .setExpression(DEFAULT_CLIENT_TIMEOUT);
        clientTimeoutParam .setDisplayName("Client timeout");
	}
    
	public void initialize() throws IllegalActionException {
		log("============ Workflow.initialize() +++++++++++++++++++++++++");
        super.initialize();
        _init();
    }
    
	private void _init() throws IllegalActionException {
		
		try {
			// allow some time for other components to initialize (assuming we are under PN)
			Thread.sleep(2000);
		}
		catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    	
       	if ( viewObserver == null ) {
       		
       		String viewObserverClassName = viewObserverParam.stringValue();
       		try {
       			Constructor<?> constr = Class.forName(viewObserverClassName).getConstructor(new Class[0]);
       			viewObserver = (IWorkflowObserver) constr.newInstance(new Object[0]);
       		}
       		catch(Throwable ex) {
       			ex.printStackTrace();
       			throw new IllegalActionException(ex.getMessage());
       		}
       		
	        CompositeActor topLevel = (CompositeActor)getContainer();
	        Manager manager = topLevel.getManager();
	    	if ( manager != null ) {
	    		if ( !executionListenerRegistered ) {
		        	manager.addExecutionListener(executionListener);
		        	log("-----addExecutionListener");
		        	executionListenerRegistered = true;
	    		}
	    	}
	    	else {
	    		log("NO MANAGER GET");
	    	}
			int port = Integer.parseInt(portParam.stringValue());
			int serverTimeout = Integer.parseInt(serverTimeoutParam.stringValue());
			int clientTimeout = Integer.parseInt(clientTimeoutParam .stringValue());
        	try {
        		viewObserver.executionStarted(this, port, serverTimeout, clientTimeout);
			}
			catch (Exception ex) {
				System.out.println("Workflow.validate: Exception: " +ex.getMessage());
				ex.printStackTrace();
			}
			_icon.update(true, 0, 0);
    	}
	}
	
    /** If this component is removed from the model, finish execution */
    public void setContainer(CompositeEntity container) throws IllegalActionException, NameDuplicationException {
		try {
			if (container == null) {
				_executionFinished(false);
			}
		}
		finally {
			super.setContainer(container);
		}
	}

	private synchronized void _executionPaused() {
		if ( viewObserver != null ) {
			viewObserver.executionPaused();
		}
    }

	/** Gets the SS instances in the model. */
	private List<StreamServerActor> _getStreamServers() {
		return PtUtils.getStreamServerActors((CompositeEntity) getContainer());
	}
	
	/**
	 * Finishes the execution of all NI-related components.
	 * @param updateGui If true, the GUI of this view is updated.
	 */
	private synchronized void _executionFinished(boolean updateGui) {
		if ( viewObserver == null ) {
			return;
		}
		try {
			viewObserver.executionFinished();
			List<StreamServerActor> current_ssas = _getStreamServers();
			if ( current_ssas.size() > 0 ) {
				// get a copy of current SS actors:
				final List<StreamServerActor> ssas = new ArrayList<StreamServerActor>(current_ssas);

				for ( StreamServerActor ssa : ssas ) {
					try {
						ssa.setContainer(null);
					}
					catch (NameDuplicationException ex) {
						// should not happen, because of the null container argument
						ex.printStackTrace();
					}
					catch(Throwable thr) {
						System.out.println("UNEXPECTED THROWABLE: " +thr);
					}
				}

				for ( StreamServerActor ssa : ssas ) {
					try {
						// interrupt all current SS actors:
						ssa.closeClients();
					}
					catch(Throwable thr) {
						System.out.println("UNEXPECTED THROWABLE: " +thr);
					}
				}
			}

			if ( updateGui ) {
				_icon.update(false, 0, 0);
			}
		}
		finally {
			viewObserver = null;
		}
	}
	

	private StreamServerActor _getStreamServer(String ssaName) {
		return PtUtils.getStreamServerActor((CompositeEntity) getContainer(), ssaName);
	}

	void _requestChange(ChangeRequest changeRequest) {
		toplevel().requestChange(changeRequest);
	}

	void streamServerActorIdle(final StreamServerActor ssa) {
		System.out.println("\n---\n---streamServerActorIdle CALLED but IGNORED\n---");
		if ( false ) {
    	ChangeRequest request = new ChangeRequest(this, "Releasing SS actor") {
    		protected void _execute() throws Exception {
    			// remove ssa from its container
    			try {
    				System.out.println("\n---\n---releasing SS actor\n---");
    				ssa.setContainer(null);
    			}
    			catch( Exception ex ) {
    				System.out.println("streamServerActorIdle: " +ex.getMessage()+ ": ssa.setContainer(null);");
    				ex.printStackTrace();
    			}
    		}
    	};
    	_requestChange(request);
		}
	}

	private IOPort _getOutputStreamPort(String streamName) {
		return PtUtils.getOutputStream((CompositeEntity) getContainer(), streamName);
	}
	
	private static String _getStreamServerName(String streamName) {
		return "SS_" +streamName.replace('.', '_');
	}
	
	private StreamServerActor _getStreamServerForStream(String streamName) {
		String ssa_name = _getStreamServerName(streamName);
		return  _getStreamServer(ssa_name);
	}
	
	
	public void addRemoteClient(final RemoteClient remoteClient) throws IOException {
		if ( true ) {
			addRemoteClient2(remoteClient);
		}
		else {
			addRemoteClient1(remoteClient);
		}
	}
	
	public void removeRemoteClient(RemoteClient remoteClient) {
		if ( true ) {
			removeRemoteClient2(remoteClient);
		}
		else {
			removeRemoteClient1(remoteClient);
		}
	}
	
	/**
	 * Adds a remote client to the workflow.
	 * If no StreamServerActor yet exists handling the requested stream, then
	 * one is created and inserted in the workflow
	 */
	private void addRemoteClient1(final RemoteClient remoteClient) throws IOException {
		final String streamName = remoteClient.getRequestedStream();
		System.out.println("\n---\n---Workflow.addRemoteClient: stream: " +streamName+ "\n---");
		IOPort outport = _getOutputStreamPort(streamName);
		if ( outport == null ) {
			remoteClient.sendNoSuchStream(streamName);
			return;
		}
		
		// look for a SSA already dispatching the requested stream:
		final String ssa_name = _getStreamServerName(streamName);
		final StreamServerActor previous_ssa = _getStreamServer(ssa_name);
		if ( previous_ssa == null ) {
			// doesn't exist yet; so, create it and insert it in the container:
			final CompositeEntity container = (CompositeEntity) getContainer();
			container.requestChange(new ChangeRequest(this, "Connecting a new SS to the requested stream") {
				protected void _execute() throws Exception {
					try {
						StreamServerActor ssa = new StreamServerActor(container, ssa_name);
						ssa.setWorkflow(Workflow.this);
						IOPort outport = _getOutputStreamPort(streamName);
						container.connect(outport, ssa.input);
						ssa.addRemoteClient(remoteClient);
					}
					catch(Exception ex) {
						remoteClient.send("ChangeRequest: error: " +ex.getMessage());
						throw ex;
					}
				}
			});
		}
		else {
			previous_ssa.addRemoteClient(remoteClient);
		}
		_icon.update(true, ++numberRemoteClients, streamServers.size());
	}
	
	public void removeRemoteClient1(RemoteClient remoteClient) {
		String streamName = remoteClient.getRequestedStream();
		if ( streamName != null ) {
			StreamServerActor ssa = _getStreamServerForStream(streamName);
			if ( ssa != null ) {
				ssa.removeRemoteClient(remoteClient);
				_icon.update(true, --numberRemoteClients, streamServers.size());
			}
		}
	}

	
	public List<StreamDef> getStreamDefs() {
		List<StreamDef> list = new ArrayList<StreamDef>();
		for ( IStreamActor streamActor : PtUtils.getStreamActors((CompositeEntity) getContainer()) ) {
			List<StreamDef> streamDefs = streamActor.getStreamDefs();
			if ( streamDefs != null ) {
				list.addAll(streamDefs);
			}
		}
		return list;
	}

	public boolean existsStream(final String streamName) {
		IOPort outport = _getOutputStreamPort(streamName);
		return outport != null;
	}
	
	/**
	 * @return all IServerConnection associated to actors in the model.
	 */
	private Set<IServerConnection> _getServerConnection() {
		Set<IServerConnection> gscons = new LinkedHashSet<IServerConnection>();
		for ( IStreamActor streamActor : PtUtils.getStreamActors((CompositeEntity) getContainer()) ) {
			IServerConnection gscon = streamActor.getServerConnection();
			gscons.add(gscon);
		}
		return gscons;
	}


	/**
	 * @return the list of vectors from all ImageStreamReaderActor instances in the model.
	 */
	public List<VectorInfo> getVectorInfos() {
		// first, collect the gscons (no repetition):
		// now obtain the vectors from the gscons:
		List<VectorInfo> list = new ArrayList<VectorInfo>();
		for ( IServerConnection gscon : _getServerConnection() ) {
			List<VectorInfo> vectors = gscon.getVectors();
			list.addAll(vectors);
		}
		return list;
	}

	public VectorInfo getVectorInfo(String vectorName) {
		List<VectorInfo> vectors = getVectorInfos();
		for ( VectorInfo vectorInfo : vectors) {
			if ( vectorName.equals(vectorInfo.getName()) ) {
				return vectorInfo;
			}
		}
		return null;
	}
	
	public ReferenceSpace getReferenceSpace() {
		ReferenceSpace refSpace = null;
		for ( ImageStreamReaderActor actor : PtUtils.getImageStreamReaderActors((CompositeEntity) getContainer()) ) {
			IServerConnection gscon = actor.getServerConnection();
			ReferenceSpace refSpace2 = gscon.getReferenceSpace();
			if ( refSpace == null ) {
				refSpace = refSpace2;
			}
			else {
				// we assume all references spaces are equal.
				// print a warning if not:
				if ( !refSpace.equals(refSpace2) ) {
					System.out.println("getReferenceSpace: " +
							"WARNING: Different reference spaces are associated. " +
							"Will use one arbitrarily chosen.");
				}
			}
		}
		return refSpace;
	}


	/**
	 * @return the saved pattern from the first server connection (arbitrarily)
	 */
	public String getSavedPattern() {
		String savedPattern = null;
		for ( IStreamActor streamActor : PtUtils.getStreamActors((CompositeEntity) getContainer()) ) {
			IServerConnection gscon = streamActor.getServerConnection();
			savedPattern = gscon.getSavedPattern();
			if ( savedPattern != null ) {
				break;
			}
		}
		return savedPattern;
	}
	
	
	public void incrClients(int incr) {
		numberRemoteClients = Math.max(0, numberRemoteClients +incr);
		_icon.update(true, numberRemoteClients, streamServers.size());
	}
	
	/** streamName -&gt; MyStreamServer mapping */
	private Map<String, MyStreamServer> streamServers = new HashMap<String, MyStreamServer>();
	
	/**
	 * Adds a remote client to the workflow.
	 * If no MyStreamServer yet exists handling the requested stream, then
	 * one is created and put to listen on the corresponding port.
	 */
	public void addRemoteClient2(RemoteClient remoteClient) throws IOException {
		String streamName = remoteClient.getRequestedStream();
		System.out.println("\n---\n---Workflow.addRemoteClient2: stream: " +streamName+ "\n---");
		IOPort outport = _getOutputStreamPort(streamName);
		if ( outport == null ) {
			remoteClient.sendNoSuchStream(streamName);
			return;
		}
		
		synchronized ( streamServers ) {
			MyStreamServer streamServer = streamServers.get(streamName);
			if ( streamServer == null ) {
				// doesn't exist yet:
				streamServer = new MyStreamServer(streamName, outport);
				streamServer.setWorkflow(this);
				streamServer.addRemoteClient(remoteClient);
				streamServers.put(streamName, streamServer);
			}
			else {
				streamServer.addRemoteClient(remoteClient);
			}
		}
		_icon.update(true, ++numberRemoteClients, streamServers.size());
	}	

	public void removeRemoteClient2(RemoteClient remoteClient) {
		String streamName = remoteClient.getRequestedStream();
		if ( streamName != null ) {
			MyStreamServer streamServer = streamServers.get(streamName);
			if ( streamServer != null ) {
				streamServer.removeRemoteClient(remoteClient);
				_icon.update(true, --numberRemoteClients, streamServers.size());
			}
		}
	}

	/** 
	 * Removes the listener (stream server) from the port. 
	 * This is called in MyStreamServer.tokenSentEvent, so the removal
	 * of the listener from the port must be done in a different thread
	 * to avoid a concurrent-modification exception.
	 * 
	 * NOTE: However, IOPort._tokenSent is buggy: it throws ConcurrentModificationExceptions.
	 * Workaround: nothing is done in this method for now -- the listener is NOT removed. 
	 */
	void streamServerIdle(final MyStreamServer streamServer) {
		System.out.println("\n---\n---streamServerIdle: Nothing done -- bug in Ptolemy's IOPort._tokenSent");
		if ( false ) { // TODO when _tokenSent gets fixed.
		Runnable run = new Runnable() {
			public void run() {
				String streamName = streamServer.getStreamName();
				synchronized ( streamServers ) {
					streamServers.remove(streamName);
				}
				IOPort port = streamServer.getPort();
				port.removePortEventListener(streamServer);
//                port.removeTokenSentListener(streamServer);
			}
		};
		new Thread(run).start();
		}
	}

	static class MyStreamServer implements PortEventListener { //TokenSentListener {
		
		private String streamName;
		private IOPort port;

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
	    public MyStreamServer(String streamName, IOPort port) {
	    	this.streamName = streamName;
	    	this.port = port;
            if ( port.isOutput() ) {
                port.addPortEventListener(this);
//                port.addTokenSentListener(this);
            }
			System.out.println("MyStreamServer for stream '" +this.streamName+ "' created.");
	    }
		
	    private void _updateIcon(final int numberOfClients) {
			// TODO updateIcon
	    }

	    void setWorkflow(Workflow view) {
			this.workflow = view;
		}

		void addRemoteClient(RemoteClient remoteClient) throws IOException {
			System.out.println("\n---\n---MyStreamServer.addRemoteClient: stream: " +remoteClient.getRequestedStream()+ "\n---");
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
			System.out.println("(((((((( MyStreamServer#removeRemoteClient: " +remoteClient);
			_activeClients.remove(remoteClient);
			_printStatus();
		}

		/** Sends the image in the event to active clients. 
		 */
//		public void tokenSentEvent(TokenSentEvent event) {
        public void portEvent(PortEvent event) {
	        if ( _activeClients.size() == 0 ) {
				return;
			}
	        Token token = event.getToken();
			
			ObjectToken objToken = (ObjectToken) token;
			IImage img = (IImage) objToken.getValue();
			
//			System.out.println(this+ "--[[ tokenSentEvent: " +getStreamName()+ "]]--" +
//					" img.isNull() = " +img.isNull());
			
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
					System.out.println("MyStreamServer: client is no longer active");
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
			
			// No clients left active?, then notify workflow
			if ( _activeClients.size() == 0 ) {
				workflow.streamServerIdle(this);
			}
		}

		public IOPort getPort() {
			return port;
		}

		public String getStreamName() {
			return streamName;
		}
	}

	// null-implementation just to get rid of the compilation error
	public List<String> getStreamIDs() {
		// TODO Auto-generated method stub
		return null;
	}

}
