#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define NUM_THREADS 2

void *BusyWork(void *t)
{
	int i;
	long tid;
	double result=0.0;
	tid = (long)t;
	printf("Thread %ld starting...\n",tid);
	for (i=0; i<(tid+1)*1000000; i++) {
		result = result + sin(i) * tan(i);
	}
	printf("Thread %ld done. Result = %e \n",tid, result);
}


void *NestedBusyWork ()
{
	pthread_t thread[NUM_THREADS];
	int rc;
	long t;
	void *status;

	for(t=0; t<NUM_THREADS; t++) {
	    printf("Nested: creating thread %ld \n", t);
	    rc = pthread_create(&thread[t], NULL, BusyWork, (void *)t);  
	    if (rc) {
		  printf("ERROR; return code in nested from pthread_create() is %d \n", rc);
		  exit(-1);
		  }
	    }

	/* Wait for the other threads */
	for(t=0; t<NUM_THREADS; t++) {
		rc = pthread_join(thread[t], &status);
		if (rc) {
		  printf("ERROR; return code in nested from pthread_join() is %d\n", rc);
		  exit(-1);
		}
	   printf("Nested: completed join with thread %ld having a status of %ld \n",t,(long)status);
	}

	printf("Nested: program completed. Exiting.\n");
}



int main (int argc, char *argv[])
{
	pthread_t thread[NUM_THREADS];
	int rc;
	long t;
	void *status;

	for(t=0; t<NUM_THREADS; t++) {
	    printf("Main: creating thread %ld \n", t);
	    rc = pthread_create(&thread[t], NULL, NestedBusyWork, NULL);  
	    if (rc) {
		  printf("ERROR; return code in main from pthread_create() is %d \n", rc);
		  exit(-1);
		  }
	    }

	/* Wait for the other threads */
	for(t=0; t<NUM_THREADS; t++) {
		rc = pthread_join(thread[t], &status);
		if (rc) {
		  printf("ERROR; return code in main from pthread_join() is %d\n", rc);
		  exit(-1);
		}
	   printf("Main: completed join with thread %ld having a status of %ld \n",t,(long)status);
	}

	printf("Main: program completed. Exiting.\n");
}
