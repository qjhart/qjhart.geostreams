package geostreams.image;


/**
 * All components declared to generate IImage streams should
 * implement this interface.
 * 
 * <p>
 * TODO : 0 istream
 * 
 * @author Carlos Rueda-Velasques
 * @version $Id: IStreamGenerator.java,v 1.1 2007/06/03 21:15:37 crueda Exp $
 */
public interface IStreamGenerator {

	/**
     * @return the stream associated with the images
     * generated from this generator.
     */
	public IStream getGeneratedStream();
}
