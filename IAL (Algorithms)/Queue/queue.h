/**
 * @file queue.h
 * @author Michal Ľaš
 * @brief header file for queue.c (Queue)
 * 
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdbool.h>


#define MAX_QUEUE 50    // Size of queue

extern int QUEUE_SIZE;


extern bool error_flag; // Global variable - error flag


/** Queue implemented using static array */
typedef struct {
	char array[MAX_QUEUE];  // array for data
	int firstIndex;         // index of first element
	int freeIndex;          // index of first empty position in Queue
} Queue;


/**
 * @brief Function set error flag and print error message to stderr
 * 
 */
void Queue_Error();


/**
 * @brief Function initialize new Queue
 *  - set all member in queue to '*'
 *  - index of first element set to 0
 *  - index of first empty postition set to 0
 * 
 * @param queue pointer to queue structure
 */
void Queue_Init(Queue *queue);


/**
 * @brief Function return non zero value if queue is empty else return 0
 * 
 * @param queue pointer to initialized queue
 * @return int non zero if empty else 0
 */
int Queue_IsEmpty(const Queue *queue);


/**
 * @brief Function return non zero value if queue is full else return 0
 * 
 * @param queue pointer to initialized queue
 * @return int non zore if full else 0
 */
int Queue_IsFull(const Queue *queue);


/**
 * @brief Function in parameter dataPtr return value of first element
 * If queue is empty then call function Queue_Error()
 * 
 * @param queue pointer to initialized queue
 * @param dataPtr pointer to data that function will set
 */
void Queue_Front(const Queue *queue, char *dataPtr);


/**
 * @brief Function remove first element of queue
 * If queue is empty than call Queue_Error()
 * 
 * @param queue pointer to initialized queue
 */
void Queue_Remove(Queue *queue);


/**
 * @brief Function remove first element of queue and in parameter dataPtr return its value
 * If queue is empty than call Queue_Error()
 * 
 * @param queue pointer to initialized queue
 * @param dataPtr pointer to data that function will set
 */
void Queue_Dequeue(Queue *queue, char *dataPtr);


/**
 * @brief Function add new element to queue
 * If queue is full than call Queue_Error()
 * 
 * @param queue pointer to initialized queue
 * @param data data that will be set in new element
 */
void Queue_Enqueue(Queue *queue, char data);

#endif

/* END OF FILE */
