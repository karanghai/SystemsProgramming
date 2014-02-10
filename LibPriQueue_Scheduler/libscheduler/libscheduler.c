/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/
typedef struct _job_t
{
	int arrival;
	int start;
	int recent_start;
	int run_time;
	int run_time_left;
	int temp_end;
	int finished_time;
	int priority;
	int id;
	int just_added;
} job_t;

/**
	GLOBAL VARIABLES
*/

job_t ** core_list;		//array of cores
int core_list_size;		//number of cores in the list
priqueue_t *jobs;	
scheme_t input;	
int job_num;							

int * arrival_time;
int * duration;
int * end_time;
int * first_start;
/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/



void scheduler_start_up(int cores, scheme_t scheme)	
{

	int i;
	job_num=0;
	arrival_time = malloc(sizeof(int)*100);
	duration=malloc(sizeof(int)*100);
	end_time=malloc(sizeof(int)*100);
	first_start=malloc(sizeof(int)*100);

	for(i=0;i<core_list_size;i++)		//intialize array of specific times
	{
		arrival_time[i] = -1;
		duration[i] = -1;
		end_time[i] =-1;		
	}

	core_list_size = cores;	
	core_list = malloc(sizeof(job_t*)*core_list_size);			
	
	for(i=0;i<core_list_size;i++)
	{
		core_list[i]=NULL;
	}
	input = scheme;
	jobs = malloc(sizeof(priqueue_t));
	switch(input)
	{
		case FCFS : 	priqueue_init(jobs,compareFC_RR);
				break;
		case SJF : 	priqueue_init(jobs,compareSJF);
				break;
		case PSJF : 	priqueue_init(jobs,compareSJF);
				break;
		case PRI : 	priqueue_init(jobs,comparePRI);
				break;
		case PPRI : 	priqueue_init(jobs,comparePRI);
				break;
		case RR : 	priqueue_init(jobs,compareFC_RR);
				break;
		default : return;
	}


}

int compareFC_RR(const void * x,const  void * y)		//if negative x has lower id==higher priority
{
	return 1;
}

int compareSJF(const void * x, const void * y)
{
	job_t *x1 = (job_t*)x;
	job_t *y1 = (job_t*)y;
	
	if(x1->run_time_left < y1->run_time_left)
			return -1;
	else if (x1->run_time_left == y1->run_time_left)
		if(x1->arrival < y1->arrival)
			return -1;

	return 1;

}

int comparePRI(const void * x, const void * y)
{
	job_t *x1 = (job_t*)x;
	job_t *y1 = (job_t*)y;
	
	if(x1->priority < y1->priority)
			return -1;
	else if (x1->priority == y1->priority)
		if(x1->arrival < y1->arrival)
			return -1;

	return 1;

}

/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	
	int i;
	int empty= -1 ;
	int res=-1;	
	job_num++;
	job_t *new_job = malloc(sizeof(job_t));
	new_job->id = job_number;	
	new_job->arrival = time; //next cycle
	new_job->run_time = running_time;
	new_job->run_time_left = running_time;
	new_job->finished_time =-1;			//will be updated when job finishes
	new_job->start =-1;
	new_job->recent_start =-1;
	new_job->priority = priority;
	new_job->just_added =0;

	for(i=0;i<core_list_size;i++){				//updates run_time left
		
		if(core_list[i]!=NULL)	{
			if(core_list[i]->just_added!=1)
				core_list[i]->run_time_left = core_list[i]->run_time_left + core_list[i]->recent_start - time; 

			if(core_list[i]->just_added==1 )
				core_list[i]->just_added =0;
			}	

	}		
	




	for(i=0;i<core_list_size;i++){					//checks for NULL core with lowest_id
		if(core_list[i]==NULL){
			empty = 1;
			break;
		}
	}	
	
	if(empty==1)
	{		
		core_list[i] = new_job;			// i is the first index with No Job
		if(new_job->start==-1)
			new_job->start = time;			//it starts for the first time now
			
		new_job ->recent_start = time;	
	//	printf("start time of JOB %d is %d\n",new_job->id,new_job->start);		
		res = i;			
		return res;
	}

	else if(empty!=1 && (input == PSJF || input == PPRI) ){			//if no core is idle -- preempt i.e PSJF and PPRI
		
		if(input == PSJF)
		{
		
			res =0; 
			
			for(i=0;i<core_list_size;i++)					//this determines the longest job!
			{	
				if(compareSJF(core_list[res],core_list[i])==-1)
					res=i;
			}					
			
			if(compareSJF(new_job,core_list[res])==-1)
			{				
					job_t * holder = core_list[res];				
					
					core_list[res] = new_job;
					if(core_list[res]->start==-1)
						core_list[res]->start = time;			//it starts for the first time now
					
					core_list[res]->recent_start = time;	
					priqueue_offer (jobs,holder);
					return res;
		
			}

			
		}
		
		else if(input == PPRI)
		{
			res =0; 
			for(i=0;i<core_list_size;i++)					//this determines the lowest priority!
			{	
				if(comparePRI(core_list[res],core_list[i])==-1)
					res=i;
			}					
			
			if(comparePRI(new_job,core_list[res])==-1)
			{
				job_t * holder = core_list[res];				
					
				core_list[res] = new_job;
				if(core_list[res]->start==-1)
				core_list[res]->start = time;			//it starts for the first time now
			
				core_list[res]->recent_start = time;	
				priqueue_offer (jobs,holder);
				return res;
				
					
		
			}
		
		
		} //else-if #2

		//no more conditons--the above are the only preemptive scheduling techniques being implemented
	}//else_if #1	
	
	priqueue_offer (jobs,new_job);			//if no core idle and non-preemptive scheduling
