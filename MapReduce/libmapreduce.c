/** @file libmapreduce.c */
/* 
 * CS 241
 * The University of Illinois
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libmapreduce.h"
#include "libds/libds.h"


static const int BUFFER_SIZE = 2048;  /**< Size of the buffer used by read_from_fd(). */

int epoll_fd;

unsigned long revision_up;
int num_values;
char ** buffer;
int ** fds;
pthread_t thread;


/**
 * Adds the key-value pair to the mapreduce data structure.  This may
 * require a reduce() operation.
 *
 * @param key
 *    The key of the key-value pair.  The key has been malloc()'d by
 *    read_from_fd() and must be free()'d by you at some point.
 * @param value
 *    The value of the key-value pair.  The value has been malloc()'d
 *    by read_from_fd() and must be free()'d by you at some point.
 * @param mr
 *    The pass-through mapreduce data structure (from read_from_fd()).
 */
static void process_key_value(const char *key, const char *value, mapreduce_t *mr)
{
	char *old;
	char *new;
	unsigned long rev_num;
	unsigned long success;	
	old = datastore_get(mr->k_v_r, key , &rev_num);

	if (old == NULL) // does not already exist
	{
//		printf("datastore_put\n");	
		datastore_put( mr->k_v_r , key, value);

	}

	else	//already exists , we need to reduce
	{
		while(1)
		{
			free(old);
			old = datastore_get(mr->k_v_r, key , &rev_num);
			new = mr->myreduce(old, value);
			success = datastore_update(mr->k_v_r , key , new , rev_num); // get the latest revision 
			
			free(new);
			if(success)
			{	
		
				break;
			}

		}
	
		
	}
	free(old);
	
	free(key);
	free(value);

	return;
}


/**
 * Helper function.  Reads up to BUFFER_SIZE from a file descriptor into a
 * buffer and calls process_key_value() when for each and every key-value
 * pair that is read from the file descriptor.
 *
 * Each key-value must be in a "Key: Value" format, identical to MP1, and
 * each pair must be terminated by a newline ('\n').
 *
 * Each unique file descriptor must have a unique buffer and the buffer
 * must be of size (BUFFER_SIZE + 1).  Therefore, if you have two
 * unique file descriptors, you must have two buffers that each have
 * been malloc()'d to size (BUFFER_SIZE + 1).
 *
 * Note that read_from_fd() makes a read() call and will block if the
 * fd does not have data ready to be read.  This function is complete
 * and does not need to be modified as part of this MP.
 *
 * @param fd
 *    File descriptor to read from.
 * @param buffer
 *    A unique buffer associated with the fd.  This buffer may have
 *    a partial key-value pair between calls to read_from_fd() and
 *    must not be modified outside the context of read_from_fd().
 * @param mr
 *    Pass-through mapreduce_t structure (to process_key_value()).
 *
 * @retval 1
 *    Data was available and was read successfully.
 * @retval 0
 *    The file descriptor fd has been closed, no more data to read.
 * @retval -1
 *    The call to read() produced an error.
 */
static int read_from_fd(int fd, char *buffer, mapreduce_t *mr)

{
//	printf("in read_from_fd\n");
	/* Find the end of the string. */
	int offset = strlen(buffer);

	/* Read bytes from the underlying stream. */
	int bytes_read = read(fd, buffer + offset, BUFFER_SIZE - offset);
	if (bytes_read == 0) {
		return 0;
	}
	else if(bytes_read < 0)
	{
		fprintf(stderr, "error in read.\n");
		return -1;
	}

	buffer[offset + bytes_read] = '\0';

	/* Loop through each "key: value\n" line from the fd. */
	char *line;
//	printf("before while loop\n");
	while ((line = strstr(buffer, "\n")) != NULL)
	{
		*line = '\0';

//		printf("after while loop\n");

		/* Find the key/value split. */
		char *split = strstr(buffer, ": ");
		if (split == NULL)
			continue;

		/* Allocate and assign memory */
		char *key = malloc((split - buffer + 1) * sizeof(char));
		char *value = malloc((strlen(split) - 2 + 1) * sizeof(char));

		strncpy(key, buffer, split - buffer);
		key[split - buffer] = '\0';

		strcpy(value, split + 2);

//		printf("key=%s value=%s\n", key, value);

		
		/* Process the key/value. */
		process_key_value(key, value, mr);

		/* Shift the contents of the buffer to remove the space used by the processed line. */
		memmove(buffer, line + 1, BUFFER_SIZE - ((line + 1) - buffer));
		buffer[BUFFER_SIZE - ((line + 1) - buffer)] = '\0';
	}

	return 1;
}


