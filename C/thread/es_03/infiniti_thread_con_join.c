#include <errno.h>
#include <inttypes.h> /* PRIiPTR */
#include <pthread.h>
#include <stdint.h> /* uintptr_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM 1000

void *func(void *arg) {
    int *index = (int*)arg;
    printf("This thread has index: %d\n", *index);
    free(arg);
    pthread_exit(NULL);
}

int main() {
    pthread_t vector_tid[NUM];
    int rc, t, *p;

    while (1) {
        for (t = 0; t < NUM; t++) {
            p = (int *)malloc(sizeof(int));
            if (p == NULL) {
                perror("Malloc failed\n");
                pthread_exit(NULL);
            }
            *p = t;
            printf("Creating thread...\n");
            rc = pthread_create(&vector_tid[t], NULL, func, p);
            if (rc) {
                char msg[100];
                int ret;
                ret = strerror_r(rc, msg, 100);
                if (ret != 0) { /* anche la funzione strerror_r potrebbe causare errore */
                    printf("strerror failed: errore %i \n", ret);
                    fflush(stdout);
                }
                /* stampo il messaggio di errore provocato dalla join */
                printf("pthread_create failed: %i %s\n", rc, msg);
            }
        }
        for(t=0; t<NUM; t++){
            rc=pthread_join(vector_tid[t], NULL);
            if (rc) {
                char msg[100];
                int ret;
                ret = strerror_r(rc, msg, 100);
                if (ret != 0) { /* anche la funzione strerror_r potrebbe causare errore */
                    printf("strerror failed: errore %i \n", ret);
                    fflush(stdout);
                }
                /* stampo il messaggio di errore provocato dalla join */
                printf("pthread_join failed: %i %s\n", rc, msg);
            }
            printf("received thread\n");
        }
    }
    pthread_exit(NULL);
    return 0;
}

