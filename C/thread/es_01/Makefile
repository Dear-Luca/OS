CFLAGS=-ansi -Wpedantic -Wall -D_THREAD_SAFE -D_REENTRANT -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread

all: strutture.exe

strutture.exe: strutture.o
	gcc -o strutture.exe strutture.o ${LIBRARIES}

strutture.o: strutture.c
	gcc -c ${CFLAGS} strutture.c

.PHONY: clean

clean:
	rm strutture.exe strutture.o


