/* usare_strerror_r.c  */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/* VIC: AGGIUNGERE QUI SOTTO il file .h per usare strerror_r */
#include <string.h>
/* VIC: FINE AGGIUNTA */

int main()
{
	pthread_t tid;
	int rc;
	void *ptr;

	/* prendo il mio stesso identificatore di thread */
	tid = pthread_self();
	/* faccio la join con me stesso cio' provoca errorei
	   perche' mi bloccherei aspettando la mia stessa morte */
	rc = pthread_join( tid , &ptr );
	if (rc){
		char msg[100]; int ret;
		ret=strerror_r(rc,msg,100);
		if( ret != 0 ) {
			printf("strerror failed: errore %i \n", ret );
			fflush(stdout);
			return(1);
		}
		/* stampo il messaggio di errore provocato dalla join */
		printf("pthread_join failed: %i %s\n", rc, msg );
		
	} else {
		printf("join ok\n");
		fflush(stdout);
	}

	return(0);
}
