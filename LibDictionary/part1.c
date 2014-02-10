/** @file part1.c */

/*
 * Machine Problem #1
 * CS 241 Fall2013
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp1-functions.h"

/**
 * (Edit this function to print out the ten "Illinois" lines in mp1-functions.c in order.)
 */
int main()
{

//first_step
first_step(81);



//second_step
int * value = malloc(sizeof(int));	
*value = 132;
second_step(value);	
free(value);


//double_step
int ** value2 = malloc(sizeof(int*));	
*value2 = malloc(sizeof(int));
*value2[0]=8942;
double_step(value2);
free(*value2);
free(value2);

//strange_step
int * value3 ;//= malloc(sizeof(int));	/
value3 = 0;
strange_step(value3);
free(value3);

//empty_step
int temp =1;
void * value4 = &temp;
empty_step(value4);
//free(value4);//newly added



//two_step
char* s2 = malloc(sizeof(char)*4);
s2[3]='u';
void * s ;//= malloc(sizeof(char));
s=s2;
two_step(s,s2);
free(s2);	//maybe just need to free once


//three_step
char * first = malloc(sizeof(char));
char * second ;
char * third ;
second = first +2;
third = second +2 ;
three_step(first,second,third);
free(first);//correct
//free(third);

//step_step_step
//char * first , * second, * third;//uncommented
first = malloc(sizeof(char)*2);
second = malloc(sizeof(char)*3);
third = malloc (sizeof(char)*4);
second[2] = first[1]+8;
third[3] = second[2]+ 8 ;
free(first);//correct
free(second);
free(third);
//it_may_be_odd
char * a = malloc(sizeof(int));
*a = 1 ;
it_may_be_odd(a,1);
free(a);
//the_end
temp=2049;//defined temp earlier
void * orange = &temp;
void * blue = &temp;

the_end(orange,blue);

return 0;
}
