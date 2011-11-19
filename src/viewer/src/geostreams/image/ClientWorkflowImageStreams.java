package geostreams.image;


import geostreams.util.Utils;

import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.InterruptedIOException;
import java.io.ObjectInputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;

import javax.swing.Timer;

/**
 * Creates IImageStream objects to get access to the streams
 * in a remote net-enabled workflow.
 * 
 * based on geostream.Client -- some methods even with the original (gui-related) name
 * 
 * @author Carlos Rueda
 * @version $Id: ClientWorkflowImageStreams.java,v 1.13 2007/06/27 03:39:40 crueda Exp $
 */
public class ClientWorkflowImageStreams {

	private static Map<String,ClientWorkflowImageStreams> instances = new HashMap<String,ClientWorkflowImageStreams>();
	
	public static ClientWorkflowImageStreams getInstance(String server, int port) throws Exception {
		if ( true ) {
			return new ClientWorkflowImageStreams(server, port);
		}
		else {
			// TODO Why I was doing this?
			String key = server+ ":" +port;
			if ( !instances.containsKey(key) ) {
				instances.put(key, new ClientWorkflowImageStreams(server, port));
			}
			return instances.get(key);
		}
	}
	
	//////// instance ///////////////
	
	private String server;
	private int port;
	
	private SocketReader socketReader;
	private Socket socket;
	private PrintWriter pw;
	private ObjectInputStream ois;

	private IStream istream;

	private IServerConnection adHocConnection = null;
	
	private Timer timer;
	
	
	private List<String> streamIDs = new ArrayList<String>();
	
	/** streamID->StreamDef */
	private Map<String,StreamDef> streamDefs = new LinkedHashMap<String,StreamDef>();
	private ReferenceSpace referenceSpace;

	private String overviewStreamID;	
	private List<VectorInfo> vectors = new ArrayList<VectorInfo>();
	
	public String getOverviewStreamID() {
		Utils.log(" ///////// GET  getOverviewStreamID: " +overviewStreamID);
		return overviewStreamID;
	}
	private void setOverviewStreamID(String overviewStreamID) {
		Utils.log(" ///////// SET   setOverviewStreamID: " +overviewStreamID);
		this.overviewStreamID = overviewStreamID;
	}

	List<VectorInfo> getVectors() {
		return vectors;
	}

	private volatile boolean isActive;

	/** Map: ChannelID -> ClientImageStream */
	private Map<String,ClientImageStream> imgStreams = new ConcurrentHashMap<String,ClientImageStream>();
	private String savedPattern;
	
