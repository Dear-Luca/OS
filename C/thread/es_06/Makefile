CFLAGS=-ansi -Wall -Wpedantic -D_THREAD_SAFE -D_REENTRANT -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread

all: es3_fachiri.exe

es3_fachiri.exe: es3_fachiri.o DBGpthread.o
	gcc -o es3_fachiri.exe es3_fachiri.o DBGpthread.o ${LIBRARIES}

es3_fachiri.o: es3_fachiri.c DBGpthread.h
	gcc -c ${CFLAGS} es3_fachiri.c

DBGpthread.o: DBGpthread.c DBGpthread.h
	gcc -c ${CFLAGS} DBGpthread.c

.PHONY: clean

clean:
	rm es3_fachiri.exe es3_fachiri.o DBGpthread.o 
