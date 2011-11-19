package geostreams.ptolemy;

import geostreams.ptolemy.icon.NetInterfaceIcon;
import ptolemy.actor.TypedAtomicActor;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;


/**
 * Base Workflow as an extension of TypedAtomicActor.
 * 
 * @author Carlos Rueda
 * @version $Id: AbstractWorkflow.java,v 1.7 2007/05/26 07:01:50 crueda Exp $
 */
abstract class AbstractWorkflow extends TypedAtomicActor {
	
	protected NetInterfaceIcon _icon;
	
	public AbstractWorkflow(CompositeEntity container, String name) throws IllegalActionException, NameDuplicationException {
		super(container, name);
		_icon = new NetInterfaceIcon(this, "_icon");
	}
	
	protected void log(String msg) {
		System.out.println("         " +msg);
	}
}
