#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>


#define SIZE 10			/* Size of matrices */
int N;				/* number of threads */

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];

void fill_matrix(int m[SIZE][SIZE])
{
  int i, j, n = 0;
  for (i=0; i<SIZE; i++)
    for (j=0; j<SIZE; j++)
      m[i][j] = n++;
}

void print_matrix(int m[SIZE][SIZE])
{
  int i, j = 0;
  for (i=0; i<SIZE; i++) {
    printf("\n\t| ");
    for (j=0; j<SIZE; j++)
      printf("%2d ", m[i][j]);
    printf("|");
  }
}


void* mmult (void* slice)
{
  int s = (intptr_t) slice;
  int from = (s * SIZE)/N;	/* note that this 'slicing' works fine */
  int to = ((s+1) * SIZE)/N;	/* even if SIZE is not divisible by N */
  int i,j,k;

  printf("computing slice %d (from row %d to %d)\n", s, from, to-1);
  for (i=from; i<to; i++) 
    for (j=0; j<SIZE; j++) {
      C[i][j]=0;
      for (k=0; k<SIZE; k++)
	C[i][j] += A[i][k]*B[k][j];
    }

  printf("finished slice %d\n", s);
  return 0;
}

int main(int argc, char *argv[])
{
  pthread_t *thread;
  int i;

  if (argc!=2) {
    printf("Usage: %s number_of_threads\n",argv[0]);
    exit(-1);
  }  

  N=atoi(argv[1]);
  fill_matrix(A);
  fill_matrix(B);
  thread = (pthread_t*) malloc(N*sizeof(pthread_t));

  for (i=1; i<N; i++) {
    if (pthread_create (&thread[i], NULL, mmult, (void*) (intptr_t) i) != 0 ) {
      perror("Can't create thread");
      exit(-1);
    }
  }

  /* master thread is thread 0 so: */
  mmult(0);

  for (i=1; i<N; i++) pthread_join (thread[i], NULL);

  printf("\n\n");
  print_matrix(A);
  printf("\n\n\t       * \n");
  print_matrix(B);
  printf("\n\n\t       = \n");
  print_matrix(C);
  printf("\n\n");

  return 0;

}

