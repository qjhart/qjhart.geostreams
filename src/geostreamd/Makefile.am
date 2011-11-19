bin_PROGRAMS=geostreamd

#geostreamd_SOURCES=geostreamd.c gvar.cpp server.c log.c request.c client.c buffer.c ll.c rwlock.c geostreamd.h
# geostreamd_SOURCES=geostreamd.cpp GvarStream.cpp server.c log.c request.c client.c buffer.c ll.c rwlock.c geostreamd.h
geostreamd_SOURCES=geostreamd.cpp GvarStream.cpp Log.cpp HttpRequest.cpp HttpRequestBuffer.cpp Connection.cpp GeostreamServer.cpp
EXTRA_geostreamd_SOURCES=config.cpp

##  uncomment the following if foo requires the math library
##geostreamd_DEPENDANCIES=@CONFIG_OBJ@
if WITH_DOTCONF
AM_CFLAGS=-DUSE_CONFIG_FILE
AM_CXXFLAGS=-DUSE_CONFIG_FILE
LIBADD_DOTCONF=config.o -ldotconf 
geostreamd_DEPENDANCIES=config.o
else
LIBADD_DOTCONF=
endif

geostreamd_LDADD=$(top_builddir)/gvar/libgvar.la $(top_builddir)/lib/liblib.la -luri -levent -lpthread $(LIBADD_DOTCONF)


EXTRA_DIST= #NAME.texinfo foo.spec.in

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

#  build and install the .info pages
#info_TEXINFOS = foo.texinfo
#foo_TEXINFOS = gpl.texinfo

#  install the man pages
man_MANS=geostreamd.1

geostreamd.1: geostreamd.pod
	pod2man -c 'GeoStreams Server' $< > $@


SUBDIRS=gvar lib .

