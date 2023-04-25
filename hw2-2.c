/*Kemika Weerakulwatana 6310682619*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 16
#define MAX_VALUE 100100000

int counter = 0;
pthread_mutex_t mutex;

int thread_ranks[MAX_THREADS];

void* count(void* arg) {
    int thread_id = *(int*)arg;
    int count = 0;

    while (count < MAX_VALUE) {
        count++;
    }

    int thread_rank = 0;
    pthread_mutex_lock(&mutex);
    counter++;
    thread_rank = counter;
    pthread_mutex_unlock(&mutex);
    thread_ranks[thread_id - 1] = thread_rank;

    return NULL;
}

int main() {
    pthread_t threads[MAX_THREADS];
    int thread_ids[MAX_THREADS];

    pthread_mutex_init(&mutex, NULL);

    // Create 16 threads
    for (int i = 0; i < MAX_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, count, &thread_ids[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print thread ranks in order
    printf("Ranking of threads\n");
    for (int rank = 1; rank <= MAX_THREADS; rank++) {
        for (int i = 0; i < MAX_THREADS; i++) {
            if (thread_ranks[i] == rank) {
                printf("Rank %d: Thread %d\n", rank, i + 1);
                break;
            }
        }
    }

    // Destroy mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}



