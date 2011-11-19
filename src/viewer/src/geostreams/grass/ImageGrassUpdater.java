/*
 * ECS289F Project
 * Carlos Rueda-Velasquez, Haiyan Yang
 */
package geostreams.grass;

import geostreams.image.IImage;

import java.io.*;


/**
  * ImageGrassUpdater.
  * Reads a stream of IImages and updates a GRASS
  * database with the frames extracted.
  *
  * Currently, the GRASS database specification is hard-coded
  * as indicated in the public fields machine and grass_dir.
  *
  * Files are transfered via the scp program, see the field
  * SCP_CMD.
  *
  * @author Carlos Rueda-Velasquez and Haiyan Yang
  * @version $Id: ImageGrassUpdater.java,v 1.3 2007/06/07 01:13:13 crueda Exp $
  */
public class ImageGrassUpdater {

	/** The scp command: "/usr/bin/scp" 
	  * @tagvalue "/usr/bin/scp"
	  */
	public static final String SCP_CMD  = "/usr/bin/scp";

	
	/** The machine where the GRASS database is located.
	  * Hard-coded to: "geostreams.cs.ucdavis.edu"
	  */
	public String machine = "geostreams.cs.ucdavis.edu";
	
	/** The directory corresponding to the GRASS mapset under which
	  * the rasters will be created.
	  * Hard-coded to: "/home/haiyan/grassdata/geostreamtest/qjhart"
	  */
	public String grass_dir = "/home/haiyan/grassdata/geostreamtest/qjhart";
	
	/** indicator of first call to fire() method after initialize() */
	private boolean first_fire;

	private int no_imgs;
	private int no_frames_transferred;

	private int prev_x;
	private int prev_y;
	/** raster header information */
	private static class Header {
		int west;
		int east;
		int north;
		int south;
		int cols;
		int rows;
	}

	private Header header = new Header();

	/** used to create the filename */
	private int next_frame_number;

	private String filename;

	private DataOutputStream bin_out = null;
	
	/** Creates a GRASS updater with default machine/grass_dir
	  * values.
	  */
	public ImageGrassUpdater() throws Exception {
		initialize();
	}
	
    public void initialize() throws Exception {
		no_imgs = 0;
		no_frames_transferred = 0;
		prev_x = -9999999;
		prev_y = -9999999;
		next_frame_number = 0;
		first_fire = true;

		// create binary output file:
		filename = "testfilename_" + next_frame_number;
		bin_out = _createBinaryFile(filename);
    }
	
	/** Returns the number of images that have been processed
	  * since the last call to initialize();
	  */
	public int getNumImages() {
		return no_imgs;
	}
	
	/** Returns the number of frames that have been completed
	  * and transferred to the GRASS database since the last
	  * call to initialize();
	  */
	public int getNumFrames() {
		return no_frames_transferred;
	}
	
	/** Returns true iff a frame is currently being created.
	  * A frame is finished and transferred when a null
	  * image is seen or when a change in location is detected
	  * in the stream of images.
	  */
	public boolean isFrameBeingCreated() {
		return bin_out != null;
	}

	/** Notifies this updater about a new image from the incoming
	  * stream. 
	  * @param img The image to be processed. If this img is null
	  *        or img.isNull() is true, then the current frame contents
	  *        is finished and transferred to the GRASS destination.
	  *        Otherwise, the location of this image is analysed;
	  *        if this location indicates a new frame, then the current
	  *        frame is finished, and a new frame with this first row
	  *        is initiated.
	  */
	public void addImage(IImage img) throws Exception {
		// end of the input image stream?
		if ( img == null || img.isNull() ) {
			// update header info and finish frame:
			header.south = -prev_y;
			header.rows = header.north - header.south + 1;
			_closeFinishedFrame(header, filename);
			bin_out = null;
			_doTransferToGrass(filename);
			return;
		}

		// FIXME or remove me!  IImage.getData() was removed from  06/06/07
		
		assert false;
		
		int[] data = null; //img.getData();
		int x = img.getX();
		int y = img.getY();

		if ( data == null ) {
			throw new Exception("Image data is null.");
		}
		int len = data.length;
		
		int sampleSize = 10; 
		// TODO: initially it was := img.getChannelDef().getSampleSize();

		no_imgs++;
		if ( first_fire ) {
			// write data
			_writeData(data, sampleSize);

			// start info for header:
			header.west = x;
			header.east = header.west + len;
			header.cols = len;
			header.north = - y;

			prev_x = x;
			prev_y = y;
			first_fire = false;
		}
		else {
			// are we in the same frame? This is the case if we are at the same
			// column and with the same lenght, only on the next row:
			if (prev_x == x ) { //   PENDING: && prev_y + 1 == y && prev_len == len) {
				// just write one more chunck of data:
				_writeData(data, sampleSize);
			}
			else {
				// A new frame has just started.
				// So, first close the previous output file;
				// then create the new file and write its first chunk of data.

				// update header info and finish frame:
				header.south = -prev_y;
				header.rows = header.north - header.south + 1;
				_closeFinishedFrame(header, filename);
				
				_doTransferToGrass(filename);

				// Now, start new frame
				next_frame_number++;
				filename = "testfilename_" + next_frame_number;

				// start header info for new frame:
				header.west = x;
				header.east = header.west + len;
				header.cols = len;
				header.north = - y;

				// create new binary and write chunk of data:
				bin_out = _createBinaryFile(filename);
				_writeData(data, sampleSize);

			}
			// keep track of last row, col, and len:
			prev_y = y;
			prev_x = x;
		}
	}

