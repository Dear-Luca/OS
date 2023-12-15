/* file:  NProd_NCons.c 
   NProduttori e NConsumatori che si scambiano prodotti
   mediante un unico Buffer. E' la versione efficente perche' 
   - utilizza due pthread_cond_var (una per Prod e una per Cons)
   - sveglia i pthread solo quando hanno qualcosa da fare.
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

/* settati per vedere output di debug
#define DEBUGCONTROLLARIEMPIMENTOBUFFER
#define DEBUGSTAMPAAZIONI
#define DEBUGSTAMPASTATOBUFFER
*/

#define NUMPROD 5
#define NUMCONS 3 
#define NUMBUFFER 1	/* NON MODIFICARE MAI */

/* dati da proteggere */
uint64_t valGlobale=0;

/* variabili per la sincronizzazione */
pthread_mutex_t  mutex; 
pthread_cond_t   condProd, condCons; 
int numBufferPieni=0; /* 0 o 1 */
int numProdWaiting=0;
int numProdWaitingAndSignalled=0; /* deve essere <= #buffer vuoti */
int numConsWaiting=0;
int numConsWaitingAndSignalled=0; /* deve essere <= #buffer pieni */

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

		/* se tutti i buffer sono pieni allora il
		   produttore deve aspettare che un consumatore
		   lo risvegli dopo avere svuotato un posto.

		In generale il problema e' che se ho NUMBUFFER buffer
		ed ho numBufferPieni buffer pieni
		ed ho (NUMBUFFER-numBufferPieni) buffer vuoti
		ed ho numProdWaiting Produttori in coda
		ed ho numProdWaitingAndSignalled Produttori 
		in coda e gia' abilitati a proseguire (cioe' signalled)
		SE i Prod in Coda e signalled SONO >= dei buffer vuoti
		cioe'
		 SE numProdWaitingAndSignalled >= (NUMBUFFER-numBufferPieni)
		allora
		NON POSSO FAR PROSEGUIRE dei Prod SENZA METTERLI NELLA CODA
		*/
		if( numProdWaitingAndSignalled >= (NUMBUFFER-numBufferPieni)){

#ifdef DEBUGSTAMPAAZIONI
			printf("Prod %s ENTRA CODA\n",Plabel);fflush(stdout);
#endif

			numProdWaiting++;
			DBGpthread_cond_wait(&condProd,&mutex,Plabel);
			numProdWaiting--;
			numProdWaitingAndSignalled--;
		}
#ifdef DEBUGSTAMPAAZIONI
		else {
			printf("Prod %s SALTA CODA\n",Plabel);fflush(stdout);
		}
#endif
#ifdef DEBUGCONTROLLARIEMPIMENTOBUFFER
		if( numBufferPieni!=0 ) {
			printf("Prod %s vede BUFFER %d!=0 \n",Plabel,numBufferPieni);fflush(stdout);
			exit(1);
		}
#endif
		/* riempie il buffer col dato prodotto prima
		   in realta' per semplicita' incremento la var 
		*/
		valGlobale=valProdotto;

		printf("Prod %s inserisce %lu \n", Plabel, valGlobale ); 
		fflush(stdout);

		numBufferPieni++;


#ifdef DEBUGCONTROLLARIEMPIMENTOBUFFER
		if( numBufferPieni!=1 ) {
		    printf("Prod %s vede BUFFER %d!=1\n",Plabel, numBufferPieni);fflush(stdout);
			exit(1);
		}
#endif

#ifdef DEBUGSTAMPASTATOBUFFER
		printf( "Prod %s bufPieni %d prodincoda %d prodincodasign %d "
			"consincoda %d consincodasign %d\n",
			Plabel, numBufferPieni,numProdWaiting,
			numProdWaitingAndSignalled,numConsWaiting,
			numConsWaitingAndSignalled);
		fflush(stdout);
