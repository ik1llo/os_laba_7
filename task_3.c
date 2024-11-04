#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#define DEFAULT_ARRAY_SIZE 10
#define DEFAULT_WRITERS_AMOUNT 2
#define DEFAULT_READERS_AMOUNT 4
#define DEFAULT_RUNTIME 5

int*    array;
int     array_size;

pthread_mutex_t mutex;

void* writer (void* arg) {
    while (1) {
        int index = rand() % array_size;
        int value = rand() % 100;

        pthread_mutex_lock(&mutex);
        array[index] = value;
        printf("writer %ld wrote %d at index %d\n", (long) arg, value, index);
        pthread_mutex_unlock(&mutex);

        usleep((rand() % (300000 - 100000 + 1)) + 100000);
    }

    return NULL;
}

void* reader (void* arg) {
    while (1) {
        int index = rand() % array_size;

        pthread_mutex_lock(&mutex);
        int value = array[index];
        printf("reader %ld read %d from index %d\n", (long) arg, value, index);
        pthread_mutex_unlock(&mutex);

        usleep((rand() % (300000 - 100000 + 1)) + 100000);
    }

    return NULL;
}

void* print_array_state (void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        printf("current array state: ");
        for (int k = 0; k < array_size; k++)
            printf("%d ", array[k]);
        printf("\n");

        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    return NULL;
}

void print_final_array_state () {
    pthread_mutex_lock(&mutex);

    printf("final array state: ");
    for (int k = 0; k < array_size; k++)
        printf("%d ", array[k]);
    printf("\n");

    pthread_mutex_unlock(&mutex);
}

int main (int argc, char *argv[]) {
    srand(time(NULL));
    
    int writers_amount = DEFAULT_WRITERS_AMOUNT;
    int readers_amount = DEFAULT_READERS_AMOUNT;
    int runtime = DEFAULT_RUNTIME;           

    int opt;
    while ((opt = getopt(argc, argv, "s:w:r:t:")) != -1) {
        switch (opt) {
            case 's':
                array_size = atoi(optarg);
                break;
            case 'w':
                writers_amount = atoi(optarg);
                break;
            case 'r':
                readers_amount = atoi(optarg);
                break;
            case 't':
                runtime = atoi(optarg);
                break;
            default:
                return 1;
        }
    }

    if (array_size <= 0) array_size = DEFAULT_ARRAY_SIZE;
    if (writers_amount <= 0) writers_amount = DEFAULT_WRITERS_AMOUNT;
    if (readers_amount <= 0) readers_amount = DEFAULT_READERS_AMOUNT;
    if (runtime <= 0) runtime = DEFAULT_RUNTIME;

    array = malloc(array_size * sizeof(int));
    pthread_t writers[DEFAULT_WRITERS_AMOUNT], readers[DEFAULT_READERS_AMOUNT], printer;

    pthread_mutex_init(&mutex, NULL);

    for (long k = 0; k < writers_amount; k++)
        pthread_create(&writers[k], NULL, writer, (void*) k);
    for (long k = 0; k < readers_amount; k++)
        pthread_create(&readers[k], NULL, reader, (void*) k);

    pthread_create(&printer, NULL, print_array_state, NULL);

    sleep(runtime);

    for (int k = 0; k < writers_amount; k++)
        pthread_cancel(writers[k]);
    for (int k = 0; k < readers_amount; k++)
        pthread_cancel(readers[k]);
    pthread_cancel(printer);

    print_final_array_state();

    pthread_mutex_destroy(&mutex);
    free(array);
    
    return 0;
}