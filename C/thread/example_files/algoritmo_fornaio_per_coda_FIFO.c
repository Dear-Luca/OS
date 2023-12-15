/* file:  algoritmo_fornaio_per_coda_FIFO.c
	  i clienti accedono da soli al bancone del pane
	  nell'ordine con cui hanno ottenuto il biglietto.
*/ 

#ifndef _THREAD_SAFE
	#define _THREAD_SAFE
#endif
#ifndef _POSIX_C_SOURCE
	#define _POSIX_C_SOURCE 200112L
#endif


#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <stdint.h>	/* uint64_t intptr_t */
#include <inttypes.h>	/* uint64_t  PRIiPTR */
#include <sys/time.h>	/* gettimeofday()    struct timeval */
#include <pthread.h> 

#include "printerror.h"
#include "DBGpthread.h"

#define DO_OUTPUT 1

/* 
Al fornaio non piace stare al bancone del suo negozio,
i clienti si servono da soli, ma UNO ALLA VOLTA.

Prima di accedere al bancone del pane 
ciascun cliente prende il biglietto
contenente un numero che stabilisce il suo turno.
	Il biglietto viene preso in mutua esclusione mediante
	una variabile apposita mutex   mutexDistributoreBiglietti.
Il primo biglietto distribuito vale 0,
i successivi vengono incrementati di 1 alla volta.

La variabile globale condivisa bigliettoSulDisplay 
indica quale e' il numero del biglietto del
prossimo cliente che puo' andare al bancone a servirsi.
	Una variabile mutex   mutexTurno
	protegge l'accesso alla variabile globale bigliettoSulDisplay.

La variabile globale bigliettoSulDisplay 
viene settata dal cliente che ha finito di servirsi
quando va via dal bancone,
incrementando di 1 il valore precedente.

Ciascun cliente impiega 1 secondo a servirsi al bancone.
*/


#define NUMCLIENTICONTEMPORANEAMENTE 50 

/* variabili da proteggere */
int64_t bigliettoGlob=0;
int64_t bigliettoSulDisplay=0;
/* variabili per la sincronizzazione */
pthread_mutex_t  mutexDistributoreBiglietti;
pthread_mutex_t  mutexTurno;
pthread_cond_t   condCambioTurno;

void *cliente (void *arg) 
{ 
	char Clabel[128];
	intptr_t indice;
	int64_t biglietto;

	indice=(intptr_t)arg;
	sprintf(Clabel,"C%" PRIiPTR "" , indice);

	while(1) 
	{
	
		/* il cliente aspetta qualche giorno = 0 sec e poi va dal fornaio */
	
		/* cliente prende il biglietto */
		/* prendo la mutua esclusione */
		DBGpthread_mutex_lock(&mutexDistributoreBiglietti,Clabel); 
		biglietto=bigliettoGlob;
		bigliettoGlob++;
		/* rilascio la mutua esclusione sul distributore */
		DBGpthread_mutex_unlock(&mutexDistributoreBiglietti,Clabel); 
	
#ifdef DO_OUTPUT
		printf("il cliente %s ha preso il biglietto %ld \n", Clabel, biglietto ); 
		fflush(stdout);
#endif

	
		/* cliente controlla ripetutamente il biglietto sul Display
		   per vedere se e' il suo turno oppure no.
		*/
		
		/* prendo la mutua esclusione */
		DBGpthread_mutex_lock(&mutexTurno,Clabel); 
		while( bigliettoSulDisplay != biglietto ) 
		{
			DBGpthread_cond_wait( &condCambioTurno, &mutexTurno,Clabel); 
		}

		/* il cliente si serve al bancone */
#ifdef DO_OUTPUT
		printf("il cliente %s si serve al bancone: biglietto %ld  bigliettoSulDisplay %ld \n", Clabel, biglietto, bigliettoSulDisplay ); 
		fflush(stdout);
#endif
		/* il cliente impiega 1 secondo a servirsi */
		DBGnanosleep( 999999999, Clabel );
	
		/* il cliente ha finito, definisce chi e' prossimo di turno */
		bigliettoSulDisplay++;
		/* il cliente risveglia i clienti bloccati in attesa del cambio turno */
		DBGpthread_cond_broadcast( &condCambioTurno, Clabel ); 

		/* e il cliente se ne va */
#ifdef DO_OUTPUT
		printf("il cliente %s se ne va \n", Clabel ); 
		fflush(stdout);
#endif
	
		/* rilascio mutua esclusione */
		DBGpthread_mutex_unlock(&mutexTurno,Clabel); 

	}

	pthread_exit(NULL); 
} 

int main (int argc, char* argv[] ) 
{ 
	pthread_t   th; 
	int  rc;
	intptr_t i;

	rc = pthread_mutex_init( &mutexDistributoreBiglietti, NULL); 
	if( rc ) PrintERROR_andExit(rc,"pthread_mutex_init failed");
	rc = pthread_mutex_init( &mutexTurno, NULL); 
	if( rc ) PrintERROR_andExit(rc,"pthread_mutex_init failed");
	rc = pthread_cond_init( &condCambioTurno, NULL); 
	if( rc ) PrintERROR_andExit(rc,"pthread_cond_init failed");

	bigliettoGlob=0;
	bigliettoSulDisplay=0;
	
	/* lancio i clienti */
	for(i=0;i<NUMCLIENTICONTEMPORANEAMENTE;i++) {
		rc=pthread_create( &th, NULL,cliente,(void*)i); 
		if(rc) PrintERROR_andExit(rc,"pthread_create failed");
	}

	/* aspetto 1 minuto e guardo quanti clienti sono stati serviti */
	sleep(60);
	printf("num clienti serviti: %ld \n", bigliettoSulDisplay);
	fflush(stdout);

	pthread_exit(NULL); 

	return(0); 
} 
  
  
  
