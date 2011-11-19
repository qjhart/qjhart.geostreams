package geostreams.image;

/**
  * A basic image stream.
  *                                   
  * @author Carlos Rueda-Velasquez
  * @version $Id: BasicImageStream.java,v 1.3 2007/06/01 21:40:01 crueda Exp $
  */ 
abstract class BasicImageStream implements IImageStream {
	/** BasicImage.nullImage has already been sent? */
	protected boolean _nullImageSent = false;
		
	protected ReferenceSpace referenceSpace = new ReferenceSpace();

	/**
	 */
	protected String streamID;
	
	/** My channel ID */
	protected String channelID;
	
	/** Associated observer */
	protected IImageStream.IObserver _imgObserver;
	
	/**
	 * 
	 * @param channelID Used for both streamID and channelID
	 */
	protected BasicImageStream(String channelID) {
		this.streamID = this.channelID = channelID;
	}
	
	protected String _getName() {
		return getClass().getSimpleName()+ "(\"" +channelID+ "\")";
	}
	
	/** Sets the observer to the stream. Must be called right after creation. 
	  */
	public void setImageStreamObserver(IImageStream.IObserver obs) {
		this._imgObserver = obs;
	}
	
	public String getStreamID() {
		return streamID;
	}
	
	public String getChannelID() {
		return channelID;
	}
	
	public ReferenceSpace getReferenceSpace() {
		return referenceSpace;
	}

	public String toString() {
		return _getName();
	}
}

