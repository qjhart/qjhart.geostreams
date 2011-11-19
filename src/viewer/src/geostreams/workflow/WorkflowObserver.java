/**
 * 
 */
package geostreams.workflow;


/**
 * @author Carlos Rueda
 * @version $Id: WorkflowObserver.java,v 1.2 2007/01/08 10:38:53 crueda Exp $
 */
public class WorkflowObserver implements IWorkflowObserver {
	
	private IWorkflow view;
	private NIManager niManager;

	public void executionStarted(IWorkflow view, int port, int serverTimeout, int clientTimeout) 
	throws Exception {
		this.view = view;
		niManager = new NIManager(this, port, serverTimeout, clientTimeout);
	}

	public void executionPaused() {
		System.out.println("WorkflowObserver.executionPaused");
		niManager.executionPaused();
	}

	public void executionFinished() {
		System.out.println("WorkflowObserver.executionFinished");
		niManager.executionFinished();
	}

	IWorkflow getWorkflow() {
		return view;
	}

}
