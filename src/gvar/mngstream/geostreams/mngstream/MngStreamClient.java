/*
	GeoStreams Project
	MngStreamClient
	
	A non-GUI interface to read a mng stream and keep the most
	up-to-date image.
	
	Carlos A. Rueda
	$Id: MngStreamClient.java,v 1.7 2004/08/10 21:15:52 crueda Exp $
*/
package geostreams.mngstream;

import geostreams.goes.GOESConstants;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.net.*;
import java.awt.image.*;
import java.util.List;

/**
	A non-GUI interface to read a mng stream and keep the most
	up-to-date image on disk.
 */
public class MngStreamClient  {
	/** Creates a mng stream client. */
	public MngStreamClient(
		String mng_in, String saved_filename, final int delay,
		final long alive
	) {
		this.mng_in = mng_in;
		this.saved_filename = saved_filename;
		
		processStart = new Date().getTime();
		
		cumImg = new CumulativeImage(imgSize, Color.black);
		
		saved_url = getURL(saved_filename);
		load();
		MngInputStream mng = connect(false);
		imageUpdater = new ImageUpdater(mng, cumImg);
		imageUpdater.start();
		if ( delay > 0 ) {
			new Timer().scheduleAtFixedRate(new TimerTask() {
				public void run() {
					_checkProcess();
					long now = new Date().getTime();
					if ( alive > 0 ) {
						if ( now - processStart >= alive ) {
							System.out.println("Max time alive reached. Exiting");
							System.exit(0);
						}
						else {
							System.out.println((alive - (now - processStart))/(1000)+ " seconds remaining to exit.");
						}
					}
				}
			}, delay, delay);
		}
	}

	/** Main program */
    public static void main(String[] args) {
		String mng_in = "http://casil.ucdavis.edu/~crueda/goes/goes2";
		String saved_filename = "saved_vis.jpg";
		long alive = 40;   // minutes
		int delay = 60;    // seconds
		int port = 0;

		final String usage = 
			"Usage: MngStreamClient [-url mngURL] [-saved filename] " +
			"[-delay seconds] " +
			"[-alive minutes] " +
			"[-port number]\n" +
			"  Defaults:\n" +
			"	url   = " +mng_in+ "\n" +
			"	saved = " +saved_filename+ "\n" +
			"	alive = " +alive+ " minutes\n" +
			"	delay = " +delay+ " seconds\n" +
			"	port  = " +port+ "\n"
		;
		for ( int i = 0; i < args.length; i++ ) {
			if ( "-url".equals(args[i]) )
				mng_in = args[++i];
			else if ( "-saved".equals(args[i]) ) 
				saved_filename = args[++i];
			else if ( "-alive".equals(args[i]) )
				alive = Long.parseLong(args[++i]);
			else if ( "-delay".equals(args[i]) )
				delay = Integer.parseInt(args[++i]);
			else if ( "-port".equals(args[i]) )
				port = Integer.parseInt(args[++i]);
			else {
				System.err.println(usage);
				return;
			}
		}
		
		if ( port > 0 ) {
			try {
				socket = new ServerSocket(port, 2, InetAddress.getByName("127.0.0.1"));
			}
			catch(Throwable ex) {
				System.out.println("MngStreamClient: " +ex.getMessage());
				return;
			}
		}
		
		System.out.println(new Date()+ ": Starting");
		System.out.println("port  = " +port);
		System.out.println("url   = " +mng_in);
		System.out.println("saved = " +saved_filename);
		System.out.println("alive = " +alive+ " minutes");
		System.out.println("delay = " +delay+ " seconds");

		new MngStreamClient(mng_in, saved_filename, 1000*delay, 60*1000*alive);
	}


	//{{{ private members

	// getURL
	/** gets the URL corresponding to a filename. */
	private static URL getURL(String filename) {
		URL url = null;
		try {
			File file = new File(filename);
			if ( file.exists() )
				url = file.toURL();
			else
				System.out.println(filename + " does not exist");
		}
		catch (IOException ex ) {
			System.err.println("getURL: " +ex.getMessage());
		}
		return url;
	}

	// connect
	private MngInputStream connect(boolean reconnection) {
		if ( reconnection )
			System.out.println("Reconnecting...");
		else
			System.out.println("Connecting...");
		
		MngInputStream mng = null;
		try {
			InputStream is;
			if ( "-".equals(mng_in) ) {
				if ( reconnection ) {
					System.out.println("Reconnection is not possible for stdin. Exiting...");
					System.exit(0);
				}
				is = System.in;     // use stdin
			}
			else {
				is = new URL(mng_in).openStream();
			}
			mng = new MngInputStream(is);
			cumImg.setFullDimension(mng.getFullDimension());
		}
		catch (IOException ex) {
			ex.printStackTrace();
			System.exit(0);
		}
		return mng;
	}
	
	
	/** Checks the status of the process.
	  * The image updater is restarted if necessary
	  * but no more than MAX_RESTARTS times.
	  * Then it calls save(). 
	  */ 
	private void _checkProcess() {
		boolean restart = false;
		// allow for a maximum of 15 mins with no new data
		if ( lastSaved > 0
		&& (new Date().getTime()) - lastSaved > 15*60*1000 ) {
			restart = true;
			System.out.println("Too much time waiting for data.");
		}
		if ( !restart && !imageUpdater.isAlive() ) {
			restart = true;
			System.out.println("Something wrong happened.");
		}
		if ( restart ) {
			if ( restarts == MAX_RESTARTS ) {
				System.out.println("Max restarts reached.  Exiting.");
				System.exit(0);
			}
			restarts++;
			MngInputStream mng = connect(true);
			imageUpdater = new ImageUpdater(mng, cumImg);
			imageUpdater.start();
			return;
		}
		restarts = 0;

		System.out.println("(" +new Date()+ ") freemem=" +Runtime.getRuntime().freeMemory());
		save();
	}
		
	/** saves the cumulated image only if new data has arrived. */
	private void save() {
		try {
			System.out.println("Saving: " +cumImg.getNumPngs()+ " images in this session");
			if ( cumImg.getNumPngs() > no_imgs_saved ) {
				cumImg.save(saved_filename);
				no_imgs_saved = cumImg.getNumPngs();
				System.out.println("   DONE");
				lastSaved = new Date().getTime();
			}
			else {
				System.out.println(" **NOTHING new**");
			}
		} catch (IOException ex ) {
			ex.printStackTrace();
		}
	}

	/** loads the specified image into the cumulated one. */	
	private void load() {
		if ( saved_url != null ) {
			try { 
				System.err.println("Loading " +saved_url);
				cumImg.load(saved_url);
			} 
			catch (IOException ex ) {
				ex.printStackTrace();
			}
		}
	}

	/** used to have only one instance of this process on a given port. */
	private static ServerSocket socket;
	
	/** number of consecutive unsuccessful restarts. */
	private int restarts;
	
	/** maximum number of restarts */
	private static final int MAX_RESTARTS = 2;
	
	// desired image size (my arbitrary election)
	private static final int imgWidth =  2000;
	private static final int imgHeight = (int) (GOESConstants.aspect * imgWidth);
	private static final Dimension imgSize = new Dimension(imgWidth, imgHeight);

	/** Each Png is "added" to this cumulative image. */
	private final CumulativeImage cumImg;
	private final String mng_in;

	private int no_imgs_saved = 0;
	
	// name of file with saved complete image (e.g. saved.png)
	private String saved_filename;
	private URL saved_url; 
	private long lastSaved = 0; 
	
	/** time this process started */
	private long processStart = 0; 
	
	private ImageUpdater imageUpdater;
	
}
