
CXX = g++
CXXFLAGS = -O3 -g -Wall # -fPIC -DPTHREADS
# CXXFLAGS = -g -Wall # -fPIC -DPTHREADS

SIT:=/home/quinn/src/SpatialIndexTools
SI:=$(SIT)/spatialindex

all: dctQuery

dctQuery: dctQuery.o $(SI)/lib/libspatialindex.a $(SIT)/tools/lib/libtools.a
	$(CXX) $< $(CXXFLAGS) -I$(SI)/lib -I$(SIT)/tools/lib -L$(SI)/lib -L$(SIT)/tools/lib -lspatialindex -ltools -lpthread  -o $@

debug:
	$(MAKE) CXXFLAGS='-Wall -g'

dctQuery.o: Node.h DCT.h

.cc.o:
	$(CXX) $(CXXFLAGS) -I$(SI)/lib -I$(SIT)/tools/lib -c $<

clean:
	rm -rf dctQuery *~ *.o gmon.out *.stackdump
