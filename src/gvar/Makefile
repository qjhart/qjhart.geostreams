# $Id: Makefile,v 1.11 2005/02/09 23:55:59 crueda Exp $

.PHONY: all depend clean

INCPATH=/usr/local/include
LIBPATH=/usr/local/lib

CC := g++
CCFLAGS := -Wall -g -levent
CCFLAGS2 := -Wall -g -lpng -lz

common_SRC := \
  gvarHeader/gvarHeader.cpp \
  gvarBlock/gvarBlock.cpp \
  utils/unpack/unpack10.cpp \
  utils/cdaTime/cdaTime.cpp \
  lineDoc/lineDoc.cpp \
  block1to10/block1to10.cpp \
  block0/block0.cpp \
  block0/block0Doc.cpp \
  image/image_listofimages.cpp \
  image/point_xy.cpp \
  image/point_xyt.cpp \
  image/pointset_xyatt.cpp \
  image/imageatt.cpp \
  mngstream/MngStream.cpp 

goesd_SRC:= goesd.cpp
gvarnetapi_SRC:= gvnetapi.cpp

common_OBJS := $(patsubst %.cpp,%.o,$(common_SRC))

goesd_OBJS := $(common_OBJS) $(patsubst %.cpp,%.o,$(goesd_SRC))

gvarnetapi_OBJS := $(common_OBJS) $(patsubst %.cpp,%.o,$(gvarnetapi_SRC))

# test:
# 	@echo $(goesd_OBJS)
# 	@echo $(goesd_OBJS)

all: goesd gvnetapi

PHONY: depend

.depend:
	echo "# DO NOT DELETE" > $@

depend: .depend
	makedepend -f.depend -- $(CCFLAGS) -- $(common_SRC)

goesd: $(goesd_OBJS)
	$(CC) $(CCFLAGS) -o $@ $^

gvnetapi: $(gvarnetapi_OBJS)
	$(CC) $(CCFLAGS2) -o  $@  $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CCFLAGS) -I . -c -o $@ $<

clean:
	rm -f $(common_OBJS) gvnetapi goesd
	rm -f `find . -name '*~'`

