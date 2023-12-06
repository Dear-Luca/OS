CFLAGS=-ansi -Wpedantic -Wall -D_REENTRANT -D_THREAD_SAFE -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread

all: 	 es1_banche.exe

es1_banche.exe:  es1_banche.o 
	gcc -o es1_banche.exe es1_banche.o ${LIBRARIES}

es1_banche.o: es1_banche.c printerror.h
	gcc -c ${CFLAGS} -I. es1_banche.c


.PHONY:	clean

clean:
	-rm -f es1_banche.o  es1_banche.exe