//	printf("after adding , size of queue: %d\n", priqueue_size(jobs));	
	return -1;
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
//	printf("Job finished, size of queue: %d\n", priqueue_size(jobs));

	core_list[core_id]->finished_time  = time;	

	arrival_time[core_list[core_id]->id]= core_list[core_id]->arrival;		
	duration[core_list[core_id]->id]= core_list[core_id]->run_time;
	end_time[core_list[core_id]->id]= core_list[core_id]->finished_time;
	first_start[core_list[core_id]->id]= core_list[core_id]->start;
	
	job_t * delete_job = core_list[core_id];
	free(delete_job);
	(core_list[core_id])=NULL;		//this core is ready to use
	
	void * fresh_job = priqueue_poll(jobs);	

	
	if( ((job_t*)fresh_job) == NULL)
		return -1;
	((job_t*)fresh_job)->just_added=1;
	core_list[core_id] = ((job_t*)fresh_job);
	if(core_list[core_id]->start==-1)
		core_list[core_id]->start = time;			//it starts for the first time now
			
	core_list[core_id]->recent_start = time;	
	return ((job_t*)fresh_job)->id;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	job_t *temp = core_list[core_id];
	temp->run_time_left = temp->run_time_left - (time - temp->recent_start);
	
//	printf("Reaches before offer\n");
	priqueue_offer(jobs, temp);
	(core_list[core_id])=NULL;
	
	void * fresh_job = priqueue_poll(jobs);	

	if( ((job_t*)fresh_job) == NULL)
		return -1;	
	
	core_list[core_id]= ((job_t *)fresh_job);
	if(core_list[core_id]->start==-1)
		core_list[core_id]->start = time;			//it starts for the first time now
			
	core_list[core_id]->recent_start = time;
	return ((job_t*)fresh_job)->id;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	int i;
	float hold=0;
	float res;
	
	for(i=0;i<job_num;i++)
		hold += (end_time[i] - arrival_time[i]) - duration[i];

	res = hold/job_num ;
	return res;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	int i;
	float hold=0;
	float res;
	
	for(i=0;i<job_num;i++)
		hold += end_time[i] - arrival_time[i];

	res = hold/job_num ;
	return res;// return 0.0;
}

/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	int i;
	float hold=0;
	float res;
	
	for(i=0;i<job_num;i++){
		hold += first_start[i] - arrival_time[i];
	}
	res = hold/job_num ;
	return res;
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
	free(core_list);
	free(end_time);
	free(first_start);
	free(duration);
	free(arrival_time);
	priqueue_destroy(jobs);
	free(jobs);
	return;
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
	qnode* temp = jobs->head;
	while(temp!=NULL)
	{
		printf("%d ", ((job_t*)temp->elem)->id);
		temp = temp->next;
	} 

}
