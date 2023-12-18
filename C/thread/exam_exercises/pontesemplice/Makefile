CFLAGS=-ansi -Wpedantic -Wall -D_REENTRANT -D_THREAD_SAFE -D_POSIX_C_SOURCE=200112L -D_DEFAULT_SOURCE
LFLAGS=
LIBRARIES=-lpthread

all: 	pontepericolante_semplice.exe

pontepericolante_semplice.exe: pontepericolante_semplice.o DBGpthread.o
	gcc ${LFLAGS} -o pontepericolante_semplice.exe pontepericolante_semplice.o DBGpthread.o ${LIBRARIES}

pontepericolante_semplice.o: pontepericolante_semplice.c DBGpthread.h printerror.h
	gcc ${CFLAGS} -c pontepericolante_semplice.c

DBGpthread.o: DBGpthread.c printerror.h
	gcc ${CFLAGS} -c DBGpthread.c

.PHONY:	clean

clean:
	-rm -f pontepericolante_semplice.o DBGpthread.o pontepericolante_semplice.exe


