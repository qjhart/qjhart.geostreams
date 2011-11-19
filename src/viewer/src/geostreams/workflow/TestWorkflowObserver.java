package geostreams.workflow;



/**
 * 
 * @author Carlos Rueda
 * @version $Id: TestWorkflowObserver.java,v 1.1 2006/11/18 03:15:58 crueda Exp $
 */
public class TestWorkflowObserver implements IWorkflowObserver {

	public void executionStarted(IWorkflow view, int port, int serverTimeout, int clientTimeout) {
		System.out.println("TestWorkflowObserver.executionStarted");
		System.out.println("   view:" +view);
		System.out.println("   port:" +port);
		System.out.println("   serverTimeout:" +serverTimeout);
		System.out.println("   clientTimeout:" +clientTimeout);
		
	}

	public void executionPaused() {
		System.out.println("TestWorkflowObserver.executionPaused");
	}
	
	public void executionFinished() {
		System.out.println("TestWorkflowObserver.executionFinished");
	}
}
