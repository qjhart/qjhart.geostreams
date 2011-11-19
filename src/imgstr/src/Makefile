# GeoStreams Project
# imgstream -
# Carlos A. Rueda-Velasquez
# $Id: Makefile,v 1.6 2005/07/12 22:24:32 crueda Exp $

cc=g++
cdebug=-g
cflags= -c -Wall

target = libimgstr.a
SRC = ImgStream.cpp MngImgStream.cpp AsciiImgStream.cpp BinImgStream.cpp
OBJ = ${SRC:.cpp=.o}

$(target): $(OBJ)
	ar r $@ $(OBJ)
	
%.o:%.cpp *.h
	$(cc) $(cdebug) $(cflags) $(INCLUDE) $<

test: $(target)
	g++ test.cpp libimgstr.a -o test
	
tidy:
	rm -f *.o *~
	
clean: tidy
	rm -f $(target)
	

