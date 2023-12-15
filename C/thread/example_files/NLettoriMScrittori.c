/* file:  NLettoriMScrittori.c 
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

#define NUMLETTORI 10
#define NUMSCRITTORI 5
/* 1/10 secondo */
#define SCRITTOREATTESAPRIMADISEZIONECRITICA 100000000
/* 1/100 secondo */
#define SCRITTOREATTESADENTROSEZIONECRITICA 10000000
/* 1/100 secondo */
#define LETTOREATTESADENTROSEZIONECRITICA 10000000


/* PRMA FARE VEDERE CON QUESTA DEFINE SETTATA, 
   POI COMMENTARLA PER FAR VEDERE IL VERO MODO LETTORI/SCRITTORI
#define USAMUTUAESCLUSIONEANCHETRAILETTORI 1
*/

/* Buffer da proteggere */
uint64_t valGlobale=0;
/* var da proteggere ma usate solo per fini statistici */
uint64_t numLettureTotali=0, numScrittureTotali=0;
/* var per controllo automatico */
char ultimoaccesso='0';

/* variabili per la sincronizzazione */
pthread_mutex_t  mutexVarLettori;    /* permette l'accesso a singoli lettori */ 
pthread_mutex_t  mutexBuffer; /* permette accesso a singoli scrittori o a tutti i lettori */
int numLettoriInLettura=0;  /* numero lettori che stanno leggendo */

void *Scrittore (void *arg) 
{ 
	char Slabel[128];
	char Slabelsignal[128];
	int indice;
	uint64_t valProdotto=0;

	indice=*((int*)arg);
	free(arg);
	sprintf(Slabel,"S%d",indice);
	sprintf(Slabelsignal,"S%d->",indice);

	while(1) {
		/* QUI LO SCRITTORE PRODUCE IL DATO, un intero.
		   supponiamo che lo scrittore impieghi circa
		   1 secondo a produrre il dato
		*/
		DBGnanosleep( SCRITTOREATTESAPRIMADISEZIONECRITICA ,Slabel);
		printf("Scrittore %s PRODUCE dato %lu\n", Slabel, valProdotto+1 ); 
		fflush(stdout);
		valProdotto++;
		
		/* prendo la mutua esclusione */
		DBGpthread_mutex_lock(&mutexBuffer,Slabel); 

		/* INIZIO SEZIONE CRITICA */
		ultimoaccesso='w'; /* controllo automatizzato */
		/* riempie il buffer col dato prodotto prima */
		valGlobale=valProdotto;

		/* lo scrittore impiega un po' di tempo nella sezione critica, 1/100 sec */
		DBGnanosleep( SCRITTOREATTESADENTROSEZIONECRITICA, Slabel );

		printf("Scrittore %s inserisce %lu \n", Slabel, valGlobale ); 
		fflush(stdout);
		numScrittureTotali++; /* aggiorno statistiche */
		if( ultimoaccesso != 'w' ) {
			printf( "errore !w in scrittura \n");
			fflush(stdout);
			exit(1);
		}
		ultimoaccesso='1';
		/* FINE SEZIONE CRITICA */

		/* rilascio mutua esclusione */
		DBGpthread_mutex_unlock(&mutexBuffer,Slabel); 

	}
	pthread_exit(NULL); 
} 

