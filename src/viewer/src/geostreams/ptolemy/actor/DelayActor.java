package geostreams.ptolemy.actor;

import geostreams.image.ChannelDef;
import geostreams.image.IImage;
import geostreams.image.Images;

import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;

import ptolemy.actor.TypedIOPort;
import ptolemy.data.AWTImageToken;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

import com.vividsolutions.jts.geom.Coordinate;
import com.vividsolutions.jts.geom.Geometry;
import com.vividsolutions.jts.geom.GeometryCollection;
import com.vividsolutions.jts.geom.GeometryFactory;
import com.vividsolutions.jts.geom.LinearRing;

/**
 * Delay operator.
 * This is a spatially-aware, 1-capacity queue. For each pixel location
 * x on the given region of interest, the internal buffer will
 * contain the last known image value at x (initially "null", ie, a fully
 * transparent color).
 * On an input image a whose domain contains x, the operator will output
 * the stored value at x and then updates the stored value with a(x).
 * 
 * <p>
 * A simple implementation
 * <p>
 * The internal buffer is kept in memory as a buffered image.
 * 
 * @author Carlos Rueda-Velasquez
 * @vesion $Id: DelayActor.java,v 1.12 2007/06/01 21:40:01 crueda Exp $
 */
public class DelayActor extends UnaryOperatorActor {
	
	// <jts>
	// preliminary experimentation for computing intersection of incoming
	// images with actual used part of internal _buffer.
	// NOTE: This is not working yet.  Setting _useJts to false for now.
	//
	private static final boolean _useJts = false;
	private GeometryFactory geometryFactory;
	private Geometry _unionGeom;
	// </jts>
	
	
	private static final boolean _includeBufferOutputPort = true;
	
	/** outputs the internal buffer for testing purposes */
	public TypedIOPort bufferOutput = null;
	
	/** 
	 * The buffer image. The allocated size depends on
	 *   - ROI
	 *   - channel resolution
	 */
	private BufferedImage _buffer = null;
	private Graphics2D _graphics;
	private int _bufWidth;
	private int _bufHeight;

	private int _psx;
	private int _psy;
	

