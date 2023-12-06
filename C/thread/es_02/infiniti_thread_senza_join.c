/* con_trucco.c  */

/* messi nella riga di comando del compilatore
#define _THREAD_SAFE
#define _REENTRANT
#define _POSIX_C_SOURCE 200112L
*/

#include <errno.h>
#include <inttypes.h> /* PRIiPTR */
#include <pthread.h>
#include <stdint.h> /* uintptr_t */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUM_THREADS 5

void *func(void *arg) {
    int *index = (int *)arg;
    printf("Index of this thread: %d\n", *index);
    pthread_exit(NULL);
}

int main() {
    pthread_t tid;
    int rc, t, *p;

    for (t = 0; 1 ; t++) {
        printf("Creating thread\n");
        p = (int *)malloc(sizeof(int));
        *p = t;
        if (p == NULL) {
            perror("Malloc failed\n");
            pthread_exit(NULL);
        }
        rc = pthread_create(&tid, NULL, func, p);
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

    pthread_exit(NULL);
    return 0;
}
