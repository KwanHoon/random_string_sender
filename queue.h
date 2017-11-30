#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>

struct Element
{
	void *data;
};

// A structure to represent a queue
struct Queue
{
 int front, rear, size;
 unsigned capacity;
 //int* array;
 struct Element *array;
 pthread_mutex_t mtx;
};

// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity);


// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue);

// Queue is empty when size is 0
int isEmpty(struct Queue* queue);


// Function to add an item to the queue.  
// It changes rear and size
//void enqueue(struct Queue* queue, int item)
int enqueue(struct Queue* queue, struct Element item);


// Function to remove an item from queue. 
// It changes front and size
//int dequeue(struct Queue* queue)
//struct Element dequeue(struct Queue* queue);
void *dequeue(struct Queue* queue);


// Function to get front of queue
/*
struct Element front(struct Queue* queue)
{
 if (isEmpty(queue))
  return INT_MIN;
 return queue->array[queue->front];
}

// Function to get rear of queue
struct Element rear(struct Queue* queue)
{
 if (isEmpty(queue))
  return INT_MIN;
 return queue->array[queue->rear];
}
*/

#endif
