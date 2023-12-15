/* file:  sincroCircolareConTurno.c
   Realizza sincronizzazione circolare tra tre pthread.
   Implementata con una sola condition variable ed un turno.
*/ 


#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 

#include <stdint.h>    /* definizioni intptr_t */
#include <inttypes.h>  /* definizioni PRIiPTR */

#include <pthread.h> 
#include "printerror.h"
#include "DBGpthread.h"

#define NUMTHREADS 3

/* dati da proteggere */
uint64_t valGlobale=0;

/* variabili per la sincronizzazione */
pthread_mutex_t  mutex; 
pthread_cond_t   cond;
intptr_t turno=0;

void *Thread (void *arg) 
{ 
	uintptr_t mioindice;
	char Label[512];

	mioindice=(intptr_t)arg;
	snprintf( Label, 512, "T%" PRIiPTR "", mioindice );

	while(1) {
		DBGpthread_mutex_lock(&mutex,Label); 

		while( turno != mioindice  ) {
			DBGpthread_cond_wait(&cond,&mutex, Label);
			if( turno != mioindice ) 
				DBGpthread_cond_signal(&cond,Label);
		}

		/* SEZIONE CRITICA: incrementa var globale */
		printf(" Sono %s in sezione critica\n", Label);
		fflush(stdout);

		valGlobale=valGlobale+5;

		/* rimango un secondo in sezione critica */
		sleep(1);

		turno=(turno+1)%NUMTHREADS;

		/* sveglio un altro thread in attesa, se c'e' */
		DBGpthread_cond_signal(&cond,Label); 

		/* rilascio mutua esclusione */
		DBGpthread_mutex_unlock(&mutex,Label); 
	}
	pthread_exit(NULL); 
} 

int main () 
{ 
	pthread_t    th[NUMTHREADS]; 
	int  rc;
	intptr_t i;
	void *ptr; 

	DBGpthread_cond_init(&cond, NULL, "");
	DBGpthread_mutex_init(&mutex, NULL, ""); 

	/* all'inizio 0 deve eseguire la sua sezione critica */
	turno=0;

	for( i=0; i<NUMTHREADS; i++ ) {
		rc = pthread_create(&(th[i]), NULL, Thread, (void*)i ); 
		if (rc) PrintERROR_andExit(rc,"pthread_create failed");
	}

	for(i=0;i<NUMTHREADS;i++) {
		rc = pthread_join(th[i], &ptr ); 
		if (rc) PrintERROR_andExit(rc,"pthread_join failed");
	}

	DBGpthread_mutex_destroy(&mutex, ""); 
	DBGpthread_cond_destroy(&cond, ""); 

	return(0); 
} 
  
  
