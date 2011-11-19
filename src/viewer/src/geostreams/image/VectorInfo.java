package geostreams.image;

import java.awt.Color;
import java.awt.Point;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.Serializable;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;


/**
  * Vector datasource. 
  * Currently only a list of polylines.
  * 		   
  * @author Carlos Rueda-Velasquez
  * @version $Id: VectorInfo.java,v 1.5 2007/06/02 20:10:22 crueda Exp $
 */
public class VectorInfo implements Serializable {
	private String name;
	private String url;
	private String group;
	private Color color;
	private boolean selected;
	private List<List<Point>> polyLines;
	
	VectorInfo(String name, String url, String group, Color color, boolean selected) {
		this.name = name;
		this.url = url;
		this.group = group;
		this.color = color;
		this.selected = selected;
	}
	
	public String getName() { return name; }
	public String getURL() { return url; }
	public String getGroup() { return group; }
	public Color getColor() { return color; }
	public boolean isSelected() { return selected; }
	
	/** Each polyLine is a list of points */
	public List<List<Point>> getPolyLines() throws Exception {
		if ( polyLines == null ) {
			BufferedReader br = new BufferedReader(
				new InputStreamReader(
					new URL(url).openStream()
				)
			);
			polyLines = new ArrayList<List<Point>>();
			
			String str;
			List<Point> polyLine = null;
			
			while ( (str = br.readLine()) != null ) {
				if ( str.equals("LINESTRING") ) {
					polyLine = new ArrayList<Point>();
					polyLines.add(polyLine);
					continue;
				}
				
				if ( polyLine == null ) {
					System.out.println("getPolyLines: error: expecting LINESTRING");
					return polyLines;
				}
				
				String[] toks = str.split(",");
				if ( toks.length < 4 ) {
					continue;
				}
				
				int x = Integer.parseInt(toks[0]);
				int y = Integer.parseInt(toks[1]);
				polyLine.add(new Point(x, y));
			}
		}
		return polyLines;
	}
	
	public String toString() {
		StringBuffer sb = new StringBuffer();
		sb.append("VectorInfo:{" +name+ ", url=" +url);
		if ( polyLines != null ) {
			sb.append(", " +polyLines.size()+ " polyLines");
		}
		else {
			sb.append(", no polyLines loaded");			
		}
		sb.append("}");
		return sb.toString();
	}

}

