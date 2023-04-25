/*Kemika Weerakulwatana 6310682619*/
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#define NUM_THREADS 4

char NAME[][50] = {"Kemika Weerakulwatana"};
pthread_mutex_t mutex;

// Thread 1: Print first name
void* PrintFirstName(void* arg) {
    pthread_mutex_lock(&mutex);
    char firstName[50];
    sscanf(NAME[0], "%s", firstName);
    printf("Thread 1: First Name: %s\n", firstName);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

// Thread 2: Print last name
void* PrintLastName(void* arg) {
    pthread_mutex_lock(&mutex);
    char lastName[50];
    sscanf(NAME[0], "%*s %s", lastName);
    printf("Thread 2: Last Name: %s\n", lastName);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

// Thread 3: Print full name in reverse
void* PrintReverseFullName(void* arg) {
    pthread_mutex_lock(&mutex);
    printf("Thread 3: Full Name (Reverse): ");
    for (int i = strlen(NAME[0]) - 1; i >= 0; i--) {
        printf("%c", NAME[0][i]);
    }
    printf("\n");
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

// Thread function 4: Count characters in full name
void* CountCharacters(void* arg) {
    pthread_mutex_lock(&mutex);
    int count = 0;
    for (int i = 0; i < strlen(NAME[0]); i++) {
        if (NAME[0][i] != ' ') {
            count++;
        }
    }
    printf("Thread 4: Total Characters in Full Name: %d\n", count);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    // Create threads
    pthread_create(&threads[0], NULL, PrintFirstName, NULL);
    pthread_create(&threads[1], NULL, PrintLastName, NULL);
    pthread_create(&threads[2], NULL, PrintReverseFullName, NULL);
    pthread_create(&threads[3], NULL, CountCharacters, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}