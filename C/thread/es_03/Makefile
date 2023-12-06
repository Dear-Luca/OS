CFLAGS=-ansi -Wall -Wpedantic -D_REENTRANT -D_THREAD_SAFE -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread

all: infiniti_thread_con_join.exe

infiniti_thread_con_join.exe: infiniti_thread_con_join.o
	gcc -o infiniti_thread_con_join.exe infiniti_thread_con_join.o ${LIBRARIES}

infiniti_thread_con_join.o: infiniti_thread_con_join.c 
	gcc -c ${CFLAGS} infiniti_thread_con_join.c 

.PHONY: clean

clean:
	rm infiniti_thread_con_join.o infiniti_thread_con_join.exe