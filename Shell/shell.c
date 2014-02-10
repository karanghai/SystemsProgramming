/** @file shell.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "log.h"


log_t Log;

#define  MAX_ARG 64
#define  MAX_CWD_LEN 256


void shell()
{
	log_init(&Log);
	ssize_t line_copy;
	size_t line_size = 0;
	char *line = NULL;
	char *buffer = NULL;
	pid_t pid = getpid();
	int success;

	while(1)
	{
		char * cwd = getcwd(buffer, MAX_CWD_LEN);
		printf("(pid=%i)%s$ ", pid, cwd);
		fflush(stdout);
		free(cwd);
		line = NULL;
		line_copy = getline(&line, &line_size, stdin);
		
		line[strlen(line)-1] = '\0'; //remove trailing newline
		
		if (strncmp(line, "!", 1)==0)
                {
                	if(strncmp(line,"!#",strlen("!#"))!=0){
				char * match = log_search(&Log, line + 1);
				if(match != NULL){
			      		printf("%s matches %s\n", line + 1, match);
					if(strlen(line) < strlen(match))
						line = realloc(line, (strlen(match) + 1)*(sizeof(char*)));
					strcpy( line, match);
					
				
				 }
                	         else{
                	                printf("No Match\n");
					free(line);
					continue;
					
					}
                		}
		}
		if(!strncmp(line,"exit",strlen("exit"))){
			printf("Command executed by pid=%d\n", pid);
			log_push(&Log,line);
			log_destroy(&Log);
			return; // might need to change to return ??
		}
		
		else if (strncmp(line,"cd ",3)==0){	//added a space after cd
			printf("Command executed by pid=%d\n", pid);			
			log_push(&Log,line);
			success = chdir(line+3);
			if(success==-1){
				printf("%s: No such file or directory\n",(line+3));
			}
			
					
		}
		
		else if (!strcmp(line,"!#")){	//gives memory leak
			
			printf("Command executed by pid=%d\n", pid);
			log_print(&Log); 
		
		}
			
				
		else
		{	
			if (strncmp(line, "!", 1)!=0){
				log_push(&Log, line);
				//system(line);
				int status;
				
				char **ap, *argv[MAX_ARG];
				char * input = NULL;
				input = malloc(sizeof(char)*strlen(line));
				//char * input = NULL;
				strcpy(input,line);
           			for (ap = argv; (*ap = strsep(&input, " ")) != NULL;)
                   			if (**ap != '\0')
                           			if (++ap >= &argv[MAX_ARG])
                                   			break;							
				free(input);
				pid_t pid = fork();

				if(pid==0){
					printf("Command executed by pid=%d\n", pid);
					if(execvp(argv[0],argv)==-1)
						printf("%s: not found\n",line);
						
					
					exit(1);
				}

				else
					waitpid(pid, &status, 0);	
		
				
					
			}//if line does not have ! at front
		}//else loop
		
	}//while loop
	free(line);
	free(buffer);
	return;

}
int main()
{
	shell();
	return 0;
}

