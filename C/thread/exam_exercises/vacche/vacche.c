/* file:  vacche.c */

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

#define NUMMANGIATOIE 3
#define NUMVACCHE 6
#define SECGIRO 10
#define MINSECMANGIARE 5
#define MAXSECMANGIARE 7
#define SECINTERVALLOTRAINCENDI 15
#define SECDURATAINCENDIO 5

/* dati da proteggere */
int pagliabrucia = 0;
int mangiatoie[NUMMANGIATOIE];

/* variabili per la sincronizzazione */
pthread_mutex_t mutex;
pthread_cond_t cond;

void attendi(int min, int max) {
    int secrandom = 0;
    if (min > max)
        return;
    else if (min == max)
        secrandom = min;
    else
        secrandom = min + (random() % (max - min + 1));
    do {
        /* printf("attendi %i\n", secrandom);fflush(stdout); */
        secrandom = sleep(secrandom);
        if (secrandom > 0) {
            printf("sleep interrupted - continue\n");
            fflush(stdout);
        }
    } while (secrandom > 0);
    return;
}

int controllaMangiatoia() {
    int i;
    for (i = 0; i < NUMMANGIATOIE; i++) {
        if (mangiatoie[i] == 1) {
            return i;
        }
    }
    return -1;
}

void *Vacca(void *arg) {
    char Plabel[128];
    intptr_t indice;

    indice = (intptr_t)arg;
    sprintf(Plabel, "Vacca%" PRIiPTR "", indice);

    /*  da completare  */
    while (1) {
        int indicemangiatoia, i, secmangiare, esci = 0;

        DBGpthread_mutex_lock(&mutex, "lock failed");
        /* la vacca attende di poter mangiare */
        while ((indicemangiatoia = controllaMangiatoia()) < 0) {
            printf("vacca %s attende posto libero\n", Plabel);
            DBGpthread_cond_wait(&cond, &mutex, Plabel);
        }

        mangiatoie[indicemangiatoia] = 0;
        DBGpthread_mutex_unlock(&mutex, "unlock failed");
        /* la vacca mangia */
        printf("vacca %s mangia in mangiatoia %i\n", Plabel, indicemangiatoia);
        fflush(stdout);
        /* quale e' il tempo in secondi durante il quale la vacca mangia? */
        secmangiare = MINSECMANGIARE + (random() % (MAXSECMANGIARE - MINSECMANGIARE + 1));

        /* ogni secondo la vacca guarda se la paglia brucia */
        DBGpthread_mutex_lock(&mutex, "lock failed\n");
        while (secmangiare > 0) {
            sleep(1);
            if (pagliabrucia) {
                printf("La paglia sta bruciando aiutoooo! viaa\n");
                mangiatoie[indicemangiatoia] = 1;
                DBGpthread_mutex_unlock(&mutex, "Unlock failed\n");
                break;
            }
            secmangiare--;
        }
        /* la vacca libera la mangiatoia */
        mangiatoie[indicemangiatoia] = 1;
        DBGpthread_cond_signal(&cond, "signal failed\n");
        DBGpthread_mutex_unlock(&mutex, "Unlock failed\n");
        /* la vacca fa un giro di 10 secondi */
        printf("vacca %s inizia giro\n", Plabel);
        attendi(SECGIRO, SECGIRO);
        printf("vacca %s finisce giro\n", Plabel);
        fflush(stdout);
    }
    pthread_exit(NULL);
}

void Bovaro(void) {
    char Plabel[128];

    sprintf(Plabel, "Bovaro");
    while (1) {
        /* attesa 30 sec tra incendi */
        attendi(SECINTERVALLOTRAINCENDI, SECINTERVALLOTRAINCENDI);

        /* bovaro incendia paglia */
        DBGpthread_mutex_lock(&mutex, Plabel);
        pagliabrucia = 1;
        printf("bovaro incendia paglia\n");
        fflush(stdout);
        DBGpthread_mutex_unlock(&mutex, Plabel);

        /* durata incendio 3 sec */
        attendi(3, 3);

        /* bovaro spegne paglia */
        DBGpthread_mutex_lock(&mutex, Plabel);
        pagliabrucia = 0;
        printf("paglia spenta\n");
        fflush(stdout);
        DBGpthread_mutex_unlock(&mutex, Plabel);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t th;
    int rc;
    uintptr_t i = 0;
    int seme;

    seme = time(NULL);
    srandom(seme);

    rc = pthread_mutex_init(&mutex, NULL);
    if (rc) PrintERROR_andExit(rc, "pthread_mutex_init failed");

    pagliabrucia = 0; /* la paglia non brucia */

    /* INIZIALIZZATE LE VOSTRE ALTRE VARIABILI CONDIVISE / fate voi */
    DBGpthread_cond_init(&cond, NULL, "init cond fail");
    /*inizializzo le mangiatoie tutte libere*/
    for (i = 0; i < NUMMANGIATOIE; i++) {
        mangiatoie[i] = 1;
    }

    /* CREAZIONE PTHREAD */
    for (i = 0; i < NUMVACCHE; i++) {
        rc = pthread_create(&th, NULL, Vacca, (void *)i);
        if (rc) PrintERROR_andExit(rc, "pthread_create failed");
    }

    Bovaro();

    return (0);
}
