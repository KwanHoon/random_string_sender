#include "queue.h"

#include <stdio.h>
#include <string.h>

struct queue *create_new_queue(size_t max_size)
{
	struct queue *new_queue = NULL;
	
	new_queue = (struct queue *)malloc(sizeof(struct queue));
	if(new_queue == NULL) {
		perror("failed to create new queue");
		return NULL;
	}
	 
	new_queue->first = NULL;
	new_queue->last = NULL;
	new_queue->size = 0;

	//new_queue->mtx = PTHREAD_MUTEX_INITIALIZER;
	if(pthread_mutex_init(&new_queue->mtx, NULL) != 0) {
		perror("Failed to initialize mutex of queue");
		return NULL;
	}

	return new_queue;
}

int release_queue(struct queue *queue)
{
	if(queue == NULL) {
		fprintf(stderr, "queue to release is null");	
		return -1;
	}

	queue->first = NULL;
	queue->last = NULL;
	queue->size = 0;

	pthread_mutex_destroy(&queue->mtx);

	free(queue);
	queue = NULL;

	return 0;
}

//int enqueue(struct queue *queue, struct queue_node *node)
int enqueue(struct queue *queue, void *data)
{
	struct queue_node *new_node = NULL;

	if(queue == NULL) {
		fprintf(stderr, "queue is null\n");
		return -1;
	}

	if(data == NULL) {
		fprintf(stderr, "data is null\n");
		return -1;
	}

	pthread_mutex_lock(&queue->mtx);

	// alloc new queue node
	new_node = (struct queue_node *)malloc(sizeof(struct queue_node));
	if(new_node == NULL) {
		perror("Failed to alloc new node");
		pthread_mutex_unlock(&queue->mtx);
		return -1;
	}
	new_node->data = data;
	new_node->next = NULL;

	// insert to queue
	if(queue->first == NULL)
		queue->first = new_node;
	else
		queue->first->next = new_node;

	queue->last= new_node;
	queue->size++;

	fprintf(stderr, "queue size: %lu\n", queue->size);

	pthread_mutex_unlock(&queue->mtx);

	return 0;
}

struct queue_node dequeue(struct queue *queue)
{
	struct queue_node node;
	struct queue_node *new_first = NULL;

	memset(&node, 0x00, sizeof(node)); if(queue == NULL) {
		fprintf(stderr, "queue is null");
		return node;
	}

	if(queue->first == NULL) {
		fprintf(stderr, "queue is empty");
		return node;
	}

	pthread_mutex_lock(&queue->mtx);

	fprintf(stderr, "dequeue ~~~\n");

	// dequeue first node of queue
	node = *(queue->first);
	new_first = queue->first->next;
	free(queue->first);
	
	// re-point next node
	queue->first = new_first;

	queue->size--;


	fprintf(stderr, "queue size: %lu\n", queue->size);
	
	// when free?????

	pthread_mutex_unlock(&queue->mtx);
	
	return node;
}

int is_empty(struct queue *queue)
{
	if(queue == NULL) {
		fprintf(stderr, "queue is null\n");
		return -1;
	}

	if(queue->first != NULL && queue->size != 0)
		return -1;

	return 0;
}
