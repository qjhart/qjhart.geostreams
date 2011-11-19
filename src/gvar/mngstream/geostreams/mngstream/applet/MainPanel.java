/*
	GeoStreams Project
	MainPanel
	Carlos A. Rueda
	$Id: MainPanel.java,v 1.6 2005/02/23 21:06:08 crueda Exp $
*/
package geostreams.mngstream.applet;

import geostreams.mngstream.MngInputStream;
import geostreams.goes.GOESConstants;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import java.net.*;
import java.awt.image.*;
import javax.imageio.ImageIO;
import javax.swing.*;
import java.util.List;

/** Main container with display and controls. */
public class MainPanel extends JPanel {
	//
	// NOTE:
	// Some code here is replicated in MngStreamClient which should be used
	// in a later version through a suitable interface.  pending.
	//
	
	// desired image size	
	private static final int imgWidth =  2000;
	private static final int imgHeight = (int) (GOESConstants.aspect * imgWidth);
	private static final Dimension imgSize = new Dimension(imgWidth, imgHeight);

	private String mng_in;
	private final JLabel status = new JLabel("Click 'Connect' to start connection");
	private final JLabel mouseLabel = new JLabel("mouse location");
	private final JLabel zoomLabel = new JLabel("zoom", JLabel.RIGHT);
	private MngPanel mngPanel;
	private InputStream is;
	private MngInputStream mng;

	private final boolean isApplet;
	private int no_imgs = 0;
	private int no_imgs_saved = 0;
	
	private int zoom = 40;   // initial zoom factor percent
	
	// name of file with saved complete image (e.g. saved.png)
	// this is used to actually save the file
	private String saved_filename;
	
	// corresponding url of the saved file; used to read it.
	private URL saved_url; 
	
	/** Last moment the base image was loaded. */
	private long lastLoaded = 0;

	final JButton bConnect = new JButton("Connect");
	final JButton bStop = new JButton("Stop");
	final JButton bSave = new JButton("Save");
	final JButton bLoad = new JButton("Load");
	final JButton bZoomIn = new JButton("Zoom in");
	final JButton bZoomOut = new JButton("Zoom out");
	
	private Thread connectionThread;

	public MainPanel(String mng_in, String saved_filename, boolean isApplet) {
		this(mng_in, saved_filename, null, isApplet);
	}
	
	public MainPanel(String mng_in, URL saved_url, boolean isApplet) {
		this(mng_in, null, saved_url, isApplet);
	}
	
