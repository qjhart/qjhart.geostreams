package geostreams.workflow;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.nio.channels.ByteChannel;

import geostreams.image.IImage;
import geostreams.image.NewImageSequence;

/**
 * @author Carlos Rueda
 * @version $Id: RemoteClient.java,v 1.11 2007/05/31 09:01:55 crueda Exp $
 */
public class RemoteClient {

	//c1: 11/17/06 
	// br.close() will block if there is a concurrent br.readLine().
	// Hence, close() will only be called on the socket: clientSocket.close(),
	// which, for example, will effectively make any blocking operation terminate with
	// an IOException.

	public interface IObserver {
		void clientDied(RemoteClient client);
	}

	private class NullObserver implements IObserver {
		public void clientDied(RemoteClient client) {
		}
	}

	private IObserver observer = new NullObserver();

	// use channel mechanism to read in info from the server?
	// NOTE: this mechanism is not yet well implemented here.
	private static final boolean useChannel = false;

	private Socket clientSocket;

	private ByteChannel channel;

	private ByteBuffer byteBuffer;

	private BufferedReader br;

	// data is sent as objects to the client
	private ObjectOutputStream oos;

	private volatile boolean isActive;

	private Throwable lastException;

	private String requestedStream = null;

	public RemoteClient(IObserver observer, Socket clientSocket) {
		this.observer = observer;
		this.clientSocket = clientSocket;

		try {
			if (useChannel) {
				channel = clientSocket.getChannel();
				byteBuffer = ByteBuffer.allocate(10 * 1024);
			}
			else {
				br = new BufferedReader(new InputStreamReader(clientSocket
						.getInputStream()));
			}

			oos = new ObjectOutputStream(clientSocket.getOutputStream());

			isActive = true;
		}
		catch (Throwable ex) {
			_end(ex);
		}
	}

	private synchronized void _end(Throwable ex) {
		if (!isActive) {
			return;
		}

		observer.clientDied(this);

		lastException = ex;
		isActive = false;
		br = null;
		oos = null;

		try {
			if (channel != null) {
				channel.close();
			}
		}
		catch (Exception ex2) {
			System.out.println("Exception while closing channel: "
					+ ex2.getMessage());
		}
		channel = null;

		try {
			if (clientSocket != null) {
				clientSocket.close();
			}
		}
		catch (Exception ex2) {
			System.out.println("Exception while closing socket: "
					+ ex2.getMessage());
		}
		clientSocket = null;

	}

	// from:
	// https://www.limewire.org/fisheye/browse/limecvs/core/com/limegroup/gnutella/io/BufferUtils.java?r=1.3
	/**
	 * Reads data from the ByteBuffer, inserting it into the StringBuffer, until
	 * a full line is read. Returns true if a full line is read, false if more
	 * data needs to be inserted into the buffer until a full line can be read.
	 */
	private static boolean _readLine(ByteBuffer buffer, StringBuffer sBuffer) {
		int c = -1; // the character just read
		while (buffer.hasRemaining()) {
			c = buffer.get();
			switch (c) {
			// if this was a \n character, we're done.
			case '\n':
				return true;
			// if this was a \r character, ignore it.
			case '\r':
				continue;
			// if it was any other character, append it to the buffer.
			default:
				sBuffer.append((char) c);
			}
		}

		return false;
	}

	public String readLine() {
		String line = _readLine();
//		if ( line != null ) {
//			System.out.println("FROM CLIENT: [" +line+ "]");
//		}
		return line;
	}
	private String _readLine() {
		if (isActive) {
			try {
				if (useChannel) {
					StringBuffer sb = new StringBuffer();
					while (isActive) {
						channel.read(byteBuffer);
						if (_readLine(byteBuffer, sb)) {
							return sb.toString();
						}
						byteBuffer.rewind();
					}
				}
				else {
					while (isActive) {
						try {
							return br.readLine();
						}
						catch (SocketTimeoutException ex) {
							System.out.print("^");
							System.out.flush();
						}
					}
				}
			}
			catch (Throwable ex) {
				_end(ex);
			}
		}
		return null;
	}

	NewImageSequence nis = null;

	public synchronized void send(Object chunk) {
//		System.out.println("  TO CLIENT(" +isActive+ "): " +chunk);
				
		if (!isActive) {
			return;
		}

		try {
			if (chunk instanceof IImage) {
				IImage img = (IImage) chunk;

				if (nis == null) {
					// so, we're just to start sending images
					nis = new NewImageSequence(img);
					System.out.println("---------00000000000  Writing NIS------");
					oos.writeObject(nis);
					oos.flush();
					return;
				}
			}
			oos.writeObject(chunk);
			oos.flush();
		}
		catch (Throwable ex) {
			_end(ex);
		}
	}

	public void sendNoSuchStream(String streamName) {
		String msg = "Inexistent stream of name " +streamName;
		send(msg);
		System.out.println(msg);
	}
	
	public void sendNoSuchVector(String vectorName) {
		String msg = "Inexistent vector of name " +vectorName;
		send(msg);
		System.out.println(msg);
	}
	
	public void close() {
		if (isActive) {
			Throwable ex = null;
			System.out.println("RemoteClient: closing");
			try {
				if (useChannel) {
					channel.close();
					channel = null;
				}
				else {
					//c1: br.close();
					br = null;
				}

				//c1: oos.close();
				oos = null;

				clientSocket.close();
				clientSocket = null;
			}
			catch (Throwable ex2) {
				ex = ex2;
			}
			finally {
				_end(ex);
			}
		}
	}

	public synchronized Throwable getLastException() {
		return lastException;
	}

	public synchronized boolean isActive() {
		return isActive;
	}

	public Socket getClientSocket() {
		return clientSocket;
	}

	public String getRequestedStream() {
		return requestedStream;
	}

	public void setRequestedStream(String requestedStream) {
		this.requestedStream = requestedStream;
	}

}
