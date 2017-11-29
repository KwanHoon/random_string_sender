#include "queue.h"

// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
 struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
 queue->capacity = capacity;
 queue->front = queue->size = 0; 
 queue->rear = capacity - 1;  // This is important, see the enqueue
 //queue->array = (int *)malloc(queue->capacity * sizeof(int));
 queue->array = (struct Element *)malloc(queue->capacity * sizeof(struct Element));

 return queue;
}

// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{  
 return (queue->size == queue->capacity);  
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{  
 return (queue->size == 0); 
}

// Function to add an item to the queue.  
// It changes rear and size
//void enqueue(struct Queue* queue, int item)
int enqueue(struct Queue* queue, struct Element item)
{
 if (isFull(queue)) {
  fprintf(stderr, "queue is full\n");
  return -1;
 }

 queue->rear = (queue->rear + 1)%queue->capacity;
 queue->array[queue->rear] = item;
 queue->size = queue->size + 1;

 return 0;
}

// Function to remove an item from queue. 
// It changes front and size
//int dequeue(struct Queue* queue)
//struct Element dequeue(struct Queue* queue)
void *dequeue(struct Queue* queue)
{
 struct Element item;
 if (isEmpty(queue)) {
  fprintf(stderr, "queue is empty\n");
  return NULL;
 }

 item = queue->array[queue->front];
 queue->front = (queue->front + 1)%queue->capacity;
 queue->size = queue->size - 1;

 return item.data;
}

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
