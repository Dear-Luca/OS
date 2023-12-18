/* file: BASE per  ponte_pericolante_semplice.c */

#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "DBGpthread.h"
#include "printerror.h"

#define NUMAUTOORARIO 4
#define NUMAUTOANTIORARIO 4

#define INDICESENSOORARIO 0
#define INDICESENSOANTIORARIO 1

/* variabili condivise da proteggere */
int bigliettoDistributore[2];
int biglietto[2];

/* aggiungete le variabili che credete vi servano */
/* INIZIO PRIMA PARTE DA COMPLETARE */
int ponteOccupato;
int countCoda[2];
/* FINE PRIMA PARTE DA COMPLETARE */

/* variabili per la distribuzione del biglietto */
pthread_mutex_t mutexDistributori;

/* aggiungete le vostre variabili per la sincronizzazione */
pthread_mutex_t mutexPonte;
pthread_cond_t condPonteLibero;

void *Auto(void *arg) {
    char Plabel[128];
    intptr_t indice;
    int myBiglietto, indiceSenso;
    char senso; /* O orario  A antiorario */

    /* le prime NUMAUTOORARIO auto viaggiano in senso ORARIO
       le successive viaggiano in senso ANTIORARIO */

    indice = (intptr_t)arg;
    if (indice < NUMAUTOORARIO) {
        senso = 'O';
        indiceSenso = INDICESENSOORARIO;
    } else {
        senso = 'A';
        indiceSenso = INDICESENSOANTIORARIO;
    }

    /* ATTENZIONE ATTENZIONE, come distinguere auto che
       viaggiano in senso Orario o Antioriario.
       Se la variabile locale senso vale 'O' allora
       l'auto viaggia in senso ORARIO, altrimenti
       la variabile locale senso vale 'A' e quindi
       l'auto viaggia in senso ANTIORARIO
    */

    sprintf(Plabel, "%cA%" PRIiPTR "", senso, indice);

    while (1) {
        DBGpthread_mutex_lock(&mutexDistributori, Plabel);
        myBiglietto = bigliettoDistributore[indiceSenso];
        bigliettoDistributore[indiceSenso]++;
        DBGpthread_mutex_unlock(&mutexDistributori, Plabel);

        printf("auto %s ha preso biglietto %i \n", Plabel, myBiglietto);
        fflush(stdout);

        /* auto si mette in attesa del suo turno sul ponte
           e di poter attraversare il ponte rispettando
           la regola di precedenza
         */
        /* INIZIO SECONDA PARTE DA COMPLETARE */
        DBGpthread_mutex_lock(&mutexPonte, Plabel);
        DBGpthread_mutex_lock(&mutexDistributori, Plabel);
        countCoda[indiceSenso]++;

        while ((ponteOccupato) || (myBiglietto > biglietto[indiceSenso]) || (countCoda[indiceSenso] < countCoda[!indiceSenso]) ||
               (countCoda[indiceSenso] == countCoda[!indiceSenso] && indiceSenso == INDICESENSOANTIORARIO)) {
            DBGpthread_mutex_unlock(&mutexDistributori, Plabel);
            DBGpthread_cond_wait(&condPonteLibero, &mutexPonte, Plabel);
        }

        ponteOccupato = 1;
        countCoda[indiceSenso]--;
        biglietto[indiceSenso]++;

        /* FINE SECONDA PARTE DA COMPLETARE */
        printf("auto %s inizia attraversamento ponte con biglietto %i\n", Plabel, myBiglietto);
        fflush(stdout);

        /* Auto comincia ad attraversare il ponte */
        /* INIZIO TERZA PARTE DA COMPLETARE */
        DBGpthread_mutex_unlock(&mutexPonte, Plabel);
        DBGpthread_mutex_unlock(&mutexDistributori, Plabel);  
        /* FINE TERZA PARTE DA COMPLETARE */

        sleep(1); /* auto attraversa il ponte */

        /* auto finisce attraversamento ponte
           ed avvisa di avere finito l'attraversamento */
        /* INIZIO QUARTA PARTE DA COMPLETARE */
        DBGpthread_mutex_lock(&mutexPonte, Plabel);
        ponteOccupato = 0;

        DBGpthread_cond_broadcast(&condPonteLibero, Plabel);

        DBGpthread_mutex_unlock(&mutexPonte, Plabel);

        printf("auto %s finisce attraversamento ponte con biglietto %i\n", Plabel, myBiglietto);
        /* FINE QUARTA PARTE DA COMPLETARE */

        /* auto fa un giro intorno */
        printf("auto %s gira intorno \n", Plabel);
        fflush(stdout);
        sleep(5); /* faccio il giro attorno a Villa Inferno */
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t th;
    int rc;
    intptr_t i;

    rc = pthread_cond_init(&condPonteLibero, NULL);
    if (rc) PrintERROR_andExit(rc, "pthread_cond_init failed");

    rc = pthread_mutex_init(&mutexPonte, NULL);
    if (rc) PrintERROR_andExit(rc, "pthread_mutex_init failed");

    rc = pthread_mutex_init(&mutexDistributori, NULL);
    if (rc) PrintERROR_andExit(rc, "pthread_mutex_init failed");

    /* configuro le variabili */
    for (i = 0; i < 2; i++) {
        bigliettoDistributore[i] = 0;
        biglietto[i] = 0;
    }

    /* completare con l'inizializzazione delle vostre variabili
       se ne avete aggiunta qualcuna */
    /* INIZIO QUINTA PARTE DA COMPLETARE */
    /* 0--> ponte non occupato, 1--> ponte occupato*/
    ponteOccupato = 0;
    for (i = 0; i < 2; i++) {
        countCoda[i] = 0;
    }
    /* FINE QUINTA PARTE DA COMPLETARE */

    /* creo le auto passando loro un indice;
       sulla base dell'indice che gli viene passato
       il thread stabilisce se l'auto che rappresenta viaggia
       in senso orario o antiorario
       e poi stabilisce il proprio indice locale.
    */

    for (i = 0; i < NUMAUTOORARIO + NUMAUTOANTIORARIO; i++) {
        rc = pthread_create(&th, NULL, Auto, (void *)i);
        if (rc) PrintERROR_andExit(rc, "pthread_create failed");
    }

    pthread_exit(NULL);
    return (0);
}
