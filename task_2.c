#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

double  a = 0;
double  b = 2;
long    n = 100000;
double  result = 0;

pthread_mutex_t mutex;

double func (double x) { return 4 - x * x; }

typedef struct {
    int thread_id;
    int threads_amount;
} ThreadData;

void* integrate (void* arg) {
    ThreadData* data = (ThreadData*) arg;
    int thread_id = data->thread_id;
    int threads_amount = data->threads_amount;

    double local_result = 0.0;
    double h = (b - a) / n;
    long step_size = n / threads_amount;

    long start = thread_id * step_size;
    long end = (thread_id == threads_amount - 1) ? n : start + step_size;

    for (long k = start; k < end; ++k) {
        double x = a + (k + 0.5) * h;
        local_result += func(x) * h;
    }

    pthread_mutex_lock(&mutex);
    result += local_result;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main (int argc, char* argv[]) {
    if (argc != 2)
        return 1;

    int threads_amount = atoi(argv[1]);
    if (threads_amount <= 0)
        return 1;

    pthread_t* threads = malloc(threads_amount * sizeof(pthread_t));
    ThreadData* thread_data = malloc(threads_amount * sizeof(ThreadData));
    pthread_mutex_init(&mutex, NULL);

    for (int k = 0; k < threads_amount; ++k) {
        thread_data[k].thread_id = k;
        thread_data[k].threads_amount = threads_amount;

        pthread_create(&threads[k], NULL, integrate, &thread_data[k]);
    }

    for (int k = 0; k < threads_amount; ++k)
        pthread_join(threads[k], NULL);

    printf("result: %.10f\n", result);

    pthread_mutex_destroy(&mutex);
    free(threads);
    free(thread_data);

    return EXIT_SUCCESS;
}
