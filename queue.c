#include "queue.h"

#include <stdio.h>

struct queue *create_new_queue(size_t max_size)
{
	struct queue *new_queue = NULL;
	
	new_queue = (struct queue *)malloc(sizeof(struct queue));
	if(new_queue == NULL) {
		perror("failed to create new queue");
		return NULL;
	}
	
	/*
	if(max_size > 0) {
		new_queue->arr = (struct queue_node *)malloc(sizeof(struct queue_node) * max_size);
		if(new_queue->arr == NULL) {
			perror("failed to create queue array");
			return NULL;
		}
	}		
	new_queue->size = max_size;
	*/ 
	new_queue->first = NULL;
	new_queue->last = NULL;

	return new_queue;
}

int release_queue(struct queue *queue)
{
	if(queue == NULL) {
		fprintf(stderr, "queue to release is null");	
		return -1;
	}

	/*
	if(queue->size > 0 && queue->arr != NULL) {
		free(queue->arr);
		queue->size = 0;
	}
	*/
	queue->first = NULL;
	queue->last = NULL;

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

	// alloc new queue node
	new_node = (struct queue_node *)malloc(sizeof(struct queue_node));
	if(new_node == NULL) {
		perror("Failed to alloc new node");
		return -1;
	}
	new_node->data = data;
	new_node->next = NULL;

	// insert to queue
	if(queue->first == NULL) {
		queue->first = new_node;
		queue->last = queue->first;
	}
	else {
		queue->last->next = new_node;
	}

	return 0;
}

struct queue_node *dequeue(struct queue *queue)
{
	struct queue_node *node = NULL;

	if(queue == NULL) {
		fprintf(stderr, "queue is null");
		return NULL;
	}

	if(queue->first == NULL) {
		fprintf(stderr, "queue is empty");
		return NULL;
	}

	// dequeue first node of queue
	node = queue->first;
	
	// re-point next node
	queue->first = queue->first->next;
	
	// when free?????
	
	return node;
}

int is_empty(struct queue *queue)
{
	if(queue == NULL) {
		fprintf(stderr, "queue is null");
		return -1;
	}

	if(queue->first != NULL)
		return -1;

	return 0;
}
