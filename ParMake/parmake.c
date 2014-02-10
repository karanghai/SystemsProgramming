/** @file parmake.c */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
/**
 * Entry point to parmake.
 */
#include "parser.h"
#include "queue.h"
#include "rule.h"
#include <string.h>
#include <semaphore.h>

queue_t q;
queue_t has_ran;
queue_t depend;

int remain_size;

//sumbmission 10 53 pm

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t  condition_var1   = PTHREAD_COND_INITIALIZER;
pthread_cond_t  condition_var2   = PTHREAD_COND_INITIALIZER;


sem_t sem_mutex1;


int in_depend(char * param)
{
    int i;
    for(i=0;i<queue_size(&depend);i++)
    {
        if(strcmp(param,(char*)queue_at(&depend,i))==0)
            return 1;
    }
    return 0;
}


void remove_dep (char * param2)
{
    //printf("Enters\n");
    int i,j,k=0;
    char * hold2 = NULL;
    rule_t * param1 = NULL;
    for(j=0;j<queue_size(&q);j++)
    {
        param1 = queue_at(&q,j);                  
        for(i=0;i<queue_size(param1->deps);i++){
            hold2 = queue_at(param1->deps,i);
            if(strcmp(hold2,param2)==0){
                queue_remove_at(param1->deps,i);
        if(has_deps(param1)==0){
            sem_post(&sem_mutex1);
            k++;
        }
            }
        }
    }
  //  printf("Jobs created %d",k);
    return;

}

int is_dep(char * d1 )
{
    //printf("Hello4\n");
    int i;
    int is_dep =0;
    char * hold2 = NULL;
    for(i=0;i<queue_size(&depend);i++){
        hold2 = queue_at(&depend,i);
        if(strcmp(hold2,d1)==0){
            is_dep=1;
            break;
        }
    }
   // printf("Hello5 is it a dep? %d\n",is_dep);
    return is_dep;
}  
  


void add_deps()
{
    int i,j;
    rule_t * hold1 = NULL;
    char * hold2 = NULL;
    for(i=0;i<queue_size(&q);i++){
        hold1 = queue_at(&q,i);
        for(j=0;j<queue_size(hold1->deps);j++){
            hold2 = queue_at(hold1->deps,j);
            queue_enqueue(&depend,hold2);
        }
    }
      
}


int check_has_ran (char * r1)
{
    int i;
    int ret = 0;
    char* hold2 = NULL;
    for(i=0;i<queue_size(&has_ran);i++){
        hold2 = queue_at(&has_ran,i);
        if(strcmp(hold2,r1)==0){
            ret =1;
            break;
        }
    }

    return ret;


}

int has_deps (rule_t * param)
{

    int has_deps=1;
    if(queue_size(param->deps)==0)
        has_deps = 0;
 
    return has_deps;

}

int is_rule (void * param)
{
    int i;
    int is_rule = 0;
    rule_t * hold1 = NULL;
    char * hold2 = param ;
    for(i=0;i<queue_size(&q);i++){
        hold1 = queue_at(&q,i);
        if(strcmp(hold1->target,hold2)==0){
            is_rule = 1;
            break;
        }
    }

    return is_rule;    //returns 1 if name is a rule
  
}


rule_t *  match_rule (char * param)
{

    int i;
    rule_t * hold1 = NULL;
    char * hold2 = param ;
    for(i=0;i<queue_size(&q);i++){
        hold1 = queue_at(&q,i);
        if(strcmp(hold1->target,hold2)==0){
            return hold1;
            break;
        }
    }

    return NULL;    //returns 1 if name is a rule

}

int is_file (void * param)
{

    int is_file =0;
    char * hold2 = param;
    if(access(hold2,F_OK)==0)
        is_file = 1;
 
    return is_file;    //returns 1 if file
}

int all_file (rule_t * param)
{
    int i;
    int all_file=1;
    char * hold2 = NULL;
    for(i=0;i<queue_size(param->deps);i++){
        hold2 = queue_at(param->deps,i);
        if(is_file(hold2)==0){
            all_file = 0;
            break;
        }
     
    }

    return all_file;

}



int check_times (char * param1 , queue_t * param2)
{
    int i;

    int time=1;
    struct stat s1, s2;
    char * hold2 = NULL;     
    stat(param1, &s1);
    for(i=0;i<queue_size(param2);i++){
        hold2 = queue_at (param2,i);
        stat(hold2 ,&s2);
        if( difftime(s1.st_mtime,s2.st_mtime) < 0){    //s1 came first
            time =0;
            break;
        }
    }

    return time;

}

