package geostreams.util;

import geostreams.goes.TemperatureConversion;
import geostreams.image.ChannelDef;
import geostreams.image.IConnectionObserver;
import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.image.Images;
import geostreams.image.ServerConnections;
import geostreams.image.StreamDef;

import java.io.BufferedReader;
import java.io.EOFException;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.StreamTokenizer;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;


/**
 * Reads one or more image streams and reports the corresponding pixel values
 * at given locations.
 * 
 * @author Carlos Rueda
 * @version  $Id: GetRasterData.java,v 1.2 2007/09/13 10:13:04 crueda Exp $
 */
public class GetRasterData {
    /** default URL of geostreams server spec */
    private static final String DEFAULT_GS_SPEC = 
            "http://geostreams.ucdavis.edu/~carueda/applet/dev/geostreams2.xml"
        //  "file:/home/carueda/goesdata/local2/local.xml"
    ;
    
    /** default URL of point spec */
    private static final String DEFAULT_PIXLIN_RASTER = 
            "etc/pixlin.raster2"
    ;
    
    /** first infrared channel. */
    private static final String DEFAULT_STREAM = "1";
    
    /** default working directory */
    private static final String DEFAULT_OUTPUT_DIRECTORY = "rasterdata";

    /** default prefix to name output files */
    private static final String DEFAULT_OUTPUT_PREFIX = "realtime_raster";
    
    
    /** Time in seconds to start running the SaverTask */
    private static long firstTimeSec =  1;
    /** Period in minutes to run the SaverTask */
    private static long periodMin = 60;
    
    
    private static TemperatureConversion _tempConv;

    
    /**
     * Main program.
     * 
     * @param args  <br>
     *      --gsSpec &lt;URI of geostreams spec file><br>
     *      --stream &lt;stream names><br>
     *      --pixlinRaster &lt;URI of raster pixlin locations><br>
     *      --outputDirectory &lt;where output files will be created><br>
     *      --outputPrefix &lt;prefix to name output files><br>
     *      --firstTimeSec &lt;Time in seconds to start saver task><br>
     *      --periodMin &lt;period in minutes to run saver task><br>
     *      --temperatureConversion
     */
    public static void main(String[] args) throws Exception {
        String serverSpec = DEFAULT_GS_SPEC;
        String streamName = DEFAULT_STREAM;
        String pixlinRasterFilename = DEFAULT_PIXLIN_RASTER;
        String outputDirectory = DEFAULT_OUTPUT_DIRECTORY;
        String outputPrefix = DEFAULT_OUTPUT_PREFIX;
        boolean temperatureConversion = false;
        
        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if ( arg.equals("--gsSpec") ) {
                serverSpec = args[++i];
            }
            else if ( arg.equals("--stream") ) {
                streamName = args[++i];
            }
            else if ( arg.equals("--pixlinRaster") ) {
                pixlinRasterFilename = args[++i];
            }
            else if ( arg.equals("--temperatureConversion") ) {
                temperatureConversion = true;
            }
            else if ( arg.equals("--firstTimeSec") ) {
                firstTimeSec = Long.parseLong(args[++i]);
            }
            else if ( arg.equals("--periodMin") ) {
                periodMin = Long.parseLong(args[++i]);
            }
            else if ( arg.equals("--outputDirectory") ) {
                outputDirectory = args[++i];
            }
            else if ( arg.equals("--outputPrefix") ) {
                outputPrefix = args[++i];
            }
            else { // if ( arg.equals("--help") ) {
                System.out.println("GetRasters [options]");
                return;
            }
        }
        
        if ( temperatureConversion ) {
            int channel = 1 + Integer.parseInt(streamName);
            _tempConv = new TemperatureConversion(channel);
        }

        String logFilename = outputDirectory+ "/" + "getrasterdata.log";
        logger = new Logger(logFilename);
        
        logger.log("firstTimeSec = " +firstTimeSec+ "  periodMin = " +periodMin);

        
        logger.log("Getting connection and opening stream " +streamName);

        Lut lut = new Lut(pixlinRasterFilename);
        Region region = new Region(lut);
        
        SaverTask task = new SaverTask(streamName, region, outputDirectory, outputPrefix);
        
        new RegionUpdater(serverSpec, streamName, lut, region, task);

        logger.log("Scheduling data saver task to start in " +firstTimeSec+ " seconds");
        Timer timer = new Timer("Data saver", true);
        timer.schedule(task, firstTimeSec * 1000, periodMin * 60 * 1000);
        
