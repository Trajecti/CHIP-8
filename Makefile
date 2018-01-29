CC=gcc
CXX=g++
CXXFLAGS= -g -Wall -MMD
LINKERFLAGS=-lSDL2
SRCS=src/chip8.h src/chip8.cpp src/instructions.cpp src/tilemap.cpp src/tilemap.h
OBJS=$(subst .cc,.o,$(SRCS))
DEPENDS = ${OBJECTS:.o=.d}
EXEC = chip8

.PHONY : clean					

${EXEC} : ${OBJS}
	${CXX} ${CXXFLAGS} ${LINKERFLAGS} $^ -o $@

${OBJS} : ${MAKEFILE_NAME}

-include ${DEPENDS}

clean :
	rm -f ${DEPENDS} ${EXEC}
