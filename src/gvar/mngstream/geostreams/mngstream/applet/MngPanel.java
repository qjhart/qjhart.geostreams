/*
	GeoStreams Project
	MngPanel - Display area
	Carlos A. Rueda
	$Id: MngPanel.java,v 1.3 2004/07/29 02:46:15 crueda Exp $
*/
package geostreams.mngstream.applet;

import geostreams.mngstream.MngInputStream;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import java.net.*;
import java.awt.image.*;
import java.awt.*;
import javax.imageio.ImageIO;
import javax.swing.*;

/**
 Display area.
 @version $Id: MngPanel.java,v 1.3 2004/07/29 02:46:15 crueda Exp $
 */
class MngPanel extends JPanel {
	private MainPanel mngViewer;
	private final Dimension imgSize;
	private double scaleX;
	private double scaleY;
	private MngInputStream.Png beingAdded = null;
	private BufferedImage all_img;
	private Graphics2D all_g2d;
	private Dimension fulldim;
	
	private int zoom = 100;
	private final Dimension prefSize = new Dimension();
	
	private final Color bgColor = Color.black;
	
	private Color indicatorColor = null;
	
	private boolean my_repaint = true;

	private MouseMotionListener mouse  = new MouseMotionAdapter() {
		public void mouseMoved(MouseEvent ev) {
			double x = 1 + ev.getX();
			double y = 1 + ev.getY();
			if ( zoom == 100 ) {
				x /= scaleX;
				y /= scaleY;
			}
			else {
				x *= 100 / scaleX / zoom;
				y *= 100 / scaleY / zoom;
			}
			mngViewer.mouseMoved((int) Math.round(x), (int) Math.round(y));
		}
	};

	MngPanel(MainPanel mngViewer, Dimension imgSize, Color indicatorColor) {
		super(new GridLayout(1,1));
		this.mngViewer = mngViewer;
		this.imgSize = imgSize;
		this.indicatorColor = indicatorColor;
		
		all_img = new BufferedImage(
			imgSize.width, imgSize.height, 
			BufferedImage.TYPE_INT_RGB
			//BufferedImage.TYPE_4BYTE_ABGR_PRE
		);
		all_g2d = all_img.createGraphics();
		//all_g2d.setPaint(bgColor);
		all_g2d.setColor(bgColor);
		all_g2d.fillRect(0, 0, imgSize.width, imgSize.height);
		
		
		// NOTE:
		// this is to allow for mouse location
		// even that the mng stream has not been loaded.  
		// See the mouse listener above.
		scaleX = (double) imgSize.width / 30680;
		scaleY = (double) imgSize.height / 15787;
		addMouseMotionListener(mouse);

		updatePreferredSize();
	}
	
	/** sets the dimension of the full image. */
	public void setFullDimension(Dimension fulldim) {
		this.fulldim = fulldim;
		scaleX = (double) imgSize.width / fulldim.width;
		scaleY = (double) imgSize.height / fulldim.height;
System.out.println("Scales: " +scaleX+ " , " +scaleY);
	}
	
	public Dimension getPreferredSize() {
		return prefSize;
	}
	
	synchronized void addPng(MngInputStream.Png png) {
		if ( fulldim == null )
			return;
		beingAdded = png;
		_drawImage(beingAdded.getImage(), beingAdded.getX(), beingAdded.getY());
		_repaint();
	}
	
	private void _drawImage(BufferedImage img, int x, int y) {
		int vx0 = (int) Math.round(scaleX * x);
		int vx1 = (int) Math.round(scaleX * (x + img.getWidth() -1));
		
		int vy0 = (int) Math.round(scaleY * y);
		int vy1 = (int) Math.round(scaleY * (y + img.getHeight() -1));
		
		int vw = vx1 - vx0 + 1;
		int vh = vy1 - vy0 + 1;
		if ( vh < 1 )
			vh = 1;
		
		all_g2d.drawImage(img, vx0, vy0, vw, vh, null);
		if ( indicatorColor != null ) {
			all_g2d.setColor(indicatorColor);
			all_g2d.fillRect(vx0-1, vy0+1, vw+2, 1);
		}
	}
	
	synchronized public void update(Graphics g) {
		paint(g);
	}
	
	synchronized public void paint(Graphics g) {
		if ( my_repaint ) {
			g.setColor(bgColor);
			g.fillRect(0, 0, Integer.MAX_VALUE, Integer.MAX_VALUE);
			my_repaint = false;
		}
		
		int x = 0;   // 5700;
		int y = 0;   // 2500;
		
		if ( zoom != 100 ) {
			boolean done = g.drawImage(all_img, x, y, 
				(int) (all_img.getWidth()*zoom)/100, 
				(int) (all_img.getHeight()*zoom)/100,
				null
			);
			// System.err.println(done);
		}
		else {
			g.drawImage(all_img, x, y, null);
		}
	}
	
	
	void save(String filename) throws IOException {
		String format = filename.substring(1 + filename.lastIndexOf("."));
		File f = new File(filename);
		ImageIO.write(all_img, format, f);
	}
	void load(URL url) throws IOException {
		BufferedImage img = ImageIO.read(url);
		all_g2d.drawImage(img, 0, 0, all_img.getWidth(), all_img.getHeight(), null);
		_repaint();
	}

	void updatePreferredSize() {
		prefSize.width =  (int) (zoom * imgSize.width ) / 100; 
		prefSize.height = (int) (zoom * imgSize.height) / 100;
	}
	
	void setZoom(int zoom) {
		this.zoom = zoom;
		updatePreferredSize();
		_repaint();
	}
	
	synchronized void _repaint() {
		my_repaint = true;
		revalidate(); // so scrollbars get updated
		repaint();
	}
}


