/*
1. call a function
2. access stack variable
3. access heap variable
4. access global variable
5. loop
6. create a thread
7. mutex lock and unlock
*/

#include <pthread.h>
#include <stdlib.h>

int global = 20;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_function(void* args) {

    int* val = (int*)args;
   
    pthread_mutex_lock(&mutex);    
    for (int i=0; i<2; ++i)
        *val += global;
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

void foo(int* val) {
    (*val)++;
}


int main(int argc, char** argv) {
    
    int local = 5;
    int* heap = (int*)malloc(sizeof(int));
    *heap = 10;
    
    foo(&local);
    foo(heap);
    
    pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));
    
    pthread_create(thread, NULL, thread_function, (void*)heap);
    pthread_join(*thread, NULL);        

    free(thread);
    free(heap);

    return 0;    
}