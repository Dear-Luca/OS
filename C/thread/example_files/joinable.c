/* joinable.c  Esempio banale  */

/* gia' messi nella riga di comando del compilatore 
#define _THREAD_SAFE
#define _REENTRANT
#define _POSIX_C_SOURCE 200112L
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5

int Glob=10;
void *do_thread(void *p_index)
{
	int ris, *ptr;

	Glob+=3;
	printf("thread index %d: Glob=%d thread_ID %u\n", *((int*)p_index), Glob, (int) pthread_self() );
	/* per gioco restituisco come risultato l'indice cambiato di segno */
	ris = - *((int*)p_index);
	/* dealloco la struttura in cui mi sono stati passati i parametri */
	free(p_index);

	/* alloco la struttura in cui restituire il risultato */
	ptr=malloc(sizeof(int));
	if(ptr==NULL) {
		perror("malloc failed: ");
		pthread_exit (NULL);
	}
	else {
		*ptr=ris;
		pthread_exit ( ptr );  /* valore restituito dal thread */
	}
}

int main()
{
	pthread_t vthreads[NUM_THREADS];
	int rc, t, *p;

	printf("il main e' il thread con ID %u\n", (int) pthread_self() );

	for(t=0;t < NUM_THREADS;t++){

		/* alloco la struttura in cui passare i parametri */
		p=malloc(sizeof(int));
		if(p==NULL) {
			perror("malloc failed: ");
			pthread_exit (NULL);
		}
		*p=t;


		/* printf("Creating thread JOINABLE %d\n", t); */
		rc = pthread_create (&vthreads[t], NULL, do_thread, p );
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n",rc);
			exit(-1);
		}
		else
			printf("Created thread JOINABLE ID %u\n", (int) vthreads[t] );
			
	}

	for(t=0;t < NUM_THREADS;t++) {
		int error;
		void *ptr;

		/* attendo la terminazione del thread t-esimo  */
		error=pthread_join( vthreads[t] , &ptr );
		if(error!=0){
			printf("pthread_join() failed: error=%d\n", error ); 
			exit(-1);
		}
		else {
			printf("pthread %d-esimo ID=%u restituisce %d\n", t, (int) vthreads[t] , *((int*)ptr) );
			/* dealloco la struttura in cui mi e' stato passato il risultato */
			free(ptr);
		}
	}
	pthread_exit (NULL);
	return(1);	/* questa istruzione non viene eseguita ma,
			se necessario, potrei togliere l'istruzione qui sopra
			*/
}

