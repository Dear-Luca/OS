#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "DBGpthread.h"
#include "printerror.h"

#define NUM 10
#define FACHIRI 2

pthread_mutex_t mutex[NUM];

void *fachiro(void *arg) {
    int i;
    int *p = (int *)arg;
    while (1) {
        for (i = 0; i < NUM; i++) {
            DBGpthread_mutex_lock(&mutex[i], "pthread_mutex_lock_failed\n");
        }

        sleep(1);
        printf("Ho accesso alle spade adesso mi ammazzo... aaaaaa  %d\n", *p);

        for (i = 0; i < NUM; i++) {
            DBGpthread_mutex_unlock(&mutex[i], "pthread_mutex_unlock_failed\n");
        }
    }
    free(p);
    pthread_exit(NULL);
}

int main() {
    pthread_t tid;
    int t, rc, *p;
    for (t = 0; t < NUM; t++) {
        DBGpthread_mutex_init(&mutex[t], NULL, "pthread_mutex_init failed\n");
    }

    for (t = 0; t < FACHIRI; t++) {
        printf("Creating thread %d \n", t);
        p = (int *)malloc(sizeof(int));
        *p = t;
        rc = pthread_create(&tid, NULL, fachiro, p);
        if (rc != 0) PrintERROR_andExit(rc, "pthread_create failed\n");
    }

    pthread_exit(NULL);
    return 0;
}