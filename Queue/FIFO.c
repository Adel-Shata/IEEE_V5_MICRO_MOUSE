/*
 * FIFO.c
 *
 *  Created on: 27 Aug 2026
 *      Author: Adel Shata
 */

#include "FIFO.h"

/*Helper function 	↓↓↓*/

static void ptrReposition(queue_t* queue, int* ptr){
	if((int)(queue->size - 1) == *ptr){
		*ptr = 0;
	}
	else{
		(*ptr)++;
	}
}

/*Primary functions ↓↓↓*/

queue_status_t queueStatus(queue_t *queue){
	queue_status_t returnFlag = QUEUE_NEITHER_FULL_NOR_EMPTY;
	if(NULL == queue){
		returnFlag = QUEUE_ERROR;
	}
	// The fist condition (queue->count == 0) will be true but I have added the remain condition just for safety
	else if((queue->count == 0) ||(-1 == queue->top) || (-1 == queue->tail)){
		returnFlag = QUEUE_EMPTY;
	}
	else if(queue->size == queue->count){
		returnFlag = QUEUE_FULL;
	}
	else{/*NOTHING*/}

	return returnFlag;
}

func_status_t queueInit(queue_t *queue, queue_node *queueList , unsigned int size){

	func_status_t returnFlag = E_SUCCESS;

	// Checking if queue and queueList is valid or not
	if((NULL == queue) || (NULL == queueList) || (0 == size))
		returnFlag = E_FAILURE;
	else{
		queue->data  = queueList;
		queue->size  = size;
		queue->count = (unsigned int)QUEUE_EMPTY;
		queue->top   = queue->tail = -1;
	}

	return returnFlag;
}

func_status_t enqueue(queue_t *queue, queue_node insertedElem){

	func_status_t returnFlag = E_SUCCESS;

	// Checking if queue is valid or not
	if((NULL == queue))
		returnFlag = E_FAILURE;
	else{
		switch(queueStatus(queue)){
			case QUEUE_FULL:
			case QUEUE_ERROR:
				returnFlag = E_FAILURE;
				break;
			case QUEUE_EMPTY:
				ptrReposition(queue, &queue->tail);
			case QUEUE_NEITHER_FULL_NOR_EMPTY:
				ptrReposition(queue, &queue->top);
				queue->data[queue->top] = insertedElem;
				queue->count++;
				break;
			default:
				returnFlag = E_FAILURE;
				break;
		}
	}

	return returnFlag;
}

func_status_t dequeue(queue_t *queue, queue_node  *poppedElem){

	func_status_t returnFlag = E_SUCCESS;

	// Checking if queue is valid or not
	if((NULL == queue) || (NULL == poppedElem))
		returnFlag = E_FAILURE;
	else{
		switch(queueStatus(queue)){
			case QUEUE_EMPTY:
			case QUEUE_ERROR:
				returnFlag = E_FAILURE;
				break;
			case QUEUE_FULL:
			case QUEUE_NEITHER_FULL_NOR_EMPTY:
				*poppedElem = queue->data[queue->tail];
				ptrReposition(queue, &queue->tail);
				// Return the top and the tail
				if(QUEUE_EMPTY == (queue_status_t)(--queue->count)){
					queue->top   = queue->tail = -1;
				}
				else{/*NOTHING*/}
			break;
			default:
				returnFlag = E_FAILURE;
				break;


		}
	}
	return returnFlag;
}
