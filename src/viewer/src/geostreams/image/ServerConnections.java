package geostreams.image;

import java.awt.Color;
import java.awt.Rectangle;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Creates IServerConnection objects .
 *
 * @author Carlos Rueda-Velasquez
 * @version $Id: ServerConnections.java,v 1.12 2007/06/12 16:51:55 crueda Exp $
 * @navassoc - creates - IServerConnection
 */
public class ServerConnections {

	/**
	 * Opens a server connection according to a geostreams spec.
	 * 
	 * <p>
	 * The GeoStreams server can be specified in one of two ways:	
	 * <ul>
	 * 	<li> host:port of the server serving a net-enabled workflow
	 * 	<li> URL indicating an XML specification ..
	 * </ul>
	 * 
	 * @param serverSpec
	 * @throws Exception
	 */
	public static IServerConnection openConnection(String serverSpec) throws Exception {
		IServerConnection gscon;
		boolean validURL = true;
		try {
			new URL(serverSpec);
		}
		catch(MalformedURLException ex) {
			validURL = false;
		}
		
		if ( validURL ) {
			gscon = openServerConnection(serverSpec); 
		}
		else {
			String[] toks = serverSpec.split("\\s*:\\s*");
			if ( toks.length != 2 ) {
				throw new Exception("Invalid server specification. " +
						"Should be either a valid URL or of the form host:port");
			}
			String server = toks[0];
			int port = Integer.parseInt(toks[1]);
			gscon = openServerConnection(server, port);
		}
		return gscon;
	}
	
