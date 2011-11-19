CC := g++ -pg
CCFLAGS := -Wall -g

.PHONY: all clean SkipList_Test

all: dctWindowQueryTest dctPointQueryTest

#.depend:
#	echo "# DO NOT DELETE" > $@

#depend: .depend
#	makedepend -f.depend -- -I$(INCPATH) -I$(INCPATH1) $(CCFLAGS) -- SkipList_Test.cpp RandomHeight.cpp

SkipList_Test: SkipList_Test.o RandomHeight.o
	$(CC) -o $@  $^

dctWindowQueryTest:  FileReader.o RandomHeight.o dctWindowQueryTest.o
	$(CC) -o $@  $^

dctPointQueryTest:  FileReader.o RandomHeight.o dctPointQueryTest.o
	$(CC) -o $@  $^


FileReader.o: util/FileReader.cpp
	$(CC) $(CCFLAGS) -c $<

dctWindowQueryTest.o: CascadeNode.h DynamicCascadeTree.h
dctPointQueryTest.o: CascadeNode.h DynamicCascadeTree.h

%.o: %.cpp
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o *~ dctWindowQueryTest dctPointQueryTest