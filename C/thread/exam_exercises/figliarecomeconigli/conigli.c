/* file:  piattello.c */

#ifndef _THREAD_SAFE
#define _THREAD_SAFE
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

/* la   #define _POSIX_C_SOURCE 200112L   e' dentro printerror.h */
#ifndef _BSD_SOURCE
#define _BSD_SOURCE /* per random e srandom */
#endif

/* messo prima perche' contiene define _POSIX_C_SOURCE */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h> /* per PRIiPTR */
#include <limits.h>   /* for OPEN_MAX */
#include <pthread.h>
#include <signal.h>
#include <stdint.h> /* uint64_t intptr_t */
#include <stdio.h>
#include <stdlib.h> /* random  srandom */
#include <string.h> /* per strerror_r  and  memset */
#include <sys/stat.h>
#include <sys/time.h> /*gettimeofday() struct timeval timeval{} for select()*/
#include <sys/types.h>
#include <time.h>   /* timespec{} for pselect() */
#include <unistd.h> /* exit() etc */

#include "DBGpthread.h"
#include "printerror.h"

#define NUM_THREADS 5

/*variabili comuni*/
pthread_mutex_t mutex;
int NumThreadPresenti;

void *Conigli() {
    pthread_t thread;
    int rc;
    DBGpthread_mutex_lock(&mutex, "mutex lock failed\n");
    printf("ci sono %d threads\n", NumThreadPresenti);
    DBGpthread_mutex_unlock(&mutex, "mutex unlock failed\n");

    while (1) {
        sleep(1);
        DBGpthread_mutex_lock(&mutex, "mutex lock failed\n");
        if (NumThreadPresenti < NUM_THREADS) {
            rc = pthread_create(&thread, NULL, Conigli, NULL);
            if (rc) PrintERROR_andExit(rc, "pthread_create failed");
            printf("Coniglio padre crea figlio\n");
            NumThreadPresenti++;
            DBGpthread_mutex_unlock(&mutex, "mutex unlock failed\n");
        } else {
            printf("Ci sono troppi threads mi suicido\n");
            NumThreadPresenti--;
            DBGpthread_mutex_unlock(&mutex, "mutex unlock failed\n");
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    int rc;
    /*Inizializzazine variabili*/
    DBGpthread_mutex_init(&mutex, NULL, "Mutex init failed\n");
    while (1) {
        sleep(5);
        rc = pthread_create(&thread, NULL, Conigli, NULL);
        DBGpthread_mutex_lock(&mutex, "mutex lock failed\n");
        NumThreadPresenti++;
        if (rc) PrintERROR_andExit(rc, "pthread_create failed");
        printf("main crea thread\n");
        DBGpthread_mutex_unlock(&mutex, "mutex unlock failed\n");
    }

    pthread_exit(NULL);
    return 0;
}
