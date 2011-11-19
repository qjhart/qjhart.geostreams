package geostreams.image;

import java.awt.Rectangle;
import java.util.Collections;
import java.util.List;


/**
 * @author Carlos Rueda
 * @version $Id: AdHocServerConnection.java,v 1.10 2007/06/12 16:51:55 crueda Exp $
 */
public class AdHocServerConnection extends BasicServerConnection {

	public AdHocServerConnection(NewImageSequence status) {
		referenceSpace = status.referenceSpace;
		savedPattern = status.savedPattern;
		
		String streamID = null;
		
		if ( status.istream != null ) {
			streamID = status.istream.getStreamID();
//			streamStatus = status.istream.getStreamStatus();
		}
		else {
			if  ( status.streamDef != null ) {
				streamID = status.streamDef.getStreamID();
				streamDefs.put(streamID, status.streamDef);
//				streamStatus = status.streamStatus;
			}
		}

		addStreamDef(streamID, status.streamDef);
		
		if ( streamID != null ) {

			for (int i = 0; i < status.frameDefsArray.length; i++) {
				FrameDef obj = status.frameDefsArray[i];
				addFrameDef(streamID, obj);
			}
			for (int i = 0; i < status.frameStatusArray.length; i++) {
				FrameStatus obj = status.frameStatusArray[i];
				addFrameStatus(streamID, obj);
			}
		}
	}

	/**
	 * Does nothing in this class (but prints a log message to stdout)
	 */
	public void close() {
		System.out.println("AdHocServerConnection.close() called.");
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public void openStream(String streamID, Rectangle roi) throws Exception {
		throw new UnsupportedOperationException();
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public String getServerShortDescription() {
		throw new UnsupportedOperationException();
	}

	public List<VectorInfo> getVectors() {
		//TODO getVectors
		return Collections.emptyList();
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public void setTimeout(int timeout) {
		throw new UnsupportedOperationException();
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public void setSimulationDelay(int simulationDelay) {
		throw new UnsupportedOperationException();
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public boolean isStreamRunning(String id) {
		throw new UnsupportedOperationException();
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public void startStreamReaders() throws Exception {
		throw new UnsupportedOperationException();
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public StreamDef getOverviewStreamDef() {
		throw new UnsupportedOperationException();
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public String getOverviewStreamID() {
		throw new UnsupportedOperationException();
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public void interruptStream(String channelDef) {
		throw new UnsupportedOperationException();
	}

}
