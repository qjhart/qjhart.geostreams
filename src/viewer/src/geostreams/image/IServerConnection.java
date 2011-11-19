/**
 * 
 */
package geostreams.image;

import java.awt.Rectangle;
import java.util.Collection;
import java.util.List;

/**
 * Extracts the actual operations used by ImageStreamViewer, as an attempt to
 * let the viewer work on a simplified server connection object.
 * 
 * @author Carlos Rueda
 * @version $Id: IServerConnection.java,v 1.11 2007/06/03 08:40:28 crueda Exp $
 */
public interface IServerConnection {

	/**
	 * Adds an observer to this connection.
	 * @param obs The observer.
	 * @param streamID The ID of the stream the observer is interested in.
	 */
	public void addObserver(IConnectionObserver obs, String streamID);
	
	/** Opens a stream.
	  * @param streamID Desired stream
	  * @param roi Desired ROI. Can be null.
	  * @throws Exception if the requested stream is invalid. 
	  */
	public void openStream(String streamID, Rectangle roi) 
	throws Exception;
	
	
	/** Interrupts all stream reader threads.
	  */
	public void close();
	
	/** Gets the reference space of all streams coming through this connection. 
	  * Location of all images are relative to this space. 
	  */
	public ReferenceSpace getReferenceSpace();


	/** 
	 * Add a stream definition and status info for the streamID.
	 * 
	 * FIXME gscon-integration: is it OK to let this info be given from the outside, 
	 * and not exclusively by the connection itself?

	 * @param streamID
	 * @param streamDef
	 */
	public void addStreamDef(String streamID, StreamDef streamDef);

	/**
	 * Gets info about the available streams.
	 * 
	 * @return info about the available streams.
	 */
	public Collection<StreamDef> getStreamDefs();

	/** Gets a StreamDef */
	public StreamDef getStreamDef(String streamID);
	
	/**
	 *  convenience: Gets the channelDef of a given streamID
	 * @param streamID Stream ID
	 * @return
	 */
	public ChannelDef getChannelDefForStream(String streamID);

	public String getSavedPattern();
	
	/** 
	 * @param streamID Desired stream
	 * @return the URL of the latest image of a stream 
	 */
	public String getSavedURL(String streamID);

	/** Return the status of a stream.
	 * @return status of a stream. Could be null if no info has been received for
	 * the given streamID.
	 */
	public StreamStatus getStreamStatus(String streamID);
	
	/**
	 * 
	 * @param streamID
	 * @param frameID
	 * @return
	 */
	public FrameDef getFrameDef(String streamID, String frameID);

	/**
	 * @param streamID
	 * @param frameID
	 * @return null if the stream is invalid or if the frame ID has never been received
	 * either in a frame definition or in an image.
	 */
	public FrameStatus getFrameStatus(String streamID, String frameID);

	/**
	 * Returns the frame definitions associated to a stream.
	 * @param streamID
	 * @return
	 */
	public Collection<FrameDef> getFrameDefs(String streamID);


	/** Gets the status of the streams the server will ever generate */
	public Collection<StreamStatus> getStreamStatuses();


	/**
	 * @return
	 */
	public Collection<FrameStatus> getFrameStatuses(String streamID);

	/**
	 * @return a short description of the connection properties
	 * to the server, typically a host name and, optionally, a port.
	 */
	public String getServerShortDescription();

	/**
	 * @return
	 */
	public List<VectorInfo> getVectors();
	
	public VectorInfo getVectorInfo(String vectorName);

	/**
	 * @param timeout
	 */
	public void setTimeout(int timeout);

	/**
	 * @param simulationDelay
	 */
	public void setSimulationDelay(int simulationDelay);

	/** Is a given stream reader running?
	  */
	public boolean isStreamRunning(String streamID);

	/**
	 * TODO should be named start()
	 */
	public void startStreamReaders() throws Exception;

	/** Returns the StreamDef of the stream used for overview (nodata). 
	  * null if not specified. */
	public StreamDef getOverviewStreamDef();

	/** 
	 * Interrupts reading a stream. 
	 */
	public void interruptStream(String streamID);
}