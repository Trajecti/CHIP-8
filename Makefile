CC=gcc
CXX=g++
CXXFLAGS= -g -Wall -MMD
SRCS=src/chip8.h src/chip8.cc
OBJS=$(subst .cc,.o,$(SRCS))
DEPENDS = ${OBJECTS:.o=.d}
EXEC = chip8

.PHONY : clean					

${EXEC} : ${OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

${OBJS} : ${MAKEFILE_NAME}

-include ${DEPENDS}

clean :
	rm -f ${DEPENDS} ${OBJS} ${EXEC}
