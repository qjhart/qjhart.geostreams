package geostreams.workflow;

import geostreams.image.ReferenceSpace;
import geostreams.image.StreamDef;
import geostreams.image.VectorInfo;

import java.util.List;

/**
 * Interface to interact with a workflow.
 * 
 * @author Carlos Rueda
 * @version $Id: IWorkflow.java,v 1.11 2007/06/05 06:29:07 crueda Exp $
 */
public interface IWorkflow {

	/**
	 * Gets info about the available streams in this workflow.
	 * 
	 * @return info about the available streams in this workflow.
	 */
	public List<String> getStreamIDs();
	
	
	/**
	 * Gets info about the available streams in this workflow.
	 * 
	 * @return info about the available streams in this workflow.
	 */
	public List<StreamDef> getStreamDefs();
	
	
	/**
	 * Adds a remote client to the workflow. The workflow should start
	 * sending the requested stream to the client.
	 *  
	 * @param remoteClient
	 * @throws Exception
	 */
	public void addRemoteClient(RemoteClient remoteClient) throws Exception;
	
	/**
	 * Removes a remote client from this workflow. No more data will be sent to
	 * this client.
	 * 
	 * @param remoteClient
	 */
	public void removeRemoteClient(RemoteClient remoteClient);

	/**
	 * @return the list of vectors in the model.
	 */
	public List<VectorInfo> getVectorInfos();

	/**
	 * Gets the vector info of the requested vector name.
	 * @param vectorName
	 * @return the vector info object. null if non-existent.
	 */
	public VectorInfo getVectorInfo(String vectorName);
	
	/**
	 * @return the associated reference space.
	 */
	public ReferenceSpace getReferenceSpace();

	public String getSavedPattern();
	
	public void incrClients(int incr);
}
