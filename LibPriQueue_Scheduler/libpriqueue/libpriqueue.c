/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->head= NULL;
	q->size = 0;				//size is 0
	q->compare = comparer;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{

	qnode * node = malloc(sizeof(qnode));
	node->elem = ptr;
	node->next=NULL;
	
	
	int i=0;	
	qnode * temp = q->head;
	qnode * temp1=NULL;


	if(temp==NULL){				//changed for from if q->size is 0;
		q->head = node;
		q->size++;
		return 0;
	}
	
	else if( q->compare(node->elem,temp->elem) < 0 ){		//first comparison with head
		 				
		node->next = q->head;
		q->head = node;		
		q->size++;
		return 0;
	}		
	
	else {
		while(temp!=NULL && q->compare(node->elem,temp->elem) >= 0){		//iterates atleast once
			temp1=temp;		
			temp=temp->next;
			i++;
		}
		if(temp1==NULL) printf("temp1 is NULL \n");
		temp1->next = node;		
		node->next = temp;

		q->size++;
		return i;		
	}
	


	return -1;

}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if(q->size!=0)
	{
		void * res = q->head->elem;
		return res;
	}


	return NULL;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{	
	if(q->head!=NULL)	//&&q->size!=0
	{
		qnode * temp = q->head;
		q->head = (q->head)->next;
		q->size--;
		
		void * head_elem = temp->elem;		
		free(temp);
		return head_elem;	
	}


	return NULL;

}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	while(q->head!=NULL)
	priqueue_poll(q);

}
