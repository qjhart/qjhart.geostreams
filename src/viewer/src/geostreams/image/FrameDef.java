/*
 * GeoStreams Project
 * Carlos Rueda-Velasquez
 */
package geostreams.image;

import java.awt.Rectangle;
import java.io.Serializable;

/**
  * Describes the geometry (currently a rectangle) of a particular 
  * frame in a stream. 
  * An ID is used to identify the frame.
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: FrameDef.java,v 1.3 2007/06/13 00:08:43 crueda Exp $
  */
public class FrameDef implements Serializable {
	/** ID of this frame definition */
	String ID;
	
	/** rectangle associated to this frame. */
	Rectangle rect;

	
	public FrameDef() {
		rect = new Rectangle();
	}
	
	public FrameDef(String ID, Rectangle rect) {
		this.ID = ID;
		this.rect = (Rectangle) rect.clone();
	}
	
	/** ID of this frame definition. */
	public String getID() { return ID; }
	
	/** Gets the rectangle associated to this frame. */
	public Rectangle getRectangle() { return rect; }

	public String toString() {
		return "frame=" +ID;
	}
		
	public String getDescriptiveString() {
		return "ID=" +ID
		     + " x=" +rect.x
		     + " y=" +rect.y
		     + " w=" +rect.width
		     + " h=" +rect.height
		;
	}
}

