/*
	GeoStreams Project
	CumulativeImage
	Carlos A. Rueda
	$Id: CumulativeImage.java,v 1.8 2004/07/29 20:33:50 crueda Exp $
*/
package geostreams.mngstream;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import java.net.*;
import java.awt.image.*;
import java.awt.*;
import javax.imageio.ImageIO;

/**
 Creates an image comprised of multiple Png images.
 @version $Id: CumulativeImage.java,v 1.8 2004/07/29 20:33:50 crueda Exp $
 */
public class CumulativeImage {
	private final Dimension imgSize;
	private double scaleX;
	private double scaleY;
	private BufferedImage all_img;
	private Graphics2D all_g2d;
	private Dimension fulldim;
	private int numPngs;
	
	/** Starts the creation of an cumulative image.
	    @param imgSize Size of the image
	    @param bgColor Background color for the image
	 */
	public CumulativeImage(Dimension imgSize, Color bgColor) {
		this.imgSize = imgSize;
		all_img = new BufferedImage(
			imgSize.width, imgSize.height, 
			BufferedImage.TYPE_INT_RGB
		);
		all_g2d = all_img.createGraphics();
		all_g2d.setColor(bgColor);
		all_g2d.fillRect(0, 0, imgSize.width, imgSize.height);
		numPngs = 0;
	}
	
	/** sets the dimension of the full image. */
	public void setFullDimension(Dimension fulldim) {
		this.fulldim = fulldim;
		scaleX = (double) imgSize.width / fulldim.width;
		scaleY = (double) imgSize.height / fulldim.height;
		//System.out.println("Scales: " +scaleX+ " , " +scaleY);
	}
	
	
	/** Paints a PNG to this image. */
	public synchronized void addPng(MngInputStream.Png png) {
		if ( fulldim == null )
			return;
		BufferedImage img = png.getImage(); 
		_drawImage(img, png.getX(), png.getY());
		numPngs++;
		img.flush();
	}
	
	/** Gets the number of PNGs that have been added. */
	public synchronized int getNumPngs() {
		return numPngs;
	}
	
	/** draws a given image in a location. */
	private void _drawImage(BufferedImage img, int x, int y) {
		
		//System.err.println(x+ ", " +y+ ", " +img.getWidth()+ ", " +img.getHeight());
		
		int vx0 = (int) Math.round(scaleX * x);
		int vx1 = (int) Math.round(scaleX * (x + img.getWidth() -1));
		
		int vy0 = (int) Math.round(scaleY * y);
		int vy1 = (int) Math.round(scaleY * (y + img.getHeight() -1));
		
		int vw = vx1 - vx0 + 1;
		int vh = vy1 - vy0 + 1;
		if ( vh < 1 )
			vh = 1;
		
		all_g2d.drawImage(img, vx0, vy0, vw, vh, null);
	}
	
	/** Saves the current contents of the image.
	    @param filename Its extension sets the format.
	 */
	public synchronized void save(String filename) throws IOException {
		String format = filename.substring(1 + filename.lastIndexOf("."));
		File f = new File(filename);
		ImageIO.write(all_img, format, f);
	}
	
	/** Sets the current contents of the image. 
	   @param url URL of the image to read in.
	 */
	public synchronized void load(URL url) throws IOException {
		BufferedImage img = ImageIO.read(url);
		all_g2d.drawImage(img, 0, 0, all_img.getWidth(), all_img.getHeight(), null);
	}
}


