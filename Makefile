# gtrieScanner: quick discovery of network motifs
#
# Pedro Ribeiro - CRACS & INESC-TEC, DCC/FCUP
# ------------------------------------------------
# Makefile
# ------------------------------------------------

# Executable name
EXEC_NAME=motifModularity

# Compiler to use
CC=g++

# Default Options to use
# CFLAGS= -Wall -Wno-write-strings -O0 -g
CFLAGS= -Wno-write-strings -O3
#CFLAGSFINAL= -O3
CLIBS=

#NAUTYSRC= nauty.c nautil.c naugraph.c

# Source files
SRC =                   \
	include/gtries/GraphMatrix.cpp	\
	include/gtries/GraphUtils.cpp \
	include/gtries/Error.cpp \
	src/CLI.cpp	\
	src/FailObject.cpp \
	src/ArrayPartition.cpp \
	src/main.cpp 

OBJ =  ${SRC:.cpp=.o}

#------------------------------------------------------------

all: ${EXEC_NAME}

${EXEC_NAME}: ${OBJ}
	${CC} ${CFLAGS} ${CLIBS} -o ${EXEC_NAME} ${OBJ}

%.o: %.cpp
	${CC} ${CFLAGS} -c -o $@ $+

clean:
	rm ${EXEC_NAME} *.o */*.o *~ *# -rf
