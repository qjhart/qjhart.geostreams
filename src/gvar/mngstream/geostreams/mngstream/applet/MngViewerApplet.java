/*
	GeoStreams Project
	MngViewerApplet - Applet to display a MNG stream from a URL.
	Carlos A. Rueda
	$Id: MngViewerApplet.java,v 1.4 2005/02/09 20:02:59 crueda Exp $
*/
package geostreams.mngstream.applet;

import javax.swing.JApplet;
import java.net.URL;
import java.net.MalformedURLException;

/**
 Applet to display a MNG stream from a URL.
 Expected parameters: "url", "saved", "newframe".  
 See applet.php
			   
 @version $Id: MngViewerApplet.java,v 1.4 2005/02/09 20:02:59 crueda Exp $
 */
public class MngViewerApplet extends JApplet {
	MainPanel mainPanel;
	MngViewer mngViewer;
	
	public void init() {
		String url = getParameter("url");
		String saved_filename = getParameter("saved");
		URL saved_url = null;
		try {
			saved_url = new URL(getCodeBase(), saved_filename);
		}
		catch(MalformedURLException ex) {
			System.out.println(ex.getMessage());  // shoudn't occur
		}
		mainPanel = new MainPanel(url, saved_url, true);
		
		if ( "yes".equals(getParameter("newframe")) ) {
			mngViewer = new MngViewer(mainPanel, true, 0);
			mngViewer.setTitle(url);
		}
		else {
			getContentPane().add(mainPanel);
		}
	}
	
	public void start() { }
	public void stop() { }
	
	public void destroy() {
		if ( mngViewer != null )
			mngViewer.destroy();
		else
			mainPanel.stop();
	}
			
}
