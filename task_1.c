#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

long long shared_variable = 0;

void* increment(void* arg) {
    long iterations_amount = *(long*) arg;
    for (long k = 0; k < iterations_amount; ++k)
        shared_variable++;
    
    return NULL;
}

void* decrement(void* arg) {
    long iterations_amount = *(long*) arg;
    for (long k = 0; k < iterations_amount; ++k)
        shared_variable--;

    return NULL;
}

int main (int argc, char* argv[]) {
    long threads_amount = 2;
    long iterations_amount = 1000000;

    for (int k = 1; k < argc; k += 2) {
        if (strcmp(argv[k], "-t") == 0 && k + 1 < argc)
            threads_amount = atol(argv[k + 1]);
        else if (strcmp(argv[k], "-i") == 0 && k + 1 < argc)
            iterations_amount = atol(argv[k + 1]);
    }

    printf("initial variable value: %lld\n", shared_variable);
    printf("threads amount: %ld | operations per thread: %ld\n", threads_amount, iterations_amount);

    pthread_t* threads = malloc(threads_amount * 2 * sizeof(pthread_t));

    for (long k = 0; k < threads_amount; ++k) {
        pthread_create(&threads[k * 2], NULL, increment, &iterations_amount);
        pthread_create(&threads[k * 2 + 1], NULL, decrement, &iterations_amount);
    }

    for (long k = 0; k < threads_amount * 2; ++k)
        pthread_join(threads[k], NULL);

    printf("final variable value: %lld\n", shared_variable);

    free(threads);
    return 0;
}