	/**
	 * Opens a server connection according to a geostreams spec.
	 * 
	 * @param geostreams_xml
	 * @throws Exception
	 */
	private static IServerConnection openServerConnection(String geostreams_xml) throws Exception {
		
		// - get info from the spec
		// - validate the info
		// - create object to be returned
		// - add some props to the object
		// - return created object
		
		// to gather info from the spec:
		final Map<String, Object> params = new HashMap<String, Object>();
		final List<VectorInfo> vectors = new ArrayList<VectorInfo>();
		final Map<String, Color> groupColorMap = new HashMap<String, Color>();
		DefaultHandler handler = new DefaultHandler() {
			public void startElement(String namespaceURI,
									 String lName, // local name
									 String qName, // qualified name
									 Attributes attrs)
			throws SAXException {
				String eName = lName; // element name
				if ( "".equals(eName) ) { 
					eName = qName; // namespaceAware = false
				}
				if ( "GeoStreams".equals(eName) ) {
					if ( attrs == null ) {
						throw new SAXException("GeoStreams element with no attributes");
					}
					int idx = attrs.getIndex("server");
					if ( idx < 0 ) {
						throw new SAXException("GeoStreams element with no server attribute");
					}
					params.put("serverName", attrs.getValue(idx));
				}
				else if ( "ReferenceSpace".equals(eName) ) {
					if ( attrs == null ) {
						throw new SAXException("ReferenceSpace element with no attributes");
					}
					if ( attrs.getIndex("x") < 0 
					||   attrs.getIndex("y") < 0
					||   attrs.getIndex("width") < 0
					||   attrs.getIndex("height") < 0 ) {
						throw new SAXException("ReferenceSpace element with no expected attributes");
					}
					Rectangle rect = (Rectangle) params.get("referenceSpace");
					if ( rect != null ) {
						throw new SAXException("ReferenceSpace redefinition");
					}
					rect = new Rectangle();
					rect.x = Integer.parseInt(attrs.getValue(attrs.getIndex("x")));
					rect.y = Integer.parseInt(attrs.getValue(attrs.getIndex("y")));
					rect.width = Integer.parseInt(attrs.getValue(attrs.getIndex("width")));
					rect.height = Integer.parseInt(attrs.getValue(attrs.getIndex("height")));
					params.put("referenceSpace", rect);
				}
				else if ( "saved".equals(eName) ) {
					if ( attrs == null ) {
						throw new SAXException("saved element with no attributes");
					}
					if ( attrs.getIndex("pattern") < 0 )  {
						throw new SAXException("saved element with no pattern attribute");
					}
					params.put("savedPattern", attrs.getValue(attrs.getIndex("pattern")));
				}
				else if ( "overview".equals(eName) ) {
					if ( attrs == null ) {
						throw new SAXException("overview element with no attributes");
					}
					if ( attrs.getIndex("channel") < 0 ) {
						throw new SAXException("overview element with no channel attribute");
					}
					
					//
					// TODO "channel" in XML to be changed to "stream"
					params.put("overviewStreamID", attrs.getValue(attrs.getIndex("channel")));
				}
				else if ( "Vector".equals(eName) ) {
					if ( attrs == null ) {
						throw new SAXException("Vector element with no attributes");
					}
					if ( attrs.getIndex("name") < 0 
					||   attrs.getIndex("url") < 0 ) {
						throw new SAXException("Vector element with no expected attributes");
					}
					String name = attrs.getValue(attrs.getIndex("name"));
					String url = attrs.getValue(attrs.getIndex("url"));
					String group = null;
					if ( attrs.getIndex("group") >= 0 ) {
						group = attrs.getValue(attrs.getIndex("group"));						
					}
					Color color = Color.WHITE;
					if ( attrs.getIndex("color") >= 0 ) {
						color = Color.decode(attrs.getValue(attrs.getIndex("color")));						
					}
					else if ( group != null && groupColorMap.containsKey(group) ) {
						color = groupColorMap.get(group);
					}
					boolean selected = false;
					if ( attrs.getIndex("selected") >= 0 ) {
						selected = Boolean.valueOf(attrs.getValue(attrs.getIndex("selected")));						
					}
					vectors.add(new VectorInfo(name, url, group, color, selected));
					groupColorMap.put(group, color);
				}
			}
		};
		
		// parse the file:
		SAXParserFactory factory = SAXParserFactory.newInstance();
		SAXParser saxParser = factory.newSAXParser();
		saxParser.parse(geostreams_xml, handler);

		// complete validations:
		String serverName = (String) params.get("serverName");
		if ( serverName == null ) {
			throw new Exception("ServerConnection: GeoStreams element undefined");
		}
		
		Rectangle rect = (Rectangle) params.get("referenceSpace");
		if ( rect == null ) {
			throw new Exception("ServerConnection: ReferenceSpace undefined");
		}
		ReferenceSpace referenceSpace = new ReferenceSpace(); 
		referenceSpace.rect = rect;
		
		// create object:
		ServerConnection gscon = new ServerConnection(serverName, referenceSpace, vectors);
		
		// add some props if given in spec:
		String savedPattern = (String) params.get("savedPattern");
		if ( savedPattern != null ) {
			gscon.setSavedPattern(savedPattern);
		}
		
		String overviewStreamID = (String) params.get("overviewStreamID");
		if ( overviewStreamID != null ) {
			gscon.getOverviewStreamID(overviewStreamID);
		}

		// get info about the available channels
		URL server_url = new URL(serverName);
		InputStream is;
		if ( server_url.getProtocol().equals("file")) {
			File file = new File(server_url.getPath()+ "/geostream?getInfo");
			if ( !file.exists() ) {
				file = new File(server_url.getPath()+ "/geostream_getInfo");	
			}
			is = new FileInputStream(file);
		}
		else {
			String url_str = serverName+ "/geostream?getInfo";
			System.out.println("url_str = " +url_str);
			URL cmd_url = new URL(url_str);
			is = cmd_url.openStream();
		}
		BufferedReader br = new BufferedReader(new InputStreamReader(is));
		String line;
		while ( (line = br.readLine()) != null ) {
			String[] tokens = line.split(" ", 7);
			if ( tokens.length == 0 ) {
				continue;
			}
			if ( !tokens[0].equals("CH") ) {
				throw new IOException("Expecting CH chunk");
			}
			
			// format:
			//	tokens[0] = "CH"
			//	tokens[1] = channel ID
			//	tokens[2] = pixel size in x
			//	tokens[3] = pixel size in y
			//	tokens[4] = aspect ratio
			//	tokens[5] = sample size
			//	tokens[6] = name
			
			if ( tokens.length < 7 ) {
				throw new IOException("CH line must have at least 7 tokens");
			}

			String channelID = tokens[1];
			int psx = Integer.parseInt(tokens[2]);
			int psy = Integer.parseInt(tokens[3]);
			double aspect = Double.parseDouble(tokens[4]);
			int sampleSize = Integer.parseInt(tokens[5]);
			String name = tokens[6];

			ChannelDef channelDef = new ChannelDef(
				channelID,
				name,
				psx,
				psy,
				aspect,
				sampleSize
			);
			
			// NOTE: Use the same channelID as the streamID
			StreamDef streamDef = new StreamDef(channelID, channelDef);
			gscon.addStreamDef(streamDef.getStreamID(), streamDef);
		}
		
		return gscon; 
	}

	/**
	 * Opens a server connection to a net-enabled workflow.
	 * 
	 * @param server server name
	 * @param port port
	 * @throws Exception
	 */
	private static IServerConnection openServerConnection(String server, int port) throws Exception {
		ServerConnection gscon = new ServerConnection(server, port);
		return gscon;
	}	
	
	/**
	 * Creates an adhoc server connection.
	 * 
	 * @param is
	 * @throws Exception
	 */
	public static IServerConnection createAdHocServerConnection(NewImageSequence is) {
		return new AdHocServerConnection(is);
	}

	/** test program: argument: geostreams server spec */
	public static void main(String[] args) throws Exception {
		IServerConnection gscon = openConnection(args[0]);
		System.out.println("\nserver='" +gscon.getServerShortDescription()+ "'");
		System.out.println("reference space: " +gscon.getReferenceSpace());
		System.out.println("overview stream: " +gscon.getOverviewStreamDef());
		System.out.println("channels:");
		for ( StreamDef def : gscon.getStreamDefs() ) {
			System.out.println("  streamDef: " +def);
		}
		System.out.println("vectors:");
		for ( VectorInfo v :gscon.getVectors() ) {
			System.out.println("  vector: name='" +v.getName()+ "'\n"+
				               "           url='" +v.getURL()+ "'"
			);
		}
	}
	
	private ServerConnections() {}
}
