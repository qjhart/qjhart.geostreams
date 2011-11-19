/*
 * GeoStreams
 * Carlos Rueda-Velasquez
 */
package geostreams.image;

import java.awt.image.BufferedImage;
import java.awt.image.PixelGrabber;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.Iterator;

import javax.imageio.ImageIO;
import javax.imageio.ImageReader;
import javax.imageio.stream.MemoryCacheImageInputStream;

/** An image stream from a MNG stream generated by the 
  * C++ class MngStream.
  * 
  * TODO: Long time without testing according to new adjustments --> REVISIT
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: MngImageStream.java,v 1.3 2007/06/01 21:40:01 crueda Exp $
  */ 
class MngImageStream extends BasicImageStream {
	private static final byte[] png_signature = {(byte)137,80,78,71,13,10,26,10};
	
	/** the source input stream */
	private final InputStream is;

	// I try to have just one instance for each auxiliary class:
	
	/** current chunk */
	private final Chunk chunk = new Chunk();
	
	/** each PNG (including the PNG signature) is "collected"
	in this output stream */
	private final MyByteArrayOutputStream baos = new MyByteArrayOutputStream();
	
	/** each "collected" PNG is put in this input stream which
	is used for ImageIO.read */
	private final MyByteArrayInputStream bais = new MyByteArrayInputStream(baos.getBuf());

	/** Creates a MNG input stream that reads from the specified InputStream. 
	    @param is input stream to read from.
	*/
	public MngImageStream(String channelDef, InputStream is) {
		super(channelDef);
		this.is = is;
		System.out.println("MngImageStream created");
	}
	
	public void startReading() throws IOException {
		System.out.println("MngImageStream: reading stream ...");
		// read and skip the MNG signature
		for ( int i = 0; i < 8; i++ )
			is.read();  // and ignore
			
		// read MHDR
		chunk.read(is);
		if ( !chunk.getName().equals("MHDR") ) {
			throw new IOException("Expecting MHDR. Received: [" +chunk.getName()+ "]");
		}
		//
		// NOTE:
		// The full size in the MNG stream:
		//    width = bytearray2int(chunk.getData(), 8),
		//    height = bytearray2int(chunk.getData(), 8 +4)
		// but we don't need that here.
		//
	}
	
	public IImage nextImage() throws IOException {
		chunk.read(is);
		if ( !chunk.getName().equals("DEFI") )
			return null;
		
		byte[] chunk_data;
		int chunk_data_len;
		
		chunk_data = chunk.getData();
		chunk_data_len = chunk.getDataLength();
		int x = bytearray2int(chunk_data, chunk_data_len -4 -4 -4);
		int y = bytearray2int(chunk_data, chunk_data_len -4 -4);
		
		chunk.read(is);
		if ( !chunk.getName().equals("IHDR") ) {
			throw new IOException("Expecting IHDR");
		}
		chunk_data = chunk.getData();
		chunk_data_len = chunk.getDataLength();
		
		// start baos with PNG signature:
		baos.reset();
		baos.write(png_signature, 0, png_signature.length);
		
		// now write the chunks
		baos.write(chunk_data, 0, chunk_data_len);
		while ( true ) {
			chunk.read(is);
			chunk_data = chunk.getData();
			chunk_data_len = chunk.getDataLength();
			baos.write(chunk_data, 0, chunk_data_len);
			if ( chunk.getName().equals("IEND") )
				break;
		}
		bais.reset(baos.getBuf(), baos.getCount());
		
		BufferedImage buff_img;
		if ( true ) {
			// fragment under testing.
			MemoryCacheImageInputStream mciis = new MemoryCacheImageInputStream(bais);
			ImageReader image_reader = null;
			for ( Iterator it = ImageIO.getImageReaders(mciis); it.hasNext(); ) {
				image_reader = (ImageReader) it.next();
				break;   // just take 	the first one
			}
			if ( image_reader == null )
				throw new Error("An ImageReader was expected");
			
			image_reader.setInput(mciis);
			buff_img = image_reader.read(0);
			image_reader.dispose();      // the key operation!
		}
		else {
			buff_img = ImageIO.read(bais);
		}
		
		BasicImage basicImage = new MyBasicImage(this);
		basicImage.x = x;
		basicImage.y = y;
		basicImage.width = buff_img.getWidth(null);
		basicImage.height = buff_img.getHeight(null);
		basicImage.data = null;
		basicImage.buff_img = buff_img;
		return basicImage;
	}
	
