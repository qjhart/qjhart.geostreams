package geostreams.ptolemy.actor;

import geostreams.ptolemy.VergilUtils;

import java.io.File;
import java.util.HashMap;
import java.util.Map;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;
/**
 * for quick tests. 
 * @author Carlos Rueda-Velasquez
 */
public class TestActor1 extends TypedAtomicActor {
	/** {@inheritDoc} */
	public TestActor1(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name);

		Map<String,Object> props = new HashMap<String,Object>();
        props.put("name", "test");
		_attachText("_iconDescription", ActorUtilities.getIconText(props));
	}

    public void initialize() throws IllegalActionException {
    	VergilUtils.openLibrary(new File(
    			"/home/carueda/workspace/viewer/workflows/geostreamslib.xml"));
    }
}
