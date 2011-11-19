package geostreams.applet;

import geostreams.goes.GOESConstants;
import geostreams.image.IServerConnection;
import geostreams.image.ServerConnections;
import geostreams.vizstreams.VizStreams;

import java.awt.Dimension;
import java.net.URL;
import java.net.MalformedURLException;
import javax.swing.JApplet;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

/**
  * VizStreams applet.
  * <p>
  * Expected parameters: "geostreams" with the server spec. See
  * {@link geostreams.image.ServerConnections#openConnection(String)}
  * 
  * <p>
  * Optional parameters: "developer", "initzoom"  
  * 		   
  * @author Carlos Rueda-Velasquez
  * @version $Id: VizStreamsApplet.java,v 1.1 2007/06/02 19:54:09 crueda Exp $
 */
public class VizStreamsApplet extends JApplet {
	private VizStreams vizstreams;

	/** Gets the following parameters and instantiates a
	  * VizStreams.
	  *
	  * <pre>
	  *   geostreams: URL indicating geostreams info
	  *   developer: "yes" to include developer controls
	  *   initzoom: initial zoom factor
	  * </pre>
	  */
	public void init() {
		System.out.println("VizStreamsApplet: init " +(vizstreams != null));
		_setLookAndFeel();

		// Desired background image size; see below
		// I want width=2000 and the height according to aspect:
		int bg_width =  2000;
		int bg_height = (int) (GOESConstants.aspect * bg_width);
		Dimension canvasSize = new Dimension(bg_width, bg_height);

		int initial_zoom = 40;
		
		// parameters:
		try {
			initial_zoom = Integer.parseInt(getParameter("initzoom"));
		}
		catch(NumberFormatException ex) {
			// ignore
		}
		String serverSpec = getParameter("geostreams");
		boolean developer = "yes".equals(getParameter("developer"));
		System.out.println("geostreams=" +serverSpec);
		
		vizstreams = new VizStreams(canvasSize);
		vizstreams.setInitialZoomFactor(initial_zoom);
		
		if ( developer ) {
			System.out.println("Including developer controls");
			vizstreams.includeDevControls();
		}
		getContentPane().add(vizstreams);
		
		// check if geostreams_xml is a valid URL
		boolean put_codebase = false;
		try {
			new URL(serverSpec);
		}
		catch(Exception ex) {
			put_codebase = true;
		}
		if ( put_codebase ) {
			try {
				serverSpec = new URL(getCodeBase(), serverSpec).toString();
			}
			catch(MalformedURLException ex) {
				// shouldn't occur
				ex.printStackTrace();
			}
		}
		
		// now connect:
		System.out.println("ServerConnection: connecting using " +serverSpec);
		try {
			IServerConnection gscon = ServerConnections.openConnection(serverSpec);
			vizstreams.init(gscon);
		}
		catch(Exception ex) {
			ex.printStackTrace();
			return;
		}
		try {
			vizstreams.showOverview();
			vizstreams.start();
		} catch (Exception e) {
			JOptionPane.showMessageDialog(this, "Error starting viewer: " +e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
		}
	}
	
	private void _setLookAndFeel() {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
			SwingUtilities.updateComponentTreeUI(this);
		} catch (Exception ex) {
			System.out.println("Exception while setting L&F: " +ex.getMessage());
		}
	}
		
	/** Destroys this applet */
	public void destroy() {
		System.out.println("VizStreamsApplet: destroy");
		if ( vizstreams != null ) {
			vizstreams.stop();
			vizstreams = null;
		}
	}
}
