#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_THREADS	2


void wait2seconds() {
  usleep(2000);
}

void wait500ms() {
  usleep(500);
}

void nestedWait(long tid) {

  printf("Nest waiting on thread #%ld!\n", tid);
  usleep(1000);
  wait500ms();
}

void *HelloThread(void *threadid)
{
  long tid;
  tid = (long) threadid;
  printf("Hello World! It's me, thread #%ld!\n", tid);
  nestedWait(tid);
  pthread_exit(NULL);
}

void *NestedHellosThread(void *threadid)
{
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;
  long tid;
  tid = (long) threadid;
  wait500ms(threadid);
  for(t=0;t<NUM_THREADS;t++){
    printf("NestedHellosThread: creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, HelloThread, (void *)t);

    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;

   wait2seconds();

   for(t=0;t<NUM_THREADS;t++){
     printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, NestedHellosThread, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
       }
     }

   /* Last thing that main() should do */
   pthread_exit(NULL);
}