        Thread.sleep(Long.MAX_VALUE);
    }

    static final class Lut {
        float north, south, east, west;
        int cols, rows;
        private Map<Long,List<Long>> map;
        
        Lut(String pixlinFilename) throws IOException {
            map = new HashMap<Long,List<Long>>();
            
            logger.log("Lut: Creating LUT from  " +pixlinFilename);
                    
            BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(pixlinFilename)));
            StreamTokenizer st = new StreamTokenizer(reader);
            st.whitespaceChars(',', ',');
            no_vals = 0;
            accept(st, "north"); north = nextVal(st);
            accept(st, "south"); south = nextVal(st);
            accept(st, "east");  east = nextVal(st);
            accept(st, "west");  west =  nextVal(st);
            accept(st, "rows");  rows = (int) nextVal(st);
            accept(st, "cols");  cols = (int) nextVal(st);
            
            logger.log("Lut: rows = " +rows+ "  cols=" +cols);
            
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    float val = nextVal(st);
                    if ( Float.isNaN(val) ) {
                        continue;
                    }
                    
                    long x = (int) val;
                    long y = (int) nextVal(st);
                    
                    long xy = (x << 32) | y;
                    long ji = ((long) j << 32) | (long) i;
                    
                    List<Long> jis = map.get(xy);
                    if ( jis == null ) {
                        jis = new ArrayList<Long>();
                        map.put(xy, jis);
                    }
                    jis.add(ji);
                }
            }
            
            reader.close();
            logger.log("Lut: LUT created.");
        }
        
        final List<Long> getJI(int x, int y) {
            long xy = ((long) x << 32) | (long) y;
            return map.get(xy);
        }
        
        private static int no_vals = 0;
        private static void accept(StreamTokenizer st, String w) throws IOException {
            st.nextToken();
            if ( st.ttype == StreamTokenizer.TT_WORD ) {
                if ( st.sval.equals(w) ) {
                    int colon = st.nextToken();
                    assert  colon == ':';
                    return; // OK
                }
                else
                    throw new IOException(st.lineno()+ ": `" +w+ "' expected.  " +st.sval+ " encountered");
            }
            else if ( st.ttype == StreamTokenizer.TT_EOF )
                throw new EOFException(st.lineno()+ ": `" +w+ "' expected.  EOF encountered");
            else
                throw new IOException(st.lineno()+ ": `" +w+ "' expected.  " +st.ttype+ " encountered");
        }
        private static float nextVal(StreamTokenizer st) throws IOException {
            st.nextToken();
            if ( no_vals++ % 10000 == 0 ) {
                System.err.print(".");
                System.err.flush();
            }
            if ( st.ttype == StreamTokenizer.TT_NUMBER )
                return (float) st.nval;
            else if ( st.ttype == StreamTokenizer.TT_WORD ) {
                if ( st.sval.equals("N") )
                    return Float.NaN;
                else
                    throw new IOException(st.lineno()+ ": number expected. " +st.sval+ " encountered");
            }
            else if ( st.ttype == StreamTokenizer.TT_EOF )
                throw new EOFException(st.lineno()+ ": number expected. " +"Unexpected eof");
            else
                throw new IOException(st.lineno()+ ": number expected. " +"Unexpected token type");
        }
    }
    
    static final class Region {
        Lut lut;
        int[] data;
        
        Region(Lut lut) {
            this.lut = lut;
            data = new int[lut.cols * lut.rows];
            for ( int k = 0; k < data.length; k++ ) {
                data[k] = Integer.MIN_VALUE; // as a NULL indicator
            }
        }
        void set(int j, int i, int value) {
            try {
//                data[i + lut.rows*j] = value; // see Creator in ggis (vit project)
                data[i*lut.cols + j] = value; // --> but should be like this.
            }
            catch (ArrayIndexOutOfBoundsException e) {
                throw new RuntimeException("Region.set:  j,i = " +j+ "," +i+ "  value=" +value, e);
            }            
        }
        synchronized void save(String filename) {
            PrintWriter pw = null;
            try {
                pw = new PrintWriter(new FileOutputStream(filename, false), true);
                
                pw.println("north:" +lut.north);
                pw.println("south:" +lut.south);
                pw.println("east:" +lut.east);
                pw.println("west:" +lut.west);
                pw.println("rows:" +lut.rows);
                pw.println("cols:" +lut.cols);

                for ( int k = 0; k < data.length; k++ ) {
                    int value = data[k];
                    if ( value != Integer.MIN_VALUE ) {
                        
                        if ( _tempConv != null ) {
                            double t = _tempConv.convert(value);
                            pw.print(t+ " ");
                        }
                        else {
                            pw.print(value+ " ");
                        }
                    }
                    else {
                        pw.print("N ");
                    }
                    
                    if ( k > 0 && k % lut.cols == 0 ) {
                        pw.println();
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
    }
    
    static final class RegionUpdater extends IConnectionObserver.Adapter {
        String streamName;
        IServerConnection gscon;
        Lut lut;
        Region region;
        SaverTask task;
        
        
        int prev_miny = 0;
        int prev_maxy = 0;
        

        
        RegionUpdater(String serverSpec, String streamName, Lut lut, Region region, SaverTask task) throws Exception {
            this.streamName = streamName;
            this.lut = lut;
            this.region = region;
            this.task = task;
            
            gscon = ServerConnections.openConnection(serverSpec);
            gscon.openStream(streamName, null);
            
            gscon.addObserver(this, streamName);
        }
        
        // no default log messages as a connection observer
        protected void _log(String msg) {}
        
        public void newImage(IImage iimg) {
            if ( iimg.isNull() ) {
                // End-of-stream: so, nothing more to do:
                logger.log("null-image received. Will exit after saving.. ");
                task.exitAfterSave();
                return;
            }
            
            iimg.setServerConnection(gscon);
            
            // Get resolutions:
            
            StreamDef streamDef = gscon.getStreamDef(iimg.getStreamID());
            ChannelDef channelDef = streamDef.getChannelDef();

            assert channelDef != null || Utils.fail("GetRasterData: channelDef not found for streamID: " +iimg.getStreamID());
            
            int psx = channelDef.getPixelSizeX();
            int psy = channelDef.getPixelSizeY();
            /////
            
            
            int x0 = iimg.getX();
            int y0 = iimg.getY();
            int w = iimg.getWidth() * psx;
            int h = iimg.getHeight() * psy;
            int x1 = x0 + w -1;
            int y1 = y0 + h -1;
            
            int minx = Math.min(x0, x1);
            int maxx = Math.max(x0, x1);
            int miny = Math.min(y0, y1);
            int maxy = Math.max(y0, y1);

            
            if ( true ) {
                if ( prev_miny > 0 ) {
                    if ( miny == prev_maxy + 1 ) {
                        // OK: most expected case.
                    }
                    else if ( miny > prev_maxy + 1 ) {
//                        logger.log("prev_maxy+1 < miny:  " +
//                                   "prev_maxy+1=" +(prev_maxy+1)+ "  miny=" +miny);
                        
                        int diff = miny - (prev_maxy + 1);
                        if ( diff < psy ) {
                            miny = prev_maxy + 1;
//                            logger.log("  adjusting: miny=" +miny);
                        }
                    }
                }
                prev_miny = miny;
                prev_maxy = maxy;
            }
            
            
            synchronized(region) {
            
                for ( int y = miny; y <= maxy; y++ ) {
                    for ( int x = minx; x <= maxx; x++ ) {

                        List<Long> jis = lut.getJI(x, y);
                        if ( jis != null ) {
                            int value = Images.extractPixel(iimg, x, y);
                            for ( long ji : jis ) {
                                int j = (int) (0xffffffff & (ji >>> 32));
                                int i = (int) (0xffffffff & ji);
                                region.set(j, i, value);
                            }
                        }
                    }
                }
            
            }
        }
    }
    
    
    /** Updates output file with the current pixel values in region
     */
    static final class SaverTask extends TimerTask {
        String outputDirectory;
        String outputPrefix;
        String streamName;
        Region region;
        private boolean exitAfterSave;
        
        SaverTask(String streamName, Region region, String outputDirectory, String outputPrefix) {
            this.streamName = streamName;
            this.region = region;
            this.outputDirectory = outputDirectory;
            this.outputPrefix = outputPrefix;
        }

        void exitAfterSave() {
            exitAfterSave = true;
        }

        public void run() {
            
            logger.log("Saver task starts...");
            
            Calendar cal = Calendar.getInstance();
            
            SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
            SimpleDateFormat hourFormat = new SimpleDateFormat("HHmm");
            
            String date = dateFormat.format(cal.getTime());
            String hour = hourFormat.format(cal.getTime());

            String temperat = "";
            if ( _tempConv != null ) {
                temperat = "_temp";
            }
            String filename = outputDirectory+ "/" 
                +outputPrefix + "_" +date+ "_" +hour+ temperat+ "_s_" +streamName+ ".raster";
            
            region.save(filename);

            System.gc();
            logger.log("... Saver task ends.\n");
        
            if ( exitAfterSave ) {
                System.exit(0);
            }
        }
    }
    
    
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
}
