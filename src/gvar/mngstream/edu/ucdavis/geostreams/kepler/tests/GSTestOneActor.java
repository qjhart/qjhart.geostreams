/**
 * Basic tests on defining Kepler actors
 *
 * @author Haiyan Yang, Carlos Rueda 
 */
package edu.ucdavis.geostreams.kepler.tests;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.*;
import ptolemy.actor.IOPort;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.Token;
import ptolemy.data.StringToken;
import ptolemy.data.type.BaseType;
import ptolemy.data.type.Type;

/**
 * This actor outputs a simple string message.
 */
public class GSTestOneActor extends TypedAtomicActor {

    public GSTestOneActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
		
		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(BaseType.STRING);

        _attachText("_iconDescription", "<svg>\n" +
                "<rect x=\"0\" y=\"0\" "
                + "width=\"60\" height=\"20\" "
                + "style=\"fill:white\"/>\n" +
                "</svg>\n");
    }

    ///////////////////////////////////////////////////////////////////
    ////                     ports and parameters                  ////

	/** The output port.  The type of this port will be set to String.
     */
    public TypedIOPort output = null;

	
    ///////////////////////////////////////////////////////////////////
    ////                         public methods                    ////

    public void fire() throws IllegalActionException {
        super.fire();
		output.send(0, new StringToken("Welcome to GSTestOne!"));
    }
}

