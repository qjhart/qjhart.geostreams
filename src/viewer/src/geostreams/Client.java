package geostreams;

import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.PrintWriter;
import java.net.ConnectException;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.Timer;

import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.image.IStream;
import geostreams.image.NewImageSequence;
import geostreams.image.ServerConnections;
import geostreams.image.StreamDef;

/**
  * A simple client GUI to test a net-enabled workflow.
  * 
  * @author Carlos Rueda-Velasquez
  * @version $Id: Client.java,v 1.9 2007/06/12 16:51:56 crueda Exp $
  */
public class Client {
	
	private static int numChunksReceived;
	
	private String server;
	private int port;
	
	private SocketReader socketReader;
	private Socket socket;
	private PrintWriter pw;
	private ObjectInputStream ois;

	private IStream istream;

	private IServerConnection adHocConnection = null;
	
	private Timer timer;
	
	private boolean isActive;
	synchronized boolean isActive() {
		return isActive;
	}
	synchronized void setActive(boolean b) {
		isActive = b;
	}

	////// GUI elements ///////
	private JLabel statusLabel = new JLabel("status");
	private JLabel receivedLabel = new JLabel("");

	private JFrame jframe;
	
	public interface IChunkObserver {
		/** Called when a new chunk has been received through the connection. */
		// TODO This would be actually an IImage in a similar way as IConnectionObserver
		public void newChunk(Object chunk);
		public void exceptionThrown(Throwable ex);
		public void end(String msg);
	}
	
	/** The unique observer associated to this client */
	private IChunkObserver chunkObserver = new IChunkObserver() {
		public void newChunk(Object chunk) {}
		public void exceptionThrown(Throwable ex) {}
		public void end(String msg) {}
	};

	/** Sets the unique observer associated to this client */
	public void setChunkObserver(IChunkObserver chunkObserver) {
		this.chunkObserver = chunkObserver;
	}
	
	
	private class MyPanel extends JPanel {
		MyPanel(List<String> streams, String[] vectors) {
			super(new GridLayout(0, 1));
			if ( streams == null || streams.size() == 0 ) {
				addButton("connect", null);
				_updateStatusLabel("Click connect to connect");
			}
			else {
				addButton("disconnect", null);
				addButton("status", null);
				for ( String stream : streams ) {
					addButton("getStream:" +stream, null);
				}
				if ( vectors != null ) {
					for (int i = 0; i < vectors.length; i++) {
						String vector = vectors[i];
						addButton("getVector:" +vector, null);
					}
				}
				receivedLabel.setMaximumSize(new Dimension(250, 250));
				this.add(receivedLabel);
			}
			statusLabel.setMaximumSize(new Dimension(250, 250));
			this.add(statusLabel);
		}
		
		private void addButton(String name, String actionCmd) { 
			JButton button = new JButton(name);
			if ( actionCmd != null ) {
				button.setActionCommand(actionCmd);
			}
			button.addActionListener(al);
			this.add(button);
		}
	}
	
