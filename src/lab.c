#include "../tests/harness/unity.h"
#include "../src/lab.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

// Define the queue structure
struct queue {
    void **buffer;            // Array to store queue elements
    int capacity;             // Maximum number of elements in the queue
    int size;                 // Current number of elements
    int front;                // Index of the front element
    int rear;                 // Index of the next insertion point
    bool shutdown;            // Flag indicating if the queue is shutting down
    pthread_mutex_t lock;     // Mutex for synchronizing access
    pthread_cond_t not_empty; // Condition variable signaled when queue is not empty
    pthread_cond_t not_full;  // Condition variable signaled when queue is not full
};

// Initialize the queue
queue_t queue_init(int capacity) {
    struct queue *q = malloc(sizeof(struct queue));
    if (q == NULL) {
        return NULL; 
    }
    q->buffer = malloc(sizeof(void *) * capacity);
    if (q->buffer == NULL) {
        free(q);
        return NULL;
    }
    q->capacity = capacity;
    q->size = 0;
    q->front = 0;
    q->rear = 0;
    q->shutdown = false;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
    return q;
}

// Destroy the queue and free resources
void queue_destroy(queue_t q) {
    if (q == NULL) {
        return;
    }
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
    free(q->buffer);
    free(q);
}

// Add an item to the queue
void enqueue(queue_t q, void *data) {
    pthread_mutex_lock(&q->lock);
    while (q->size == q->capacity && !q->shutdown) {
        // Wait until there's space in the queue
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    if (q->shutdown) {
        pthread_mutex_unlock(&q->lock);
        return; 
    }

    // Add the data to the queue
    q->buffer[q->rear] = data;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;

    // Signal that the queue is not empty
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

// Remove and return an item from the queue
void *dequeue(queue_t q) {
    pthread_mutex_lock(&q->lock);
    while (q->size == 0 && !q->shutdown) {
        // Wait until there's data in the queue
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    if (q->size == 0 && q->shutdown) {
        pthread_mutex_unlock(&q->lock);
        return NULL; // Exit if queue is shutting down and empty
    }

    // Remove the data from the queue
    void *data = q->buffer[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;

    // Signal that the queue is not full
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return data;
}

// Set the shutdown flag and wake up all waiting threads
void queue_shutdown(queue_t q) {
    pthread_mutex_lock(&q->lock);
    q->shutdown = true;
    pthread_cond_broadcast(&q->not_empty);
    pthread_cond_broadcast(&q->not_full);
    pthread_mutex_unlock(&q->lock);
}

// Check if the queue is empty
bool is_empty(queue_t q) {
    pthread_mutex_lock(&q->lock);
    bool empty = (q->size == 0);
    pthread_mutex_unlock(&q->lock);
    return empty;
}

// Check if the queue is in shutdown mode
bool is_shutdown(queue_t q) {
    pthread_mutex_lock(&q->lock);
    bool shutdown = q->shutdown;
    pthread_mutex_unlock(&q->lock);
    return shutdown;
}