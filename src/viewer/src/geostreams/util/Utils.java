package geostreams.util;

/**
 * Some general utilities.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: Utils.java,v 1.2 2007/06/03 03:10:07 crueda Exp $
 */
public final class Utils {
	private Utils() {}
	
	/**
	 * Prints a message a returns false always.
	 * @param msg Message to print
	 * @return false always
	 */
	public static boolean fail(String msg) {
		System.out.println(("\n" +msg).replaceAll("\n", "\nFAIL: ")+ "\n");
		return false;
	}

	public static void log(String msg) {
		System.out.println(("\n" +msg).replaceAll("\n", "\nLOG: "));
	}
}
