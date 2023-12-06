/* file:  busywaiting_gallivauovavolpe.c 
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

void produce(void){ sleep(3); }

void *gallina_produce_uova(void *arg) 
{
	int rc;
	while(1) {
		produce();  /* gallina produce uova impiegando 3 sec ....*/  
		rc=pthread_mutex_lock(&mutex);
		if( rc ) PrintERROR_andExit(rc,"pthread_mutex_lock failed");
		/* gallina depone uova */
		uova ++;
		printf("coccode\' deposto un ovetto caldo caldo\n");fflush(stdout); 
		rc=pthread_mutex_unlock(&mutex);
		if( rc ) PrintERROR_andExit(rc,"pthread_mutex_unlock failed");
	}
}

void mangia(void){ printf("gnam gnam\n"); fflush(stdout); sleep(1); }
void burp(void){ sleep(1); printf("burp\n\n"); fflush(stdout); } 

void *volpe_mangia_uova(void *arg)  /* volpe mangia un uovo alla volta e poi digerisce */
{
	int rc; int mannaggia_counter=0;
	while(1) {
		rc=pthread_mutex_lock(&mutex); 
		if( rc ) PrintERROR_andExit(rc,"pthread_mutex_lock failed");
		if( uova > 0 ) {
			mangia(); /* volpe mangia uovo impiegando quasi niente ....*/
			uova --;
			rc=pthread_mutex_unlock(&mutex);
			if( rc ) PrintERROR_andExit(rc,"pthread_mutex_unlock failed");
			burp(); /* volpe digerisce impiegando tempo */
			mannaggia_counter=0;
		} else {
			/* spiacente, nessun uovo disponibile / prova piu' tardi */
			rc=pthread_mutex_unlock(&mutex);
			if( rc ) PrintERROR_andExit(rc,"pthread_mutex_unlock failed");
			printf("mannaggia %i\n", ++mannaggia_counter); fflush(stdout);
		}
	} 		
}

int main(void) 
{		pthread_t tid;
		int rc;
		rc=pthread_create( &tid, NULL, gallina_produce_uova, NULL );
		if (rc) PrintERROR_andExit(rc,"pthread_create failed"); /* no EINTR */
		rc=pthread_create( &tid, NULL, volpe_mangia_uova, NULL );
		if (rc) PrintERROR_andExit(rc,"pthread_create failed"); /* no EINTR */
		pthread_exit(NULL);
		return(0);
}

