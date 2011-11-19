/**
 * 
 */
package geostreams.workflow;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;


/**
 * @author Carlos Rueda
 * @version $Id: NIServer.java,v 1.3 2007/01/09 05:57:21 crueda Exp $
 */
class NIServer extends Thread {
	private ServerSocket serverSocket;
	private NIManager niManager;
	private int port;
	private int clientTimeout;

	public NIServer(NIManager niManager, int port, int serverTimeout, int clientTimeout) throws Exception {
		super("NIServer");
		this.niManager = niManager;
		this.port = port;
		this.clientTimeout = clientTimeout;
		serverSocket = new ServerSocket(this.port);
		if ( serverTimeout > 0 ) {
			serverSocket.setSoTimeout(serverTimeout);
		}
		System.out.println("||||||||||||||||||||||||||||||||||||||||||||");
		System.out.println("|||NIServer LISTENING on port: " +port+ "...");
		System.out.println("|||  serverTimeout=" +serverTimeout+ "  clientTimeout=" +clientTimeout);
		System.out.println("||||||||||||||||||||||||||||||||||||||||||||");
	}
	
	public void interrupt() {
		try {
			serverSocket.close();
		}
		catch (IOException ex) {
			System.out.println("Exception while closing socket: " +ex.getMessage());
		}
		finally {
			super.interrupt();
		}
	}

	public void run() {
		while ( !Thread.currentThread().isInterrupted() ) {		
			Socket clientSocket = null;
			try  {
				clientSocket = serverSocket.accept();
				System.out.println("NIServer: client accepted");
				
				if ( clientTimeout > 0 ) {
					clientSocket.setSoTimeout(clientTimeout);
				}
				
				niManager.clientAccepted(clientSocket);
			}
			catch (SocketTimeoutException ex) {
				System.out.print("."); System.out.flush();
			}
			catch (Exception ex) {
				System.out.println("NIServer: Exception: error dispatching client: " +ex.getMessage());
				break;
			}
		}
		try {
			serverSocket.close();
		}
		catch (Exception ex) {
			System.out.println("NIServer: Exception: error closing serverSocket: " +ex.getMessage());
		}
	}
}
