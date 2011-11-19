package geostreams.ptolemy;

import geostreams.image.IServerConnection;
import geostreams.image.StreamDef;

import java.util.List;

/**
 * All ptolemy actors dealing with IImage streams should
 * implement this interface.
 * 
 * <p>
 * TODO Preliminary version
 * 
 * @author Carlos Rueda-Velasques
 * @version $Id: IStreamActor.java,v 1.4 2007/06/01 21:40:01 crueda Exp $
 */
public interface IStreamActor {

	/**
	 * @return The stream defs corresponding to the output ports
	 * that generate the image streams. null if the actor cannot
	 * determine this information, ie., it has no server connection.
	 */
	public List<StreamDef> getStreamDefs();

	/**
     * @return the server connection providing the stream of images
     * generated from or going through this actor. null if the actor
     * cannot determine this information, eg., when no image token
     * has been received to obtain the connection from it.
     */
	public IServerConnection getServerConnection();
}
