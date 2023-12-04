
CFLAGS=-ansi -Wpedantic -Wall -D_THREAD_SAFE -D_REENTRANT -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread

all: premortem.exe

premortem.exe: premortem.o
	gcc -o premortem.exe premortem.o ${LIBRARIES}

premortem.o: premortem.c
	gcc -c ${CFLAGS} premortem.c
 
.PHONY: clean

clean: 
	rm -f premortem.exe premortem.o

