# GeoStreams Project
# mngstream - Module for MNG stream management
# $Id: Makefile,v 1.10 2004/07/25 06:11:30 crueda Exp $

cc=g++
cdebug=-g
cflags= -c -Wall

all: pngs2mng pgms2mng


pgms2mng: pgms2mng.o MngStream.o
	gcc MngStream.o pgms2mng.o -lpng -o $@
	
pngs2mng: pngs2mng.o MngStream.o
	gcc MngStream.o pngs2mng.o -lpng -o $@
	
%.o:%.cpp *.h
	$(cc) $(cdebug) $(cflags) $(INCLUDE) $<

tidy:
	rm -f *.o *~
	
clean: tidy
	rm -f pgms2mng pngs2mng
	