	public Client(JFrame jframe, String server, int port) throws Exception {
		this.jframe = jframe;
		this.server = server;
		this.port = port;

		this.jframe.getContentPane().add(new MyPanel(null, null));
		jframe.pack();
		jframe.setVisible(true);
		
		timer = new Timer(2000, new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if ( !isActive() ) {
					timer.stop();
				}
				send("alive");
			}
		}) ;
		timer.start();
		setActive(true);
	}
	
	private void _updateStatusLabel(final String text) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				statusLabel.setText(text);
			}
		});
	}
	private void _updateReceivedLabel(final String text) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				receivedLabel.setText(text);
			}
		});
	}
	
	private void _updateGui(final List<String> streams, final String[] vectors) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				jframe.getContentPane().removeAll();
				MyPanel myPanel = new MyPanel(streams, vectors);
				jframe.getContentPane().add(myPanel);
				jframe.getContentPane().validate();
				jframe.pack();
				myPanel.requestFocusInWindow();
			}
		});
	}

	synchronized void send(String cmd) {
		if ( socketReader != null ) {
			pw.println(cmd);
			pw.flush();
		}
	}
	
	
	class SocketReader extends Thread {
		final List<String> streams = new ArrayList<String>();
		String[] vectors;

		SocketReader() throws Exception {
			_updateStatusLabel("connecting to " +server+ "  port=" +port);
			try {
				socket = new Socket(server, port);
				System.out.println("Socket opened.");
				ois = new ObjectInputStream(socket.getInputStream());
				pw = new PrintWriter(socket.getOutputStream());
				_updateStatusLabel("connected");
			}
			catch(Exception ex) {
				ex.printStackTrace();
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
		
		public void run() {
			try {
				send("getInfo");
				send("referenceSpace");
				send("overviewStreamID");
				send("streams");
				send("vectors");
				send("savedPattern");
				
				Object chunk;
				while ( isActive() && !isInterrupted() && null != (chunk = ois.readObject()) ) {
					if ( chunk instanceof String ) {
						String str = (String) chunk;
						if ( str.startsWith("vectors|") ) {
							vectors = str.substring("vectors|".length()).split("\\|");
							_updateGui(streams, vectors);
						}
					}
					
					// streams
					else if ( chunk instanceof String[] ) {
						String[] streamIDs = (String[]) chunk;
						for (int i = 0; i < streamIDs.length; i++) {
							System.out.println("CLIENT: streamID=" +streamIDs[i]);
							streams.add(streamIDs[i]);
						}
						_updateGui(streams, vectors);
					}
					
					else if ( chunk instanceof StreamDef ) {
						StreamDef streamDef = (StreamDef) chunk;
						streams.add(streamDef.getStreamID());
						_updateGui(streams, vectors);
					}
					
					// handle the case of a new image sequence:
					if ( chunk instanceof NewImageSequence ) {
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
						}
					}

					chunkObserver.newChunk(chunk);
					_updateReceivedLabel(chunk.toString());
				}
			}
			catch(Throwable ex) {
				chunkObserver.exceptionThrown(ex);
				_updateGui(null, null);
				return;
			}
			finally {
				send("bye");
				socketReader = null;
				if ( this.isInterrupted() ) {
					_updateStatusLabel("interrupted.");
				}
				else {
					chunkObserver.end("complete.");
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
						System.out.println("Exception while closing socket: " +ex.getMessage());
					}
				}
			}
			finally {
				super.interrupt();
				socket = null;
				_updateStatusLabel("socketReader: interrupted.");
			}
		}
	}

	void connect() {
		if ( socketReader == null ) {
			numChunksReceived = 0;
			String exMsg = null;
			try {
				socketReader = new SocketReader();
				socketReader.start();
			}
			catch(ConnectException ex) {
				exMsg = ex.getMessage();
			}
			catch (Exception ex) {
				exMsg = ex.getMessage();
				ex.printStackTrace();
			}
			if ( exMsg != null ) {
				System.out.println("exception: " +exMsg);
				_updateStatusLabel(exMsg);
			}
		}
	}
	
	synchronized void disconnect() {
		if ( socketReader != null ) {
			send("bye");
			socketReader.interrupt();
			socketReader = null;
		}
		_updateGui(null, null);
	}
	
	ActionListener al = new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			String cmd = e.getActionCommand();
			if ( cmd.equals("connect") ) {
				connect();
			}
			else if ( cmd.equals("disconnect") ) {
				disconnect();
			}
			else {
				send(cmd);
			}
		}
	};
	
	public void destroy() {
		 disconnect();
		 setActive(false);
	}

	/** standalone program */
	public static void main(String[] args) throws Exception {
		String server = "localhost";
		int port = 35813;
		for (int i = 0; i < args.length; i++) {
			String arg = args[i];
			if ( arg.equals("-server") ) {
				server = args[++i];
			}
			else if ( arg.equals("-port") ) {
				port = Integer.parseInt(args[++i]);
			}
		}
		JFrame jframe = new JFrame("Client: " +server+ ":" +port);
		jframe.setLocation(700,500);
		jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		Client client = new Client(jframe, server, port);

		IChunkObserver chunkObserver = new IChunkObserver() {
			public void newChunk(Object chunk) {
				System.out.println((++numChunksReceived)+ ": RECEIVED: " +chunk);
			}

			public void exceptionThrown(Throwable ex) {
				System.out.println("exception: " +ex.getMessage());
				if ( !(ex instanceof SocketException) ) {
					ex.printStackTrace();
				}
			}

			public void end(String msg) {}
		}; 
		client.setChunkObserver(chunkObserver);

		client.connect();
		
		Thread.sleep(Long.MAX_VALUE);
	}
	
}