void *Lettore (void *arg) 
{ 
	uint64_t val;
	uint64_t numLetture=0;  /* conta letture effettuate per decidere quando riposarsi */
	char Llabel[128];
	char Llabelsignal[128];
	int indice;

	indice=*((int*)arg);
	free(arg);
	sprintf(Llabel,"L%d",indice);
	sprintf(Llabelsignal,"L%d->P",indice);

	
	while(1) {
#ifndef USAMUTUAESCLUSIONEANCHETRAILETTORI
		/* prendo la mutua esclusione sulla variabile dei lettori */
		DBGpthread_mutex_lock(&mutexVarLettori,Llabel); 

		/* se non ci sono altri lettori gia' a leggere dal buffer
		   significa che nessun altro lettore ha gia' preso la mutua esclusione
		   per il gruppo dei lettori, quindi
		   devo prendere io la mutua esclusione per tutto il gruppo dei lettori
		*/
		if ( numLettoriInLettura <= 0 ) {
#endif
			DBGpthread_mutex_lock(&mutexBuffer,Llabel); 
			ultimoaccesso='R';
#ifndef USAMUTUAESCLUSIONEANCHETRAILETTORI
		}
		/* ora vado in lettura anche io */
		numLettoriInLettura++;
		/* ma permetto ad altri lettori di vedere come stanno le cose */
		/* rilascio la mutua esclusione sulla variabile dei lettori */
		DBGpthread_mutex_unlock(&mutexVarLettori,Llabel); 
#endif


		/* SEZIONE CRITICA: leggo cio' che c'e' nel buffer */
		val=valGlobale;
		numLettureTotali++; /* aggiorno statistiche */

		/* il lettore impiega un po' di tempo nella sezione critica 1/100 di sec */
		DBGnanosleep( LETTOREATTESADENTROSEZIONECRITICA , Llabel );

		printf("Lettore %s legge %lu \n", Llabel, val ); 
		fflush(stdout);

		if( ultimoaccesso != 'R' ) {
			printf( "errore !R in lettura \n");
			fflush(stdout);
			exit(1);
		}
		/* FINE SEZIONE CRITICA */

#ifndef USAMUTUAESCLUSIONEANCHETRAILETTORI
		/* riprendo la mutua esclusione sulla variabile dei lettori */
		DBGpthread_mutex_lock(&mutexVarLettori,Llabel); 
		/* io non leggo piu' */
		numLettoriInLettura--;
		/* se sono l'ultimo lettore che era in lettura,
		   devo rilasciare la mutua esclusione per il gruppo dei lettori
		*/
		if ( numLettoriInLettura <= 0 ) {
#endif
			ultimoaccesso='0';
			/* rilascio mutua esclusione sul buffer */
			DBGpthread_mutex_unlock(&mutexBuffer,Llabel); 
#ifndef USAMUTUAESCLUSIONEANCHETRAILETTORI
		}
		/* rilascio la mutua esclusione sulla variabile dei lettori */
		DBGpthread_mutex_unlock(&mutexVarLettori,Llabel); 
#endif


		/* fuori da mutua esclusione, uso il dato 
		   supponiamo che ci voglia 1 secondo
		*/
		printf("Lettore %s USA dato %lu \n", Llabel, val ); 
		fflush(stdout);

		numLetture++;
		/* dopo 500 letture, il lettore aspetta 10 secondi */
		if( numLetture%500 == 0 ) {
			printf("Lettore %s si riposa per 10 secondi \n", Llabel ); 
			fflush(stdout);
			DBGsleep(10,Llabel);
		}
	}
	pthread_exit(NULL); 
} 

int main (int argc, char* argv[] ) 
{ 
	pthread_t    th; 
	int  rc, i, *intptr;
	struct timeval t0, t1;

	rc = pthread_mutex_init(&mutexVarLettori, NULL); 
	if( rc ) PrintERROR_andExit(rc,"pthread_mutex_init failed");
	rc = pthread_mutex_init(&mutexBuffer, NULL); 
	if( rc ) PrintERROR_andExit(rc,"pthread_mutex_init failed");

	gettimeofday( &t0, NULL );

	numLettoriInLettura=0;	/* all'inizio nessun lettore sta leggendo */

	for(i=0;i<NUMLETTORI;i++) {
		intptr=malloc(sizeof(int));
		if( !intptr ) { printf("malloc failed\n");exit(1); }
		*intptr=i;
		rc=pthread_create( &th,NULL,Lettore,(void*)intptr); 
		if(rc) PrintERROR_andExit(rc,"pthread_create failed");
	}

	for(i=0;i<NUMSCRITTORI;i++) {
		intptr=malloc(sizeof(int));
		if( !intptr ) { printf("malloc failed\n");exit(1); }
		*intptr=i;
		rc=pthread_create( &th,NULL,Scrittore,(void*)intptr); 
		if(rc) PrintERROR_andExit(rc,"pthread_create failed");
	}

	/* ora il main ogni 10 secondi stampa in output le statistiche di utilizzo
	   ok, e' un modo un po' alla cazzo di cane, perche' dovrei accedere alle variabili
	   contenenti le statistiche in mutua esclusione, ma cosi' cambierei il flusso
	   di esecuzione .....
	*/
	while(1) {
		DBGsleep( 10, "main" );
		gettimeofday( &t1, NULL );
		printf("******************* STATISTICHE D\'USO approssimate"
			" numLettureTotali %lu  numScrittureTot %lu NumLettureAlSecondo %f\n", 
			numLettureTotali, numScrittureTotali,
			((float)numLettureTotali) / (t1.tv_sec-t0.tv_sec) );
	}
	pthread_exit(NULL); 

	return(0); 
} 
  
  
  
