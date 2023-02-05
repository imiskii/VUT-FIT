/**
 * @file queue.c
 * @author Michal Ľaš
 * @brief Queue - implemented using static array
 * 
 */

#include "queue.h"

int QUEUE_SIZE = MAX_QUEUE;
int error_flag;


void Queue_Error() {

	fprintf(stderr, "*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}


void Queue_Init( Queue *queue ) {

    if (queue == NULL){
        Queue_Error();
    }
    else {
        for (int index = 0; index < QUEUE_SIZE; index++){
            queue->array[index] = '*';
        }
        queue->firstIndex = 0;
        queue->freeIndex = 0;
    }
}


int nextIndex( int index ) {

	return (index + 1) % QUEUE_SIZE;
}


int Queue_IsEmpty( const Queue *queue ) {

	return queue->firstIndex == queue->freeIndex;
}


int Queue_IsFull( const Queue *queue ) {

	return nextIndex(queue->freeIndex) == queue->firstIndex;
}


void Queue_Front( const Queue *queue, char *dataPtr ) {

    if (Queue_IsEmpty(queue)){
        Queue_Error();
    }
    else if (queue == NULL){
        Queue_Error();
    }
    else {
        *dataPtr = queue->array[queue->firstIndex];
    }
}


void Queue_Remove( Queue *queue ) {

    if (Queue_IsEmpty(queue)){
        Queue_Error();
    }
    else if (queue == NULL){
        Queue_Error();
    }
    else {
        queue->firstIndex = nextIndex(queue->firstIndex);
    }
}


void Queue_Dequeue( Queue *queue, char *dataPtr ) {

    if (Queue_IsEmpty(queue)){
        Queue_Error();
    }
    else if (queue == NULL){
        Queue_Error();
    }
    else {
        Queue_Front(queue, dataPtr);
        Queue_Remove(queue);
    }
}


void Queue_Enqueue( Queue *queue, char data ) {

    if (Queue_IsFull(queue)){
        Queue_Error();
    }
    else if (queue == NULL) {
        Queue_Error();
    }
    else {
        queue->array[queue->freeIndex] = data;
        queue->freeIndex = nextIndex(queue->freeIndex);
    }
}

/* Konec příkladu c203.c */
