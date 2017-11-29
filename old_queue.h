#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdlib.h>		// malloc 
#include <pthread.h>

struct queue_node
{
	void *data;	
	struct queue_node *next;
};

struct queue
{
	struct queue_node *first;
	struct queue_node *last;

	size_t size;

	pthread_mutex_t mtx;
};


struct queue *create_new_queue(size_t max_size);
int release_queue(struct queue *queue);

//int enqueue(struct queue *queue, struct queue_node *node);
int enqueue(struct queue *queue, void *data);
struct queue_node dequeue(struct queue *queue);

int is_empty(struct queue *queue);

#endif
