/**
 * 
 */
package geostreams.workflow;

/**
 * @author Carlos Rueda
 * @version $Id: IWorkflowObserver.java,v 1.1 2006/11/18 03:15:58 crueda Exp $
 */
public interface IWorkflowObserver {
	
	/**
	 * Called when the execution if the model is about to be run 
	 * 
	 * @param view 
	 * @param port 
	 * @param serverTimeout 
	 * @param clientTimeout 
	 */
	public void executionStarted(IWorkflow view, int port, int serverTimeout, int clientTimeout)
	throws Exception;

	/**
	 * Called when the execution has been paused.
	 */
	public void executionPaused();

	/**
	 * Called when the execution has finished
	 */
	public void executionFinished();
}
