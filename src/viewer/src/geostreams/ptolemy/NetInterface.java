package geostreams.ptolemy;

import geostreams.image.ChannelDef;
import geostreams.image.IServerConnection;
import geostreams.image.IStream;
import geostreams.image.IStreamGenerator;
import geostreams.image.ReferenceSpace;
import geostreams.image.StreamDef;
import geostreams.image.VectorInfo;
import geostreams.ptolemy.icon.NetInterfaceIcon;
import geostreams.util.Utils;
import geostreams.workflow.IWorkflow;
import geostreams.workflow.IWorkflowObserver;
import geostreams.workflow.RemoteClient;

import java.awt.Color;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import ptolemy.actor.CompositeActor;
import ptolemy.actor.ExecutionListener;
import ptolemy.actor.IOPort;
import ptolemy.actor.Manager;
import ptolemy.actor.TypedAtomicActor;
import ptolemy.data.expr.StringParameter;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.ChangeRequest;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * An implementation of IWorkflow to interact with the view of a ptolemy model.
 *
 * @author Carlos Rueda
 * @version $Id: NetInterface.java,v 1.7 2007/09/09 08:10:21 crueda Exp $
 */
public class NetInterface extends TypedAtomicActor implements IWorkflow {
	
	private static final String DEFAULT_VIEW_OBSERVER = "geostreams.workflow.WorkflowObserver";
    
    private static final String DEFAULT_REGISTRY = "http://geostreams.ucdavis.edu/gisp-registry/";

	private static final String DEFAULT_PORT = "35813";

	private static final String DEFAULT_SERVER_TIMEOUT = "2000";

	private static final String DEFAULT_CLIENT_TIMEOUT = "0";
	
	protected NetInterfaceIcon _icon;

	/** Class name of IWorkflowObserver implementation */
	public StringParameter viewObserverParam = null;    
	
    /** Registry - TODO not used yet*/
    public StringParameter registryParam;
    
    /** Streams to publish - TODO not used yet*/
    public StringParameter streamsToPublishParam;    
    
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
//			Utils.log("         " +msg);
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
	
