
SRCS = ELUG.cpp ELUG_C.c 
OBJS = ELUG.o ELUG_C.o  
CFLAGS = -Wall -g
CXXFLAGS = -Wall -g

all: libELUG.a

libELUG.a: $(OBJS)
	ar r libELUG.a $(OBJS)

testOne: testOne.cpp libELUG.a
	g++ $(CXXFLAGS) testOne.cpp libELUG.a -o testOne

testCplus: testCplus.cpp libELUG.a
	g++ $(CXXFLAGS)  testCplus.cpp libELUG.a -o testCplus

test4time: test4time.cpp libELUG.a
	g++ $(CXXFLAGS)  test4time.cpp libELUG.a -o test4time

tidy:
	rm -f *.o

clean:
	rm -f *.o *.a testCplus testOne a.out
