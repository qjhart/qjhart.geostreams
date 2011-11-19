package geostreams.image;

import java.awt.Rectangle;
import java.io.Serializable;

/**
  * Defines the space (currently a rectangle) in which all frames
  * associated to a given stream are expected to be contained.
  * 
  * This concept is more useful for clients interested in doing some
  * kind of visualization, like the applet viewer, since they will use
  * this to allocate the area in which incoming images (rows) will be
  * displayed.
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: ReferenceSpace.java,v 1.4 2007/06/04 21:18:08 crueda Exp $
  */
public class ReferenceSpace implements Serializable {
	Rectangle rect;
	
	public ReferenceSpace() {
		rect = new Rectangle();
	}

	public ReferenceSpace(int x, int y, int w, int h) {
		this.rect = new Rectangle(x, y, w, h);
	}

	public Rectangle getRectangle() {
		return rect;
	}
	
	public String toString() {
		return "x=" +rect.x+ ", y=" +rect.y+ ", w=" +rect.width+ ", h=" +rect.height;
	}
	
	public boolean equals(Object obj) {
		return obj instanceof ReferenceSpace
		    && rect.equals(((ReferenceSpace) obj).rect)
		;
	}
	
	public ReferenceSpace clone() {
		ReferenceSpace rs = new ReferenceSpace();
		rs.rect = new Rectangle(this.rect);
		return rs;
	}
	
}

