package geostreams.image;

import java.awt.Rectangle;
import java.io.Serializable;


/**
  * Info about a frame that has been received or indicated in an image. 
  * This info is associated to a streamID.
  * It could happen that no frame definition has been received.
  * 
  * @author Carlos Rueda-Velasquez
  * @version $Id: FrameStatus.java,v 1.8 2007/06/13 00:08:42 crueda Exp $
  */
public class FrameStatus implements Serializable {
	private StreamDef streamDef;
	private FrameDef frameDef;       // could be null
	private String frameID;          // but at least we have its ID
	
	// number of rows received for this frame in this stream
	private int rows;
	
	// first row received for this frame:
	private int firstRow = Integer.MAX_VALUE;

	// last row received for this frame:
	private int lastRow;

	
	public FrameStatus(FrameDef frameDef) {
		assert frameDef != null;
		this.frameDef = frameDef;
		this.frameID = frameDef.getID();
	}
	
	
	
	/**
	 * creates a frame status object with known frame definition.
	 * @param streamDef must be non null.
	 * @param frameDef must be non null.
	 */
	FrameStatus(StreamDef streamDef, FrameDef frameDef) {
		assert streamDef != null;
		assert frameDef != null;
		this.streamDef = streamDef;
		this.frameDef = frameDef;
		this.frameID = frameDef.getID();
	}
	
	/**
	 * creates a frame status object with known frame ID.
	 * @param streamDef must be non null.
	 * @param frameID must be non null.
	 */
	FrameStatus(StreamDef streamDef, String frameID) {
		assert streamDef != null;
		this.streamDef = streamDef;
		this.frameID = frameID;
	}

	public String getFrameID() {
		return frameID;
	}
	
	/**
	 * Returns true if this frame has been completed.
	 * Returns false if no frame definition has been associated.
	 * @return true if this frame has been completed.
	 */
	public boolean completed() {
		if ( frameDef == null ) {
			return false;
		}
		if ( streamDef == null ) {
			return false;
		}
		Rectangle rect = frameDef.getRectangle();
		return (lastRow + streamDef.getChannelDef().getPixelSizeY() >= rect.y + rect.height);
	}
	
	/**
	 * Gets the number of received.
	 * @return
	 */
	public int getNumberOfRows() {
		return rows;
	}
	
	/**
	 * Only meaningful if getNumberOfRows() > 0
	 * @return
	 */
	public int getFirstRow() {
		return firstRow;
	}

	/**
	 * Only meaningful if getNumberOfRows() > 0
	 * @return
	 */
	public int getLastRow() {
		return lastRow;
	}
	
	/**
	 * @return could be null if no frame definition has been received.
	 */
	public FrameDef getFrameDef() {
		return frameDef;
	}

	public String toString() {
		if ( streamDef != null ) {
			return
				"[FrameStatus: streamID=" +streamDef.getStreamID()+
				",channelID=" +streamDef.getChannelDef().getID()+ 
				", frameDef=" +(frameDef == null ? "unknown" : frameDef.ID)+
				(rows == 0 ? " (no rows)" : (", first=" +firstRow+ ", last=" +lastRow)) +
				"]"
			;
		}
		else {
			return
				"[FrameStatus: streamID=" +"unknown"+
				",channelID=" +"unknown"+ 
				", frameDef=" +(frameDef == null ? "unknown" : frameDef.ID)+
				(rows == 0 ? " (no rows)" : (", first=" +firstRow+ ", last=" +lastRow)) +
				"]"
			;
		}
	}

	void incrementRows() {
		rows++;
	}

	void setLastRow(int y) {
		lastRow = y;
	}

	void setFrameDef(FrameDef frameDef) {
		this.frameDef = frameDef;
		this.frameID = frameDef.getID();
	}

	public StreamDef getStreamDef() {
		return streamDef;
	}
}	

