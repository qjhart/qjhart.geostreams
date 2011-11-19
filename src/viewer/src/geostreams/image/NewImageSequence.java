package geostreams.image;

import java.io.Serializable;
import java.util.Collection;

/**
 * Runtime information that can be transmitted.
 * 
 * @author Carlos Rueda
 * @version $Id: NewImageSequence.java,v 1.6 2007/06/05 06:29:07 crueda Exp $
 */
public class NewImageSequence implements Serializable {

	String streamID;
	IStream istream;
	IImage iimg;
	ReferenceSpace referenceSpace;
	StreamDef streamDef;
//	StreamStatus streamStatus;
	FrameDef[] frameDefsArray;
	FrameStatus[] frameStatusArray;
	String savedPattern;
	

	public NewImageSequence(IImage iimg) {
		// clone to have a "new" object to transmit and update its server connection
		this.iimg = iimg.clone();

		istream = iimg.getStream();
		streamID = iimg.getStreamID();
		
		Collection<FrameDef> frameDefs;
		Collection<FrameStatus> frameStatuses;
		
		if ( istream != null ) {
			streamID = istream.getStreamID();
			this.iimg.setStreamID(streamID);
			System.out.println("NewImageSequence. iimg.getStreamID=" +iimg.getStreamID());
			referenceSpace = istream.getReferenceSpace();
			
//			streamStatus = istream.getStreamStatus();
//			System.out.println("NewImageSequence.streamStatus=" +streamStatus);
			
			frameDefs = istream.getFrameDefs().values();
			frameStatuses = istream.getFrameStatusMap().values();
			
			ChannelDef channelDef = istream.getChannelDef();
			streamDef = new StreamDef(streamID, channelDef);
		}
		else {
			IServerConnection gscon = iimg.getServerConnection();
			referenceSpace = gscon.getReferenceSpace();
			
//			streamStatus = gscon.getStreamStatus(streamID);
			
			frameDefs = gscon.getFrameDefs(streamID);
			frameStatuses = gscon.getFrameStatuses(streamID);
			savedPattern = gscon.getSavedPattern();
			streamDef = gscon.getStreamDef(streamID);
		}

		frameDefsArray = new FrameDef[frameDefs.size()];
		frameDefsArray = frameDefs.toArray(frameDefsArray);

		frameStatusArray = new FrameStatus[frameStatuses.size()];
		frameStatusArray = frameStatuses.toArray(frameStatusArray);
	}

	public IImage getImage() {
		return iimg;
	}

	public IStream getStream() {
		return istream;
	}
}

