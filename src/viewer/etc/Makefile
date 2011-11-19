# We depend on ELUG:
CXXFLAGS = -Wall -g -I../../ELUG/src -L../../ELUG/src

all: elug.point elug.line elug.gshhs elug.ll2pl

elug.point: elug.point.cpp myelug.cpp 
	g++ $(CXXFLAGS) elug.point.cpp myelug.cpp   -lELUG -o elug.point

elug.line: elug.line.cpp myelug.cpp  
	g++ $(CXXFLAGS) elug.line.cpp myelug.cpp   -lELUG -o elug.line

elug.gshhs: elug.gshhs.cpp myelug.cpp  
	g++ $(CXXFLAGS) elug.gshhs.cpp myelug.cpp  -lELUG -o elug.gshhs

elug.ll2pl: elug.ll2pl.cpp myelug.cpp  
	g++ $(CXXFLAGS) elug.ll2pl.cpp myelug.cpp   -lELUG -o elug.ll2pl

elug.cimis.ll2pl: elug.cimis.ll2pl.cpp csv/Csv.cc myelug.cpp  
	g++ $(CXXFLAGS) -Icsv elug.cimis.ll2pl.cpp csv/Csv.cc myelug.cpp   -lELUG -o elug.cimis.ll2pl

elug.cimis.ll2pl.raster: elug.cimis.ll2pl.raster.cpp myelug.cpp  
	g++ $(CXXFLAGS) elug.cimis.ll2pl.raster.cpp myelug.cpp   -lELUG -o elug.cimis.ll2pl.raster

clean:
	rm -f *.o elug.line elug.point elug.gshhs a.out


#####  vector data  ######
VECTORS=vectors
.PHONY: v_init rec.dat vectors shorelines lakes meridians_parallels

v_init: elug.gshhs rec.dat
	mkdir -p $(VECTORS)

vectors: shorelines lakes meridians_parallels

shorelines: v_init
	./elug.gshhs GSHHS/gshhs_c.b > $(VECTORS)/shorelines_l.txt  # low resolution
	./elug.gshhs GSHHS/gshhs_l.b > $(VECTORS)/shorelines_n.txt  # normal resolution
	
lakes: v_init
	./elug.gshhs --level 2 --min_area 10000 GSHHS/gshhs_l.b > $(VECTORS)/lakes10000.txt
	./elug.gshhs --level 2 --min_area 5000 GSHHS/gshhs_l.b > $(VECTORS)/lakes5000.txt
	./elug.gshhs --level 2 --min_area 2000 GSHHS/gshhs_l.b > $(VECTORS)/lakes2000.txt

meridians_parallels: v_init
	./meridians_parallels.sh > $(VECTORS)/meridians_parallels.txt
