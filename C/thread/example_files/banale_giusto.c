/* banale_giusto.c  */

/*  i simboli _THREAD_SAFE _REENTRANT _POSIX_C_SOURCE=200112L
*   sono stati definiti nella riga di comando per la compilazione con il gcc
*   nel Makefile
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5

int Glob=10;

void *PrintHello(void *p_index)
{
	/* NB: qui sotto faccio un assegnamento NON ATOMICO,
		 potenziale Problema, ma per ora non lo consideriamo */
	Glob+=3; 
	sleep(4);

	/* uso l'intero che mi è stato dato nell'area di memoria passatami */
	printf("\n index %d: Hello World! Glob=%d\n", *((int*)p_index), Glob);
	/* rilascio l'area di memoria che mi è stata passata */
	free(p_index);
	pthread_exit (NULL);
}

int main()
{
	pthread_t tid;
	int rc, t, *p;       /* notare la variabile puntatore p */

	for(t=0;t < NUM_THREADS;t++){

		/* alloco memoria sullo heap in cui mettere il parametro per il pthread */
		p=malloc(sizeof(int));       
		if(p==NULL) {
			perror("malloc failed: ");
			pthread_exit (NULL);
		}
		*p=t;
		printf("Creating thread %d\n", t);
		rc = pthread_create (&tid, NULL, PrintHello, p );  /* NOTARE l'ultimo parametro */
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n",rc);
			exit(-1);
		}

	}

	printf("fine main\n"); fflush(stdout);
	
	pthread_exit (NULL);  /* provare a commentare questa riga */
	return(0);
}
