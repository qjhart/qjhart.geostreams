CXX = g++
CXXFLAGS = -DNDEBUG -Wall

SIT:=/home/quinn/src/SpatialIndexTools
SI:=$(SIT)/spatialindex

all: MemRTreeQuery

MemRTreeQuery: MemRTreeQuery.cc $(SI)/lib/libspatialindex.a $(SIT)/tools/lib/libtools.a
	g++ $< $(CXXFLAGS) -I$(SI)/lib -I$(SIT)/tools/lib -L$(SI)/lib -L$(SIT)/tools/lib -lspatialindex -ltools -lpthread -o $@

clean:
	rm -rf *~ *.o gmon.out *.stackdump
