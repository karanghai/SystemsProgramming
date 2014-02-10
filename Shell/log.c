/** @file log.c */
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include <stdio.h>
/**
 * Initializes the log.
 *
 * You may assuem that:
 * - This function will only be called once per instance of log_t.
 * - This function will be the first function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @returns
 *   The initialized log structure.
 */
void log_init(log_t *l) 
{
	l->item = NULL;
	l->next = NULL;
	l->prev = NULL;
	l->logsize=0;
}

/**
 * Frees all internal memory associated with the log.
 *
 * You may assume that:
 * - This function will be called once per instance of log_t.
 * - This funciton will be the last function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure to be destoryed.
 */
void log_destroy(log_t* l) 
{
	if(l==NULL)	
		return;
	log_t * curr = l->next;
	if(l->item!=NULL)	
		free(l);
	
	log_destroy(curr);
	


}

/**
 * Push an item to the log stack.
 *
 *
 * You may assume that:
* - All pointers will be valid, non-NULL pointer.
*
 * @param l
 *    Pointer to the log data structure.
 * @param item
 *    Pointer to a string to be added to the log.
 */
void log_push(log_t* l, const char *item)
{
	log_t * curr = malloc(sizeof(log_t));
	curr->item = item;
	curr->next = l->next;
	curr->prev = l;
	l->next = curr;
	l->logsize=(l->logsize)+1;
	
	return;
}


/**
 * Preforms a newest-to-oldest search of log entries for an entry matching a
 * given prefix.
 *
 * This search starts with the most recent entry in the log and
 * compares each entry to determine if the query is a prefix of the log entry.
 * Upon reaching a match, a pointer to that element is returned.  If no match
 * is found, a NULL pointer is returned.
 *
 *
 * You may assume that:
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure.
 * @param prefix
 *    The prefix to test each entry in the log for a match.
 *
 * @returns
 *    The newest entry in the log whose string matches the specified prefix.
 *    If no strings has the specified prefix, NULL is returned.
 */
char *log_search(log_t* l, const char *prefix) 
{
	log_t * curr = l->next;
	while(curr!=NULL)    
	{
		if(!strncmp(curr->item, prefix, strlen(prefix)))
			return (char*)curr->item;
		curr=curr->next;
	}
	return NULL;
}

void log_print(log_t* l) 
{
	if(l->logsize>0){	
	int i=0 ;
	log_t * curr = l->next;
	
	 char ** array = calloc(256,256);
	//array=NULL;//intialize
	//array = malloc(sizeof(char*)*256);
	
	while(curr!=NULL){
		if(curr->item!=NULL){
			
			array[i] = (char*)curr->item;
				
		}
		curr=curr->next;	
		i++;
	}
	for(i=strlen(*array)-1;i>=0;i--){
		if(array[i]!=NULL){
		printf("%s\n",array[i]);		 
		//free((char*)array[i]);		
		}
	}	
	free(array);
	}
	return;

}
/*************************************************END*************************************************/