	/** creates the binary output file */
	private static DataOutputStream _createBinaryFile(String filename) throws Exception {
		try {
			return new DataOutputStream(
				new BufferedOutputStream(new FileOutputStream(filename), 1024*1024)
			);
		}
		catch(IOException ex) {
			throw new Exception("Cannot create binary file: " + ex.getMessage());
		}
	}

	/** Uses 1.4  (constructor PrintWriter(String) comes in 1.5) */
	private static PrintWriter createPrintWriter(String filename) throws FileNotFoundException {
		return new PrintWriter(new OutputStreamWriter(new FileOutputStream(filename)));
	}
	
	/** writes a header file */
	private static void _writeHeader(Header header, String filename)
	throws Exception {
		try {
			PrintWriter header_out = createPrintWriter(filename);
			header_out.println("proj:       0");
			header_out.println("zone:       0");
			header_out.println("north:      " + header.north);
			header_out.println("south:      " + header.south);
			header_out.println("east:       " + header.east);
			header_out.println("west:       " + header.west);
			header_out.println("cols:       " + header.cols);
			header_out.println("rows:       " + header.rows);
			header_out.println("e-w resol:  1");
			header_out.println("n-s resol:  1");
			header_out.println("format:     0");
			header_out.println("compressed: 0");
			header_out.close();
		}
		catch (IOException ex) {
			throw new Exception("Cannot write header: " + ex.getMessage());
		}
	}


	/** closes the binary output file, and writes its header and color files */
	private void _closeFinishedFrame(Header header, String filename)
	throws Exception {
		assert bin_out != null;
		try {
			bin_out.close();
			_writeHeader(header, filename + ".hdr");
			_writeColor(filename + ".clr");
		}
		catch (IOException ex) {
			throw new Exception("Error while closing a finished frame: " + ex.getMessage());
		}
	}


	/** writes a header file */
	private void _writeColor(String filename) throws Exception {
		try {
			PrintWriter color_out = createPrintWriter(filename);
			color_out.println("% 0 255");
			color_out.println("0:0 255:255");
			color_out.close();
		}
		catch (IOException ex) {
			throw new Exception("Cannot write color: " + ex.getMessage());
		}
	}


	private void _writeData(int[] data, int sampleSize) throws Exception {
		for (int j = 0; j < data.length; j++) {
			int val = data[j];
			if ( sampleSize > 8 ) {
				// take 8 most-significant bits:
				val = 0xFF & (val >> (sampleSize - 8));
			}
			try {
				bin_out.writeByte(val);
			}
			catch (IOException ex) {
				throw new Exception("Ooops! something happen: " + ex.getMessage());
			}
		}
	}                          

	private void _doTransferToGrass(String filename) throws Exception {
		// make up commands
		String cmdbin  = SCP_CMD+ " " +filename+ " " +machine+ ":" +grass_dir+ "/cell/testfile";
		String cmdhead = SCP_CMD+ " " +filename+ ".hdr " +machine+ ":" +grass_dir+ "/cellhd/testfile";
		
		// run commands:
		try {
			Runtime rt = Runtime.getRuntime();
			System.out.println("Running: " +cmdbin);
			Process proc = rt.exec(cmdbin);
			int status = proc.waitFor();
			if ( status != 0 ) {
				throw new Exception("Error transfering binary raster to GRASS. status = " +status);
			}
			System.out.println("Running: " +cmdhead);
			proc = rt.exec(cmdhead);
			status = proc.waitFor();
			if ( status != 0 ) {
				throw new Exception("Error transfering raster header to GRASS. status = " +status);
			}
		}
		catch(Exception ex) {
			throw new Exception("Error transfering to GRASS: " +ex.getMessage());
		}
		no_frames_transferred++;
	}                              
}

