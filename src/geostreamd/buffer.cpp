/*
Look for "Coordinating Activities with Semaphores"

A Buffer is an object that can have one writer, and any number of
readers.  The main role of the buffer is to keep track of the data
between the readers and writers, and also to monitor the threads that
are being used by the readers, and to stop any readers that have
caught up to the writer.  The buffer can be either fixed or varying
size.  For fixed buffers, readers that are too slow can be
invalidated, or the writer can be stopped, based on the calling
parameter. 

Fixed size buffers use a circular array.  The writer knows when it has
overtaken a reader when the reader's semaphore count exceeds the
length of the buffer.  Here, the buffer can invalidate the reader, or
stop the writer thread.

 0 1 2 3 4 5 6 7 8 9 a b c d e f g
| | | | | | | | | | | | | | | | | | 
   ^         ^           ^
   |         |           | Writer
   |         Reader[0]      
    Reader[1]

Varying buffers can use a linked list.  They should maintain a pool of
list elements, so that there's not too much overhead in allocaing list
elements.  



It would be nice to include the ability to communicate via callbacks
to the reader functions as well.

Still unclear how to manage the tuples in the system, In terms of
readers removing the tuples.  Can have either a StorageManager, or
have Tuples, that count their usage.

The idea of the Buffer, is that there is a BufferPointer Class for
each of the readers.  The count in this class is both a count of the
number of available buffers, and also acts as a semaphore, blocking
reads when the buffer is empty.

*/

#ifndef __buffer_h
#define __buffer_h

namespace Geostream {
  class BufferPointer {
  public:
	Buffer *buffer;
	virtual ~BufferPointer();
  } ;

  class BufferWriter: public BufferPointer {
  private:
    int write_p ;
  public:
        BufferWriter (Buffer *buffer, int write_p) {
          this->buffer = buffer ;
          this->write_p = write_p ;
        }
	
	void write ( void *data);
	
	void write( void *data) {
	  setd:set <BufferReaders*> readers = this->buffer->readers;
	  mutex_lock
		add_data
		//end lock
		for( i=0; i<buffer->num_readers;  i++ ) {
		  semaphore_up(reader[i]->count);
		  If (reader[i]->count > length);
		  invalidate_reader;
		}
	}

        friend class Buffer ;
  } ;

  class BufferReader: public BufferPointer {
  private:
	semaphore count;
	int buffer_p;				/* Pointer into the Buffer */
	
  public:
	BufferReader(Buffer* buffer, int buffer_p);

	BufferReader(Buffer* buffer, int buffer_p) {
          this->buffer = buffer ;
          this->buffer_p = buffer_p ;
	  semaphore_init(count);
	}

	void read() {
	  semaphore_down(this->count) ;
	  data=buff->buffer[bp->buffer_p];

	  (MUTEX)
		this->buffer_p = this->buffer_p % buff.length
		return data;
	}

	~BufferReader() {
	  Buffer->removeReader(this);
	}

        friend class BufferWriter ;
  } ;

  class Buffer {
  private:
        BufferWriter *writer ;
	std::set<BufferPointer*> readers;
	void **buffer;			/* This should be a std::vector? */

  public:
	Buffer(int size) {
          buffer = new (void*)[size] ;
	}

        BufferWriter* newWriter () {
          writer = new BufferWriter (this, 0) ;
          return writer ;
        }

	BufferReader* newReader() {
	  reader = new BufferReader (this, writer->write_p) ;
	  readers->add(reader);
	  return reader;
	}
	
	void removeReader(BufferReader* reader) {
	  readers->remove(reader);
	}

        friend class BufferWriter ;
        friend class BufferReader ;
  } ;
}

main() {
  buffer = new Buffer(max)
  wp = buffer->newWriter ();
  pthread_create(&writer_thread,--,writer_function,wp);

  for (i=0,i<n;i++) {
    rp = buffer->newReader();
    pthread_create(&reader_thread,--,reader_function[i],rp);
  }
}

writer_function() {
 for(1) {
  foo=get_new()
  wp->write(foo);
}

reader[i] {
 for(1) {
  foo=rp->read();
}
