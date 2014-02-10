/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>	//added   


/** Structure for each dictionary entry **/
typedef struct _dictionary_entry_t{
	//void * ptr;
	size_t size;//int size;
	int free;
	struct _dictionary_entry_t * next ;
	struct _dictionary_entry_t * prev ;	//attempt 5 added
	char memory[];	//added, this is our memory
	}dictionary_entry_t;

dictionary_entry_t * dictionary = NULL;
//dictionary_entry_t * list = NULL;
/**
 * Allocate space for array in memory
 * 
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 * 
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size)
{
	/* Note: This function is complete. You do not need to modify it. */
	void *ptr = malloc(num * size);
	
	if (ptr)
		memset(ptr, 0x00, num * size);

	return ptr;
}


/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size)
{
	dictionary_entry_t ** p = & dictionary;
	dictionary_entry_t **chosen = NULL;
	//best fit or first fit??
	//lets try with first fit first , then best fit...match stats for the test allocations
	//first fit gives 207.44%	
	/*while(*p!=NULL)
	{
		if((*p)->size >= size){
			if(chosen==NULL){
				chosen =p;
			}
		}
		p=&(*p)->next;
	}
	*/		
	//looks like it works, now on to best fit. 

	while(*p!=NULL)
	{
		if((*p)->size >= size){
			if(chosen ==NULL || (*chosen)->size > (*p)->size){
				chosen = p;
			}
		}
	p = &(*p)->next	;	
	}
	
	if(chosen!=NULL)
	{
		void * memory = (*chosen)->memory;
		
		*chosen = (*chosen)->next;
		return memory;

	}
	//gives 206.75%....so use best fit...........dammit it oscillates, hard to figure out which ones better
	
	//else

	dictionary_entry_t * new_entry = sbrk(size + sizeof(dictionary_entry_t));
	new_entry -> size = size ;
	new_entry -> free = 0;//added for attempt 6
	//new_entry -> prev = *p;//added for attempt 5..wow
	//(*p)->next = new_entry;
	
	new_entry->next =NULL;
	return new_entry->memory;
	




//return NULL;
}


/**
 * Deallocate space in memory
 * 
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *y_entry_t * p - 
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr)
{
	// "If a null pointer is passed as argument, no action occurs."
	 if (!ptr)
        	return;


	//size_t avg_size =0 ;
	/*dictionary_entry_t * p = &((dictionary_entry_t*) ptr)[-1];
	p->next = dictionary;
	dictionary = p;
	return ;	*/
	//the above is circular list implementation

	//Below we have non K&R free, doubly linked list, non-circular

	/*void * dictionary_entry_ptr = ptr-sizeof(dictionary_entry_t);
	dictionary_entry_t * dictionary_entry = (dictionary_entry_t * )	dictionary_entry_ptr;
	dictionary_entry->free =1;
	dictionary_entry_t * p = dictionary_entry;	
	if(p->next!=NULL){	
		while(p!=NULL){
			p=p->next;
			if(p->free){
				dictionary_entry->next = p;
				break;
			}
		}	
	}
	else
		dictionary_entry->next =NULL;	
	p = dictionary_entry;//reset p value

	
	if(p->prev!=NULL){
		while (p!=NULL){
		p=p->prev;
		if(p->free){
			dictionary_entry->prev = p;
			break;		
			}	
		}
	}
	
	else
		dictionary_entry->prev=NULL;*/
//dammit, gives wrong data after realloc
	
    	dictionary_entry_t * p =ptr-sizeof(dictionary_entry_t);
	//dictionary_entry_t * right = p->next;
	//dictionary_entry_t * left = p->prev;
			
	//why doesn't not deleteing it work...urgghhh
	p->next = dictionary;
	dictionary = p;
	
/*	if(right!=NULL)
	{	
		if(right->free){
			right->next = dictionary;
			dictionary = right;
		}
	}

	if(left!=NULL)
	{	
		if(left->free){
			left->next = dictionary;
			dictionary = left;
		}
	}
	*/ //hmm coalescing decreases performance

	return;






	/*void * dictionary_entry_ptr = ptr-sizeof(dictionary_entry_t);
	dictionary_entry_t * dictionary_entry = (dictionary_entry_t * )	dictionary_entry_ptr;
	dictionary_entry->free =1;
    	return;*/

}
/**
 * Reallocate memory block
 * 
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *    
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size)
{
	 // "In case that ptr is NULL, the function behaves exactly as malloc()"
	if (!ptr)
		return malloc(size);

	 // "In case that the size is 0, the memory previously allocated in ptr
	 //  is deallocated as if a call to free() was made, and a NULL pointer
	 //  is returned."
	if (!size)
	{
		free(ptr);
		return NULL;
	}
	
	dictionary_entry_t * p = ptr-sizeof(dictionary_entry_t);	
	size_t temp;
	//I dont know if this will work but here goes nothing..
	
			
			
	if(size <= p->size)	//if the new size is smaller or equal to the current size
		 temp = size;	
	
	else 
		temp  = p->size;

/*	dictionary_entry_t * temp_p = p;	
	while(temp_p!=NULL)
	{
				
			temp_p=temp_p->next;
			if(temp_p!=NULL){
				if(temp_p->size>=size){
					free(ptr);
					return temp_p;
				}
			}

		
	} //made it 205.xx % */
		
/*	temp_p=p; //reset
	
	while(temp_p!=NULL)
	{
				
			temp_p=temp_p->prev;
			if(temp_p!=NULL){
				if(temp_p->size>=size){
					free(ptr);
					return temp_p;
				}
			}
	}*/
	
	/*if(p->next!=NULL && p->free){//this brought it down to 204.xx%
		if(p->size<=size){
			free(ptr);
			return p;
			}
	}*/	

	void * new_memory = malloc(size);	
	memcpy(new_memory,ptr,temp);
	free( ptr);
	return new_memory;
	
	
}




