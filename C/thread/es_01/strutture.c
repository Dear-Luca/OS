#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DIM 4
struct Struttura {
    int N;
    char Str[100];
    int index;
};

void *func(void *arg) {
    struct Struttura *S = (struct Struttura *)arg;
    printf("Inizio del thread N %d Indice %d \n", S->N, S->index);
    sleep(1);

    if (S->N > 1) {
        pthread_t vector_tid[DIM];
        int t, rc, join;
        struct Struttura *S1;

        for (t = 0; t < (S->N - 1); t++) {
            S1 = (struct Struttura *)malloc(sizeof(struct Struttura));
            if (S1 == NULL) {
                perror("Malloc failed\n");
                pthread_exit(NULL);
            }
            S1->N = S->N - 1;
            S1->index = t;
            strcpy(S1->Str, "thread");
            rc = pthread_create(&vector_tid[t], NULL, func, S1);
            if (rc) {
                printf("Error, return code is %d \n", rc);
                exit(-1);
            }
        }

        for (t = 0; t < (S->N - 1); t++) {
            join = pthread_join(vector_tid[t], (void **)&S1);
            if (join) {
                printf("Error, return code is %d \n", rc);
                exit(-1);
            }
            printf("received %s \n", S1->Str);
            free(S1);
        }
    }

    sprintf(S->Str, " %d %d", S->N, S->index);
    pthread_exit(S);
}

int main() {
    pthread_t vector_tid[DIM];
    struct Struttura *S;
    int t, rc, join;

    for (t = 0; t < DIM; t++) {
        S = (struct Struttura *)malloc(sizeof(struct Struttura));
        if (S == NULL) {
            perror("Malloc failed\n");
            pthread_exit(NULL);
        }
        S->N = DIM - 1;
        S->index = t;
        strcpy(S->Str, "thread");
        printf("Creating thead %d\n", t);
        rc = pthread_create(&vector_tid[t], NULL, func, S);
        if (rc) {
            printf("Error, return code is %d \n", rc);
            exit(-1);
        }
    }

    for (t = 0; t < DIM; t++) {
        join = pthread_join(vector_tid[t], (void**)&S);
        if (join) {
            printf("Error, return code is %d \n", rc);
            exit(-1);
        }
        printf("received %s \n", S->Str);
        free(S);
    }

    pthread_exit(NULL);
    return 0;
}
