package geostreams.ptolemy;

import java.io.File;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import javax.swing.SwingUtilities;

import ptolemy.actor.gui.Configuration;
import ptolemy.actor.gui.Tableau;
import ptolemy.actor.gui.UserActorLibrary;
import ptolemy.vergil.VergilApplication;

/**
 * Some utilities to update the vergil GUI.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: VergilUtils.java,v 1.1 2007/01/13 10:45:03 crueda Exp $
 */
public class VergilUtils {

	@SuppressWarnings("unchecked")
	private static Configuration _getConfiguration() {
		List<Configuration> confs = Configuration.configurations();
    	return confs.size() == 0 ? null : confs.get(0);
	}
	/**
	 * Opens a library.
	 * @param file
	 */
	public static void openLibrary(File file) {
		Configuration conf = _getConfiguration();
    	if (  conf == null ) {
    		return;
    	}
    	try {
			UserActorLibrary.openLibrary(conf, file); 
		} 
		catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Opens a model.
	 * @param url Model URL, e.g., URL("file:workflows/roi1.xml")
	 */
	public static void openModel(URL url) {
		Configuration conf = _getConfiguration();
    	if (  conf == null ) {
    		return;
    	}
    	try {
			Tableau t = conf.openModel(
					null, 
					url, 
					url.toExternalForm()
			);
			t.show();
		} 
		catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Runs VergilApplication.
	 * @param args Args for VergilApplication
	 */
	public static void runVergilApplication(String[] args) {
		VergilApplication.main(args);
	}
	/**
	 * Runs the VergilApplication accepting a new option
	 * --openLib filename to open a library
	 * @param args Regular args for VergilApplication except 
	 * any ("--openLib" "filename") pairs.
	 */
	public static void main(final String[] args) {
		List<String> newArgs = new ArrayList<String>();
		File libFile = null;
		for ( int i = 0; i < args.length; i++ ) {
			if ( args[i].equals("--openLib") ) {
				libFile = new File(args[++i]);
				System.out.println("--openlib " +libFile);
			}
			else {
				newArgs.add(args[i]);
			}
		}
		runVergilApplication((String[]) newArgs.toArray(new String[0]));
		
		if ( libFile != null ) {
			final File file = libFile;
	        SwingUtilities.invokeLater(new Runnable() {
	            public void run() {
	                try {
	                	openLibrary(file);
	                } 
	                catch (Throwable throwable) {
	                    System.out.println("MyVergil: Command failed: " +throwable);
	                }
	            }
	        });
		}
	}
}
