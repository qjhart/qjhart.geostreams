/**
 * 
 */
package geostreams.util;

/**
 * A 1-element fifo container for synchronization of two (producer and consumer)
 * threads, by means of corresponding calls to the methods put() and take().
 * 
 * @author Carlos Rueda
 * @version $Id: SyncFifo.java,v 1.1 2006/11/28 06:17:43 crueda Exp $
 */
public class SyncFifo {
	/** The single stored object, if any */
	private Object currObj = null;
	
	/** Puts an object in this container. This is a blocking operation 
	 * (to be called by the producer thread) that waits until a 
	 * previously stored object is taken by the consumer thread. */
	public synchronized void put(Object obj) throws InterruptedException {
		while ( currObj != null ) {
			wait();  // until consumer thread takes object
		}
		currObj = obj;
		notifyAll();
	}

	/** Returns the stored object. This is a blocking operation
	 * (to be called by the consumer thread) that waits until the producer 
	 * thread stores an object in this container.
	 * @return the stored object.
	 */
	public synchronized Object take() throws InterruptedException {
		Object obj = null;
		while ( currObj == null ) {
			wait();  // until producer thread puts an object
		}
		obj = currObj;
		currObj = null;
		notifyAll();
		return obj;
	}
}
