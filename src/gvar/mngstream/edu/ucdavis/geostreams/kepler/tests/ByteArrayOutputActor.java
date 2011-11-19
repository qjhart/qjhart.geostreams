/*
 * Basic tests on defining Kepler actors
 * Carlos Rueda-Velasquez 
 */
package edu.ucdavis.geostreams.kepler.tests;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.kernel.CompositeEntity;
import ptolemy.data.UnsignedByteToken;
import ptolemy.data.ArrayToken;
import ptolemy.kernel.util.*;
import ptolemy.actor.IOPort;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.Token;
import ptolemy.data.StringToken;
import ptolemy.data.type.BaseType;
import ptolemy.data.type.Type;
import ptolemy.data.type.ArrayType;

/**
 * This actor outputs a byte array
 */
public class ByteArrayOutputActor extends TypedAtomicActor {

	/** Construct an actor with the given container and name.
	  *  @param container The container.
	  *  @param name The name of this actor.
	  *  @exception IllegalActionException If the actor cannot be contained
	  *   by the proposed container.
	  *  @exception NameDuplicationException If the container already has an
	  *   actor with this name.
	  */
    public ByteArrayOutputActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
		
		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(new ArrayType(BaseType.UNSIGNED_BYTE));

        _attachText("_iconDescription", "<svg>\n" +
                "<rect x=\"0\" y=\"0\" "
                + "width=\"60\" height=\"20\" "
                + "style=\"fill:white\"/>\n" +
                "</svg>\n");
    }

    ///////////////////////////////////////////////////////////////////
    ////                     ports and parameters                  ////

	/** The output port.
     */
    public TypedIOPort output = null;

	
    ///////////////////////////////////////////////////////////////////
    ////                         public methods                    ////

    public void fire() throws IllegalActionException {
        super.fire();
		byte[] bytes = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
		Token[] byte_tokens = new Token[bytes.length];
		
		for ( int i = 0; i < bytes.length; i++ ) {
			byte_tokens[i] = new UnsignedByteToken(bytes[i]);
		}
		output.send(0, new ArrayToken(byte_tokens));
    }
}

