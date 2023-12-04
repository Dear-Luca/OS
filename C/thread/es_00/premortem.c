#define _DEFAULT_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct info {
    int index;
    pthread_t id;
} info;


void *create_child(void *arg) {
    info *parent_info = (info *)arg;

    /*dati del nuovo thread*/
    pthread_t tid;
    int rc, join;
    info *child_info = (info *)malloc(sizeof(info));
    if (child_info == NULL) {
        perror("Malloc failed\n");
        pthread_exit(NULL);
    }
    child_info->id = pthread_self();
    child_info->index = parent_info->index + 1;

    printf("index: %d\n", parent_info->index);
    usleep(1000);
    if (parent_info->index < 100) {
        rc = pthread_create(&tid, NULL, create_child, child_info);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        join = pthread_join(parent_info->id, NULL);
        if (join != 0) {
            printf("pthread_join() failed: error=%d\n", join);
            exit(-1);
        }
    }
    free(parent_info);
    pthread_exit(NULL);
}

int main() {
    pthread_t tid;
    int rc;
    info *information;
    /* alloco memoria sullo heap in cui mettere il parametro per il pthread */ 
    information = (info *)malloc(sizeof(info));
     /*controllo*/
    if (information == NULL) {
        perror("Malloc failed\n");
        pthread_exit(NULL);
    }
    information->index = 1;
    information->id = pthread_self();
    usleep(1000);
    rc = pthread_create(&tid, NULL, create_child, information);
    if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
    pthread_exit(NULL);
    return 0;
}

