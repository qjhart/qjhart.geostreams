package geostreams.util;

import geostreams.goes.TemperatureConversion;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Properties;


/**
 * Gets GOES point data and updates a postgres db.
 * 
 * 
 * <p>
 * Initial version based on CimisGoesPointReaderActor.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: PgUpdateGoesPoints.java,v 1.1 2007/08/23 09:21:19 crueda Exp $
 */
public class PgUpdateGoesPoints {
	private static final String DRIVER = "org.postgresql.Driver";
	private static final String TABLE = "goes_cimis_temperature";

	private static final String DEFAULT_CONNECTION = "jdbc:postgresql://localhost:5432/cimistest";

	private static boolean verbose = false;


	private String connection;
	private String user;
	private String password;
	
	private String date;
	private int stream;
	
	private Connection _con;
	private Statement _stmt;
	
	private int updated;
	private int inserted;
	private boolean forceInsert;
	
		

    public PgUpdateGoesPoints(String connection, String user, String password) throws Exception {
    	this.connection = connection;
    	this.user = user;
    	this.password = password;
		_getConnection();
	}

	private void prepareForUpdate(boolean forceInsert, String date, int stream) {
		this.forceInsert = forceInsert;
		this.date = date;
		this.stream = stream;
		updated = inserted = 0;
	}


	private void _getConnection() throws Exception {
		try {
		    Class.forName(DRIVER);
		    Properties props = new Properties();
		    props.put("CONNECTION", connection);
		    props.put("user", user);
		    props.put("password", password);
		    log("Getting connection: props = " +props);
		    _con = DriverManager.getConnection(props.getProperty("CONNECTION"), props);
		    _stmt = _con.createStatement();
		}
		catch (ClassNotFoundException ex) {
		    throw new Exception("The database driver class is not found! Please make sure that the postgreSQL jdbc lib is included!", ex);
		}
		catch (SQLException ex) {
			throw new Exception("Establishing a connection to database failed: " + ex.toString(), ex);
		}
    }
    
    
	private static void log(String string) {
		if ( verbose ) {
			System.out.println(string);
		}
		
	}

	private void processRow(int id, String hour, int col, int lin, float value, float temperature) throws SQLException {
		if ( forceInsert || !_existsRow(id, hour) ) {
			_insertRow(id, hour, col, lin, value, temperature);
			inserted++;
		}
		else {
			_updateRow(id, hour, col, lin, value, temperature);
			updated++;
		}
	}


	private boolean _existsRow(int id, String hour) throws SQLException {
        String query = "SELECT * FROM " +TABLE+ " WHERE " +
        	"st_number='" +id+ "' AND date='" +date+ "' AND hour='" +hour+ "' AND channel='" +stream+ "'" +
        ";";

        log(query);
        
        ResultSet rs = _stmt.executeQuery(query);
       	return rs.next();
	}

	private boolean _updateRow(int id, String hour, int col, int lin, float value, float temperature) throws SQLException {
        String query = "UPDATE " +TABLE+ " SET " +
        	"(st_number,date,hour,channel,col,lin,raw_value,temperature)=" +
        	"('" +id+ "','" +date+ "','" +hour+ "','" +stream+ "','" +col+ "','" +lin+ "','" +value+ "','" +temperature+ "')" +
        	" WHERE " + "st_number='" +id+ "' AND date='" +date+ "' AND hour='" +hour+ "'" +
        ";";

        log(query);

        return _stmt.execute(query);
	}


	private boolean _insertRow(int id, String hour, int col, int lin, float value, float temperature) throws SQLException {
        String query = "INSERT INTO " +TABLE+ " VALUES (" +
        		"'" +id+ "','" +date+ "','" +hour+ "','" +stream+ "','" +col+ "','" +lin+ "','" +value+ "','" +temperature+ "'" +
        	");";
        
        log(query);

        return _stmt.execute(query);
	}
    
	private void done() throws SQLException {
		if ( _con != null ) {
			_con.close();
		}
	}
	
	private boolean createTable() throws SQLException {
        String query = 
        	"CREATE TABLE goes_cimis_temperature ( st_number integer, date date, hour time, channel integer, col integer, lin integer, raw_value real, temperature real );\n"+
        	"create unique INDEX pkey on goes_cimis_temperature (st_number , date, hour, channel);"
        ;
        
        log(query);

        return _stmt.execute(query);
	}



