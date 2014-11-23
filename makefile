# Makefile for Solar.

# Author: John M. Weiss, Ph.D., Derek Stotz, Charles Parsons
# Written Fall 2014 for CSC433/533 Computer Graphics.

# Usage:  make target1 target2 ...

#-----------------------------------------------------------------------

# GNU C/C++ compiler and linker:
CC = gcc
CXX = g++
LINK = g++

# Turn on optimization (add -g for debugging with gdb):
CXXFLAGS = -O3 -std=c++11 -g

# OpenGL/Mesa libraries for Linux:
GL_LIBS = -lglut -lGLU -lGL -lm

#-----------------------------------------------------------------------

OBJS = solar.o solarGraphics.o structs.o bmpRead.o


solar: solar.o solarGraphics.o structs.o bmpRead.o 

	g++ ${OBJS} ${CXXFLAGS} -o solar ${GL_LIBS}


bmpRead.o: bmpRead.cpp

	g++ ${CXXFLAGS} -c bmpRead.cpp ${GL_LIBS}


solar.o: solar.cpp solarGraphics.h bmpRead.o

	g++ ${CXXFLAGS} -c solar.cpp ${GL_LIBS}


solarGraphics.o: solarGraphics.cpp solarGraphics.h

	g++ ${CXXFLAGS} -c solarGraphics.cpp ${GL_LIBS}


structs.o: structs.cpp structs.h

	g++ ${CXXFLAGS} -c structs.cpp ${GL_LIBS}

clean:

	rm -rf *o solar
