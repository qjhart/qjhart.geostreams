include $(CGAL_MAKEFILE)

.PHONY: all

# INCPATH := /usr/include/g++-3
DEBUG_OPT=-pg

CXXFLAGS = \
           $(CGAL_CXXFLAGS) \
           -Iinclude \
           $(LONG_NAME_PROBLEM_CXXFLAGS) \
           $(DEBUG_OPT)

LIBPATH = \
          $(CGAL_LIBPATH)

LDFLAGS = \
          $(LONG_NAME_PROBLEM_LDFLAGS) \
          $(CGAL_LDFLAGS) -pg

all: SegmentTreeTest

#.depend:
#	echo "# DO NOT DELETE" > $@

#depend: .depend
#	makedepend -f.depend -- $(CCFLAGS) -- SegmentTreeTest.cpp

SegmentTreeTest: FileReader.o SegmentTreeTest.o
	$(CGAL_CXX) $(LIBPATH) -o  $@  $^ $(LDFLAGS)

FileReader.o: util/FileReader.cpp
	$(CGAL_CXX) $(LIBPATH) $(CXXFLAGS) $(CCFLAGS) -c $<

%.o: %.cpp
	$(CGAL_CXX) $(LIBPATH) $(CXXFLAGS) $(CCFLAGS) -c $<

clean:
	rm -f *.o *~ SegmentTreeTest
