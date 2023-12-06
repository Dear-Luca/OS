CFLAGS=-ansi -Wpedantic -Wall -D_REENTRANT -D_THREAD_SAFE -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread
DBGPTHREADFUNCTION_SOURCE_DIR=./

all: 	 es2_banche.exe

es2_banche.exe:  es2_banche.o 
	gcc -o es2_banche.exe es2_banche.o ${LIBRARIES}

es2_banche.o: es2_banche.c ${DBGPTHREADFUNCTION_SOURCE_DIR}printerror.h
	gcc -c ${CFLAGS} -I${DBGPTHREADFUNCTION_SOURCE_DIR} es2_banche.c


.PHONY:	clean

clean:
	-rm -f es2_banche.o  es2_banche.exe


