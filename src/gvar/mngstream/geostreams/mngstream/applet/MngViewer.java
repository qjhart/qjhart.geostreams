/*
	GeoStreams Project
	MngViewer - Program to display a MNG stream from a URL.
	Carlos A. Rueda
	$Id: MngViewer.java,v 1.5 2005/02/24 09:13:37 crueda Exp $
*/
package geostreams.mngstream.applet;

import javax.swing.*;
import java.awt.event.*;
import java.util.Date;

/**
 Standalone visualizer of the goes mng stream.
 See MngStreamClient for a non-gui mng stream reader.
 */
public class MngViewer extends JFrame  {
	private boolean from_main = false;
	private MainPanel mainPanel;
	private boolean gui;

	public MngViewer(MainPanel mainPanel, boolean gui, final int delay) {
		super();
		this.mainPanel = mainPanel;
		this.gui = gui;
		
		getContentPane().add(mainPanel);
		if ( gui ) {
			setSize(600,500);
			setVisible(true);
			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent ev) {
					destroy();
				}
			});
		}
		else {
			setSize(600,100);
			setVisible(true);
			// try{Thread.sleep(5*1000);}catch(InterruptedException ex){}
			// setVisible(false);
			addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent ev) {
					MngViewer.this.setVisible(false);
				}
			});
		}
		
		if ( delay > 0 ) {
			ActionListener taskPerformer = new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
					MngViewer.this.mainPanel.save();
				}
			};
			new Timer(delay, taskPerformer).start();
		}
	}

	void destroy() {
		mainPanel.stop();
		dispose();
		System.out.println(new Date()+ ": MngViewer: Exiting");
		if ( from_main )
			System.exit(0);
	}

    public static void main(String[] args) {
		String mng_in = "http://casil.ucdavis.edu/~crueda/goes/goes";
		String saved_filename = "saved_vis.jpg";
		boolean gui = true;
		int delay = 0;   // no automatic saving
		
		final String usage = 
			"Usage: java MngViewer [-url mngURL] [-saved filename] " +
			"[-gui {yes|no}] [-delay seconds]\n" +
			"  Defaults:\n" +
			"	url      " +mng_in+ "\n" +
			"	saved    " +saved_filename+ "\n" +
			"	gui      " +(gui ? "yes" : "no")+ "\n" +
			"	delay    " +delay+ "\n"
		;
		
		for ( int i = 0; i < args.length; i++ ) {
			if ( args[i].equals("-url") )
				mng_in = args[++i];
			else if ( args[i].equals("-gui") )
				gui = args[++i].equals("yes");
			else if ( args[i].equals("-saved") )
				saved_filename = args[++i];
			else if ( args[i].equals("-delay") )
				delay = 1000 * Integer.parseInt(args[++i]);
			else {
				System.err.println(usage);
				return;
			}
		}
		
		System.out.println(new Date()+ ": Starting");		
		System.out.println("url   = " +mng_in);
		System.out.println("saved = " +saved_filename);
		System.out.println("gui   = " +(gui ? "yes" : "no"));

		MainPanel mainPanel = new MainPanel(mng_in, saved_filename, false);
		MngViewer mngViewer = new MngViewer(mainPanel, gui, delay);
		mngViewer.setTitle(mng_in);
		mngViewer.from_main = true;
	}
}

