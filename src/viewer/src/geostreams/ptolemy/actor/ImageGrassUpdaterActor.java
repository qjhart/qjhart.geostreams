// NEEDS REVIEW
/*
 * ECS289F Project
 * Carlos Rueda-Velasquez, Haiyan Yang
 */
package geostreams.ptolemy.actor;

import geostreams.grass.ImageGrassUpdater;
import geostreams.image.IImage;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;
import ptolemy.data.Token;
import ptolemy.data.ObjectToken;
import ptolemy.data.expr.StringParameter;

import javax.swing.JFrame;
import javax.swing.JTextArea;

/**
  * ImageGrassUpdaterActor.
  * This actor reads rows froms its input and updates a GRASS
  * database with frames extracted.
  *
  * @author Carlos Rueda-Velasquez, Haiyan Yang
  * @version $Id: ImageGrassUpdaterActor.java,v 1.1 2006/11/28 20:00:19 crueda Exp $
  */
public class ImageGrassUpdaterActor extends TypedAtomicActor {

	/** Generated files go to this GRASS mapset */ 
	private static final String defaultMachine = "geostreams.cs.ucdavis.edu";
	private static final String defaultMapsetDirectory = "/home/haiyan/grassdata/geostreamtest/qjhart";
	
	/** Machine that has the GRASS database. */
	public StringParameter machine_p;
	
	/** GRASS directory corresponding to mapset under which 
	  * the raster will be uploaded. */
	public StringParameter grass_dir_p;
	
	/** The port from which images are read */
	public TypedIOPort input = new TypedIOPort(this, "input", true, false);
	
	/** Once a raster is updated, its name its output in this port */
	public TypedIOPort output_filename = new TypedIOPort(this, "outputfile_name", false, true);

	private ImageGrassUpdater grassUpdater = null;
	
	/** a window to display current update status. */
	private JFrame jframe = null;
	private JTextArea jtext;


	/** Construct an actor with the given container and name.
	 *  @param container The container.
	 *  @param name The name of this actor.
	 *  @exception IllegalActionException If the actor cannot be contained
	 *   by the proposed container.
	 *  @exception NameDuplicationException If the container already has an
	 *   actor with this name.
	 */
	public ImageGrassUpdaterActor(CompositeEntity container, String name)
	throws IllegalActionException, NameDuplicationException {

		super(container, name);

		//
		// parameters
		//
		machine_p = new StringParameter(this, "Machine with GRASS database");
		machine_p.setExpression(defaultMachine);
		grass_dir_p = new StringParameter(this, "GRASS mapset directory");
		grass_dir_p.setExpression(defaultMapsetDirectory);

		//
		// Input port
		//
		input.setTypeEquals(BaseType.OBJECT);
		input.setMultiport(false);

		//
		// output port
		//
		output_filename.setTypeEquals(BaseType.STRING);

		_attachText("_iconDescription", 
			ActorUtilities.getIconText(105, "Updater")
		);
	}

    public void initialize() throws IllegalActionException {
        super.initialize();
		//System.out.println("ImageGrassUpdaterActor.initialize");
		
		if ( jframe == null ) {
			jtext = new JTextArea();
			jframe = new JFrame("Image GRASS Updater");
			jframe.getContentPane().add(jtext);
			jframe.setSize(350,100);
			jframe.setLocation(600,100);
			jframe.setVisible(true);
		}
		
		try {
			jtext.setText("Initializing ImageGrassUpdater");
			if ( grassUpdater == null ) {
				grassUpdater = new ImageGrassUpdater();
			}
			else {
				grassUpdater.initialize();
			}
		}
		catch(Exception ex) {
			throw new IllegalActionException(this, ex.getMessage());
		}
    }

   /** True iff there is currently a frame being created.
     */
    public boolean prefire() throws IllegalActionException {
		return grassUpdater.isFrameBeingCreated() && super.prefire();
    }

	/** adds a new incoming image to the updater */
	public void fire() throws IllegalActionException {
        super.fire();
		//System.out.println("ImageGrassUpdaterActor.fire");
		try {
			Token token = input.get(0);
			ObjectToken obj = (ObjectToken) token;
			IImage img = (IImage) obj.getValue();
			grassUpdater.addImage(img);
		}
		catch (Exception ex) {
			throw new IllegalActionException(this, ex, ex.getMessage());
		}
		if ( jtext != null ) {
			jtext.setText("\n  " +
				grassUpdater.getNumImages()+ " rows collected    " +
				grassUpdater.getNumFrames()+ " frames transferred"
			);
		}
	}
}

