package geostreams.image;


import geostreams.util.Utils;

import java.awt.Rectangle;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


/**
 * Manages a connection to a GeoStreams repository or a net-enabled workflow
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: ServerConnection.java,v 1.14 2007/06/27 02:43:56 crueda Exp $
 */
class ServerConnection extends BasicServerConnection {

	private static abstract class StreamOpener {
		abstract BasicImageStream openStream(String streamID, Rectangle roi, boolean nodata)
		throws Exception;
	}
	private static class RepositoryStreamOpener extends StreamOpener {
		private String serverName;
		RepositoryStreamOpener(String serverName) {
			this.serverName = serverName;
		}
		BasicImageStream openStream(String streamID, Rectangle roi, boolean nodata) throws Exception {
//			Utils.log("RepositoryStreamOpener.openStream: streamID=" +streamID);
			return ImageStreams.openStream(serverName, streamID, roi, nodata);	
		}
	}
	private class NWorkflowStreamOpener extends StreamOpener {
		ClientWorkflowImageStreams cwiss;
		NWorkflowStreamOpener(String server, int port) throws Exception {
			cwiss = ClientWorkflowImageStreams.getInstance(server, port);
			vectors.addAll(cwiss.getVectors());
			getOverviewStreamID(cwiss.getOverviewStreamID());
			
			setStreamIDs(cwiss.getStreamIDs());
			setStreamDefs(cwiss.getStreamDefs());
			referenceSpace = cwiss.getReferenceSpace();
			
			setSavedPattern(cwiss.getSavedPattern());
		}
		BasicImageStream openStream(String streamID, Rectangle roi, boolean nodata) throws Exception {
//			Utils.log("NWorkflowStreamOpener.openStream: streamID=" +streamID);
			return cwiss.openStream(streamID, roi, nodata);	
		}
	}


	private StreamOpener streamOpener;
	
	private String serverShortDescription;
	private String overviewStreamID;

	/** streamId->StreamReaderThread */
	private Map<String,StreamReaderThread> streamReaders = new HashMap<String,StreamReaderThread>();
	
	
	
	/** 
	 * Opens a connection to GeoStreams repository
	 * Package private constructor called by ServerConnections.
	 * 
	 * @param serverName
	 * @param referenceSpace
	 * @param vectors
	 */
	ServerConnection(String serverName, ReferenceSpace referenceSpace, List<VectorInfo> vectors) {
		this.serverShortDescription = serverName;
		this.referenceSpace = referenceSpace;
		this.vectors.addAll(vectors);
		streamOpener = new RepositoryStreamOpener(serverName);
	}
	
	/** 
	 * Opens a connection to a net-enabled workflow.
	 * Package private constructor called by ServerConnections.
	 * 
	 * @param server
	 * @param port
	 * @throws Exception 
	 */
	ServerConnection(String server, int port) throws Exception {
		this.serverShortDescription = server+ ":" +port;
		streamOpener = new NWorkflowStreamOpener(server, port);
	}
	
	
	/** Sets the timeout in seconds */
	public void setTimeout(int timeout) {
		for (StreamReaderThread streamReader : streamReaders.values()) {
			streamReader.setTimeout(timeout);
		}
	}
	public void setSimulationDelay(int simulationDelay) {
		for (StreamReaderThread streamReader : streamReaders.values()) {
			streamReader.setSimulationDelay(simulationDelay);
		}
	}
	
	
	/**
	 * Starts the reading of the open streams.
	 */
	public void startStreamReaders() throws Exception {
		openOverviewStream();
		for (StreamReaderThread streamReader : streamReaders.values()) {
			streamReader.start();
		}
	}
	
	public void close() {
		for (String streamID : streamReaders.keySet()) {
			interruptStream(streamID);
		}
	}

	public void interruptStream(String streamID) {
		_removeObservers(streamID);
		StreamReaderThread streamReader = streamReaders.get(streamID);
		if (streamReader != null) {
			streamReader.interrupt();
		}
	}

	public boolean isStreamRunning(String streamID) {
		StreamReaderThread streamReader = streamReaders.get(streamID);
		return streamReader != null && streamReader.isAlive() && !streamReader.isInterrupted();
	}

	
	/** Gets the name of the GeoStreams server. */
	public String getServerShortDescription() {
		return serverShortDescription;
	}
	
	public StreamDef getOverviewStreamDef() {
		if ( overviewStreamID == null ) {
			Utils.log("ServerConnection: overviewStreamID undefined");
			return null;
		}
		StreamDef ocd = getStreamDef(overviewStreamID);
		assert ocd != null;
		return ocd;
	}
	
	
	private StreamReaderThread overviewStreamReader;
	
	
	/** Opens the stream used for overview (nodata). 
	  * @return false if an overview stream is not specified for this connection. 
	  */
	private boolean openOverviewStream()
	throws Exception {
		if (overviewStreamReader != null ) {
			return true;
		}
		if ( overviewStreamID == null ) {
			Utils.log("ServerConnection: openOverviewStream: no overviewStreamID");
			return false;
		}
		BasicImageStream imgStream = streamOpener.openStream(overviewStreamID, null, true);
		imgStream.setImageStreamObserver(this);
		overviewStreamReader = new StreamReaderThread(this, imgStream);
		streamReaders.put(overviewStreamID, overviewStreamReader);
		Utils.log("ServerConnection: openOverviewStream: done " +imgStream);
		return true;
	}
	
	
	
	public void openStream(String streamID, Rectangle roi) 
	throws Exception {
		
		// FIXME: mechanism to get the stream and its associated channelDef ...
		
//		ChannelDef channelDef = getChannelDef(channelID);
//		if ( channelDef == null ) {
//			throw new Exception("ServerConnection: channelDef not found for channel ID: " +channelID);
//		}
		
		BasicImageStream imgStream = streamOpener.openStream(streamID, roi, false);
//			ImageStreams.openStream(serverShortDescription, channelID, roi, false);
		if ( imgStream != null ) {
			imgStream.setImageStreamObserver(this);
			StreamReaderThread streamReader = new StreamReaderThread(this, imgStream);
			streamReaders.put(streamID, streamReader);
			streamReader.start();
		}
	}

		

	void getOverviewStreamID(String overviewStreamID) {
		this.overviewStreamID = overviewStreamID;
	}

	public String toString() {
		return "[ServerConnection: " +serverShortDescription+ "]";
	}
}