#endif

		/*
		SE ho qualche Cons in coda non segnalato
		(cioe' SE numConsWaitingAndSignalled < numConsWaiting )
		E SE i Cons in Coda e signalled SONO < dei buffer pieni
		(cioe' SE numConsWaitingAndSignalled < numBufferPieni) allora
		POSSO ABILITARE altri Cons a proseguire e uscire dalla wait.
		*/
		if( 
			( numConsWaitingAndSignalled < numConsWaiting ) 
			&&
			( numConsWaitingAndSignalled < numBufferPieni ) 
		  ) {
			/* risveglio un Consumatore per svuotare un buffer */
			DBGpthread_cond_signal(&condCons,Plabelsignal); 
			numConsWaitingAndSignalled++;
		}

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

		/* se tutti i buffer sono vuoti  allora
		   aspettiamo che un Prod ci risvegli 

		In generale il problema e' che se ho NUMBUFFER buffer
		ed ho numBufferPieni buffer pieni
		ed ho numConsWaiting Cousumatori in coda
		ed ho numConsWaitingAndSignalled Consumatori 
		   in coda e gia' abilitati a proseguire (cioe' signalled)
		SE i Cons in Coda e Signalled SONO >= dei buffer pieni
		cioe'
		 SE numConsWaitingAndSignalled >= numBufferPieni
		allora
		NON POSSO FAR PROSEGUIRE dei Cons SENZA METTERLI NELLA CODA
		*/
		if( numConsWaitingAndSignalled >= numBufferPieni ) {

#ifdef DEBUGSTAMPAAZIONI
			printf("Cons %s ENTRA CODA\n",Clabel);fflush(stdout);
#endif

			numConsWaiting++;
			DBGpthread_cond_wait(&condCons,&mutex,Clabel);
			numConsWaiting--;
			numConsWaitingAndSignalled--;
		}
#ifdef DEBUGSTAMPAAZIONI
		else {
			printf("Cons %s SALTA CODA\n",Clabel);fflush(stdout);
		}
#endif
#ifdef DEBUGCONTROLLARIEMPIMENTOBUFFER
		if( numBufferPieni!=1 ) {
			printf("Cons %s vede BUFFER %d!=1 \n",Clabel, numBufferPieni);fflush(stdout);
			exit(1);
		}
#endif

		/* prendo cio' che c'e' nel buffer */
		val=valGlobale;

		printf("Cons %s legge %lu \n", Clabel, val ); 
		fflush(stdout);

		numBufferPieni--;

#ifdef DEBUGCONTROLLARIEMPIMENTOBUFFER
		if( numBufferPieni!=0 ) {
			printf("Cons %s vede BUFFER %d!=0 \n",Clabel,numBufferPieni);fflush(stdout);
			exit(1);
		}
#endif

		/*
		SE i Prod in Coda e signalled SONO >= dei buffer vuoti
		NON POSSO ABILITARE altri Prod in coda a proseguire

		invece	
		SE ho qualche Prod in coda non segnalato
		(cioe' SE numProdWaitingAndSignalled < numProdWaiting )
		E SE i Prod in Coda e signalled SONO < dei buffer pieni
		cioe' 
		 SE numProdWaitingAndSignalled < (NUMBUFFER-numBufferPieni)
		allora
		POSSO ABILITARE altri Prod a proseguire e uscire dalla wait.
		*/
		if( 
		    (numProdWaitingAndSignalled < numProdWaiting ) 
			&&
		    (numProdWaitingAndSignalled < (NUMBUFFER-numBufferPieni)) 
		  ) 
		{ 
			/* risveglio 1 Prod per riempire il buffer svuotato */
			DBGpthread_cond_signal(&condProd,Clabelsignal); 
			numProdWaitingAndSignalled++;
		}

#ifdef DEBUGSTAMPASTATOBUFFER
		printf( "Cons %s bufPieni %d prodincoda %d prodincodasign %d "
			"consincoda %d consincodasign %d\n",
			Clabel, numBufferPieni,numProdWaiting,
			numProdWaitingAndSignalled,numConsWaiting,
			numConsWaitingAndSignalled);
		fflush(stdout);
#endif
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
	numProdWaiting=0;
	numProdWaitingAndSignalled=0;
	numConsWaiting=0;
	numConsWaitingAndSignalled=0;

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
  
  
  
