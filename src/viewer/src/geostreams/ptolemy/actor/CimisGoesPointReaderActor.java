package geostreams.ptolemy.actor;

import geostreams.ptolemy.icon.ReaderIcon;

import java.awt.Color;
import java.awt.Font;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;

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
 * Works like CimisReaderActor but reads GOES point data.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: CimisGoesPointReaderActor.java,v 1.4 2007/08/03 00:48:47 crueda Exp $
 */
public class CimisGoesPointReaderActor extends TypedAtomicActor {
	
	private static final String[] FILENAME_TEMPLATES = {
		"http://geostreams.ucdavis.edu/~carueda/goes-pointdata/data/realtime_{DATE}_s_{STREAM}.csv",
		"file:_pointdata/realtime_{DATE}_s_{STREAM}.csv",
	};
	
	public TypedIOPort output;
	
	private ReaderIcon _icon;
	
	
	public StringParameter dateParam;
	public StringParameter hourParam;
	public StringParameter fileTemplateParam;
	public StringParameter streamParam;

	private String date;
	private String hour;
	private String stream;
	
	private String filename;
		

    public CimisGoesPointReaderActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
        _icon = new ReaderIcon(this, "_icon");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);
        _icon.setText("GoesReader");
        _icon.setFont(new Font("SansSerif", Font.PLAIN, 14));

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(new ArrayType(BaseType.DOUBLE));
//		output.setTypeEquals(BaseType.DOUBLE);
		output.setMultiport(true);
		
		dateParam = new StringParameter(this, "dateParam");
		dateParam.setExpression("2007-07-28");
		dateParam.setDisplayName("Date (yyyy-mm-dd)");
		
		hourParam = new StringParameter(this, "hourParam");
		hourParam.setExpression("20");
		hourParam.setDisplayName("Hour (HH...)");

		streamParam = new StringParameter(this, "streamParam");
		streamParam.setExpression("1");
		streamParam.setDisplayName("Stream name");

		fileTemplateParam = new StringParameter(this, "fileTemplate");
		fileTemplateParam.setExpression(FILENAME_TEMPLATES[0]);
		for ( String fileTemplate : FILENAME_TEMPLATES ) {
			fileTemplateParam.addChoice(fileTemplate);
		}
		fileTemplateParam.setDisplayName("File template");		
    }
    
    
    public synchronized void initialize() throws IllegalActionException {
//		System.out.println("CimisGoesPointReader.initialize");
        super.initialize();
        _icon.setIconActive(true);
        
        date = dateParam.stringValue();
        hour = hourParam.stringValue();
        stream = streamParam.stringValue();

        if ( hour.length() < 2 ) {
        	throw new IllegalActionException(this, "Hour must be at least 2 chars long");
        }
        // take only the integral hour
        hour = hour.substring(0, 2);
        
        filename = fileTemplateParam.stringValue().replaceAll("\\{DATE\\}", date).replaceAll("\\{STREAM\\}", stream);
    }    
    
	public void fire() throws IllegalActionException {
		super.fire();

		InputStream in;
		System.out.println("GoesReader: Opening " +filename);
		try {
			URL url = new URL(filename);
			in = url.openStream(); 
//				new FileInputStream(filename);
		}
		catch (Exception e) {
			throw new IllegalActionException(this, null, e, "Cannot open " +filename);
		}

		try {
			
			Token[] array = new Token[204];
			
			BufferedReader br = new BufferedReader(new InputStreamReader(in)); 
			String line;
			try {
				while (( line = br.readLine()) != null ) {
					String[] toks = line.split("\\s*,\\s*");
					
					String hr = toks[0];
					String id_s = toks[1];
					String val_s = toks[4];
					
					if ( !hr.startsWith(hour) ) {
						continue;
					}
					
					int id;
					try {
						id = Integer.parseInt(id_s);
					}
					catch (RuntimeException ex) {
						// ignore
						continue;
					}
	
					float value;
					try {
						value = Float.parseFloat(val_s);
					}
					catch (RuntimeException ex) {
						// ignore
						continue;
					}
					
					
					if ( id < array.length ) {
						array[id] = new DoubleToken(value);
					}
					
				}
			}
			catch (IOException e) {
				throw new IllegalActionException(this, null, e, "Error reading " +filename);
			}		

			for (int i = 0; i < array.length; i++) {
				if ( array[i] == null ) {
					array[i] = DoubleToken.NIL;
				}
			}
			
			output.broadcast(new ArrayToken(array));
		}
		finally {
			try {
				in.close();
			}
			catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
	}
    
}
