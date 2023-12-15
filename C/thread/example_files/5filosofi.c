/* file:  5filosofi.c 
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
#include <stdint.h>	/* uint64_t */
#include <sys/time.h>	/* gettimeofday()    struct timeval */
#include <pthread.h> 
#include "printerror.h"
#include "DBGpthread.h"

#define NUMFILOSOFI 5

/* stati del filosofo */
#define PENSA  0
#define HAFAME 1
#define MANGIA 2			/* l l l lui  lui maaaaaaaaangiaa */
/* variabili da proteggere */
int statoFilosofo[NUMFILOSOFI];
int forchettaoccupata[NUMFILOSOFI];

/* variabili per la sincronizzazione */
pthread_mutex_t  mutex;    /* permette l'accesso alle variabili di stato */ 
pthread_cond_t   condFilosofo[NUMFILOSOFI]; 

int indiceasinistra(int indice) {
	return( (indice+1)%NUMFILOSOFI );
}
int indiceadestra(int indice) {
	if( indice==0 ) return( NUMFILOSOFI-1 );
	else		return( (indice-1)%NUMFILOSOFI );
}

/* gli indici crescono in senso orario, da 0 a NUMFILOSOFI-1 
	indice	indiceasinistra	  indiceadestra
	0		1		0
	1		2		1
	2		3		2
	3		4		3
	4		0		4
	indice	indiceprimaforc	  indicsecondaforc
	0		0		1    <--inversione contro deadlock
	1		2		1
	2		3		2
	3		4		3
	4		0		4

queste 2 funzioni qui sotto servirebbero se implementassi
la versione anti-deadlock che usa l'inversione della prima forchetta presa

int indiceprimaforchetta( int indicefilosofo ) {
	if( indicefilosofo==0 )	return( 0 );
	else			return( (indicefilosofo+1)%NUMFILOSOFI );
}
int indicesecondaforchetta( int indicefilosofo ) {
	if( indicefilosofo==0 )	return( 1 );
	else			return( indicefilosofo );
}
*/

/* funzione che deve essere eseguita in mutua esclusione */
int puomangiare( int indice )  {
	if ( 
		statoFilosofo[indice]==HAFAME
		&&
		statoFilosofo[ indiceadestra(indice) ] != MANGIA
		&&
		statoFilosofo[ indiceasinistra(indice) ] != MANGIA
	   )
		return( 1 );
	else
		return( 0 );
}

void *filosofo (void *arg) 
{ 
	char Flabel[128];
	char Flabelsignal[128];
	int indice;

	indice=*((int*)arg);
	free(arg);
	sprintf(Flabel,"F%d",indice);
	sprintf(Flabelsignal,"F%d->",indice);

	
	while(1) {
		printf("filosofo %s sta pensando \n", Flabel ); 
		fflush(stdout);
		/* il filosofo pensa per 1/2 di sec piu o meno */
		DBGnanosleep( 500000000+indice*1000000, Flabel );


		/* prendo le forchette */
		/* prendo la mutua esclusione */
		DBGpthread_mutex_lock(&mutex,Flabel); 

		/* mi dichiaro affamato, cosi' i vicini mi risveglieranno
		   quando finiranno di mangiare
		*/
		statoFilosofo[indice]=HAFAME;

		if ( puomangiare( indice ) )  {
			/* le forchette ai mie lati sono libere,
			   quindi prendo entrambe le forchette 
			*/
			statoFilosofo[indice]=MANGIA;
		}
		else {
			/* devo attendere che qualche mio vicino mi risvegli
			   perche' smette di mangiare e vede che io ho fame
			   cosi' mi ordina di mangiare e mi mette gia' in stato MANGIA
			*/
			DBGpthread_cond_wait( &(condFilosofo[indice]), &mutex, Flabel); 

			/* esco, prendo le forchette e mangio */
			/* gia' settato statoFilosofo[indice]=MANGIA;  */
		}
		/* rilascio la mutua esclusione */
		DBGpthread_mutex_unlock(&mutex,Flabel); 

		/*  mangio */
		printf("filosofo %s MANGIA \n", Flabel ); 
		fflush(stdout);
		/* il filosofo pensa per 3/4 di sec */
		DBGnanosleep( 750000000+indice*1000000, Flabel );
		printf("filosofo %s FINISCE DI MANGIARE \n", Flabel ); 
		fflush(stdout);


		/* rilascio le forchette */
		/* prendo la mutua esclusione */
		DBGpthread_mutex_lock(&mutex,Flabel); 

		/* dico che ho smesso di mangiare  */
		statoFilosofo[indice]=PENSA;

		/* cerco di svegliare i vicini se hanno fame e se possono mangiare */
		if ( puomangiare( indiceadestra(indice) ) )  { 
		   statoFilosofo[ indiceadestra(indice) ]=MANGIA;
		   DBGpthread_cond_signal( &(condFilosofo[indiceadestra(indice)]), Flabel); 
		}
		if ( puomangiare( indiceasinistra(indice) ) )  {
		   statoFilosofo[ indiceasinistra(indice) ]=MANGIA;
		   DBGpthread_cond_signal( &(condFilosofo[indiceasinistra(indice)]),Flabel); 
		}
		/* rilascio la mutua esclusione */
		DBGpthread_mutex_unlock(&mutex,Flabel); 
	}
	pthread_exit(NULL); 
} 

int main (int argc, char* argv[] ) 
{ 
	pthread_t    th; 
	int  rc, i, *intptr;

	for(i=0;i<NUMFILOSOFI;i++) {
		rc = pthread_cond_init( &(condFilosofo[i]) , NULL);
		if( rc ) PrintERROR_andExit(rc,"pthread_cond_init failed");
	}
	rc = pthread_mutex_init( &mutex, NULL); 
	if( rc ) PrintERROR_andExit(rc,"pthread_mutex_init failed");

	for(i=0;i<NUMFILOSOFI;i++) {
		statoFilosofo[i]=PENSA;
		forchettaoccupata[i]=0;   /* forchetta libera */
	}

	for(i=0;i<NUMFILOSOFI;i++) {
		intptr=malloc(sizeof(int));
		if( !intptr ) { printf("malloc failed\n");exit(1); }
		*intptr=i;
		rc=pthread_create( &th,NULL,filosofo,(void*)intptr); 
		if(rc) PrintERROR_andExit(rc,"pthread_create failed");
	}

	while( 1 )  {
		int num=0;
		/* attendo 1/4 di sec */
		DBGnanosleep( 250000000, NULL );
		/*
		printf("stato: \t 0 %d \t 1 %d \t 2 %d \t 3 %d \t 4 %d\n", 
			statoFilosofo[0],
			statoFilosofo[1],
			statoFilosofo[2],
			statoFilosofo[3],
			statoFilosofo[4]
			);
		*/
		for( i=0; i<NUMFILOSOFI; i++ )
			if( statoFilosofo[i]==MANGIA )
				num++;	
		printf("numero di filosofi che mangiano contemporaneamente %d\n", num );
	}
	pthread_exit(NULL); 

	return(0); 
} 
  
  
  
