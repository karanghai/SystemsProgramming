/** @file msort.c */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int size;
int * res;

typedef struct segment{
	int size_seg;
	int top;
	int top2;
	int bottom;
//	int id;
}segment;


int compare (const void * a, const void * b);
void* sort_help(void * arg);
void* merge(void *args);

int main(int argc, char **argv)
{
	
	int i =0;
//	int max = 200;
//	int * elems = malloc(sizeof(int)*1024);

	
	while(scanf("%d",&i)!=EOF){		//reads stdin into res  TASK1
	size++;
	res =  realloc(res,sizeof(int)*size);
	res[size-1]=i;
	}
	int k;
	
	const char * cmd_line = argv[1];

	int segment_count = atoi(cmd_line);	// segment_count =  number of segements  TASK2
	
	pthread_t * tid = malloc(sizeof(pthread_t) * segment_count);
	pthread_t *tid1 = malloc(sizeof(pthread_t)*segment_count);
	segment * list = malloc(sizeof(segment)* segment_count);
	

	int values_per_segment; /**< Maximum number of values per segment. */
	
	int input_ct = size;						//TASK 3
	if (input_ct % segment_count == 0)
    	values_per_segment = input_ct / segment_count;	
	else
    	values_per_segment = (input_ct / segment_count) + 1;

	//int values_last = size - values_per_segment; 

									//TASK4

	int j=0;
	for(j=0; j<segment_count-1; j++)
	{		
		list[j].top=j*values_per_segment;
		list[j].bottom=(j+1)*values_per_segment-1;
		list[j].size_seg=values_per_segment;
		pthread_create(&tid[j], NULL,sort_help, &list[j]);
	}		
		list[j].top=j*values_per_segment;
		list[j].bottom=size-1;
		list[j].size_seg=list[j].bottom-list[j].top+1;
		pthread_create(&tid[j], NULL, sort_help, &list[j]);

	
	for(j=0; j<segment_count; j++)
		pthread_join(tid[j],NULL);
			
	//	free(tid);
	//	free(list)
								//TASK5
		
	
	//segment * list1 = malloc(sizeof(segment )* segment_count);
			int seg_count1 = segment_count;
			while(seg_count1 > 1)
			{
				
				for(i=1; i<seg_count1; i+=2)
				{
					list[i/2].top=list[i-1].top;
					list[i/2].top2=list[i].top;
					list[i/2].bottom=list[i].bottom;
					list[i/2].size_seg=list[i/2].bottom-list[i/2].top+1;
					pthread_create(&tid1[i/2], NULL, merge, &list[i/2]);
				}
				if(i==seg_count1-1)
				{
					list[i/2].top=list[seg_count1-1].top;
					list[i/2].bottom=list[seg_count1-1].bottom;
					list[i/2].size_seg=list[seg_count1-1].size_seg;
					pthread_create(&tid1[i/2], NULL, merge, &list[i/2]);				
				}


				for(i=0; i<seg_count1/2;i++)
					pthread_join(tid1[i],NULL);

				if(seg_count1%2==0)
					seg_count1/=2;
				else
					seg_count1=(seg_count1 / 2)+1;

				
				//list=realloc(list, sizeof(segment)*segment_count));
			}

	//TASK6
	qsort(&res[0], size , sizeof(int) ,compare);
	for(k = 0; k < size; k++)
		printf("%d\n", res[k]);


	free(list);	
	free(tid);
	free(tid1);
	
	free(res);
	return 0;
}

void* sort_help(void * arg)
{
	segment * t = (segment *) arg;
   	
	qsort(&res[t->top], t->size_seg , sizeof(int) ,compare);
	fprintf(stderr, "Sorted %d elements.\n", t->size_seg);

	return NULL;
}

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

void * merge ( void * args)
{

segment * temp=(segment*)args;
	int  sorted[temp->size_seg];
	int dupes=0,j=0;
	int i=temp->top;
	int k=temp->top2;
	while(i<temp->top2 && k<=temp->bottom)
	{
		if(res[i]==res[k])
		{
			dupes++;
			sorted[j++]=res[i++];
		}
		else if(res[i]<res[k])
			sorted[j++]=res[i++];

		else
			sorted[j++]=res[k++];
	}

	while(i <temp->top2)
		sorted[j++]=res[i++];

	
	while(k<=temp->bottom)
		sorted[j++]=res[k++];


	for(j=0; j<temp->size_seg; j++)
	{
		res[temp->top+j]=sorted[j];
	}
	
	
	fprintf(stderr, "Merged %d and %d elements with %d duplicates.\n",temp->top2 -temp->top ,temp->bottom - temp->top2 +1 , dupes);
	return NULL;

}





