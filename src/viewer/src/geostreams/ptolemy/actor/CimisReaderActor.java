package geostreams.ptolemy.actor;

import edu.ucdavis.cstars.cimis.Cimis;
import edu.ucdavis.cstars.cimis.Utils;
import geostreams.ptolemy.icon.ReaderIcon;

import java.awt.Color;
import java.awt.Font;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

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
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: CimisReaderActor.java,v 1.4 2007/08/01 02:47:21 crueda Exp $
 */
public class CimisReaderActor extends TypedAtomicActor {
	
	public TypedIOPort output;
	
	private ReaderIcon _icon;
	
	public StringParameter dateParam;
	public StringParameter hourParam;

	private String date;
	private String hour;
	
	
	private Cimis cimis;

    public CimisReaderActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
        _icon = new ReaderIcon(this, "_icon");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);
        _icon.setText("CimisReader");
        _icon.setFont(new Font("SansSerif", Font.PLAIN, 14));

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(new ArrayType(BaseType.DOUBLE));
//		output.setTypeEquals(BaseType.DOUBLE);
		output.setMultiport(true);
		
		dateParam = new StringParameter(this, "dateParam");
		dateParam.setExpression("2007-07-29");
		dateParam.setDisplayName("Date (yyyy-mm-dd|today|yesterday)");
		
		hourParam = new StringParameter(this, "hourParam");
		hourParam.setExpression("0100");
		hourParam.setDisplayName("Hour (HHMM|now)");
	}
    
    
    public synchronized void initialize() throws IllegalActionException {
//		System.out.println("CimisReader.initialize");
        super.initialize();
        _icon.setIconActive(true);
        
        date = dateParam.stringValue();
        hour = hourParam.stringValue().replaceAll(":", "");

        cimis = new Cimis();
        String sensor = "HLY_AIR_TMP";
        cimis.setSensor(sensor);
    }    
    
	public void fire() throws IllegalActionException {
		super.fire();
		
		String user = "carueda";
		String pass = "cimis";
		try {
			cimis.login(user, pass);
		}
		catch (Exception e) {
			throw new IllegalActionException(this, null, e, "Cannot login");
		}

		if ( date.equals("today") || date.equals("yesterday") ) {
			Calendar cal = Calendar.getInstance();
			if ( date.equals("yesterday") ) {
				cal.add(Calendar.DAY_OF_MONTH, -1);
			}
			Date jdate = cal.getTime();
			SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
			date = dateFormat.format(jdate);
		}
		
		if ( hour.equals("now") ) {
			// this is current time:
			Calendar cal = Calendar.getInstance();
			Date jdate = cal.getTime();
			SimpleDateFormat hourFormat = new SimpleDateFormat("HH00");
			hour = hourFormat.format(jdate);
		}
		
		cimis.setDate(date);
		cimis.setHours(hour);

		cimis.setReport("hourly");
		String outputMethod = "NON";
		cimis.setOutputMethod(outputMethod);

		String expectedContentType = Utils.getExpectedContentType(outputMethod);
		
		InputStream in;
		try {
			try {
				in = cimis.download();
			}
			catch (Exception e) {
				throw new IllegalActionException(this, null, e, "Error downloading CIMIS data. " +cimis);
			}
			String contentType = cimis.getContentType();
			
			if ( expectedContentType == null
			|| contentType.indexOf(expectedContentType) < 0 ) {
				throw new IllegalActionException(this, "Error in returned content-type: " +contentType+ ". " +cimis);	
			}
	
	
			Token[] array = new Token[204];
			
			BufferedReader br = new BufferedReader(new InputStreamReader(in)); 
			String line;
			try {
				while (( line = br.readLine()) != null ) {
					String[] toks = line.split("\\s*,\\s*");
					if ( toks[0].startsWith("#") ) {
						continue;
					}
					
					int id;
					try {
						String s = toks[0].trim();
						id = Integer.parseInt(s);
					}
					catch (RuntimeException ex) {
						// ignore
						continue;
					}
	
					float value;
					try {
						String s = toks[5].trim();
						value = Float.parseFloat(s);
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
				throw new IllegalActionException(this, null, e, "Error reading CIMIS data");
			}		

			for (int i = 0; i < array.length; i++) {
				if ( array[i] == null ) {
					array[i] = DoubleToken.NIL;
				}
			}
			
			output.broadcast(new ArrayToken(array));
		}
		finally {
			cimis.disconnect();
		}
		
	}
    
}