/**
 * Initialize the mapreduce data structure, given a map and a reduce
 * function pointer.
 */
void mapreduce_init(mapreduce_t *mr, 
                    void (*mymap)(int, const char *), 
                    const char *(*myreduce)(const char *, const char *))
{

	mr->mymap = mymap;
	mr->myreduce = myreduce;
	mr ->k_v_r = malloc(sizeof(datastore_t));
	datastore_init ( mr->k_v_r);

	
}


/**
 * Starts the map() processes for each value in the values array.
 * (See the MP description for full details.)
 */

void * workerthread(void * ptr)
{

//	printf("in worker thread\n");

	mapreduce_t *mr = ptr;		
	int i;
	
	buffer = malloc(sizeof(char*)*num_values);
	for(i=0;i<num_values;i++)
	{
		buffer[i] = malloc(sizeof(char)*(BUFFER_SIZE+1));
		buffer[i][0] = '\0';
	}
	

	int active_fd =num_values;
	
	while(active_fd > 0 )
	{
		struct epoll_event ev;
		epoll_wait (mr->epoll_fd , &ev , 1 ,-1);


		/** how do we find which buffer to use ? **/
		//check for fd in  event list

		i=0;		
		while(i<num_values)
		{	
			if(ev.data.fd == mr->ev[i].data.fd)
				break;
			i++;
		}		
	
		size_t bytes =  read_from_fd( ev.data.fd , buffer[i], mr ) ;



		if(bytes==1)	//data was read successfully
		{
			/** what do i do here ???? **/	/** DO NOTHING **/					
		}
		
		else if (bytes==0)	// no more data to be read
		{
			epoll_ctl( mr->epoll_fd, EPOLL_CTL_DEL , ev.data.fd , NULL);
			active_fd --; 
		}
		
		

	}
	
	return NULL;
	/** is this the end of the function ? **/
}

void mapreduce_map_all ( mapreduce_t *mr, const char **values )
{
	int i;

	for(num_values=0; values[num_values]!=NULL;num_values++);
	
	mr->epoll_fd = epoll_create(num_values);
	mr->ev = malloc( sizeof( struct epoll_event ) * num_values);


	fds = malloc(sizeof(int*)*num_values); 

	/** fork function **/
	for(i=0; i< num_values;i++)
	{	

		fds[i] = malloc(2 * sizeof(int));
		pipe(fds[i]); 	

		int read_fd = fds[i][0];
		int write_fd = fds[i][1];			
		
		pid_t pid = fork();
		
		if(pid == 0)
		{
			close(read_fd);
		
			/** in description **/

			mr->mymap(write_fd, values[i]);
			exit(0);

		}

		else
		{			
			close(write_fd);
		}
		
		/** in piazza post **/
		bzero(&mr->ev[i], sizeof(mr->ev[i]));
		/** from discussion **/		
		mr->ev[i].events= EPOLLIN;
		mr->ev[i].data.fd = read_fd; // should be read_from_fd ????
		epoll_ctl( mr->epoll_fd, EPOLL_CTL_ADD , read_fd , & mr->ev[i] );	


	}
	/** more code to go here **/
	/** need to call the worker thread **/
	

	pthread_create( &thread, NULL , workerthread, (void *)mr );

}


/**
 * Blocks until all the reduce() operations have been completed.
 * (See the MP description for full details.)
 */
void mapreduce_reduce_all(mapreduce_t *mr)
{
	 pthread_join(thread,NULL);
}


/**
 * Gets the current value for a key.
 * (See the MP description for full details.)
 */
const char *mapreduce_get_value(mapreduce_t *mr, const char *result_key)
{
	// do we need to malloc this ?? Also what revision number do we use?
	const char* value = datastore_get( mr->k_v_r, result_key , &revision_up);
	return value;
}


/**
 * Destroys the mapreduce data structure.
 */
void mapreduce_destroy(mapreduce_t *mr)
{
	// how do i delete key , value ????
	
	
	
		
	int i;
	for(i=0;i<num_values;i++)
	{
		free(fds[i]);
		free(buffer[i]);
	
	}
	free(fds);
	free(buffer);
	free(mr->ev);
	
	datastore_destroy( mr->k_v_r);
	free ( mr->k_v_r);
	// free all the malloced code
	// This may be incomplete
}