	public NetInterface(CompositeEntity container, String name) throws IllegalActionException, NameDuplicationException {
		super(container, name);
		
		_icon = new NetInterfaceIcon(this, "_icon");
		_icon.setBackgroundColor(new Color(0x68e5e7)); //Color.YELLOW);

		//
		// parameters
		//
//		viewObserverParam = new StringParameter(this, "viewObserverParam");
//		viewObserverParam.setExpression(DEFAULT_VIEW_OBSERVER);
//		viewObserverParam.setDisplayName("View observer class");

        registryParam = new StringParameter(this, "registryParam");
        registryParam.setExpression(DEFAULT_REGISTRY);
        registryParam.setDisplayName("Registry");
        
        streamsToPublishParam = new StringParameter(this, "streamsToPublishParam");
        streamsToPublishParam.setExpression("ALL");
        streamsToPublishParam.setDisplayName("Publish streams");

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
		Utils.log("============ Workflow.initialize() +++++++++++++++++++++++++");
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
       		
       	    String viewObserverClassName = DEFAULT_VIEW_OBSERVER;
            if ( viewObserverParam != null ) {
                viewObserverClassName = viewObserverParam.stringValue();
            }
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
		        	Utils.log("-----addExecutionListener");
		        	executionListenerRegistered = true;
	    		}
	    	}
	    	else {
	    		Utils.log("NO MANAGER GET");
	    	}
			int port = Integer.parseInt(portParam.stringValue());
			int serverTimeout = Integer.parseInt(serverTimeoutParam.stringValue());
			int clientTimeout = Integer.parseInt(clientTimeoutParam .stringValue());
        	try {
        		viewObserver.executionStarted(this, port, serverTimeout, clientTimeout);
			}
			catch (Exception ex) {
				Utils.log("Workflow.validate: Exception: " +ex.getMessage());
				ex.printStackTrace();
			}
			_icon.update(true, 0, 0);
    	}
	}
	
    public void wrapup() throws IllegalActionException {
    	super.wrapup();
    	Utils.log("NetInterface#wrapup");
    	_executionFinished(true);
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
			if ( updateGui ) {
				_icon.update(false, 0, 0);
			}
		}
		finally {
			viewObserver = null;
		}
	}
	

	void _requestChange(ChangeRequest changeRequest) {
		toplevel().requestChange(changeRequest);
	}

	private IOPort _getOutputStreamPort(String streamName) {
		return PtUtils.getOutputStream((CompositeEntity) getContainer(), streamName);
	}
	
	public void addRemoteClient(final RemoteClient remoteClient) throws IOException {
		addRemoteClient2(remoteClient);
	}
	
	public void removeRemoteClient(RemoteClient remoteClient) {
		removeRemoteClient2(remoteClient);
	}
	
	public List<String> getStreamIDs() {
		List<String> list = new ArrayList<String>();
		List<IStreamGenerator> streamGenerators = PtUtils.getStreamGenerators((CompositeEntity) getContainer());
		for ( IStreamGenerator streamActor : streamGenerators ) {
			IStream istream = streamActor.getGeneratedStream();
			if ( istream != null ) {
				list.add(istream.getStreamID());
			}
		}
		Utils.log("NetInterface.getStreamIDs: " +list);
		return list;
	}

	public List<StreamDef> getStreamDefs() {
		List<StreamDef> list = new ArrayList<StreamDef>();
		
		List<IStreamGenerator> streamGenerators = PtUtils.getStreamGenerators((CompositeEntity) getContainer());
		if ( streamGenerators.size() > 0 ) {
			for ( IStreamGenerator streamActor : streamGenerators ) {
				IStream istream = streamActor.getGeneratedStream();
				if ( istream != null ) {
					ChannelDef channelDef = istream.getChannelDef();
					list.add(new StreamDef(istream.getStreamID(), channelDef));
				}
			}
		}
		else {
			for ( IStreamActor streamActor : PtUtils.getStreamActors((CompositeEntity) getContainer()) ) {
				List<StreamDef> streamDefs = streamActor.getStreamDefs();
				if ( streamDefs != null ) {
					list.addAll(streamDefs);
				}
			}
		}
		Utils.log("NetInterface.getStreamDefs: " +list);
		return list;
	}

	public boolean existsStream(final String streamName) {
		IOPort outport = _getOutputStreamPort(streamName);
		return outport != null;
	}
	
	/**
	 * @return the list of vectors from all IStreamGenerators instances in the model.
	 */
	public List<VectorInfo> getVectorInfos() {
		List<VectorInfo> list = new ArrayList<VectorInfo>();
		for ( IStreamGenerator sg : PtUtils.getStreamGenerators((CompositeEntity) getContainer()) ) {
			IStream istream = sg.getGeneratedStream();
			if ( istream != null ) {
				Utils.log(istream+ ": istream.getVectors() = " +istream.getVectors());
				list.addAll(istream.getVectors());
			}
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
		for ( IStreamGenerator sg : PtUtils.getStreamGenerators((CompositeEntity) getContainer()) ) {
			IStream istream = sg.getGeneratedStream();
			if ( istream == null ) {
				// the generator hasn't yet know about the generated stream.
				// Skip.
				continue;
			}
			ReferenceSpace refSpace2 = istream.getReferenceSpace();
			if ( refSpace == null ) {
				refSpace = refSpace2;
			}
			else {
				// we assume all references spaces are equal.
				// print a warning if not:
				if ( !refSpace.equals(refSpace2) ) {
					Utils.log("getReferenceSpace: " +
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
	
	/** streamName -&gt; StreamServer mapping */
	private Map<String, StreamServer> streamServers = new HashMap<String, StreamServer>();
	
	/**
	 * Adds a remote client to the workflow.
	 * If no StreamServer yet exists handling the requested stream, then
	 * one is created and put to listen on the corresponding port.
	 */
	public void addRemoteClient2(RemoteClient remoteClient) throws IOException {
		String streamName = remoteClient.getRequestedStream();
		Utils.log("\n---\n---NetInterface.addRemoteClient2: stream: " +streamName+ "\n---");
		IOPort outport = _getOutputStreamPort(streamName);
		if ( outport == null ) {
			remoteClient.sendNoSuchStream(streamName);
			return;
		}
		
		synchronized ( streamServers ) {
			StreamServer streamServer = streamServers.get(streamName);
			if ( streamServer == null ) {
				// doesn't exist yet:
				streamServer = new StreamServer(streamName, outport);
				streamServer.setNetInterface(this);
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
			StreamServer streamServer = streamServers.get(streamName);
			if ( streamServer != null ) {
				streamServer.removeRemoteClient(remoteClient);
				_icon.update(true, --numberRemoteClients, streamServers.size());
			}
		}
	}

	/** 
	 * Removes the listener (stream server) from the port. 
	 * This is called in StreamReader.tokenSentEvent, so the removal
	 * of the listener from the port must be done in a different thread
	 * to avoid a concurrent-modification exception.
	 * 
	 * NOTE: However, IOPort._tokenSent is buggy: it throws ConcurrentModificationExceptions.
	 * Workaround: nothing is done in this method for now -- the listener is NOT removed. 
	 */
	void streamServerIdle(final StreamServer streamServer) {
		Utils.log("\n---\n---streamServerIdle: Nothing done -- bug in Ptolemy's IOPort._tokenSent");
		if ( false ) { // TODO when _tokenSent gets fixed.
		Runnable run = new Runnable() {
			public void run() {
				String streamName = streamServer.getStreamName();
				synchronized ( streamServers ) {
					streamServers.remove(streamName);
				}
				IOPort port = streamServer.getPort();
//				port.removeTokenSentListener(streamServer);
                port.removePortEventListener(streamServer);
			}
		};
		new Thread(run).start();
		}
	}


}
