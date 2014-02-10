/** @file server.c */  
  
#define _GNU_SOURCE  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <pthread.h>  
#include <netdb.h>  
#include <unistd.h>  
#include <signal.h>  
#include <queue.h>  
#include <assert.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
  
#include "queue.h"  
#include "libhttp.h"  
#include "libdictionary.h"  
  
const char *HTTP_404_CONTENT = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>The requested resource could not be found but may be available again in the future.<div style=\"color: #eeeeee; font-size: 8pt;\">Actually, it probably won't ever be available unless this is showing up because of a bug in your program. :(</div></html>";  
const char *HTTP_501_CONTENT = "<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1>The server either does not recognise the request method, or it lacks the ability to fulfill the request.</body></html>";  
  
const char *HTTP_200_STRING = "OK";  
const char *HTTP_404_STRING = "Not Found";  
const char *HTTP_501_STRING = "Not Implemented";  
  
/**  
 * Processes the request line of the HTTP header.  
 *   
 * @param request The request line of the HTTP header.  This should be  
 *                the first line of an HTTP request header and must  
 *                NOT include the HTTP line terminator ("\r\n").  
 *  
 * @return The filename of the requested document or NULL if the  
 *         request is not supported by the server.  If a filename  
 *         is returned, the string must be free'd by a call to free().  
 */ 
 
int *  client_fd1 ; 
pthread_t * threads;
void signal_callback_handler(); // defining header
int num_threads=0;
int sock_fd;

  
char* process_http_header_request(const char *request)  
{  
    // Ensure our request type is correct...  
    if (strncmp(request, "GET ", 4) != 0)  
        return NULL;  
  
    // Ensure the function was called properly...  
    assert( strstr(request, "\r") == NULL );  
    assert( strstr(request, "\n") == NULL );  
  
    // Find the length, minus "GET "(4) and " HTTP/1.1"(9)...  
    int len = strlen(request) - 4 - 9;  
  
    // Copy the filename portion to our new string...  
    char *filename = malloc(len + 1);  
    strncpy(filename, request + 4, len);  
    filename[len] = '\0';  
  
    // Prevent a directory attack...  
    //  (You don't want someone to go to http://server:1234/../server.c to view your source code.)  
    if (strstr(filename, ".."))  
    {  
        free(filename);  
        return NULL;  
    }  
  
    return filename;  
}  
  



    /* PART 3 */  
  
  
void * worker_thread ( void * clientfd )  
{  
    while(1) {        //3f      
          
 

    int client_fd = (int) clientfd;   
      
    /* Part 3 a */          
    
    http_t buff; 
    int check_read = http_read ( &buff , client_fd); 
    if (check_read==-1) break;
  
    /* Part 3 b */  
          
     
    const char * temp = http_get_status(&buff); 
    char * resp1 = process_http_header_request( temp );  //gets me a SEG_FAULT !!!! ++> Solved with checking read
    /* FREE STUFF */
    //free ( temp );
    /* Part 3 c */ 
  
    int response_code;  
    
    FILE * file_res;  
    
    if ( resp1 == NULL ) 
    {   
        response_code = 501;  
    } 
     
    else if ( resp1 != NULL)  
    {  
        response_code = 200 ; 
     
    	char * web_insert = malloc(1000); 
    	strcpy(web_insert,"web");   
    	strcat(web_insert,resp1); 
    	free(resp1); 
    	resp1=malloc(1000); 
    	strcpy(resp1 ,web_insert); 
    	/*  FREE STUFF */
	free(web_insert);

    	if(strcmp(resp1,"web/")==0) 
    	    strcat(resp1,"index.html"); 
 
    	file_res = fopen(resp1,"r"); 
 
    	if(file_res==NULL) // if no such file exist on the hard disk 
    	    response_code = 404; 
    }  
  
    /* Part 3 d */  
    char * type = malloc(sizeof(char) * 500);  
 
    if(response_code == 404 || response_code == 501) 
        strcpy(type , "text/html"); 
    else if ( response_code == 200 ) 
    { 
        if ( strstr( resp1 , ".html" )!= NULL )  
            strcpy(type , "text/html");  
        else if ( strstr( resp1 , ".css"  )!= NULL )  
            strcpy(type , "text/css");  
        else if ( strstr( resp1 , ".jpg" )!= NULL )  
            strcpy(type , "image/jpeg");  
        else if ( strstr( resp1 , ".png"  )!= NULL )  
            strcpy(type , "image/png");  
        else  
            strcpy(type , "text/plain");  
     } 
      
    /* Part 3 e */  
  
    char * h_buffer = malloc(sizeof(char) * 100000); //sprintf  
    char * response_code_string = malloc(sizeof(char)*100);  
      
    if(response_code == 501)  
        strcpy(response_code_string ,HTTP_501_STRING);  
    else if(response_code == 404)  
        strcpy(response_code_string ,HTTP_404_STRING);  
    else if(response_code == 200)  
        strcpy(response_code_string ,HTTP_200_STRING);  
          
    sprintf ( h_buffer , "HTTP/1.1 %d %s\r\n", response_code , response_code_string );    //HTTP/1.1 200 OK\r\n  
   
    sprintf ( h_buffer + strlen(h_buffer) , "Content-Type: %s\r\n",type );    //Content-Type: text/html\r\n  
  
    int h_length = 0;  
    char * file_read; 
    int result; 
 
    if(response_code == 501)  
        h_length = strlen(HTTP_501_CONTENT);  
    
    else if( response_code == 404 )  
        h_length = strlen(HTTP_404_CONTENT);  
    
    else if ( response_code == 200) //resp1 can't be null 
    {          
    fseek(file_res, 0, SEEK_END);    // file_res was assigned when the file was opened  
        h_length = ftell(file_res);  
        rewind(file_res); 
    file_read = malloc(sizeof(char) * h_length); 
    //if (file_read == NULL) {fputs ("Memory error",stderr); exit (2);} 
    result = fread (file_read,1,h_length,file_res);
    h_length = result ; // this should be a given 
      //if (result != h_length) {fputs ("Reading error",stderr); exit (3);}
     
    fclose(file_res);
    // CLOSE TAKES CARE OF DISASSOCIATING MEMORY 
     
    }  
  
    sprintf ( h_buffer+ strlen(h_buffer), "Content-Length: %d\r\n", h_length );    //Content-Length: 12\r\n  
  
    char * connection_type ;//= malloc(sizeof(char)*500); 
    connection_type = http_get_header( &buff , "Connection" ); // gets value related to key : Connection 
    

     
    if(connection_type != NULL && strcasecmp(connection_type,"Keep-Alive")==0)  
    	sprintf ( h_buffer+ strlen(h_buffer) ,"Connection: %s\r\n", connection_type );    //Connection: Keep-Alive\r\n  
        
    else if (connection_type != NULL || strcasecmp(connection_type,"Keep-Alive")!=0) 
        { 
        	strcpy(connection_type,"close"); 
        	sprintf ( h_buffer+ strlen(h_buffer),"Connection: %s\r\n", connection_type ); 
        } 

	
  
 
   sprintf(h_buffer+ strlen(h_buffer), "\r\n");    //\r\n 
 
     
   send(client_fd, h_buffer ,strlen(h_buffer), 0);         
   
   if(response_code == 200)
   { 
   	send(client_fd, file_read ,result, 0);    //Hello World! // result equals h_length  
   	free ( file_read );	
   }
   else if ( response_code == 501 )
           send(client_fd, HTTP_501_CONTENT ,strlen (HTTP_501_CONTENT) , 0);     
  
   else if ( response_code == 404 )
        send ( client_fd , HTTP_404_CONTENT , strlen (HTTP_404_CONTENT) , 0 ); 
 
 

   /** Free stuff **/

   	 
     free ( type );
     free ( h_buffer );
     free ( response_code_string );
     if(response_code != 501)
   	free ( resp1 ) ;


    /* Part 3 f */  
  
   if ( strcasecmp(connection_type,"Keep-Alive") !=0 ) { 
	
	
	http_free(&buff);
        break;
    }

   http_free(&buff);
  	




   } // 3f     
  

    return NULL;  
   
}  



   





  
  
