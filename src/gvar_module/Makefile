##
##  Makefile -- Build procedure for fast3lpoad Apache module
##
##  This is a C++ module so things have to be handled a little differently.

#   the used tools
APXS=apxs
APACHECTL=apachectl
# APXS=apxs2
# APACHECTL=/usr/sbin/apache2ctl

# Get all of apxs's internal values.
APXS_CC=`$(APXS) -q CC`   
APXS_TARGET=`$(APXS) -q TARGET`   
APXS_CFLAGS=`$(APXS) -q CFLAGS`   
APXS_SBINDIR=`$(APXS) -q SBINDIR`   
APXS_CFLAGS_SHLIB=`$(APXS) -q CFLAGS_SHLIB`   
APXS_INCLUDEDIR=`$(APXS) -q INCLUDEDIR`
APXS_LD_SHLIB=`$(APXS) -q LD_SHLIB`
APXS_LIBEXECDIR=`$(APXS) -q LIBEXECDIR`
APXS_LDFLAGS_SHLIB=`$(APXS) -q LDFLAGS_SHLIB`
APXS_SYSCONFDIR=`$(APXS) -q SYSCONFDIR`
APXS_LIBS_SHLIB=`$(APXS) -q LIBS_SHLIB`

CC=g++
CCFLAGS=-g -fPIC -I/usr/local/include -L/usr/local/lib -I$(GVAR_INCLUDEDIR) -L$(GVAR_LIBDIR)

GVAROBJECT=\
GvarStream.o

OBJECTS=\
$(GVAROBJECT) ShmRowFifo.o GvarConverter.o

IMAGE_OBJECTS=\
image/point_xy.o image/point_xyt.o image/pointset_xyatt.o image/imageatt.o\
image/image_listofimages.o 

.PHONY: all mod_geostream.so create_grass_files clean install convert_gvar convert_gvar_shm retrieve_elug_params

#   the default target
all: create_grass_files convert_gvar convert_gvar_shm mod_geostream.so

# compile the shared object file. use g++ instead of letting apxs call
# ld so we end up with the right c++ stuff. We do this in two steps,
# compile and link.

# compile
mod_geostream.o: mod_geostream.cpp
	$(CC) -c $(CCFLAGS) -I$(APXS_INCLUDEDIR) -I. $(APXS_CFLAGS) $(APXS_CFLAGS_SHLIB) -Wall -o $@ $<

# link
mod_geostream.so:  mod_geostream.o $(OBJECTS) 
	$(CC) $(CCFLAGS) -shared -o $@ $< $(APXS_LIBS_SHLIB) $(OBJECTS) -lpthread -levent -lgvar -limgstr

create_grass_files: create_grass_files.o GrassWriter.o $(OBJECTS) $(IMAGE_OBJECTS)
	$(MAKE) -C image
	$(CC) $(CCFLAGS) -o $@ $< $(OBJECTS) GrassWriter.o -lpthread -levent -lgvar -limgstr

convert_gvar: convert_gvar.o $(OBJECTS) $(IMAGE_OBJECTS)
	$(MAKE) -C image
	$(CC) $(CCFLAGS) -o $@ $< $(OBJECTS) -lpthread -levent -lgvar -limgstr

convert_gvar_shm: convert_gvar_shm.o $(OBJECTS) $(IMAGE_OBJECTS)
	$(MAKE) -C image
	$(CC) $(CCFLAGS) -o $@ $< $(OBJECTS) -lpthread -levent -lgvar -limgstr

retrieve_elug_params: retrieve_elug_params.o $(OBJECTS)
	$(CC) $(CCFLAGS) -o $@ $< $(OBJECTS) -lpthread -levent -lgvar -limgstr

%.o: %.cpp
	$(CC) $(CCFLAGS) -I . -c -o $@ $<

# install the shared object file into Apache 
install: all
	$(APXS) -i -n 'geostream' mod_geostream.so

# display the apxs variables
check_apxs_vars:
	@echo APXS_CC $(APXS_CC);\
	echo APXS_TARGET $(APXS_TARGET);\
	echo APXS_CFLAGS $(APXS_CFLAGS);\
	echo APXS_SBINDIR $(APXS_SBINDIR);\
	echo APXS_CFLAGS_SHLIB $(APXS_CFLAGS_SHLIB);\
	echo APXS_INCLUDEDIR $(APXS_INCLUDEDIR);\
	echo APXS_LD_SHLIB $(APXS_LD_SHLIB);\
	echo APXS_LIBEXECDIR $(APXS_LIBEXECDIR);\
	echo APXS_LDFLAGS_SHLIB $(APXS_LDFLAGS_SHLIB);\
	echo APXS_SYSCONFDIR $(APXS_SYSCONFDIR);\
	echo APXS_LIBS_SHLIB $(APXS_LIBS_SHLIB)

#   cleanup
clean:
	$(MAKE) -C image clean
	-rm -f *.so *.o *~ create_grass_files convert_gvar convert_gvar_shm

#   install and activate shared object by reloading Apache to
#   force a reload of the shared object file
reload: install restart

#   the general Apache start/restart/stop
#   procedures
start:
	$(APACHECTL) start
restart:
	$(APACHECTL) restart
stop:
	$(APACHECTL) stop
