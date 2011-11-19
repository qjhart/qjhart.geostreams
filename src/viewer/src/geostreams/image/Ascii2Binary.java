/*
 * GeoStreams
 * Carlos Rueda-Velasquez
 */
package geostreams.image;

import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;

/**
  * Utility program to convert an ascii image stream file 
  * in Jie format to binary format.
  * <pre>
  *		frame ID     --->   (ignored: not converted)
  *		x location   --->   short
  *		y location   --->   short
  *		width        --->   short
  *		data         --->   short* (width*1 vales)
  * </pre>
  * @author Carlos Rueda-Velasquez
  * @version $Id: Ascii2Binary.java,v 1.1 2006/11/28 06:17:44 crueda Exp $
  */ 
public class Ascii2Binary {
	private BufferedReader reader;
	private DataOutputStream dos;
	private boolean withHeader;
	
	/** creates an image stream. 
	  * @param reader Provider of data. 
	  */
	public Ascii2Binary(BufferedReader reader, DataOutputStream dos, boolean withHeader) 
	throws Exception {
		super();
		this.reader = reader;
		this.dos = dos;
		this.withHeader = withHeader;
		System.out.println("Ascii2Binary: converting ...");
	}
	
	/** converts the next line */
	public boolean nextImage() throws IOException {
		if ( reader == null )
			return false;
		
		String line = null;
		try {
			line = reader.readLine();
		}
		catch(IOException ex) {
			reader = null;
		}
		if ( line == null )
			return false;

		String[] tokens = line.split("\\s+");
		
		if ( tokens.length == 0 )
			return false;
		
		if ( tokens.length < 1 + 4 )
			throw new IOException("Line must have at least 5 tokens");
		
		
		
		// convert basic info: x, y, width
		int x = Integer.parseInt(tokens[1]); 
		int y = Integer.parseInt(tokens[2]); 
		int width = Integer.parseInt(tokens[3]);
		int height = 1;
		if ( withHeader ) {
			dos.writeShort(x);
			dos.writeShort(y);
			dos.writeShort(width);
		}
		
		// convert data
		int numvals = width * height;
		for ( int i = 0; i < numvals; i++ ) {
			int val = Integer.parseInt(tokens[1 + 3 + i]);
			dos.writeShort(val);
		}
		
		return true;		
	}
	
	public static void main(String[] args) throws Exception {
		boolean withHeader = true;
		if ( args.length < 2 ) {
			System.err.println("USAGE: ascii2binary <file.txt> <file.bin> [--noHeader]");
			return;
		}
		String txt_filename = args[0];
		String bin_filename = args[1];
		if ( args.length >= 3 )
			withHeader = ! "--noHeader".equals(args[2]);
		
		BufferedReader reader = new BufferedReader(
			new InputStreamReader(new FileInputStream(txt_filename)));
		DataOutputStream dos = new DataOutputStream(
			new BufferedOutputStream(
				new FileOutputStream(bin_filename), 1000*1024));
		
		System.out.println("images with header: " +withHeader);
		
		Ascii2Binary ascii2binary = new Ascii2Binary(reader, dos, withHeader);
		int num_imgs = 0;
		while ( ascii2binary.nextImage() ) {
			if ( ++num_imgs % 21 == 0 )
				System.out.print("\r" +num_imgs+ " images converted");
		}
		dos.close();
		System.out.print("\r" +num_imgs+ " images converted");
		System.out.println("\nDone.");
	}
}

