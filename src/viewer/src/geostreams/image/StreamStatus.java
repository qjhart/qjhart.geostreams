package geostreams.image;

import java.io.Serializable;


/**
  * Info about a stream being visualized. 
  * 
  * @author Carlos Rueda-Velasquez
  * @version $Id: StreamStatus.java,v 1.2 2007/06/12 16:51:55 crueda Exp $
  */
public class StreamStatus implements Serializable {
	private String streamID;
	private int no_imgs;
	private int no_frames;
	
	public StreamStatus(String streamID) {
		this .streamID = streamID;
	}
	
	/** increments counter of images received. */
	public void incrementImageCount() {
		no_imgs++;
	}
	
	/** returns the counter of images received. */
	public int getImageCount() {
		return no_imgs;
	}
	
	/** increments counter of frames received. */
	void incrementFrameCount() {
		no_frames++;
	}
	
	/** returns the counter of frames received. */
	public int getFrameCount() {
		return no_frames;
	}

	public String getStreamID() {
		return streamID;
	}
	
	public String toString() {
		return "[StreamStatus: ID=" +streamID+ ", imgs=" +no_imgs+ ", frames=" +no_frames+ "]";
	}
}	

