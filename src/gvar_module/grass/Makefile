# Builds grasswriter
# Carlos Rueda 
# $Id: Makefile,v 1.1 2007/10/11 07:50:11 crueda Exp $
# See README.txt
#
GRASS=/usr/local/grass-6.2.2
GRASS_INCLUDE=$(GRASS)/include
GRASS_LIB=$(GRASS)/lib

GVAR_INCLUDE=/usr/local/include/libgvar
GVAR_MODULE_INCLUDE=..

OBJECTS=make_mapset.o GrassWriterUsingLib.o

# These need to be compiled first, or course :)
GVARSTREAM_OBJECTS=../GvarStream.o ../ShmRowFifo.o

CC=gcc
CCFLAGS=-g -Wall -fPIC -I/usr/local/include -L/usr/local/lib \
	-I$(GVAR_INCLUDEDIR) -L$(GVAR_LIBDIR) \
	-I$(GRASS_INCLUDE) -L$(GRASS_LIB) \
	-I$(GVAR_INCLUDE) \
	-I$(GVAR_MODULE_INCLUDE)


grasswriter: grasswriter.cpp $(OBJECTS) $(GVARSTREAM_OBJECTS)
	$(CC) $(CCFLAGS) -o $@ $< $(OBJECTS) $(GVARSTREAM_OBJECTS) -lgrass_gis  -lpthread -levent  -lgvar -lz -lm 


%.o: %.cpp
	$(CC) $(CCFLAGS) -I . -c -o $@ $<
	
%.o: %.c
	$(CC) $(CCFLAGS) -I . -c -o $@ $<
	
clean:
	rm -f *.o
