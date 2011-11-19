/**
 * 
 */
package geostreams.workflow;

import geostreams.image.StreamDef;
import geostreams.image.VectorInfo;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * @author Carlos Rueda
 * @version $Id: DispatchRemoteClient.java,v 1.14 2007/06/12 16:51:55 crueda Exp $
 */
class DispatchRemoteClient extends Thread {
	private NIManager manager;
	private RemoteClient remoteClient;
	private Set<String> requestedStreams = new HashSet<String>();
	
	public DispatchRemoteClient(NIManager manager, RemoteClient remoteClient) throws Exception {
		super("DispatchRemoteClient: " +remoteClient.getClientSocket().getPort());
		this.manager = manager;
		this.remoteClient = remoteClient;
	}

	public void run() {
		System.out.println("DispatchRemoteClient: entering run(): " +remoteClient);
		try  {
			_run();
		} 
		catch (Exception ex)  {
			System.out.println("DispatchRemoteClient: error while processing connection: " +ex.getMessage());
			ex.printStackTrace();
		}
		System.out.println("DispatchRemoteClient: exiting run(): " +remoteClient);
	}
	
	private void _run() throws Exception {
		String inputLine;

		while ( !Thread.currentThread().isInterrupted() ) {
			
			inputLine = remoteClient.readLine();
			if ( inputLine == null ) {
				break;
			}

			// alive
			if ( inputLine.equals("alive") ) {
				System.out.print("!"); System.out.flush();
				// and just continue reading commands.
				continue;
			}
			
			// status
			if ( inputLine.equals("status") ) {
				remoteClient.send("OK");
			}

			// streams
			else if ( inputLine.equals("streams") ) {
				
				if ( true ) {
					for (StreamDef streamDef : manager.getWorkflow().getStreamDefs() ) {
						remoteClient.send(streamDef);
					}
				}
				else {
					List<String> streamIDs = manager.getWorkflow().getStreamIDs();

					System.out.println("SEND: array " +streamIDs);
					String[] array = new String[streamIDs.size()];
					remoteClient.send(streamIDs.toArray(array));
				}
			}
			
			// getInfo  TODO which info besides the command "streams" above?
			else if ( inputLine.equals("getInfo") ) {
//				for (ChannelDef channelDef : manager.getWorkflow().getChannelDefs() ) {
//					remoteClient.send(channelDef);
//				}
			}
			
			// overviewStreamID
			else if ( inputLine.equals("overviewStreamID") ) {
				String overviewStreamID = null;
				List<StreamDef> streamDefs = manager.getWorkflow().getStreamDefs();
				if ( streamDefs.size() > 0 ) {
					overviewStreamID = streamDefs.get(0).getStreamID(); 
				}

				if ( overviewStreamID != null ) {
					remoteClient.send("overviewStreamID:" +overviewStreamID);
				}
			}
			
			// referenceSpace
			else if ( inputLine.equals("referenceSpace") ) {
				remoteClient.send(manager.getWorkflow().getReferenceSpace());
			}
			
			// vectors
			else if ( inputLine.equals("vectors") ) {
				for (VectorInfo vectorInfo : manager.getWorkflow().getVectorInfos() ) {
					remoteClient.send(vectorInfo);
				}
			}
			
			// savedPattern
			else if ( inputLine.equals("savedPattern") ) {
				remoteClient.send("savedPattern:" +manager.getWorkflow().getSavedPattern());
			}
			
			// getStream
			else if ( inputLine.startsWith("getStream:") ) {
				String streamName = inputLine.substring("getStream:".length());
				if ( !requestedStreams.contains(streamName) ) {
					
					// TODO cleanup the following once the new mechanism is fully tested:
					
					// this first check is based on old mechanism; we keep it for supporting
					// the old demo workflows:
					if ( manager.existsStream(streamName) ) {
						remoteClient.setRequestedStream(streamName);
						manager.sendStreamToRemoteClient(remoteClient);
						requestedStreams.add(streamName);
					}
					else {
						// But this is the new mechanism for the new workflows:
						if ( manager.getWorkflow().getStreamIDs().contains(streamName) ) {
							remoteClient.setRequestedStream(streamName);
							manager.sendStreamToRemoteClient(remoteClient);
							requestedStreams.add(streamName);
						}
						else {
							remoteClient.sendNoSuchStream(streamName);
						}
					}
				}
				else {
					remoteClient.send("NI: stream already being sent: " +streamName);
					System.out.println("NI: stream already being sent: " +streamName);
				}
			}
			
			// getVector
			else if ( inputLine.startsWith("getVector:") ) {
				String vectorName = inputLine.substring("getVector:".length());
				if ( manager.existsVector(vectorName) ) {
					manager.sendVectorToRemoteClient(remoteClient, vectorName);
				}
				else {
					remoteClient.sendNoSuchVector(vectorName);
				}
			}
			
			// bye
			else if ( inputLine.equals("bye") ) {
				break;
			}

			else {
				System.out.println("DispatchRemoteClient: unrecognized command: " +inputLine);					
				remoteClient.send("DispatchRemoteClient: unrecognized command: " +inputLine);					
			}
		}

		remoteClient.close();
	}
}
