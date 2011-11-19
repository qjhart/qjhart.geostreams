package geostreams.image;

/** 
 * Objects interested in events associated to a particular stream
 * occuring in a connection should implement this interface and register
 * themselves by calling IServerConnection.addObserver(observer).
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: IConnectionObserver.java,v 1.4 2007/06/02 05:28:21 crueda Exp $
 */
public interface IConnectionObserver {
	/** 
	 * Called when a new image has been received through the connection.
	 * @param iimg The image
	 */
	public void newImage(IImage iimg);

	/**
	 * Called when a new frame definition has been received through the connection.
	 */
	public void frameStarts(FrameStatus frameStatus);

	/**
	 * Called when no more images will come for a frame definition.
	 */
	public void frameEnds(FrameStatus frameStatus);

	/**
	 * Called when the connection is waiting to get data.
	 */
	public void connectedWaitingData(String streamID, String msg);

	/**
	 * Called when no more images will be read from the corresponding image stream.
	 * 
	 * @param streamID Associated stream.
	 * @param msg A message useful for logging
	 * @return true iff this observer should remain in the list of registered
	 * 				observers for the stream. Observers should typically return false.
	 */
	public boolean readerFinished(String streamID, String msg);

	/**
	 * Called when an exception has occurred.
	 * 
	 * @param streamID Associated stream.
	 * @param ex The exception.
	 * @return true iff this observer should remain in the list of registered
	 * 				observers for the stream. Observers should typically return false.
	 */
	public boolean connectionException(String streamID, Exception ex);
	
	/**
	 * A base implementation that by default prints messages to stdout.
	 */
	public static class Adapter implements IConnectionObserver {
		/** Prints the message to stdout */
		protected void _log(String msg) {
			System.out.println("  IConnectionObserver.Adapter: [" +msg+ "]");
		}
		public void newImage(IImage iimg) {
			_log("newImage: " +iimg);
		}
		public void frameStarts(FrameStatus frameStatus) {
			_log("frameStarts: " +frameStatus);
		}
		public void frameEnds(FrameStatus frameStatus) {
			_log("frameEnds: " +frameStatus);
		}
		public void connectedWaitingData(String streamID, String msg) {
			_log("connectedWaitingData: streamID=" +streamID+ " msg=" +msg);
		}
		public boolean readerFinished(String streamID, String msg) {
			_log("readerFinished: streamID=" +streamID+ " msg=" +msg+ " RETURNING false");
			return false; 
		}
		public boolean connectionException(String streamID, Exception ex) {
			_log("connectionException: streamID=" +streamID+ " ex=" +ex+ " RETURNING false");
			return false;
		}
	}
}