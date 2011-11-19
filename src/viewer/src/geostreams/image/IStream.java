package geostreams.image;

import java.awt.Rectangle;
import java.util.List;
import java.util.Map;

/**
 * Provides info about an existing incoming stream.
 * 
 * NOTE: These operations are a selection from those in
 * IImage and IServerConnection.
 * 
 * TODO : 0 istream
 * 
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: IStream.java,v 1.4 2007/06/13 00:08:42 crueda Exp $
 */
public interface IStream {

	public String getStreamID(); 	
	
	public ChannelDef getChannelDef();
	
	public ReferenceSpace getReferenceSpace();
	
	public Rectangle getFOV();
	public void setFov(Rectangle fov);
	
	public String getSavedURL();
	
	public StreamStatus getStreamStatus();
	
	public boolean isRunning();

	
	
	////// the following will probably be moved out from this interface
	
	public Map<String, FrameDef> getFrameDefs();
	
	public Map<String, FrameStatus> getFrameStatusMap();
	
	public List<VectorInfo> getVectors();
}
