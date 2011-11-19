package geostreams.image;

import java.io.Serializable;


/**
  * Describes a stream.
  * 
  * TODO: This class will be removed after general cleanup.
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: StreamDef.java,v 1.3 2007/06/12 16:51:55 crueda Exp $
  */
public class StreamDef implements Serializable {
	/** Creates a stream definition.
	 *  
	 *  FIXME: name taking channelDef.getName(): provide a means to set the name.
	 *  
	 * @param streamID ID for the stream.
	 * @param channelDef Associated channel.
	 */
	public StreamDef(String streamID, ChannelDef channelDef)	{
		this.streamID = streamID;
		this.channelDef = channelDef;
		
		// FIXME actually name takes the given streamID
//		this.name = this.channelDef.getName();
		this.name = this.streamID;
	}		

	/** ID of the stream. */
	public String getStreamID() { return streamID; }
	
	/** Short name of the stream. */
	public String getName() { return name; }
	
	/**
	 * Gets the channelDef of the stream.
	 * @return
	 */
	public ChannelDef getChannelDef() {
		return channelDef;
	}
	
	public String getDescriptiveString() {
		return "[streamID=" +streamID
			 + ", name=" +name
		     + ", channelDef=" +channelDef
		     + "]\n"
		;
	}
	
	public String toString() {
		return getDescriptiveString();
	}

	/** ID of this channel definition */
	private String streamID;
	
	/** A short name */
	private String name;
	
	private ChannelDef channelDef;
}

