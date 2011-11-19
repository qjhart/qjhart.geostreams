CXX = g++
CXXFLAGS = -O2

OBJECTS=\
point_xy.o point_xyt.o pointset_xyatt.o imageatt.o\
image_listofimages.o

all: $(OBJECTS)

debug:
	$(MAKE) CXXFLAGS='-g -fPIC'

profile:
	$(MAKE) CXXFLAGS='-pg -g -fPIC'

.cc.o:
	$(CXX) $(CXXFLAGS) -Wall -c $<

clean:
	rm -rf *~ *.o

