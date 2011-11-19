/**
 * 
 */
package geostreams.ptolemy;

import geostreams.image.IStream;
import geostreams.image.IStreamGenerator;
import geostreams.ptolemy.actor.ImageStreamReaderActor;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import ptolemy.actor.IOPort;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.Entity;
import ptolemy.kernel.Port;

/**
 * @author Carlos Rueda
 * @version $Id: PtUtils.java,v 1.6 2007/06/12 16:51:55 crueda Exp $
 */
public class PtUtils {
	
	
	/**
	 * TODO : 0 istream
	 * Gets the IStreamGenerator instances in the model. 
	 */
	public static List<IStreamGenerator> getStreamGenerators(CompositeEntity container) {
		List entityList = container.entityList();
		List<IStreamGenerator> list = new ArrayList<IStreamGenerator>();
		for ( Iterator it = entityList.iterator(); it.hasNext(); ) {
			Entity entity = (Entity) it.next();
			if ( entity instanceof IStreamGenerator ) {
					list.add((IStreamGenerator) entity);
			}
		}
		return list;
	}

	
	
	
	/** Gets the SS instances in the model. */
	public static List<StreamServerActor> getStreamServerActors(CompositeEntity container) {
		List entityList = container.entityList();
		List<StreamServerActor> list = new ArrayList<StreamServerActor>();
		for ( Iterator it = entityList.iterator(); it.hasNext(); ) {
			Entity entity = (Entity) it.next();
			if ( entity instanceof StreamServerActor ) {
					list.add((StreamServerActor) entity);
			}
		}
		return list;
	}

	/** Gets the IStreamActor instances in the model. */
	public static List<IStreamActor> getStreamActors(CompositeEntity container) {
		List entityList = container.entityList();
		List<IStreamActor> list = new ArrayList<IStreamActor>();
		for ( Iterator it = entityList.iterator(); it.hasNext(); ) {
			Entity entity = (Entity) it.next();
			if ( entity instanceof IStreamActor ) {
					list.add((IStreamActor) entity);
			}
		}
		return list;
	}

	/** Gets the ImageStreamReaderActor instances in the model. */
	public static List<ImageStreamReaderActor> getImageStreamReaderActors(CompositeEntity container) {
		List entityList = container.entityList();
		List<ImageStreamReaderActor> list = new ArrayList<ImageStreamReaderActor>();
		for ( Iterator it = entityList.iterator(); it.hasNext(); ) {
			Entity entity = (Entity) it.next();
			if ( entity instanceof ImageStreamReaderActor ) {
					list.add((ImageStreamReaderActor) entity);
			}
		}
		return list;
	}

	/** Gets the SS actor for the specified stream name. */
	public static StreamServerActor getStreamServerActor(CompositeEntity container, String streamName) {
		List entityList = container.entityList();

		for ( Iterator it = entityList.iterator(); it.hasNext(); ) {
			Entity entity = (Entity) it.next();
			if ( entity instanceof StreamServerActor
			&&   entity.getName().equals(streamName) ) {
					return (StreamServerActor) entity;
			}
		}
		return null;
	}

	/** Gets the output port for the specified stream name. */
	public static IOPort getOutputStream(CompositeEntity container, String streamName) {
		System.out.println("PtUtils.getOutputStream: " +streamName);

		if ( false ) {
			System.out.println(" container.getName() = " + container.getName());
			for ( Iterator it = container.entityList().iterator(); it.hasNext(); ) {
				Entity entity = (Entity) it.next();
				System.out.println(" entity.getName() = " + entity.getName());
				System.out.println(" entity.getClass() = " + entity.getClass());
				System.out.println(" entity instanceof IStreamGenerator = " +(entity instanceof IStreamGenerator));
			}
		}
			
		for ( Iterator it = container.entityList().iterator(); it.hasNext(); ) {
			Entity entity = (Entity) it.next();
			if ( entity instanceof IStreamGenerator ) {
				IStream istream = ((IStreamGenerator) entity).getGeneratedStream();
				System.out.println("    getOutputStream: istream=" +istream);

				if ( istream.getStreamID().equals(streamName) ) {

//					if ( entity.getName().equals(streamName) ) {

					for ( Iterator it2 = entity.portList().iterator(); it2.hasNext(); ) {
						Port port = (Port) it2.next();

						if ( (port instanceof IOPort) && ((IOPort) port).isOutput() ) {
							return (IOPort) port;
						}
					}
				}
			}
		}
				
		if ( false ) { // TODO remove this old scheme
			for ( Iterator it = container.entityList().iterator(); it.hasNext(); ) {
				Entity entity = (Entity) it.next();

				if ( entity instanceof StreamServerActor ) {
					continue;   // ignore
				}

				for ( Iterator it2 = entity.portList().iterator(); it2.hasNext(); ) {
					Port port = (Port) it2.next();

					if ( !(port instanceof IOPort) ) {
						continue;
					}

					IOPort ioport = (IOPort) port;
					if ( !ioport.isOutput() ) {
						continue;
					}

					if ( ioport.getName().equals(streamName) ) {
						return ioport;
					}
				}
			}
		}
		return null;
	}

	/** Gets the current streams in the workflow */
	public static List<IOPort> getStreams(CompositeEntity container) {
		List<IOPort> list = new ArrayList<IOPort>();		
		List entityList = container.entityList();

		for ( Iterator it = entityList.iterator(); it.hasNext(); ) {
			Entity entity = (Entity) it.next();
			System.out.println("Entity: " +entity);
			for ( Iterator it2 = entity.portList().iterator(); it2.hasNext(); ) {
				Port port = (Port) it2.next();
				System.out.println("   Port: " +port);
					
				if ( !(port instanceof IOPort) )
					continue;
				
				IOPort ioport = (IOPort) port;
				if ( !ioport.isOutput() )
					continue;
				
				list.add(ioport);
			}
		}
		return list;
	}

	public static void showCurrentPorts(CompositeEntity container) {		
		List entityList = container.entityList();

		for ( Iterator it = entityList.iterator(); it.hasNext(); ) {
			Entity entity = (Entity) it.next();
			System.out.println("Entity: " +entity);
			for ( Iterator it2 = entity.portList().iterator(); it2.hasNext(); ) {
				Port port = (Port) it2.next();
				System.out.println("   Port: " +port);
				
				/*
				if (port instanceof IOPort
						&& ((IOPort) port).isInput()
						&& (((IOPort) port).getWidth() == 0)) {
					container.connect(output, (IOPort) port);
					return;
				}
				*/
			}
		}
		
	}		

    private PtUtils() {}

}
