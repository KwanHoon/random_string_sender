#include "queue.h"

// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* create_queue(unsigned capacity)
{
 struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
 queue->capacity = capacity;
 queue->front = queue->size = 0; 
 queue->rear = capacity - 1;
 queue->array = (struct Element *)malloc(queue->capacity * sizeof(struct Element));

 pthread_mutex_init(&queue->mtx, NULL);

 return queue;
}

void release_queue(struct Queue *queue)
{
	if(queue != NULL) 
		free(queue->array);

	pthread_mutex_destroy(&queue->mtx);
}

// Queue is full when size becomes equal to the capacity 
int is_full(struct Queue* queue)
{  
 return queue->size == queue->capacity ? 1 : 0;
}

// Queue is empty when size is 0
int is_empty(struct Queue* queue)
{  
 return queue->size == 0 ? 1 : 0; 
}

// Function to add an item to the queue.  
// It changes rear and size
//void enqueue(struct Queue* queue, int item)
int enqueue(struct Queue* queue, struct Element item)
{
 pthread_mutex_lock(&queue->mtx);

 if (is_full(queue)) {
  pthread_mutex_unlock(&queue->mtx);
  return -1;
 }

 queue->rear = (queue->rear + 1)%queue->capacity;
 queue->array[queue->rear] = item;
 queue->size = queue->size + 1;

 pthread_mutex_unlock(&queue->mtx);

 return 0;
}

// Function to remove an item from queue. 
// It changes front and size
//int dequeue(struct Queue* queue)
//struct Element dequeue(struct Queue* queue)
void *dequeue(struct Queue* queue)
{
 struct Element item;

 pthread_mutex_lock(&queue->mtx);

 if (is_empty(queue)) {
	pthread_mutex_unlock(&queue->mtx);
	return NULL;
 }

 item = queue->array[queue->front];
 queue->front = (queue->front + 1)%queue->capacity;
 queue->size = queue->size - 1;

 pthread_mutex_unlock(&queue->mtx);

 return item.data;
}
