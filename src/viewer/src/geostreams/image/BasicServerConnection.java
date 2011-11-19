package geostreams.image;

import geostreams.util.Utils;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentLinkedQueue;


/**
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: BasicServerConnection.java,v 1.17 2007/06/12 16:51:55 crueda Exp $
 */
public abstract class BasicServerConnection 
implements IServerConnection, IImageStream.IObserver {
	protected ReferenceSpace referenceSpace;

	
	/** streamID->StreamDef */
	protected Map<String,StreamDef> streamDefs;

//	/** frameID->FrameDef */
//	protected Map<String,FrameDef> frameDefs;
	/** streamID->(frameID->FrameDef) */
	private Map<String,Map<String,FrameDef>> frameDefs;

//	/** streamID+frameID->FrameStatus */
//	protected Map<String, FrameStatus> frameStatusMap;
	/** streamID->(frameID->FrameStatus) */
	private Map<String,Map<String, FrameStatus>> frameStatusMap;
	
	/** streamID->StreamStatus*/
	protected Map<String,StreamStatus> streamStatusMap;

	protected String savedPattern;

	
	// to prevent from showing multiple warning messages for
	// same undefined stream or frame
	protected Set<String> undef_streams;

	
	protected List<VectorInfo> vectors = new ArrayList<VectorInfo>();

	/**
	 * Controls interaction with the observers.
	 */
	private class ObserversManager {
		/** Map streamID -> Observers */
		private Map<String,Collection<IConnectionObserver>> observersMap
				= new HashMap<String,Collection<IConnectionObserver>>();

		void _addObserver(IConnectionObserver obs, String streamID) {
			Collection<IConnectionObserver> observers = observersMap.get(streamID);
			if ( observers == null ) {
				observers = new ConcurrentLinkedQueue<IConnectionObserver>();
				observersMap.put(streamID, observers);
			}
			observers.add(obs);
		}

		void _removeObservers(String streamID) {
			Collection<IConnectionObserver> observers = observersMap.remove(streamID);
			if ( observers != null ) {
				observers.clear();
			}
		}

		void _notifyNewImage(String streamID, IImage iimg) {
//			String streamID = iimg.getStreamID();
			Collection<IConnectionObserver> observers = observersMap.get(streamID);
			if ( observers == null ) {
				System.out.println("ObserversManager._notifyNewImage: NO observer for streamID = "+streamID);
				return;
			}
			for (IConnectionObserver obs : observers ) {
				obs.newImage(iimg);
			}
		}

		void _notifyConnectedWaitingData(String streamID, String msg) {
			Collection<IConnectionObserver> observers = observersMap.get(streamID);
			if ( observers == null ) {
				return;
			}
			for (IConnectionObserver obs : observers) {
				obs.connectedWaitingData(streamID, msg);
			}
		}
		
		void _notifyConnectionException(String streamID, Exception ex) {
			Collection<IConnectionObserver> observers = observersMap.get(streamID);
			if ( observers == null ) {
				return;
			}
			List<IConnectionObserver> toRemove = null;
			for (IConnectionObserver obs : observers) {
				boolean keepObs = obs.connectionException(streamID, ex);
				if ( !keepObs ) {
					if ( toRemove == null ) {
						toRemove = new ArrayList<IConnectionObserver>();
					}
					toRemove.add(obs);
				}
			}
			if ( toRemove != null ) {
				observers.removeAll(toRemove);
			}
		}
		
		void _notifyReaderFinished(String streamID, String msg) {
			Collection<IConnectionObserver> observers = observersMap.get(streamID);
			if ( observers == null ) {
				return;
			}
			List<IConnectionObserver> toRemove = null;
			for (IConnectionObserver obs : observers) {
				boolean keepObs = obs.readerFinished(streamID, msg);
				if ( !keepObs ) {
					if ( toRemove == null ) {
						toRemove = new ArrayList<IConnectionObserver>();
					}
					toRemove.add(obs);
				}
			}
			observers.removeAll(toRemove);
		}

		void _notifyFrameStarts(String streamID, FrameStatus frameStatus) {
			Collection<IConnectionObserver> observers = observersMap.get(streamID);
			if ( observers == null ) {
				return;
			}
			for (IConnectionObserver obs : observers) {
				obs.frameStarts(frameStatus);
			}
		}
	}

	private ObserversManager obsManager = new ObserversManager();

	
	protected BasicServerConnection() {
		streamDefs = new LinkedHashMap<String,StreamDef>();
		frameDefs = new HashMap<String,Map<String,FrameDef>>();
		frameStatusMap = new HashMap<String, Map<String,FrameStatus>>();
		streamStatusMap = new HashMap<String,StreamStatus>();
		undef_streams = new HashSet<String>();
	}
	
	protected void setSavedPattern(String savedPattern) {
		this.savedPattern = savedPattern;
	}
	public String getSavedPattern() {
		return savedPattern;
	}

	public String getSavedURL(String streamID) { 
		return savedPattern != null ? savedPattern.replace("#", ""+streamID) : null;
	}
	  
	  
	/** Gets the reference space of all streams coming through this connection. 
	  * Location of all images are relative to this space. 
	  */
	public ReferenceSpace getReferenceSpace() {
		return referenceSpace;
	}
	
	/**
	 * Add a stream definition and status info for the streamID.
	 * @param streamID
	 * @param streamDef
	 */
	public void addStreamDef(String streamID, StreamDef streamDef) {
		streamDefs.put(streamID, streamDef);
		streamStatusMap.put(streamID, new StreamStatus(streamID));
	}

	protected void setStreamIDs(List<String> streamIDs) {
		streamDefs.clear();
		for (String streamID : streamIDs) {
			streamStatusMap.put(streamID, new StreamStatus(streamID));
		}
	}

	/**
	 * Sets stream definitions and corresponding status info.
	 * @param streamDefs
	 */
	protected void setStreamDefs(Map<String,StreamDef> streamDefs) {
		this.streamDefs = streamDefs;
		for (StreamDef streamDef : streamDefs.values()) {
			String streamID = streamDef.getStreamID();
			streamStatusMap.put(streamID, new StreamStatus(streamID));
		}
	}

	public Collection<StreamDef> getStreamDefs() {
		return streamDefs.values();
		
	}
	
	
	public Collection<StreamStatus> getStreamStatuses() {
		return streamStatusMap.values();
	}
	
	public Collection<FrameStatus> getFrameStatuses(String streamID) {
		Map<String, FrameStatus> map = frameStatusMap.get(streamID);
		return map != null ? map.values() : null;
	}
	
	public Collection<FrameDef> getFrameDefs(String streamID) {
		Map<String, FrameDef> map = frameDefs.get(streamID);
		return map != null ? map.values() : null;
	}
	

	public FrameDef getFrameDef(String streamID, String frameID) {
		Map<String, FrameDef> map = frameDefs.get(streamID);
		return map != null ? map.get(frameID) : null;
	}
	
	protected void addFrameDef(String streamID, FrameDef frameDef) {
		Map<String, FrameDef> map = frameDefs.get(streamID);
		if ( map == null ) {
			map = new HashMap<String, FrameDef>();
			frameDefs.put(streamID, map);
		}
		map.put(frameDef.getID(), frameDef);
	}

	public StreamDef getStreamDef(String streamID) {
		return streamDefs.get(streamID);
	}
	
	public ChannelDef getChannelDefForStream(String streamID) {
		StreamDef streamDef = streamDefs.get(streamID);
		return streamDef != null ? streamDef.getChannelDef() : null;
	}
	
	public FrameStatus getFrameStatus(String streamID, String frameID) {
		Map<String, FrameStatus> map = frameStatusMap.get(streamID);
		return map != null ? map.get(frameID) : null;
	}
	
	protected void addFrameStatus(String streamID, FrameStatus frameStatus) {
		Map<String, FrameStatus> map = frameStatusMap.get(streamID);
		if ( map == null ) {
			map = new HashMap<String, FrameStatus>();
			frameStatusMap.put(streamID, map);
		}
		map.put(frameStatus.getFrameID(), frameStatus);
	}

	
	public StreamStatus getStreamStatus(String streamID) {
		return streamStatusMap.get(streamID);
	}

	
	/**
	 * Reports a warning if the ID is is not recognized.
	 */
	private StreamStatus _getStreamStatus(String streamID) {
		StreamStatus streamStatus = streamStatusMap.get(streamID);
		if ( streamStatus == null ) {
			// got an image in an undefined stream
			String chkey = streamID;
			if (!undef_streams.contains(chkey)) {
				undef_streams.add(chkey);
				System.out.println("_getStreamStatus: WARNING: No StreamStatus for streamID=" + chkey);
				System.out.println("                 " +getClass().getName());
			}
		}
		return streamStatus;
	}
	

	/**
	 * Increments the number of images received for the corresponding stream
	 * and notifies my connection observers about a new image
	 */
	void notifyNewImage(String streamID, IImage iimg) {
		assert iimg != null;
		if ( !iimg.isNull() ) {
//			String streamID = iimg.getStreamID();
			StreamStatus streamStatus = _getStreamStatus(streamID);
			if ( streamStatus == null ) {
				
				// TODO: this is a trick here, sthg while I get time for general cleanup
				streamStatus = new StreamStatus(streamID);
				streamStatusMap.put(streamID, streamStatus);
				
//				TODO Remove
//				Utils.log("BasicServerConnection.notifyNewImage: No StreamStatus for streamID=" +streamID);
//				return; // Ignored.
			}
		
			streamStatus.incrementImageCount();
			FrameStatus frameStatus = getFrameStatus(streamID, iimg.getFrameID());
			if ( frameStatus == null ) {
				
				// do not add it:
				
				// we got an image but no corresponding frame definition has been received
//				frameStatus = new FrameStatus(getStreamDef(streamID), iimg.getFrameID());
			}
			else {
				frameStatus.incrementRows();
				frameStatus.setLastRow(iimg.getY());
				
				addFrameStatus(streamID, frameStatus);
			}
			
			iimg.setServerConnection(this);
		}
			
		obsManager._notifyNewImage(streamID, iimg);
	}
	
	
	void notifyConnectedWaitingData(String streamID, String msg) {
		obsManager._notifyConnectedWaitingData(streamID, msg);
	}

	void notifyConnectionException(String streamID, Exception ex) {
		obsManager._notifyConnectionException(streamID, ex);
	}
	
	void notifyReaderFinished(String streamID, String msg) {
		obsManager._notifyReaderFinished(streamID, msg);
	}
	

	/** 
	 * IImageStream.IObserver method
	 */
	public void notifyFrameStarts(String streamID, FrameDef frameDef) {
//		Utils.log("BasicServerConnection.notifyFrameStarts: streamID=" +streamID);
		StreamStatus streamStatus = _getStreamStatus(streamID);
		if ( streamStatus == null ) {
			Utils.log("BasicServerConnection.notifyFrameStarts: No StreamStatus for streamID=" +streamID);
			return; // Ignored.
		}

		StreamDef streamDef = getStreamDef(streamID);
		assert streamDef != null;
		
//		Utils.log("BasicServerConnection.notifyFrameStarts: streamDef=" +streamDef);
		
		if ( getFrameDef(streamID, frameDef.getID()) == null ) {
			addFrameDef(streamID, frameDef);
		}
		FrameStatus frameStatus = getFrameStatus(streamID, frameDef.ID);
		if ( frameStatus == null ) {
			frameStatus = new FrameStatus(streamDef, frameDef);
//			Utils.log("BasicServerConnection.notifyFrameStarts: frameStatus=" +frameStatus);
			addFrameStatus(streamID, frameStatus);
		}
		else {
			frameStatus.setFrameDef(frameDef);
		}

		obsManager._notifyFrameStarts(streamID, frameStatus);
	}

	public void addObserver(IConnectionObserver obs, String streamID) {
		obsManager._addObserver(obs, streamID);
	}

	/**
	 * Removes the observers of a given stream.
	 * @param streamID
	 */
	protected void _removeObservers(String streamID) {
		obsManager._removeObservers(streamID);
	}
	
	public void setReferenceSpace(ReferenceSpace referenceSpace) {
		this.referenceSpace = referenceSpace;
	}
	
	public List<VectorInfo> getVectors() { return vectors; }
	
	public VectorInfo getVectorInfo(String vectorName) {
		for (VectorInfo vectorInfo : vectors) {
			if ( vectorInfo.getName().equals(vectorName) ) {
				return vectorInfo;
			}
		}
		return null;
	}
}
