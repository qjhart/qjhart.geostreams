/*
 * GeoStreams Project
 * Utilities for actor creation
 * Carlos Rueda-Velasquez and Haiyan Yang
 * $Id: ActorUtilities.java,v 1.1 2006/11/28 06:17:48 crueda Exp $
 */
package geostreams.ptolemy.actor;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.Map;
import java.util.regex.Matcher;


/**
  * ActorUtilities
  * Some actor utilities.
  *
  * @author Carlos Rueda-Velasquez and Haiyan Yang
  * @version $Id: ActorUtilities.java,v 1.1 2006/11/28 06:17:48 crueda Exp $
  */ 
public final class ActorUtilities {
	
	/** Gets an icon text with a default fill color.  */
	public static String getIconText(Map<String,Object> props) {
		String name = (String) props.get("name");
		if ( name == null ) {
			throw new RuntimeException();
		}
		
		String text = getIconText(name);
		
		for (String key : props.keySet()) {
			text = text.replaceAll("\\{" +key+ "\\}", Matcher.quoteReplacement(String.valueOf(props.get(key))));
		}
		
		return text;
	}
	
	/** Gets an icon. */
	private static String getIconText(String name) {
		String icon_filename = "src/geostreams/ptolemy/actor/svg/" +name+ "_icon.svg";
//		System.out.println("trying " +icon_filename);
		if ( new File(icon_filename).exists() ) {
			// read icon from file:
			StringBuffer contents = new StringBuffer();
			try {
				BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(icon_filename)));
				String line;
				while ( (line = br.readLine()) != null )
					contents.append(line+ "\n");
			}
			catch(Exception ex) {
				System.out.println("ActorUtilities.getIconText: Error: " +ex.getMessage());
			}
			return contents.toString();
		}
		else {
			// create icon text here:
			return 
			"<svg>\n"+
			"<rect x=\"-16\" y=\"-16\"\n"+ 
			" width=\"{width}\" height=\"{height}\"\n"+
			" style=\"fill:{fillColor}\"\n"+
			"/>\n"+
			"<circle cx=\"0\" cy=\"0\" r=\"14\"\n"+
			" style=\"fill:blue\"\n"+
			"/>\n"+
			"<text x=\"-20\" y=\"7\"\n"+
			" style=\"font-size:22; fill:white; font-family:SansSerif\">\n"+
			"  G\n"+    
			"</text>\n"+
			"<text x=\"4\" y=\"5\"\n"+
			" style=\"font-size:18; fill:black; font-family:SansSerif\">\n"+
			"  {name}\n"+
			"</text>\n"+
			"</svg>\n"	
			;
		}
	}

	/** Gets an icon text with a default fill color.  */
	public static String getIconText(int width, String name) {
		return getIconText(width, name, "0xbbd5ee");
	}
	
	/** Gets an icon. */
	public static String getIconText(int width, String name, String fillColor) {
		return getIconText(width, 32, name, fillColor);
	}
	
	/** Gets an icon. */
	public static String getIconText(int width, int height, String name, String fillColor) {
		String icon_filename = name+ "_icon.svg";
//		System.out.println("trying " +icon_filename);
		if ( new File(icon_filename).exists() ) {
			// read icon from file:
			StringBuffer contents = new StringBuffer();
			try {
				BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(icon_filename)));
				String line;
				while ( (line = br.readLine()) != null )
					contents.append(line+ "\n");
			}
			catch(Exception ex) {
				System.out.println("ActorUtilities.getIconText: Error: " +ex.getMessage());
			}
			return contents.toString();
		}
		else {
			// create icon text here:
			return 
			"<svg>\n"+
			"<rect x=\"-16\" y=\"-16\"\n"+ 
			" width=\"" +width+ "\" height=\"" +height+ "\"\n"+
			" style=\"fill:" +fillColor+ "\"\n"+
			"/>\n"+
			"<circle cx=\"0\" cy=\"0\" r=\"14\"\n"+
			" style=\"fill:blue\"\n"+
			"/>\n"+
			"<text x=\"-20\" y=\"7\"\n"+
			" style=\"font-size:22; fill:white; font-family:SansSerif\">\n"+
			"  G\n"+    
			"</text>\n"+
			"<text x=\"4\" y=\"5\"\n"+
			" style=\"font-size:18; fill:black; font-family:SansSerif\">\n"+
			"  " +name+ "\n"+
			"</text>\n"+
			"</svg>\n"	
			;
		}
	}

	private ActorUtilities() {}
}
	
