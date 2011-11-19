package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import geostreams.ptolemy.icon.ViewerIcon;

import java.awt.Color;
import java.awt.Font;

import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * TODO
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: Viz3DActor.java,v 1.1 2007/09/16 09:47:03 crueda Exp $
 */
public class Viz3DActor extends StreamSinkActor {
	
	private ViewerIcon _icon;

    /** Construct an actor with the given container and name.
     *  @param container The container.
     *  @param name The name of this actor.
     *  @exception IllegalActionException If the actor cannot be contained
     *   by the proposed container.
     *  @exception NameDuplicationException If the container already has an
     *   actor with this name.
     */
    public Viz3DActor(CompositeEntity container, String name)
	throws IllegalActionException, NameDuplicationException {

        super(container, name, true);
        _icon = new ViewerIcon(this, "_icon");
        _icon.setFont(new Font("SansSerif", Font.PLAIN, 14));
        _icon.setText("Viz3D");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);

    }

    
    protected synchronized void _sink(IImage img) throws IllegalActionException {
    	setStreamIsActive(true);
    	
    }
	
    
    public void stop() {
    	super.stop();
    	reset();
    }
    
    private void reset() {
    	setViewerIsActive(false);
    }

    public void wrapup() throws IllegalActionException {
    	super.wrapup();
		reset();
    }

    
    /** 
     * Sets whether the input stream is active or not.
     * @param active
     */
	private synchronized void setStreamIsActive(boolean active) {
		_icon.setStreamIsActive(active);
	}
    /** 
     * Sets whether the viewer is active or not.
     * @param active
     */
	private synchronized void setViewerIsActive(boolean active) {
		_icon.setIconActive(active);
	}
}