	public static void main(String[] args) throws Exception {
		String connection = DEFAULT_CONNECTION;
		String user = "goes";
		String password = "goes";
		boolean createTable = false;
		boolean forceInsert = false;
		
		SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
		
		// date initialized with today's date:
		Calendar cal = Calendar.getInstance();
		String date = dateFormat.format(cal.getTime());

		String hour = "*";  // all hours
    	boolean allHours = hour.equals("*");
    	
    	// 2 = "first" infrared channel in constants for conversion.
    	// But note that the CSV file is named with a 0-based channel -- see below
    	int channel = 2;      

		
		for ( int arg = 0; arg < args.length; arg++ ) {
			if ( args[arg].equals("--connection") ) {
				connection = args[++arg]; 
			}
			else if ( args[arg].equals("--user") ) {
				user = args[++arg]; 
			}
			else if ( args[arg].equals("--password") ) {
				password = args[++arg]; 
			}
			else if ( args[arg].equals("--createTable") ) {
				createTable = true;
			}
			else if ( args[arg].equals("--forceInsert") ) {
				forceInsert = true;
			}
			else if ( args[arg].equals("--date") ) {
				date = args[++arg]; 
			}
			else if ( args[arg].equals("--hour") ) {
				hour = args[++arg]; 
			}
			else if ( args[arg].equals("--channel") ) {
				channel = Integer.parseInt(args[++arg]); 
			}
			else if ( args[arg].equals("--verbose") ) {
				verbose = true;
			}
			else if ( args[arg].equals("--help") ) {
				System.out.println(
						"\nOptions:  (default ; other examples)\n" +
						"	--connection <con>   (" +connection+ ")\n" +
						"	--user <user>        (" +user+ ")\n" +
						"	--password <pwd>     (" +"xxxx"+ ")\n" +
						"	--createTable        just creates the table and exits\n" +
						"	--forceInsert        No check for raw existence -- speeds up insertion\n" +
						"	--date <date>        (" +date+ " ;  yesterday)\n" +
						"	--hour <hour>        (" +hour+ " ; 1000)\n" +
						"	--channel <channel>  (" +channel+ ")\n" +
						"	--verbose            prints the sql statements\n" +
						"\n"
				);
				return;
			}
		}

		String dateNote = "";
		if ( date.equals("yesterday") ) {
			cal.add(Calendar.DAY_OF_YEAR, -1);
			date = dateFormat.format(cal.getTime());
			dateNote = "(=yesterday)";
		}


		// Initializes db updater:
		PgUpdateGoesPoints updater = new PgUpdateGoesPoints(connection, user, password);
		
		if ( createTable ) {
			System.out.println(new Date()+ ": createTable..");
			updater.createTable();
			updater.done();
			return;
		}
		
		System.out.println(new Date()+ ": date=" +date+dateNote+ " hour=" +hour+ " channel=" +channel+ " forceInsert=" +forceInsert);

		TemperatureConversion tempConv = new TemperatureConversion(channel);

		updater.prepareForUpdate(forceInsert, date, channel);
		
        String filenameTemplate = 
			"http://geostreams.ucdavis.edu/~carueda/goes-pointdata/data/realtime_{DATE}_s_{STREAM}.csv";
		
        if ( hour.equals("*") ) {
        	allHours = true;
        }
        else {
	        if ( hour.length() < 2 ) {
	        	throw new Exception("Hour must be at least 2 chars long, or be equal to *");
	        }
	        // take only the integral hour
	        hour = hour.substring(0, 2);
        }
        
        // Note the (stream -1) to access the corresponding CSV file:
        String filename = filenameTemplate
        	.replaceAll("\\{DATE\\}", date)
        	.replaceAll("\\{STREAM\\}", "" +(channel - 1))
        ;

        
        // Open input file:
        
        BufferedReader br;
        log("Opening " +filename);
		try {
			URL url = new URL(filename);
			InputStream in = url.openStream(); 
			br = new BufferedReader(new InputStreamReader(in));         
		}
		catch (Exception e) {
			throw new Exception("Cannot open " +filename, e);
		}

        
		// Scan lines and update rows in db:
		String line;
		while (( line = br.readLine()) != null ) {
			String[] toks = line.split("\\s*,\\s*");

			String hr = toks[0];
			String id_s = toks[1];
			String col_s = toks[2];
			String lin_s = toks[3];
			String val_s = toks[4];

			if ( allHours ) {
				if ( hr.length() == 4 ) {
					// == 4 corresponds to the format used by GetPointData.
					hour = hr.substring(0, 2)+ ":" +hr.substring(2, 4);
				}
				else {
					// Not expected; but put something:
					hour = hr;
				}
				
			}
			else if ( !hr.startsWith(hour) ) {
				continue;
			}

			int id, col, lin;
			float value;
			try {
				id = Integer.parseInt(id_s);
				col = Integer.parseInt(col_s);
				lin = Integer.parseInt(lin_s);
				value = Float.parseFloat(val_s);
			}
			catch (RuntimeException ex) {
				// ignore
				continue;
			}
			
			float temperature = (float) tempConv.convert(value);


			updater.processRow(id, hour, col, lin, value, temperature);
		}

		updater.done();
		br.close();
		
		System.out.println("inserted=" +updater.inserted+ "  updated=" +updater.updated);
	}
	
}
