package geostreams.image;

import java.awt.Rectangle;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: BasicStream.java,v 1.6 2007/06/13 00:08:43 crueda Exp $
 */
class BasicStream implements IStream, Serializable {
	
	private String streamID;
	private ReferenceSpace referenceSpace;
	private Rectangle fov;

	private ChannelDef channelDef;
	private String savedURL;
	private StreamStatus streamStatus;
	
	/** frameID->FrameDef */
	private Map<String,FrameDef> frameDefs;
	
	/** frameID->FrameStatus */
	private Map<String, FrameStatus> frameStatusMap;
	
	private List<VectorInfo> vectors;

	
	public BasicStream(IStream istr) {
		this.streamID = istr.getStreamID();
		this.referenceSpace = istr.getReferenceSpace().clone();
		this.fov = (Rectangle) istr.getFOV().clone();
		this.channelDef = istr.getChannelDef();
		this.savedURL = istr.getSavedURL();
		this.streamStatus = new StreamStatus(this.streamID);
		
		this.frameDefs = new HashMap<String,FrameDef>();
		Map<String, FrameDef> iframeDefs = istr.getFrameDefs();
		for (String frameID : iframeDefs.keySet()) {
			this.frameDefs.put(frameID, iframeDefs.get(frameID));
		}

		this.frameStatusMap = new HashMap<String,FrameStatus>();
		Map<String, FrameStatus> iframeStatusMap = istr.getFrameStatusMap();
		for (String frameID : iframeStatusMap.keySet()) {
			this.frameStatusMap.put(frameID, iframeStatusMap.get(frameID));
		}
		
		this.vectors = new ArrayList<VectorInfo>();
		List<VectorInfo> vecs = istr.getVectors();
		if ( vecs != null) {
			for ( VectorInfo vectorInfo : istr.getVectors() ) {
				// TODO should be a clone.. but this's OK
				this.vectors.add(vectorInfo);
			}
		}
	}

	public BasicStream(String streamID, IServerConnection isconn) {
		this.streamID = streamID;
		this.referenceSpace = isconn.getReferenceSpace().clone();
		this.fov = (Rectangle) this.referenceSpace.getRectangle().clone(); 
		this.channelDef = isconn.getChannelDefForStream(streamID);
		this.savedURL = isconn.getSavedURL(streamID);
		this.streamStatus = isconn.getStreamStatus(streamID);
		
		this.frameDefs = new HashMap<String,FrameDef>();
		Collection<FrameDef> iframeDefs = isconn.getFrameDefs(streamID);
		if ( iframeDefs != null ) {
			for (FrameDef frameDef : iframeDefs ) {
				this.frameDefs.put(frameDef.getID(), frameDef);
			}
		}

		this.frameStatusMap = new HashMap<String,FrameStatus>();
		Collection<FrameStatus> iframeStatuses = isconn.getFrameStatuses(streamID);
		if ( iframeStatuses != null ) {
			for (FrameStatus frameStatus : iframeStatuses) {
				this.frameStatusMap.put(frameStatus.getFrameID(), frameStatus);
			}
		}
		
		this.vectors = isconn.getVectors();
		if ( this.vectors == null ) {
			this.vectors = new ArrayList<VectorInfo>();
		}
	}


	public BasicStream(String streamID, ReferenceSpace referenceSpace, ChannelDef channelDef) {
		this.streamID = streamID;
		this.referenceSpace = referenceSpace.clone();
		this.fov = (Rectangle) this.referenceSpace.getRectangle().clone(); 
		this.channelDef = channelDef;
		this.savedURL = null;
		this.streamStatus = new StreamStatus(this.streamID);
		
		this.frameDefs = new HashMap<String,FrameDef>();
		this.frameStatusMap = new HashMap<String,FrameStatus>();
		this.vectors = new ArrayList<VectorInfo>();
	}
	
	
	// package private
	void setStreamID(String streamID) {
		this.streamID = streamID;
	}


	public ChannelDef getChannelDef() {
		return channelDef;
	}

	public Map<String, FrameDef> getFrameDefs() {
		return frameDefs;
	}

	public Map<String, FrameStatus> getFrameStatusMap() {
		return frameStatusMap;
	}

	public ReferenceSpace getReferenceSpace() {
		return referenceSpace;
	}

	public Rectangle getFOV() {
		return fov;
	}

	public void setFov(Rectangle fov) {
		this.fov = (Rectangle) fov.clone();
	}

	public String getSavedURL() {
		return savedURL;
	}

	public String getStreamID() {
		return streamID;
	}

	public StreamStatus getStreamStatus() {
		if ( streamStatus == null ) {
			streamStatus = new StreamStatus(streamID);
		}
		return streamStatus;
	}

	public boolean isRunning() {
		return true;
	}

	public List<VectorInfo> getVectors() {
		return vectors;
	}

	public String toString() {
		return
			"[BasicStream: streamID=" +streamID+ ", ChannelDef=" +channelDef+ "]"
		;
	}
}
