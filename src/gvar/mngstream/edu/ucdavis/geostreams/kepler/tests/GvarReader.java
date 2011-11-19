/*
 * GvarReader Actor
 * Carlos Rueda-Velasquez 
 */
package edu.ucdavis.geostreams.kepler.tests;

import java.io.InputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;

import ptolemy.actor.IOPort;
import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.UnsignedByteToken;
import ptolemy.data.ArrayToken;
import ptolemy.data.type.ArrayType;
import ptolemy.data.StringToken;
import ptolemy.data.Token;
import ptolemy.data.expr.Parameter;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.Attribute;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;
import ptolemy.kernel.util.SingletonAttribute;
import ptolemy.kernel.util.StringAttribute;

/**
  * This actor connects to a GVAR server and outputs a MNG stream.
  * Initial version based on java.lang.Runtime.exec(cmd)
  *
  * @author Carlos A. Rueda-Velasquez
  * @version $Id: GvarReader.java,v 1.1 2005/02/24 09:27:44 crueda Exp $
  */
public class GvarReader extends TypedAtomicActor {

	/** Construct an actor with the given container and name.
	  *  @param container The container.
	  *  @param name The name of this actor.
	  *  @exception IllegalActionException If the actor cannot be contained
	  *   by the proposed container.
	  *  @exception NameDuplicationException If the container already has an
	  *   actor with this name.
	  */
	public GvarReader(CompositeEntity container, String name)
	throws IllegalActionException, NameDuplicationException {
		super(container, name);

		gvnetapi_p = new StringParameter(this, "gvnetapi");
		gvnetapi_p.setExpression("/home/carueda/bin/gvnetapi");

		ipAddress_p = new StringParameter(this, "ipAddress");
		ipAddress_p.setExpression("169.237.153.55");

		maxImgs_p = new Parameter(this, "maxImgs");
		maxImgs_p.setExpression("0");
        maxImgs_p.setTypeEquals(BaseType.INT);

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(BaseType.UNSIGNED_BYTE);

		_attachText("_iconDescription",
		            "<svg>\n"
		            + "<rect x=\"0\" y=\"0\" "
		            + "width=\"60\" height=\"30\" "
		            + "style=\"fill:white\"/>\n"
		            + "<text x=\"20\" y=\"25\""
		            + "style=\"font-size:20; fill:blue; font-family:SansSerif\">"
		            + "GvarReader</text>\n"
		            + "</svg>\n"
		);
	}

	/** the gvnetapi command parameter */
	public StringParameter gvnetapi_p;
	
	/** the IP parameter */
	public StringParameter ipAddress_p;
	
	/** the maxImgs parameter */
	public Parameter maxImgs_p;
	
	/** The output port. */
	public TypedIOPort output = null;


	/** 
	  * Starts the connection and outputs the stream.
	  *
	  *  @exception IllegalActionException If there is no director.
	  */
	public void fire() throws IllegalActionException {
        super.fire();

		String gvnetapi = ((StringToken) gvnetapi_p.getToken()).stringValue();
		String ip = ((StringToken) ipAddress_p.getToken()).stringValue();
		final String outfile = "-";
		int maxImgs = Integer.parseInt(((StringToken) maxImgs_p.getToken()).stringValue());
		
		String cmd = gvnetapi+ " " +ip+ " " +outfile+ " " +maxImgs;  
		System.out.println("Executing command: " +cmd);
		try {
			Runtime rt = Runtime.getRuntime();
			Process proc = rt.exec(cmd);

			System.out.println("Preparing to output mng stream ...");
			InputStream is = proc.getInputStream();
			DataInputStream dis = new DataInputStream(is);
			while ( true ) {
				byte[] bytes = { (byte) dis.readUnsignedByte() };
				Token[] byte_tokens = new Token[bytes.length];
				for ( int i = 0; i < bytes.length; i++ ) {
					byte_tokens[i] = new UnsignedByteToken(bytes[i]);
				}
				output.send(0, new ArrayToken(byte_tokens));
			}
		}
		catch (Exception ex) {
			System.out.println("<EXCEPTION>" +ex+ "\n</EXCEPTION>");
		}
	}
}

