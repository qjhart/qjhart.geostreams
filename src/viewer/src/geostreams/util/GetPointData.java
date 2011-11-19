package geostreams.util;

import geostreams.image.ChannelDef;
import geostreams.image.IConnectionObserver;
import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.image.IStream;
import geostreams.image.Images;
import geostreams.image.ServerConnections;
import geostreams.image.StreamDef;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Reads one or more image streams and reports the corresponding pixel values
 * at given locations.
 * 
 * @author Carlos Rueda
 * @version  $Id: GetPointData.java,v 1.5 2007/09/13 10:13:24 crueda Exp $
 */
public class GetPointData {
	/** default URL of geostreams server spec */
	private static final String DEFAULT_GS_SPEC = 
			"http://geostreams.ucdavis.edu/~carueda/applet/dev/geostreams2.xml"
		//	"file:/home/carueda/goesdata/local2/local.xml"
	;
	
	/** default URL of point spec */
	private static final String DEFAULT_POINT_SPEC = 
			"file:etc/pixlin-cimis-stations2007-07-22.csv"
	;
	
	/** All GOES Imager channels. 
	 * The infrared channels (1,2,3,4) can be used to calculate temperature,
	 * see {@link http://www.oso.noaa.gov/goes/goes-calibration/gvar-conversion.htm}
	 */
	private static final String DEFAULT_STREAMS = "0,1,2,3,4";
	
	/** default working directory */
	private static final String DEFAULT_OUTPUT_DIRECTORY = "data";

	/** default prefix to name output files */
	private static final String DEFAULT_OUTPUT_PREFIX = "realtime";

	/** Time in seconds to start running the SaverTask */
	private static final long firstTimeSec =  1;
	/** Period in minutes to run the SaverTask */
	private static final long periodMin = 60;

	
	/* a simple logger */
	private static class Logger {
		PrintWriter lw;
		
		Logger(String logFilename) throws FileNotFoundException {
			lw = new PrintWriter(new FileOutputStream(logFilename, true), true);
			log("---Starting---");
		}
		void log(String msg) {
			lw.println(new Date()+ ": " +msg);
		}
	}

	private static Logger logger;
	

	/** Point id, location, and value */
	static class Point {
		String id;
		int x;
		int y;
		int value = Integer.MIN_VALUE;  // as a NULL indicator
	}

	/** Obtains the point data for a particular stream */
	static class StreamProcessing extends IConnectionObserver.Adapter {
		String streamName;
		List<Point> points;

		StreamProcessing(String streamName, List<Point> points ) {
			this.streamName = streamName;
			this.points = new ArrayList<Point>();
			for ( Point p : points ) {
				Point q = new Point();
				q.id = p.id;
				q.x = p.x;
				q.y = p.y;
				this.points.add(q);
			}
		}
		
		// no default log messages as a connection observer
		protected void _log(String msg) {}
		
		public void newImage(IImage iimg) {
			// Get resolutions:
			
			///// the following block, from Images.extractPixel -- TODO: factorize
			ChannelDef channelDef = null;
			IStream istream = iimg.getStream();
			if ( istream != null ) {
				channelDef = istream.getChannelDef();
			}
			else {
				// FIXME 0 : istream  This will dissapear
				IServerConnection gscon = iimg.getServerConnection();
				if ( gscon != null ) {
					StreamDef streamDef = gscon.getStreamDef(iimg.getStreamID());
					channelDef = streamDef.getChannelDef();
				}
			}
			assert channelDef != null || Utils.fail("Image.extractPixel: channelDef not found for streamID: " +iimg.getStreamID());
			
			int psx = channelDef.getPixelSizeX();
			int psy = channelDef.getPixelSizeY();
			/////
			
			
			int x = iimg.getX();
			int y = iimg.getY();
			int w = iimg.getWidth() * psx;
			int h = iimg.getHeight() * psy;
			int x1 = x + w;
			int y1 = y + h;
			
			for ( Point p : points ) {
				if ( x <= p.x && p.x < x1 
				&&   y <= p.y && p.y < y1 ) {
					// get value
					p.value = Images.extractPixel(iimg, p.x, p.y);
				}
			}
		}
	}

	/** the streams being downloaded */
	private List<StreamProcessing> streams = new ArrayList<StreamProcessing>();
	
	
	private IServerConnection gscon;
	
	
	