	private ClientWorkflowImageStreams(String server, int port) throws Exception {
		this.server = server;
		this.port = port;

		isActive = true;
		socketReader = new SocketReader();
		socketReader.start();
		
		timer = new Timer(2000, new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if ( !isActive ) {
					timer.stop();
				}
				sendAlive();
			}
		}) ;
		timer.start();
		
		Thread.sleep(2000);
	}

	private synchronized void send(String cmd) {
		if ( socketReader != null ) {
			Utils.log("ClientWorkflowImageStreams.send: " +cmd);
			pw.println(cmd);
			pw.flush();
		}
	}
	
	private void sendAlive() {
		if ( socketReader != null ) {
			pw.println("alive");
			pw.flush();
		}
	}

	

	public BasicImageStream openStream(String streamID, Rectangle roi, boolean nodata) 
	throws Exception {
		ClientImageStream imgStream = imgStreams.get(streamID);

		if ( imgStream == null ) {
			
			// TODO cleanup for new mechanism:
			
			// OLD check
			if ( streamDefs.keySet().contains(streamID) ) {
				imgStreams.put(streamID, imgStream = new ClientImageStream(streamID));
				send("getStream:" +streamID);
			}
			else {
				// NEW check
				if ( streamIDs.contains(streamID)) {
					imgStreams.put(streamID, imgStream = new ClientImageStream(streamID));
					send("getStream:" +streamID);
				}
				else {
					Utils.log("ClientWorkflowImageStreams: ERROR: No stream for streamID: " +streamID
							+"\n Available streams: " +streamIDs
					);
				}
			}
		}
		
		return imgStream;
	}
	
	private class ClientImageStream extends BasicImageStream {

		private BlockingQueue<IImage> syncFifo = new LinkedBlockingQueue<IImage>();
		
		ClientImageStream(String channelID) {
			super(channelID);
		}
		
		public void startReading() throws IOException {
			if ( syncFifo == null ) {
				syncFifo = new LinkedBlockingQueue<IImage>();
			}
			
		}

		/** @return true if operation completed OK */
 		boolean putNewImage(IImage img) {
 			if ( syncFifo == null ) {
 				return false;
 			}
 			
			if ( !_nullImageSent ) {
				try {
					syncFifo.put(img);
					return true;
				}
				catch (InterruptedException ex) {
					ex.printStackTrace();
				}
			}
			return false;
		}

		public IImage nextImage() throws IOException {
 			if ( syncFifo == null ) {
 				return null;
 			}

 			if ( _nullImageSent ) {
				return null;
			}
			else {
				IImage img;
				try {
					img = syncFifo.take();
				}
				catch (InterruptedException ex) {
//					ex.printStackTrace();
					throw new InterruptedIOException();
				}
				if ( img.isNull() ) {
					_nullImageSent = true;
					return Images.nullImage;
				}
				else {
					return img;
				}
			}
		}

		public void close() throws IOException {
			imgStreams.remove(channelID);
 			if ( syncFifo == null ) {
 				syncFifo.clear();
 			}
		}
		
	}

	


	private void _updateStatusLabel(String text) {
		Utils.log(text);
	}
	private void _updateReceivedLabel(String text) {
		Utils.log(text);
	}
	private void _updateReferenceSpace(ReferenceSpace referenceSpace) {
		this.referenceSpace = referenceSpace;
	}
	
	private void _updateAvailableIDs(String[] streams) {
		if ( streamIDs == null ) {
			streamIDs.clear();
		}
		else {
			streamIDs.addAll(Arrays.asList(streams));
			Utils.log(" ///////// /   _updateAvailableStreamIDs: " +streamIDs);
		}
	}
	
	private void _updateAvailableStreamDefs(StreamDef streamDef) {
		if ( streamDef == null ) {
			streamDefs.clear();
		}
		else {
			streamDefs.put(streamDef.getStreamID(), streamDef);
			Utils.log(" ///////// /   _updateAvailableStreamDefs: \n" +
					" " +streamDef.getDescriptiveString());
		}
	}
	private void _updateAvailableVectors(VectorInfo vectorInfo) {
		if ( vectorInfo == null ) {
			vectors.clear();
		}
		else {
			vectors.add(vectorInfo);
		}
//		Utils.log("\n ///////// /   _updateAvailableVectors: " +vectors+ "\n");
	}

	private void _updateSavedPattern(String savedPattern) {
		if ( savedPattern.length() > 0 ) {
			this.savedPattern = savedPattern;
		}
//		Utils.log("\n ///////// /   _updateSavedPattern: [" +savedPattern+ "]  (ignored if ampty)\n");
	}

	class SocketReader extends Thread {
		String[] streams;

		SocketReader() throws Exception {
			_updateStatusLabel("ClientWorkflowImageStreams: connecting to " +server+ "  port=" +port);
			try {
				socket = new Socket(server, port);
				Utils.log("Socket opened.");
				ois = new ObjectInputStream(socket.getInputStream());
				pw = new PrintWriter(socket.getOutputStream());
				_updateStatusLabel("connected");
			}
			catch(Exception ex) {
				try {
					if ( socket != null) {
						socket.close();
					}
				}
				finally {
					socket = null;
					ois = null;
					pw = null;
				}
				throw ex;
			}
				
		}
		
		private Object _nextChunk() throws Exception {
			Object chunk = null;
			if ( isActive && !isInterrupted() ) {
				chunk = ois.readObject();
			}
			return chunk;
		}
		
		public void run() {
			try {
				send("getInfo");
				send("referenceSpace");
				send("overviewStreamID");
				send("streams");
				send("vectors");
				send("savedPattern");
				
				Utils.log("ois: " +ois);
				
				Object chunk;
				while ( null != (chunk = _nextChunk()) ) {
//					Utils.log("RECEIVED: " +chunk);
					if ( chunk instanceof String ) {
						String str = (String) chunk;
						if ( str.startsWith("overviewStreamID:") ) {
							String overviewStreamID = str.substring("overviewStreamID:".length());
							setOverviewStreamID(overviewStreamID);
						}
						else if ( str.startsWith("savedPattern:") ) {
							String savedPattern = str.substring("savedPattern:".length()).trim();
							_updateSavedPattern(savedPattern);
						}
					}
					else if ( chunk instanceof ReferenceSpace ) {
						ReferenceSpace referenceSpace = (ReferenceSpace) chunk;
						_updateReferenceSpace(referenceSpace);
					}
					
					// streams
					else if ( chunk instanceof String[] ) {
						String[] array = (String[]) chunk;
						_updateAvailableIDs(array);
					}
					
					else if ( chunk instanceof StreamDef ) {
						StreamDef streamDef = (StreamDef) chunk;
						_updateAvailableStreamDefs(streamDef);
					}
					
					else if ( chunk instanceof VectorInfo ) {
						VectorInfo vectorInfo = (VectorInfo) chunk;
						_updateAvailableVectors(vectorInfo);
					}
					
					// handle the case of a new image sequence:
					else if ( chunk instanceof NewImageSequence ) {
						// prepare for new image sequence
						NewImageSequence nis = (NewImageSequence) chunk;
						IImage img = nis.getImage();
						istream = nis.getStream();
						img.setStream(istream);

						
						// TODO remove
						adHocConnection = ServerConnections.createAdHocServerConnection(nis);
						img.setServerConnection(adHocConnection);
						
						// chunk is the image in the new sequence:
						chunk = img;
					}
					else if ( chunk instanceof IImage ) {
						IImage img = (IImage) chunk;
						if ( !img.isNull() ) {
							img.setStream(istream);
							
							// TODO remove
							img.setServerConnection(adHocConnection);
//							Utils.log("img.getStreamID() = " +img.getStreamID());
						}
					}

					// continue further processing of IImage chunk got from above:
					if ( chunk instanceof IImage ) {
						IImage img = (IImage) chunk;
						
						String streamName = img.getStreamID();
						
						ClientImageStream imgStream = imgStreams.get(streamName);
						if ( imgStream != null ) {
							if ( !imgStream.putNewImage(img) ) {
								imgStreams.remove(streamName);
								Utils.log("ClientWorkflowImageStreams: COULDN'T PUT IMAGE IN QUEUE");
							}
						}
						else {
							// should have been closed.
							Utils.log("ClientWorkflowImageStreams: STREAM CLOSED? : " +streamName);
							Utils.log("   imgStreams=" +imgStreams);
							break;
						}
					}
					
					else {
						_updateReceivedLabel(chunk.toString());
					}
				}
			}
			catch(Throwable ex) {
				ex.printStackTrace();
				return;
			}
			finally {
				send("bye");
				_updateAvailableStreamDefs(null);
				_updateAvailableVectors(null);
				socketReader = null;
				if ( this.isInterrupted() ) {
					_updateStatusLabel("interrupted.");
				}
				else {
					_updateStatusLabel("complete.");
				}
			}
		}
		
		public void interrupt() {
			try {
				if ( socket != null ) {
					try {
						socket.close();
					}
					catch (IOException ex) {
						Utils.log("Exception while closing socket: " +ex.getMessage());
					}
				}
			}
			finally {
				socket = null;
				isActive = false;
				super.interrupt();
				_updateStatusLabel("socketReader: interrupted.");
			}
		}
	}




	public Map<String, StreamDef> getStreamDefs() {
		return streamDefs;
	}
	public ReferenceSpace getReferenceSpace() {
		return referenceSpace;
	}
	String getSavedPattern() {
		return savedPattern;
	}
	public List<String> getStreamIDs() {
		return streamIDs;
	}

}
