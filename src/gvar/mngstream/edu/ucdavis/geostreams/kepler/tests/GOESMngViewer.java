/*
 * GOESMngViewer Actor
 * Carlos Rueda-Velasquez 
 */
package edu.ucdavis.geostreams.kepler.tests;

import geostreams.mngstream.applet.MngViewer;
import geostreams.mngstream.applet.MainPanel;

import java.io.InputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.net.URL;
import java.net.MalformedURLException;

import ptolemy.actor.IOPort;
import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.actor.lib.Sink;
import ptolemy.data.StringToken;
import ptolemy.data.Token;
import ptolemy.data.expr.Parameter;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
  * This actor opens a MNG stream and displays the images in it.
  *
  * @author Carlos A. Rueda-Velasquez
  * @version $Id: GOESMngViewer.java,v 1.1 2005/02/24 09:27:44 crueda Exp $
  */
public class GOESMngViewer extends Sink {

	/** Construct an actor with the given container and name.
	  *  @param container The container.
	  *  @param name The name of this actor.
	  *  @exception IllegalActionException If the actor cannot be contained
	  *   by the proposed container.
	  *  @exception NameDuplicationException If the container already has an
	  *   actor with this name.
	  */
	public GOESMngViewer(CompositeEntity container, String name)
	throws IllegalActionException, NameDuplicationException {
		super(container, name);

		mngIn_p = new StringParameter(this, "mngIn");
		mngIn_p.setExpression("http://casil.ucdavis.edu/~crueda/goes/goes");

		saved_filename_p = new StringParameter(this, "saved_filename");
		saved_filename_p.setExpression("http://casil.ucdavis.edu/~crueda/goes/saved_vis.jpg");

		_attachText("_iconDescription",
		            "<svg>\n"
		            + "<rect x=\"0\" y=\"0\" "
		            + "width=\"80\" height=\"20\" "
		            + "style=\"fill:white\"/>\n"
		            + "<text x=\"2\" y=\"15\""
		            + "style=\"font-size:10; fill:blue; font-family:SansSerif\">"
		            + "GOESMngViewer</text>\n"
		            + "</svg>\n"
		);
	}

	/** the mngIn parameter */
	public StringParameter mngIn_p;
	
	/** the saved_filename parameter */
	public StringParameter saved_filename_p;
	

	/** 
	  * Starts the connection and outputs the stream.
	  *
	  *  @exception IllegalActionException If there is no director.
	  */
	public void fire() throws IllegalActionException {
        super.fire();

		String mngIn = ((StringToken) mngIn_p.getToken()).stringValue();
		String saved_filename = ((StringToken) saved_filename_p.getToken()).stringValue();
		URL saved_url = null;
		
		try {
			saved_url = new URL(saved_filename);
		}
		catch(MalformedURLException ex) {
			// ignore.
		}
		
		MainPanel mainPanel = null;
		if ( saved_url == null ) {
			mainPanel = new MainPanel(mngIn, saved_filename, false);
		}
		else {
			mainPanel = new MainPanel(mngIn, saved_url, false);
		}
		
		boolean gui = true;
		int delay = 0;   // no automatic saving
		
		MngViewer mngViewer = new MngViewer(mainPanel, gui, delay);
		mngViewer.setTitle(mngIn);
		
	}
}