int main(int argc, char **argv)  
{  


    int check;  
      
    /* PART 1 */  
    
    //socket  
    sock_fd = socket(AF_INET , SOCK_STREAM , 0);  //make global and call  close (sock_fd)
    //getaddrinfo  
    struct addrinfo hints , * result;  
    memset ( &hints , 0x00,sizeof(struct addrinfo) );  
    hints.ai_family = AF_INET;  
    hints.ai_socktype = SOCK_STREAM;  
    //in server  
    hints.ai_flags = AI_PASSIVE;  
    getaddrinfo(NULL , argv[1] , &hints , &result); 
   
    check =bind(sock_fd , result->ai_addr , result->ai_addrlen);//need to check for failiure  
    if(check==-1) 
    {      
            exit(1);  
     
    } 
 
    listen(sock_fd , 10);  

    free( result );
      
    /* PART 2 */  
   // int num_threads=0; 
    


    client_fd1 = malloc ( sizeof(int) * 5000 );
    int i = 0;    
    for(i=0;i<5000;i++)
	client_fd1[i] = -1; 
    
    threads = malloc(sizeof(pthread_t) * 5000);


	    /* PART 4 SIGNAL */
    signal(SIGINT, signal_callback_handler); 
   
    while(1)  
    {  
 	
        client_fd1[num_threads] = accept(sock_fd , NULL , NULL );  
	//fprintf(stderr, "CONNECTION!");
        if(client_fd1[num_threads]==-1) 
            break; 
        else 
	{ 
            pthread_create( &threads[num_threads] , NULL , worker_thread, client_fd1[num_threads] );              
	}
       // printf("Number of threads in while loop %d\n", num_threads);
        num_threads++;
      
	 
      
    } 
   
    return 0;  
}

void signal_callback_handler()
{
    /* PART 4 FREE STUFF */
   // printf( "num_threads == %d \n" , num_threads);
  //  printf ( " JOINING THREADS \n" );;


    close ( sock_fd );
    int i;
    for(i=0;i<5000;i++)
	if(client_fd1[i]!=-1)
		close(client_fd1[i]);
   // printf ( "EXITING THROUGH CTRL + C\n" ); 
    free(client_fd1);
    int j; 
    for(j=0; j< num_threads ; j++ )
	{     //  printf("joining..\n"); 
        	pthread_join (threads[j], NULL ) ;  
    	}
  //  printf ( " JOINED THREADS \n" );;
     free(threads);
  
 
    exit(0);
}