	public MainPanel(String mng_in, String saved_filename, URL saved_url, boolean isApplet) {
		super(new BorderLayout());
		this.mng_in = mng_in;
		this.saved_filename = saved_filename;
		this.saved_url = saved_url;
		this.isApplet = isApplet;
		this.add(status, "South");

		if ( this.saved_url == null && this.saved_filename != null ) {
			// for load, get the corresponding URL of saved file:
			System.out.print("saved_url: ");
			try {
				this.saved_url = new File(saved_filename).toURL();
				System.out.print(this.saved_url);
			}
			catch(MalformedURLException ex) {
				System.err.println("error trying to get saved_url: " +ex.getMessage());
			}
			System.out.println();
		}
		System.out.println("Starting mng panel size: " +
			imgSize.width+ " x " +imgSize.height
		);
		
		Color indicatorColor = null;
		if ( isApplet ) 
			indicatorColor = Color.RED;
		
		mngPanel = new MngPanel(this, imgSize, indicatorColor);
		
		System.out.println("ready.");
		
		JScrollPane scrollPane = new JScrollPane(mngPanel
		, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		scrollPane.getVerticalScrollBar().setUnitIncrement(10);
		scrollPane.getHorizontalScrollBar().setUnitIncrement(10);
		this.add(scrollPane);
		

		JPanel buttons = new JPanel(new GridLayout(1, 0));
		this.add(buttons, "North");
		
		bStop.setEnabled(false);
		
		bConnect.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				connectionThread = new Thread(new Runnable() {
					public void run() {
						connect("Connecting ...");
						getImages();
					}
				});
				bStop.setEnabled(true);
				bConnect.setEnabled(false);
				connectionThread.start();
			}
		});
		buttons.add(bConnect);

		bStop.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				new Thread(new Runnable() {
					public void run() {
						stop();
					}
				}).start();
			}
		});
		buttons.add(bStop);

		buttons.add(mouseLabel);
		
		if ( !isApplet ) {
			bSave.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ev) {
					save();
				}
			});
			buttons.add(bSave);

			bLoad.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ev) {
					load();
				}
			});
			buttons.add(bLoad);
	
		}

		buttons.add(zoomLabel);
		bZoomIn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				zoomIn();
			}
		});
		buttons.add(bZoomIn);
		bZoomOut.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				zoomOut();
			}
		});
		buttons.add(bZoomOut);

		mngPanel.setZoom(zoom);
		zoomLabel.setText(zoom+ "%");

		load();
	}

	// called by MngPanel	
	void mouseMoved(int x, int y) { 
		mouseLabel.setText("[" +x+ ", " +y+ "]");
	}

	private void connect(String msg) {
		//this is to have a recent base image before going to connection
		if ( new Date().getTime() - lastLoaded > 120*1000 ) {
			load();
		}
		is = null;
		mng = null;
		try {
			System.out.println(msg);
			status.setText(msg);
			System.out.println("Opening: " +mng_in);
			URL url = new URL(mng_in);
			is = url.openStream();
			status.setText("Getting mng stream ...");
			mng = new MngInputStream(is);
			mngPanel.setFullDimension(mng.getFullDimension());
			bStop.setEnabled(true);
		}
		catch (IOException ex) {
			status.setText("IOException: " +ex.getMessage());
			bConnect.setEnabled(true);
			bStop.setEnabled(false);
			ex.printStackTrace();
		}
	}
	
	private boolean connectionActive() {
		return connectionThread != null;
	}
	
	private void getImages() {
		if ( mng == null )
			return;
		try {
			status.setText("getting images");
			MngInputStream.Png png = new MngInputStream.Png();
			while ( connectionActive()  ) {
				if ( !mng.nextPng(png) ) {
					// Retry connection
					System.err.println("Connection lost.  Retrying ...");
					connect("Connection lost.  Retrying ...");
					continue;
				}
				
				mngPanel.addPng(png);
				
				if ( ++no_imgs % 256 == 0 ) {
					System.gc();
				}
				
				status.setText(
					"Reading GOES stream. " +no_imgs+ " rows received."
				);
				
				// previous message
				// status.setText(no_imgs+ " images received."
					// + " Last one starting at (x, y) = (" +png.x+ ", " +png.y+ ")"
					// + " transferring more..."
				// );
			}
			is.close();
			is = null;
			mng = null;
			status.setText(no_imgs+ " images received.   Completed.");
		}
		catch (IOException ex) {
			status.setText("IOException: " +ex.getMessage());
		}
		finally {
			bConnect.setEnabled(true);
			bStop.setEnabled(false);
			System.gc();
			connectionThread = null;
		}
	}
	
	void stop() {
		if ( connectionThread != null ) {
			connectionThread.interrupt();
			connectionThread = null;
		}
		bConnect.setEnabled(true);
		bStop.setEnabled(false);
	}
	
    public static void main(String[] args) {
		if ( args.length != 2 ) {
			System.err.println(
				"Usage: java MainPanel URL saved-filename\n" +
				"Examples:\n" +
				"	java MainPanel 'http://casil.ucdavis.edu/~crueda/goes/goes?' saved_vis.jpg\n" +
				"	java MainPanel 'http://casil.ucdavis.edu/~crueda/goes/kk.mng' saved_vis.jpg"
			);
			return;
		}
		String mng_in = args[0];
		String saved_filename = args[1];
		new MainPanel(mng_in, saved_filename, false);
	}
	
	static void dump(InputStream is, String outfilename) throws Exception {
		FileOutputStream os = new FileOutputStream(outfilename);
		int b;
		try {
			while ( (b = is.read()) != -1 ) {
				os.write(b);
			}
		}
		catch ( EOFException ex ) {}
		finally {
			os.close();
		}
	}
	
	void save() {
		if ( isApplet ) {
			System.err.println("Cannot save (i'm an applet with no permission)");
			return;
		}
		new Thread(new Runnable() {
			public void run() {
				try {
					System.out.println("Saving " +saved_filename+ "  (" +new Date()+ ")");
					System.out.println("   " +no_imgs+ " images in this session");
					if ( no_imgs > no_imgs_saved ) {
						mngPanel.save(saved_filename);
						no_imgs_saved = no_imgs;
						System.out.println("   Done saving.");
					}
					else {
						System.out.println("   Nothing new to save.");
					}
				} catch (IOException ex ) {
					ex.printStackTrace();
				}
			}
		}).start();
	}
	
	private void load() {
		if ( saved_url != null ) {
			try { 
				System.out.println("Loading " +saved_url);
				mngPanel.load(saved_url);
				lastLoaded = new Date().getTime();
			} 
			catch (IOException ex ) {
				ex.printStackTrace();
			}
		}
	}
	
	private void zoomIn() {
		zoom += 5;   // +5%
		if ( zoom > 100 )
			zoom = 100;
		mngPanel.setZoom(zoom);
		zoomLabel.setText(zoom+ "%");
		doLayout();
	}
	private void zoomOut() {
		zoom -= 5;   // -5%
		if ( zoom < 5 )
			zoom = 5; 
		mngPanel.setZoom(zoom);
		zoomLabel.setText(zoom+ "%");
		doLayout();
	}
}
