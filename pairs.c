#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define NUM_THREADS 24 
#define NUM_PAIRS 100000000

typedef struct {
    int a;
    int b;
} Pair;

Pair pairs[NUM_PAIRS];
bool results[NUM_PAIRS];
pthread_mutex_t lock;
int task_index = 0;

void* compare_integers(void* arg) {
    int index;

    while (1) {
        // Lock the mutex to access shared variables
        pthread_mutex_lock(&lock);
        index = task_index++;
        pthread_mutex_unlock(&lock);

        if (index >= NUM_PAIRS) {
            break;
        }

        results[index] = (pairs[index].a == pairs[index].b);
    }

    return NULL;
}

void tic(struct timespec *start) {
    clock_gettime(CLOCK_MONOTONIC, start);
}

void toc(struct timespec *start) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_spent = (end.tv_sec - start->tv_sec) + (end.tv_nsec - start->tv_nsec) / 1e9;
    printf("Execution time: %f seconds\n", time_spent);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);
    struct timespec start_time;

    // Initialize pairs with random values
    srand(time(NULL));
    for (int i = 0; i < NUM_PAIRS; i++) {
        pairs[i].a = rand() % 1000;
        pairs[i].b = rand() % 1000;
    }

    tic(&start_time);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, compare_integers, NULL)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
    }

    toc(&start_time);

    pthread_mutex_destroy(&lock);

    // Print results (optional)
    //for (int i = 0; i < NUM_PAIRS; i++) {
    //    printf("Pair (%d, %d): %s\n", pairs[i].a, pairs[i].b, results[i] ? "Equal" : "Not Equal");
    //}

    return 0;
}

