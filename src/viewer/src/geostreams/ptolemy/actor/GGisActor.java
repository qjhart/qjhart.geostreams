package geostreams.ptolemy.actor;

import geostreams.ptolemy.icon.ReaderIcon;
import ggis.GGis;
import ggis.IGisDb;
import ggis.IRaster;
import ggis.IGisDb.IMapset;

import java.awt.Color;
import java.awt.Font;
import java.util.ArrayList;
import java.util.List;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ArrayToken;
import ptolemy.data.DoubleToken;
import ptolemy.data.Token;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.ArrayType;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Gets a samples a GGis raster.
 * It outputs an array with the obtained samples.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: GGisActor.java,v 1.1 2007/09/16 09:47:03 crueda Exp $
 */
public class GGisActor extends TypedAtomicActor {
	
	private static final String[] BASES = {
        "/home/carueda/data/gdb/California/BIN_GRASS",
        "http://ceres.cstars.ucdavis.edu/~carueda/grass/",
	};
	
	public TypedIOPort output;
	
	private ReaderIcon _icon;
	
	public StringParameter baseParam;
    public StringParameter mapsetParam;
    public StringParameter rasterParam;
    public StringParameter gridxParam;
    public StringParameter gridyParam;

	private String baseName;
    private String mapsetName;
    private String rasterName;
    private int gridx;
    private int gridy;

    private IGisDb gisdb;

    private IMapset mapset;

    private IRaster raster;


    public GGisActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
        _icon = new ReaderIcon(this, "_icon");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);
        _icon.setText("GGisReader");
        _icon.setFont(new Font("SansSerif", Font.PLAIN, 14));

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(new ArrayType(BaseType.DOUBLE));
//		output.setTypeEquals(BaseType.DOUBLE);
		output.setMultiport(true);
		

		baseParam = new StringParameter(this, "baseParam");
		baseParam.setExpression(BASES[0]);
		for ( String base : BASES ) {
			baseParam.addChoice(base);
		}
		baseParam.setDisplayName("Ggis base");		

        mapsetParam = new StringParameter(this, "mapsetParam");
        mapsetParam.setExpression("map");
        mapsetParam.setDisplayName("Mapset");

        rasterParam = new StringParameter(this, "raster1Param");
        rasterParam.setExpression("realtime_raster_2007-09-13_0913_temp_s_1");
        rasterParam.setDisplayName("raster 1");

        gridxParam = new StringParameter(this, "gridxParam");
        gridxParam.setExpression("10");
        gridxParam.setDisplayName("Grid X");

        gridyParam = new StringParameter(this, "gridyParam");
        gridyParam.setExpression("10");
        gridyParam.setDisplayName("Grid Y");
    }
    
    
    public synchronized void initialize() throws IllegalActionException {
        super.initialize();
        _icon.setIconActive(true);
        
        baseName = baseParam.stringValue();
        mapsetName = mapsetParam.stringValue();
        rasterName = rasterParam.stringValue();
        gridx = Integer.parseInt(gridxParam.stringValue());
        gridy = Integer.parseInt(gridyParam.stringValue());
        
        try {
            gisdb = GGis.open(baseName);
            mapset = gisdb.getMapset(mapsetName);
            raster = mapset.getRaster(rasterName);
        }
        catch (Exception e) {
            throw new IllegalActionException("Cannot open database or get raster: " +e.getMessage());
        }
    }    
    
	public void fire() throws IllegalActionException {
		super.fire();
        
        int cols = raster.getCols();
        int rows = raster.getRows();
        float[] data = raster.getData();

        List<Float> list = new ArrayList<Float>();
        
        for (int j = 0; j < cols; j += gridx) {
            for (int i = 0; i < rows; i += gridy) {
                float val = data[i + rows*j];
                if ( !Float.isNaN(val) ) {
                    list.add(val);
                }
            }
            
        }
        
        Token[] array = new Token[list.size()];
        for (int k = 0; k < array.length; k++) {
            array[k] = new DoubleToken(list.get(k));
        }
			
        output.broadcast(new ArrayToken(array));
	}
    
    public boolean postfire() throws IllegalActionException {
        return false;
    }
}