	GetPointData(String serverSpec) throws Exception {
		gscon = ServerConnections.openConnection(serverSpec);
	}
	
	
	public void openStream(String streamName, List<Point> points) {
		StreamProcessing stream = new StreamProcessing(streamName, points);
		
		gscon.addObserver(stream, streamName);
		try {
			gscon.openStream(streamName, null);
			streams.add(stream);
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		
		
	}

	/** Updates each output file with the current pixel values in the
	 * corresponding list of points.
	 */
	static class SaverTask extends TimerTask {
		String outputDirectory;
		String outputPrefix;
		List<StreamProcessing> streams;
		
		SaverTask(List<StreamProcessing> streams, String outputDirectory, String outputPrefix) {
			this.streams = streams;
			this.outputDirectory = outputDirectory;
			this.outputPrefix = outputPrefix;
		}

		public void run() {
			
			logger.log("Saver task starts...");
			
			Calendar cal = Calendar.getInstance();
			
			SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
			SimpleDateFormat hourFormat = new SimpleDateFormat("HHmm");
			
			String date = dateFormat.format(cal.getTime());
			String hour = hourFormat.format(cal.getTime());

			for ( StreamProcessing stream : streams ) {
				String filename = outputDirectory+ "/" +outputPrefix + "_" +date+ "_s_" +stream.streamName+ ".csv";

				PrintWriter pw = null;
				try {
					pw = new PrintWriter(new FileOutputStream(filename, true), true);

					for (Point p : stream.points ) {
						if ( p.value != Integer.MIN_VALUE ) {
							pw.println(hour+ " , " +p.id+ " , " +p.x+ " , " +p.y+ " , " +p.value);
						}
					}
				}
				catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				finally {

					if ( pw != null ) {
						pw.close();
					}
				}
			}
			
			System.gc();
			logger.log("... Saver task ends.\n");
		}
	}
	
	/**
	 * Main program.
	 * 
	 * @param args  <br>
	 * 		--gsSpec &lt;URI of geostreams spec file><br>
	 * 		--streams &lt;comma-separated stream names><br>
	 * 		--points &lt;URI of point locations><br>
	 * 		--outputDirectory &lt;where output files will be created><br>
	 * 		--outputPrefix &lt;prefix to name output files>
	 */
	public static void main(String[] args) throws Exception {
		String serverSpec = DEFAULT_GS_SPEC;
		String streamNames = DEFAULT_STREAMS;
		String pointSpec = DEFAULT_POINT_SPEC;
		String outputDirectory = DEFAULT_OUTPUT_DIRECTORY;
		String outputPrefix = DEFAULT_OUTPUT_PREFIX;
		
		for (int i = 0; i < args.length; i++) {
			String arg = args[i];
			if ( arg.equals("--gsSpec") ) {
				serverSpec = args[++i];
			}
			else if ( arg.equals("--streams") ) {
				streamNames = args[++i];
			}
			else if ( arg.equals("--points") ) {
				pointSpec = args[++i];
			}
			else if ( arg.equals("--outputDirectory") ) {
				outputDirectory = args[++i];
			}
			else if ( arg.equals("--outputPrefix") ) {
				outputPrefix = args[++i];
			}
			else { // if ( arg.equals("--help") ) {
				System.out.println("GetPoints [<URI of geostreams spec file>]");
				return;
			}
		}
		
		String logFilename = outputDirectory+ "/" + "getpointdata.log";
		logger = new Logger(logFilename);
		
		logger.log("periodMin = " +periodMin);
		
		logger.log("Reading point locations from " +pointSpec);
		List<Point> points = readPoints(pointSpec);
		logger.log("readPoints: read " +points.size()+ " points.");
		
		GetPointData getPoints = new GetPointData(serverSpec);
		for ( String streamName : streamNames.split("[,\\s]+") ) {
			logger.log("Opening stream " +streamName);
			getPoints.openStream(streamName, points);
		}
		
		logger.log("Scheduling data saver task");
		TimerTask task = new SaverTask(getPoints.streams, outputDirectory, outputPrefix);
		Timer timer = new Timer("Data saver", true);
		timer.schedule(task, firstTimeSec * 1000, periodMin * 60 * 1000);
	}

	

	/** Reads the point locations.
	 * The file should have at least 3 columns with comma or space separators:
	 * <pre>
		#id, pixel, line ...
		1, 17620, 4234
		2, 17582, 4270
		...
        </pre>
	 */
	private static List<Point> readPoints(String pointSpec) throws IOException {
		URL url = new URL(pointSpec);
		BufferedReader br = new BufferedReader(new InputStreamReader(url.openStream()));
		List<Point> points = new ArrayList<Point>();
		
		String line;
		while (( line = br.readLine()) != null ) {
			String[] toks = line.split("[,\\s]+");
			
			int idx = 0;
			
			String id = toks[idx++];
			if ( id.startsWith("#") ) {
				continue;
			}
			
			int x = Integer.parseInt(toks[idx++]);
			int y = Integer.parseInt(toks[idx++]);
			
			Point p = new Point();
			p.id = id;
			p.x = x;
			p.y = y;
			points.add(p);
		}
		
		return points;
	}
}