	/** {@inheritDoc} */
	public DelayActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name, "Delay");
		_createRoiParameter();
		roiParam.setExpression("15000 3000 17000 5000");

		if ( _includeBufferOutputPort ) {
			bufferOutput = new TypedIOPort(this, "bufferOutput", false, true);
			bufferOutput.setTypeEquals(BaseType.OBJECT);
		}
	}
	
	protected void _updateConnection(IImage img) {
		super._updateConnection(img);
		ChannelDef channelDef = gscon.getChannelDefForStream(img.getStreamID());
		_psx = channelDef.getPixelSizeX();
		_psy = channelDef.getPixelSizeY();
	}

	/**
	 * Called upon arrival of the first image. 
	 */
	private void _initBuffer(BufferedImage inBufImg) {
		// buffer dimensions will be according to channel pixel units, not reference space's
		_bufWidth  = _roi.width / _psx;
		_bufHeight = _roi.height / _psy;
		System.out.println(
				"||\n" +
				"|| Delay: allocating buffer image: " +_bufWidth+ " x " +_bufHeight+ "\n" +
				"||"
		);
		
		if ( false )	 {
			_buffer = new BufferedImage(_bufWidth, _bufHeight, 
					BufferedImage.TYPE_INT_RGB 
			);
		}
		// <transparency>
		else {
			_buffer = new BufferedImage(_bufWidth, _bufHeight, 
					BufferedImage.TYPE_4BYTE_ABGR
			);
			// make pixels fully transparent:
			int[] pixels = new int[_bufWidth * _bufHeight];
			int offset = 0;
			int scansize = _bufWidth;
			for (int i = 0; i < pixels.length; i++) {
				pixels[i] = 0x00000000;
			}
			_buffer.setRGB(0, 0, _bufWidth, _bufHeight, pixels, offset, scansize);
		}
		// </transparency>
		
		_graphics = _buffer.createGraphics();
	}

	/**  
	 * Location (inX, inY) is _roi-relative and scaled according to (_psx, _psy) to obtain the
	 * location in the buffer:
	 * @param inY 
	 * @param inX */
	private void _drawImageInBuffer(BufferedImage inBufImg, int inX, int inY) {
		int dx1 = (inX - _roi.x) / _psx;
		int dy1 = (inY - _roi.y) / _psy;
//		System.out.print("  _saveImageInBuffer: " +dx1+ "," +dy1+ ": " +inBufImg.getWidth()+ "x" +inBufImg.getHeight());
//		System.out.flush();
		_graphics.drawImage(inBufImg, dx1, dy1, null);
	}

	/**  
	 * Location (inX, inY) is _roi-relative and scaled according to (_psx, _psy) to obtain the
	 * location in the buffer:
	 * @param inY 
	 * @param inX */
	private BufferedImage _extractImageFromBuffer(int inX, int inY, int inW, int inH) {
		int dx1 = (inX - _roi.x) / _psx;
		int dy1 = (inY - _roi.y) / _psy;
//		System.out.print("  _extractImageFromBuffer: " +dx1+ "," +dy1+ ": " +inW+ "x" +inH);
//		System.out.flush();
		return Images.extractImage(_buffer, dx1, dy1, inW, inH);
	}

	protected IImage _operate(IImage inImg1) throws IllegalActionException {
		int inX = inImg1.getX();
		int inY = inImg1.getY();
		int inW = inImg1.getWidth();
		int inH = inImg1.getHeight();
		
		// intersect the ROI with the bounds of the input image:
		// note that dimension of inBounds needs to be in units of reference space coordinates
		Rectangle inBounds = new Rectangle(inX, inY, _psx * inW, _psy * inH);
		Rectangle inters = _roi.intersection(inBounds);
		if ( inters.isEmpty() ) {
			return null; 
		}
		
		// result of intersecting ROI and input image:
		IImage intersImg = Images.extractImage(inImg1, inters.x, inters.y, inters.width, inters.height);
		BufferedImage inBufImg = intersImg.getBufferedImage();
		
		if ( _buffer == null ) {
			_initBuffer(inBufImg);
			_drawImageInBuffer(inBufImg, inters.x, inters.y);
			
			if ( _useJts ) {
				Geometry inImgGeom = _createPolygon(inters);
				_unionGeom = inImgGeom;
			}
			
			return null;
		}
		
		IImage outImg = null;

		if ( _useJts ) {
			assert _unionGeom != null;
			Geometry inImgGeom = _createPolygon(inters);
			if ( inImgGeom.intersects(_unionGeom) ) {
				System.out.println("INTERSECTS");
				
				// get image to return from buffer  NOTE: these two line as in the !_useJts below (else block)
				BufferedImage outBufImg = _extractImageFromBuffer(inters.x, inters.y, inBufImg.getWidth(), inBufImg.getHeight());
				outImg = Images.createImage(intersImg, inters.x, inters.y, outBufImg);
				
				Geometry[] polygons = {_unionGeom, inImgGeom};
				GeometryCollection polygonCollection = geometryFactory.createGeometryCollection(polygons);
				_unionGeom = polygonCollection.buffer(0);
			}
			
		}
		else {
			// get image to return from buffer
			BufferedImage outBufImg = _extractImageFromBuffer(inters.x, inters.y, inBufImg.getWidth(), inBufImg.getHeight());
			outImg = Images.createImage(intersImg, inters.x, inters.y, outBufImg);
		}
		
		// save inBufImg on buffer:
		_drawImageInBuffer(inBufImg, inters.x, inters.y);

		if ( _includeBufferOutputPort ) {
			bufferOutput.broadcast(new AWTImageToken(_buffer));
		}
		
//		System.out.println("DelayActor: in and out channel IDs: " +inImg1.getChannelID()+ " , " +outImg.getChannelID());
//		System.out.println("DelayActor: inImg1.isNull() = " +inImg1.isNull()+ "  outImg.isNull() = " +outImg.isNull());
		
		return outImg;
	}
	
    private Geometry _createPolygon(Rectangle inters) {
		if ( geometryFactory == null ) {
			geometryFactory = new GeometryFactory();
		}
		LinearRing shell = geometryFactory.createLinearRing(
				new Coordinate[] {
						new Coordinate(inters.x, inters.y                                    ),
						new Coordinate(inters.x +inters.width -1, inters.y                   ),
						new Coordinate(inters.x +inters.width -1, inters.y + inters.height -1),
						new Coordinate(inters.x                 , inters.y + inters.height -1),
						new Coordinate(inters.x, inters.y                                    ),
				}
		);
		return geometryFactory.createPolygon(shell, null);
	}

	public void wrapup() throws IllegalActionException {
    	super.wrapup();
		_buffer = null;
		_graphics = null;
    }
}