void* start_run ( void * arg){

    arg = NULL; //no use 
    int i,j;
    int exit_while=0;
    rule_t * hold1 = NULL;
    char* hold2 = NULL;
    int size;   

    while(remain_size!=0){
   
   
    //printf("Entering thread\n");
    pthread_mutex_lock(&mutex1);
            for(i=0;i<queue_size(&q);i++){            //iterates throught the queue of rules
           
             //LOCK
         hold1 = queue_at(&q,i);
      //   printf("holding %s with deps :",hold1->target);   
     //    for(j=0;j<queue_size(hold1->deps);j++)
    //        printf(" %s ",(char*)queue_at(hold1->deps,j));
    //     printf("\n");   
          

        if(check_has_ran(hold1->target)==0 && ( queue_size(hold1->deps)==0 || all_file(hold1)==1)){    //checks if rule hasn't already been run and has no dependency
        //pthread_mutex_unlock(&mutex1);
        //run_order(hold1);        //run
        queue_enqueue(&has_ran,hold1->target);
        break;
            }     //UNLOCK        
        }//for
    pthread_mutex_unlock(&mutex1);
    if (i!= queue_size(&q)) {
        run_order(hold1);
        //condition broadcast
        pthread_mutex_lock(&mutex3);
        pthread_cond_broadcast( &condition_var1);
        pthread_mutex_unlock(&mutex3);
    } else {
        //condition wait
	if(remain_size==0) return NULL;
        pthread_mutex_lock(&mutex3);
        pthread_cond_wait( &condition_var1, &mutex3 );
        pthread_mutex_unlock(&mutex3);
    }

        //sem_wait(&sem_mutex1);
   
    }//while

    return arg;
}

void run_order (rule_t * rule_x )
{
  
    if(queue_size(rule_x->deps)==0){            // run only when no dependency
        run_rule(rule_x);
        return;
    }

  

    int res,res1,res2;
  
    res=all_file(rule_x);
        if(res==1){    //all dependencies are files
            res1 = is_file(rule_x->target); 
            if(res1==0){    // name of rule doesn't exist as file              
                
        run_rule(rule_x);
        
         }
      
            else if (res1==1){ //name of rule exists as file on disk
  
            res2=check_times(rule_x->target , rule_x->deps ); //checks if time of rule is newer than all deps     
                if(res2==1){ //if rule has newer mod time than any dep  
            if(is_dep(rule_x->target)==1){
         pthread_mutex_lock(&mutex2);    //LOCK
                 queue_enqueue(&has_ran,rule_x->target);          
                 remove_dep(rule_x->target);
         remain_size--;            
         pthread_mutex_unlock(&mutex2);    //UNLOCK
            }        
        }
              else{
            run_rule(rule_x);                  
         }
      }//else-if

    }//if



}

void run_rule (rule_t * param)
{
  

  
    int i;
    int res;
    char * hold2 = NULL; 
    for(i=0;i<queue_size(param->commands);i++){
        hold2 = queue_at(param->commands,i);     
        res = system(hold2);
        if(res!=0){
        //printf("\nexiting");       
       exit(1);
    }
    }

    pthread_mutex_lock(&mutex2);    //LOCK
   // printf("Hello1\n");   
   
  //  printf("Hello2\n");       
    if(is_dep(param->target)==1)
    {
 //   printf("reached here\n");   
    remove_dep(param->target);
  
    }
  //  printf("Hello3\n");   
    remain_size--;
    pthread_mutex_unlock(&mutex2);    //UNLOCK   
      


}


void parsed_new_target ( char *target )    /**PART2**/
{
    rule_t * q1 = malloc(sizeof(rule_t));
    rule_init(q1);
    q1->target = malloc( sizeof(char)* (strlen(target) + 1) );
    strcpy(q1->target, target);
    queue_enqueue(&q,q1);
    remain_size++;   
}

void parsed_new_dependency( char *target, char *dependency )    /**PART2**/
{ 
    int i=0;
    rule_t * q1 = queue_at(&q,i);
    for(i=0;i<queue_size(&q);i++){
        q1 = queue_at(&q,i);
        if(strcmp(q1->target,target) == 0)
            break;
    }
 
    char * dependency1 = malloc(sizeof(char)* (strlen(dependency)+1) );
    strcpy (dependency1, dependency); 
    queue_enqueue(q1->deps , dependency1 );

}

