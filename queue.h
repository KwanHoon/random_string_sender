#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>

#include "strgen.h"

#define MAX_QUEUE_SIZE 1000

/*
struct Element
{
	void *data;
};
*/

// A structure to represent a queue
struct Queue
{
 int front, rear, size;
 unsigned capacity;
 //struct Element **array;
 struct str_with_tm_t *array;
 pthread_mutex_t mtx;
};

// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* create_queue(unsigned capacity);
void release_queue(struct Queue *queue);


// Queue is full when size becomes equal to the capacity 
int is_full(struct Queue* queue);

// Queue is empty when size is 0
int is_empty(struct Queue* queue);


// Function to add an item to the queue.  
// It changes rear and size
//void enqueue(struct Queue* queue, int item)
//int enqueue(struct Queue* queue, struct Element item);
//int enqueue(struct Queue* queue, void *item);
int enqueue(struct Queue* queue, struct str_with_tm_t *item);


// Function to remove an item from queue. 
// It changes front and size
//int dequeue(struct Queue* queue)
//struct Element dequeue(struct Queue* queue);
struct str_with_tm_t dequeue(struct Queue* queue);


#endif
