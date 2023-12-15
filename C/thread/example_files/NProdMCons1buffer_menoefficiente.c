/* file:  NProdMCons1buffer_menoefficente.c 
   NProduttori e NConsumatori che si scambiano prodotti
   mediante un unico Buffer. E' la versione meno efficente
   MA COMUNQUE BUONA
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
#include <stdint.h>
#include <pthread.h> 
#include "printerror.h"
#include "DBGpthread.h"

#define NUMPROD 5
#define NUMCONS 3 
#define NUMBUFFER 1	/* NON MODIFICARE MAI */

/* dati da proteggere */
uint64_t valGlobale=0;

/* variabili per la sincronizzazione */
pthread_mutex_t  mutex; 
pthread_cond_t   condProd, condCons; 
int numBufferPieni=0; /* 0 o 1 */

void *Produttore (void *arg) 
{ 
	char Plabel[128];
	char Plabelsignal[128];
	int indice;
	uint64_t valProdotto=0;

	indice=*((int*)arg);
	free(arg);
	sprintf(Plabel,"P%d",indice);
	sprintf(Plabelsignal,"P%d->C",indice);

	while(1) {
		/* QUI IL PRODUTTORE PRODUCE IL DATO, un intero.
		   supponiamo che il produttore impieghi circa
		   un secondo a produrre il dato
		DBGsleep(1,Plabel);
		printf("Prod %s PRODUCE dato %lu\n", Plabel, valProdotto+1 ); 
		fflush(stdout);
		*/
		valProdotto++;
		
		DBGpthread_mutex_lock(&mutex,Plabel); 

		while ( numBufferPieni >= NUMBUFFER )
			DBGpthread_cond_wait(&condProd,&mutex,Plabel);

		/* riempie il buffer col dato prodotto prima
		   in realta' per semplicita' incremento la var 
		*/
		valGlobale=valProdotto;

		printf("Prod %s inserisce %lu \n", Plabel, valGlobale ); 
		fflush(stdout);

		numBufferPieni++;

		/* risveglio un Consumatore per svuotare un buffer */
		DBGpthread_cond_signal(&condCons,Plabelsignal); 

		/* rilascio mutua esclusione */
		DBGpthread_mutex_unlock(&mutex,Plabel); 
	}
	pthread_exit(NULL); 
} 

void *Consumatore (void *arg) 
{ 
	uint64_t val;
	char Clabel[128];
	char Clabelsignal[128];
	int indice;

	indice=*((int*)arg);
	free(arg);
	sprintf(Clabel,"C%d",indice);
	sprintf(Clabelsignal,"C%d->P",indice);

	
	while(1) {
		DBGpthread_mutex_lock(&mutex,Clabel); 

		while( numBufferPieni <= 0 ) 
			DBGpthread_cond_wait(&condCons,&mutex,Clabel);

		/* prendo cio' che c'e' nel buffer */
		val=valGlobale;

		printf("Cons %s legge %lu \n", Clabel, val ); 
		fflush(stdout);

		numBufferPieni--;

		DBGpthread_cond_signal(&condProd,Clabelsignal); 

		/* rilascio mutua esclusione */
		DBGpthread_mutex_unlock(&mutex,Clabel); 

		/* fuori da mutua esclusione, uso il dato 
		   supponiamo che ci voglia 1 secondo
		printf("Cons %s USA dato %lu \n", Clabel, val ); 
		fflush(stdout);
		DBGsleep(1,Clabel);
		*/
	}
	pthread_exit(NULL); 
} 

int main (int argc, char* argv[] ) 
{ 
	pthread_t    th; 
	int  rc, i, *intptr;

	rc = pthread_cond_init(&condProd, NULL);
	if( rc ) PrintERROR_andExit(rc,"pthread_cond_init failed");
	rc = pthread_cond_init(&condCons, NULL);
	if( rc ) PrintERROR_andExit(rc,"pthread_cond_init failed");
	rc = pthread_mutex_init(&mutex, NULL); 
	if( rc ) PrintERROR_andExit(rc,"pthread_mutex_init failed");

	/* all'inizio i Cons devono aspettare il primo Prod */
	numBufferPieni=0; /* 0 o 1 */

	for(i=0;i<NUMCONS;i++) {
		intptr=malloc(sizeof(int));
		if( !intptr ) { printf("malloc failed\n");exit(1); }
		*intptr=i;
		rc=pthread_create( &th,NULL,Consumatore,(void*)intptr); 
		if(rc) PrintERROR_andExit(rc,"pthread_create failed");
	}

	for(i=0;i<NUMPROD;i++) {
		intptr=malloc(sizeof(int));
		if( !intptr ) { printf("malloc failed\n");exit(1); }
		*intptr=i;
		rc=pthread_create( &th,NULL,Produttore,(void*)intptr); 
		if(rc) PrintERROR_andExit(rc,"pthread_create failed");
	}

	pthread_exit(NULL); 

	return(0); 
} 
  
  
  