	/** this subclass is to avoid the toByteArray() method which copies
	    the buffer. */
	private static class MyByteArrayOutputStream extends ByteArrayOutputStream {
		byte[] getBuf() { return buf; }
		int getCount() { return count; }
	}
	
	/** this subclass is to provide a reset(buf, count) method. */
	private static class MyByteArrayInputStream extends ByteArrayInputStream {
		MyByteArrayInputStream(byte[] buf) { super(buf); }
		void reset(byte[] buf, int count) {
			this.buf = buf;
			this.pos = 0;
			this.count = count;
		}
	}

	private static int bytearray2int(byte[] b, int from) {
		int a0 = 0xff & b[from];
		int a1 = 0xff & b[from+1];
		int a2 = 0xff & b[from+2];
		int a3 = 0xff & b[from+3];
		int len = (a0<<24) | (a1<<16) | (a2<<8) | a3;
		return len; 
	}

	/** A chunk in the stream */
	private static class Chunk {
		/** reported length of this chunk. */
		private int len;
		
		/** Name.*/
		private String name;
		
		/** the entire data for this chunk. */
		private byte[] data;
	
		/** Usable length of data array. */
		private int dataLen;
		
		// aux to read in length + name
		private static byte[] lenname = new byte[8];
	
		/** Creates an empty chunk. 
		 *  Call read(InputStream) next to fill in data .*/
		public Chunk() {
		}
		
		/** Fills in this chunk with data from the InputStream.*/
		public void read(InputStream is) throws IOException {
			// read in length + name
			for ( int i = 0; i < 8; i++ ) {
				lenname[i] = (byte) is.read();
			};
			len = bytearray2int(lenname, 0);
			name = new String(lenname, 4, 4);
			//System.out.println("len = [" +len+ "]  name = [" +name+ "]");
			
			dataLen = 4 + 4 + len + 4;
			if ( data == null || data.length < dataLen )
				data = new byte[dataLen];
			System.arraycopy(lenname, 0, data, 0, 8);
			for ( int i = 0; i < len + 4; i++ ) {
				data[8 + i] = (byte) is.read();
			}
		}
		
		/** Gets the usable length of getData(). 
		 *  (Do not use getData().length). */
		public int getDataLength() {
			return dataLen;
		}
	
		/** Gets the name this chunk. */
		public String getName() {
			return name;
		}
	
		/** Gets the entire data this chunk. */
		public byte[] getData() {
			return data;
		}
	}
	
	/** This is to create the data on demand. */
	private static class MyBasicImage extends BasicImage implements Serializable {
		MyBasicImage(IImageStream srcStream) {
//			super(srcStream);
			super(srcStream.getStreamID(), srcStream.getChannelID());
		}
		
		public int[] getData() {
			if ( data != null )
				return data;
			
			int w = width;
			int h = height;
			data = new int[w * h];
			PixelGrabber pg = new PixelGrabber(buff_img, 0, 0, w, h, data, 0, w);
			try {
				pg.grabPixels();
			} catch (InterruptedException e) {
				System.err.println("interrupted waiting for pixels!");
				Thread.currentThread().interrupt();
			}
			
			///// // else: get data from the buff_img
			///// 
			///// how this works?
			///// 
			///// Raster raster = buff_img.getData();
			///// 
			///// data = raster.getPixels(0, 0, getWidth(), getHeight(), (int[])null);
			
			
			for ( int i = 0; i < data.length; i++ ) {
				data[i] = 0xff & data[i];
			}
			
			return data;
		}
		
		/**
		 * Serialize this instance.
		 * 
		 * @param oos
		 * @throws IOException
		 */
		private void writeObject(ObjectOutputStream oos) throws IOException {
			// force data array is non-empty
			getData();
			oos.defaultWriteObject();
		}

	}

	public void close() throws IOException {
		// TODO just calling is.close() which makes sense BUT this class needs a complete revisit!
		is.close();
	}
}

