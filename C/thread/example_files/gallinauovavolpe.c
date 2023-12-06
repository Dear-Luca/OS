/* file:  gallivauovavolpe.c 
*/ 

#define _THREAD_SAFE

#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h> 
#include "printerror.h"

int uova = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

#define SETTE_DECIMIDISECONDO 700000L
#define TREDICI_DECIMIDISECONDO 1300000L
#define NUMGALLINE 1
#define NUMVOLPI 1

void produce(void){ sleep(3); }

void *gallina_produce_uova(void *arg) 
{
	int rc;
	while(1) {
		produce();  /* gallina produce uova impiegando 3 sec ....*/  
		rc=pthread_mutex_lock(&mutex);
		if( rc ) PrintERROR_andExit(rc,"pthread_mutex_lock failed");
		/* gallina depone uova */
		printf("coccode\' deposto un ovetto caldo caldo\n");fflush(stdout);
		fflush(stdout);
		uova ++;
		rc=pthread_cond_signal(&cond);	/* il suono del coccode\' avvisa volpe che c'è uovo */
		if( rc ) PrintERROR_andExit(rc,"pthread_cond_signal failed");
		rc=pthread_mutex_unlock(&mutex);
		if( rc ) PrintERROR_andExit(rc,"pthread_mutex_unlock failed");
	}
}

void mangia(void){	printf("gnam gnam ..."); fflush(stdout); usleep(SETTE_DECIMIDISECONDO);
			printf(" ahhh\n"); fflush(stdout);  }
void burp(void){ 	printf("burp\n\n"); fflush(stdout); usleep(TREDICI_DECIMIDISECONDO);  }

void *volpe_mangia_uova(void *arg)  /* volpe mangia un uovo alla volta e poi digerisce */
{
	int rc;
	while(1) {
		rc=pthread_mutex_lock(&mutex); 
		if( rc ) PrintERROR_andExit(rc,"pthread_mutex_lock failed");
		while( uova==0 ) {
			/* attendi che venga prodotto uovo */
			rc=pthread_cond_wait(&cond, &mutex);
			if( rc ) PrintERROR_andExit(rc,"pthread_cond_wait failed");
		}
		mangia(); /* volpe mangia uovo impiegando quasi niente ....*/
		uova --;
		rc=pthread_mutex_unlock(&mutex);
		if( rc ) PrintERROR_andExit(rc,"pthread_mutex_unlock failed");
		burp(); /* volpe digerisce impiegando tempo */
	} 		
}

int main(void) 
{		pthread_t tid;
		int rc; int i;
		for (i=0; i<NUMGALLINE; i++) {
			rc=pthread_create( &tid, NULL, gallina_produce_uova, NULL );
			if (rc) PrintERROR_andExit(rc,"pthread_create failed"); /* no EINTR */
		}
		for (i=0; i<NUMVOLPI; i++) {
			rc=pthread_create( &tid, NULL, volpe_mangia_uova, NULL );
			if (rc) PrintERROR_andExit(rc,"pthread_create failed"); /* no EINTR */
		}
		pthread_exit(NULL);
		return(0);
}

