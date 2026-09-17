/*
 * FIFO.h
 *
 *  Created on: 27 Aug 2026
 *      Author: Adel Shata
 */

#ifndef FIFO_H_
#define FIFO_H_

/*Section: Includes*/
#include "Project_Config.h"
#include <stdio.h>
/*Section: Macros*/
/*Section: Macro Functions*/
/*Section: User Defined Data types*/

typedef enum {
	QUEUE_EMPTY = 0,
	QUEUE_FULL,
	QUEUE_NEITHER_FULL_NOR_EMPTY,
	QUEUE_ERROR
}queue_status_t;

// The type of the queue elements, so you can configure it as you wish
typedef struct{
	int R, C;
}queue_node;

typedef struct{
	queue_node *data;
	int top;
	int tail;
	unsigned int count;
	unsigned int size;
}queue_t;

/*Section: Function Declarations*/

/**
 * @brief Get the status of the queue
 * 
 * @param queue 
 * @return queue_status_t 
 */
queue_status_t queueStatus(queue_t *queue);

/**
 * @brief Initialize the queue
 * 
 * @param queue 
 * @param queueList 
 * @param size 
 * @return func_status_t 
 */
func_status_t queueInit(queue_t *queue, queue_node *queueList , unsigned int size);

/**
 * @brief Insert an element into the queue
 * 
 * @param queue 
 * @param insertedElem 
 * @return func_status_t 
 */
func_status_t enqueue(queue_t *queue, queue_node insertedElem);

/**
 * @brief Remove an element from the queue
 * 
 * @param queue 
 * @param poppedElem 
 * @return func_status_t 
 */
func_status_t dequeue(queue_t *queue, queue_node  *poppedElem);

#endif /* FIFO_H_ */
