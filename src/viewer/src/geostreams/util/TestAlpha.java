package geostreams.util;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.net.URL;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JPanel;

// a test of BufferedImage with alpha and rendering
// with graphics.drawImage on this JPanel

public class TestAlpha  extends JPanel {
	
	static BufferedImage img1;
	static BufferedImage img2; 
	
	BufferedImage img3;
	int w, h;
	
	TestAlpha() {
		super();
		
		w = img1.getWidth();
		h = img1.getHeight();
		setPreferredSize(new Dimension(w, h));
		
		img3 = new BufferedImage(w, h, 
				BufferedImage.TYPE_4BYTE_ABGR 
		);
		int[] pixels = new int[w * h];
		int offset = 0;
		int scansize = w;
		img3.getRGB(0, 0, w, h, pixels, offset, scansize);
		for (int i = 0; i < pixels.length/2; i++) {
			pixels[i] = 0x880000ff;
		}
		img3.setRGB(0, 0, w, h, pixels, offset, scansize);

	}

	public synchronized void paint(Graphics g) {
		g.drawImage(img1, 0, 0, w/3, h/3, null);
		g.drawImage(img3, 0, 0, w/3, h/3, null);
	}
	
	
	public static void main(String[] args) throws Exception {
		URL url1 = new URL("http://geostreams.ucdavis.edu/~carueda/vizstreams/saved/current/saved_0.jpg");
		img1 = ImageIO.read(url1);
		
		JFrame jframe = new JFrame("test");
		jframe.getContentPane().add(new TestAlpha());
		jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		jframe.pack();
		jframe.setVisible(true);
	}

}
