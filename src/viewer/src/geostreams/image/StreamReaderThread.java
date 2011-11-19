package geostreams.image;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.InterruptedIOException;
import java.nio.channels.ClosedChannelException;

import javax.swing.Timer;


/** 
  * A reader from a particular stream.
  * This class is instantiated by ServerConnection.
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: StreamReaderThread.java,v 1.5 2007/06/07 05:09:44 crueda Exp $
  */
class StreamReaderThread extends Thread {
	/** Associated connection */
	private ServerConnection gscon;
	
	/** The particular stream from which I'm reading */
	private IImageStream imgStream;
	
	private long lastRead;

	private Timer timer;
	private int timeout; // in seconds
	private boolean isTimeout;
	
	private int simulationDelay = 0; // millis

	
	/**
	 * Creates a reader on the given stream.
	 * Call start() to start the actual reading of data from the stream and
	 * the corresponding notification of events to observers registered to
	 * the given connection.
	 * 
	 * @param _imgObserver The server connection.
	 * @param imgStream The stream to read from.
	 */
	StreamReaderThread(ServerConnection gscon, IImageStream imgStream) {
		super("StreamReaderThread:" +imgStream);
		assert gscon != null;
		this.gscon = gscon;
		this.imgStream = imgStream;
	}

	/** Sets the timeout in seconds */
	synchronized void setTimeout(int timeout) {
		this.timeout = timeout;
	}
	
	synchronized void setSimulationDelay(int simulationDelay) {
		this.simulationDelay = simulationDelay;
	}
	
	public synchronized void start() {
		super.start();
	
		timer = new Timer(1000, new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				long current = System.currentTimeMillis();
				long delay = current - lastRead;
				if ( timeout > 0 && delay > 1000*timeout ) {
					_timeout();
				}
				else if ( delay > 1000*3 ) {
					String msg = "Connected. Waiting for data... ";
					int mins = (int) (delay/1000) / 60;
					int secs = (int) (delay/1000) % 60;
					if ( mins == 0 ) {
						msg += secs+ " secs";
					}
					else {
						msg += mins+ "m:" +secs+ "s";
					}
					gscon.notifyConnectedWaitingData(imgStream.getStreamID(), msg);
				}
			}
		});
		timer.start();
	}
	
	/** Interrupts the thread getting data from the stream.
	  * Does nothing is not applicable.
	  */
	private void _timeout() {
		System.out.println("TIMEOUT!");
		isTimeout = true;
		interrupt();
	}
	
	/** Interrupts the thread getting data from the stream.
	  */
	public void interrupt() {
		if ( timer != null ) {
			timer.stop();
			timer = null;
		}
		System.out.println(
				"-_-_-_-_- I n t e r r u p t i n g -_-_-_-_-\n" +
				"-_-_-_-_- StreamReaderThread: stream=" +imgStream+ "\n" +
				"-_-_-_-_- gscon=" +gscon
		);
		String msg = "Interrupting stream...";
		if ( isTimeout ) {
			msg += " (" +timeout+ "s timeout)";
		}
		
		gscon.notifyReaderFinished(imgStream.getStreamID(), msg);
		
		if ( !this.isInterrupted() ) {
			try {
				super.interrupt();
			}
			catch(java.security.AccessControlException ex){
				// we are under a restricted environment, (eg. applet)
				// just print a message:
				System.out.println("AccessControlException: " + ex.getMessage());
			}
		}
		msg = "Interrupted";
		if ( isTimeout ) {
			msg += " (" +timeout+ "s timeout)";
		}
		gscon.notifyReaderFinished(imgStream.getStreamID(), msg);

		try {
			imgStream.close();
		}
		catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	
	private String msg = "";

	public void run() {
		_run();
		gscon.notifyReaderFinished(imgStream.getStreamID(), msg);
		if ( timer != null ) {
			timer.stop();
			timer = null;
		}
	}
	
	private void _run() {
		try {
			lastRead = System.currentTimeMillis();

			gscon.notifyConnectedWaitingData(imgStream.getStreamID(), "Connected. Waiting for data...");
			
			imgStream.startReading();
			
			IImage img = null;
			lastRead = System.currentTimeMillis();
			
			// start showing incoming images from this connection:
			while ( !Thread.currentThread().isInterrupted() ) {
				
				// read next image:
				img = imgStream.nextImage();
				if ( img == null ) {
					break;
				}
				
				gscon.notifyNewImage(imgStream.getStreamID(), img);
				
				if ( img.isNull() ) {
					break;
				}
				
				lastRead = System.currentTimeMillis();
				
				////// stuff only to simulate a stream read from a file
				////// for demonstration purposes
				if ( simulationDelay > 0 ) {
					String streamID = img.getStreamID();
					ChannelDef chdef = gscon.getStreamDef(streamID).getChannelDef();
					if ( chdef != null ) {
						try {
							Thread.sleep(simulationDelay*chdef.getPixelSizeY());
						}
						catch (InterruptedException ex) {
							break;
						}
					}
					// more delay with 0.01 probability:
					if ( Math.random() < 0.01 ) {
						Thread.sleep(Math.round(1000*(2 + 5*Math.random())));
					}
				}
			}
			imgStream.close();
			
			if ( Thread.currentThread().isInterrupted() ) {
				System.out.println("interrupted stream: " +imgStream);
				msg = "Interrupted";
				if ( isTimeout ) {
					msg += " (" +timeout+ "s timeout)";
				}
			}
			else if ( img == null ) {
				msg = "Connection closed by server";
			}
		}
		catch(ClosedChannelException ex) {
			System.out.println("StreamReaderThread(" +imgStream.getStreamID()+ "): ClosedChannelException : " +ex.getMessage());
			gscon.notifyConnectionException(imgStream.getStreamID(), ex);
		}
		catch(InterruptedIOException ex) {
			System.out.println("StreamReaderThread(" +imgStream.getStreamID()+ "): INTERRUPTED");
			gscon.notifyConnectionException(imgStream.getStreamID(), ex);
//			ex.printStackTrace();
		}
		catch(Exception ex) {
			System.out.println("StreamReaderThread(" +imgStream.getStreamID()+ "): An error occured: " +ex.getMessage());
			gscon.notifyConnectionException(imgStream.getStreamID(), ex);
			ex.printStackTrace();
		}
//		catch(Throwable ex) {
//			System.out.println(": Unexpected throwable: " +ex.getMessage());
//			ex.printStackTrace();
//		}
		finally {
			if ( timer != null ) {
				timer.stop();
				timer = null;
			}
		}				
	}
}