void parsed_new_command( char *target, char *command)    /**PART2**/
{
    int i=0;
    rule_t * q1 = queue_at(&q,i) ;
    for(i=0;i<queue_size(&q);i++){
        q1 = queue_at(&q,i);
        if(strcmp(q1->target,target) == 0)
            break;
    }
 
    char * command1 = malloc(sizeof(char)* (strlen(command)+1) );
    strcpy (command1, command); 
    queue_enqueue(q1->commands , command1 );

}


int main(int argc, char **argv)
{

    remain_size =0;

                /** PART 1 STARTS **/
    int opt;
    char *makefile= NULL;
    int num_threads=1; 
    while ((opt = getopt(argc, argv, "f:j:")) != -1) {
        switch(opt){
            case 'f' : makefile = optarg;
                break;
            case 'j' : num_threads = atoi( optarg );
                break;
            default  : return 0;
        }
    }

    int i ;
    int ind;
    int num = 0;
    char ** targets = malloc( (argc - optind +1 ) * sizeof( char* ) ); //not argc - optind + 1     
    if ( optind <= argc ){
        for( i = 0,ind = optind ; i< argc -optind; i++ , ind++ ){
            targets[i] = malloc( 100 ) ;
            strcpy ( targets[i] , argv[ind] );
            num++;
        }
        targets[num] = NULL;
    }
     
    char * buffer = NULL;
 
    if( makefile == NULL ){
        if (access("./makefile",F_OK) == 0 ){
            strcpy(buffer, "./makefile");
            makefile = buffer;
        }
     
        else if ( access("./Makefile",F_OK) == 0) {
            strcpy(buffer, "./Makefile");
            makefile = buffer;
        }
     
        else return -1;
    } 
    else{
        if(access(makefile,F_OK) == -1)
            return -1;
    }
 
                    /** PART 1 ENDS **/
 
                    /** PART 2 STARTS **/

    queue_init(&q);
    parser_parse_makefile( makefile , targets , parsed_new_target, parsed_new_dependency, parsed_new_command );
  

                    /** PART 2 ENDS **/

                    /** PART 3 STARTS **/
 /*   int m,n;
    rule_t * hold3;
    char * hold4,hold5;
    printf("printing the queue of rules\n");
    for(m=0;m<queue_size(&q);m++){
        hold3 = queue_at (&q,m);
        printf("target %s : deps ",hold3->target);
        for(n=0;n<queue_size(hold3->deps);n++){
            hold4 = queue_at(hold3->deps,n);          
            printf ("%s ",hold4);
        }
       printf("commands ");
        for(n=0;n<queue_size(hold3->commands);n++){
            hold4 = queue_at(hold3->commands,n);          
            printf ("%s ",hold4);
        }
        printf("\n");
    }

 exit(1);*/
      queue_init(&has_ran);
          queue_init(&depend);
      add_deps();
      int no_dep=0;     
      for(i=0;i<queue_size(&q);i++)
        if(has_deps((rule_t*)queue_at(&q,i))==0)
            no_dep++;

    //printf("NUMBER of rules with no deps, initialy %d\n",no_dep);
        /** PARALELLIZATION **/

    pthread_t* threads = malloc(sizeof(pthread_t)*num_threads);
   
    sem_init(&sem_mutex1, 0, no_dep);
   
   
    for(i=0;i<num_threads;i++)
        {   
               pthread_create(&threads[i],NULL,start_run,NULL);
        }
   
        for(i=0;i<num_threads;i++)
        {
                pthread_join(threads[i],NULL);
        }

    sem_destroy(&sem_mutex1);






        /** FREEING STUFF **/
     
    free(threads);
  
  
  

 //   start_run(NULL);

  
    rule_t * rule_hold = NULL;
    int k;
    char * hold2 = NULL;
    for(i=0;i<queue_size(&q);i++){
        rule_hold = queue_at(&q,i);
        for(k=0;k<queue_size(rule_hold->deps);k++)
        {
            hold2 = queue_at(rule_hold->deps,k);
            if(in_depend(hold2)==0)
        free(hold2);

        }
        for(k=0;k<queue_size(rule_hold->commands);k++)
        {
            hold2 = queue_at(rule_hold->commands,k);
            free(hold2);

        }
        free(rule_hold->target);
        rule_destroy(rule_hold);
        free(rule_hold)    ;
    }
    queue_destroy(&q);


    for(i=0;i<queue_size(&depend);i++)
    {  
        hold2 = queue_at(&depend,i);

        free(hold2);
    }
    queue_destroy(&depend);

  
    queue_destroy(&has_ran);  
  
    for( i = 0; i< argc - optind; i++ ){
            free(targets[i]);
    
        }  
      
    free(targets);

  


    return 0;  
}
/** END **/
