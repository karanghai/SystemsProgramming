#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Concatenates the strings "Hello " and "World!" together, and
 * prints the concatenated string.
 */
void eleven()	//maybe fixed
{
	//char *s = "Hello ";
	char * s = malloc(13*sizeof(char));
	char * h = "Hello ";
	strcpy(s,h);
	strcat(s, "World!");
	printf("%s\n", s);

	free(s);	//free
}


/**
 * Creates an array of values containing the values {0.0, 0.1, ..., 0.9}.
 */
void twelve()	//maybe fixed
{
	float *values = malloc(sizeof(float)*10);

	int i, n = 10;
	for (i = 0; i < n; i++)
		values[i] = (float)i / n;

	for (i = 0; i < n; i++)
		printf("%f ", values[i]);
	printf("\n");
	
	free(values); //free
}


/**
 * Creates a 2D array of values and prints out the values on the diagonal.
 */
void thirteen()
{
	int **values;

	int i, j;
	values = malloc(10 * sizeof(int *));
	for (i = 0; i < 10; i++)
	{	values[i]= malloc(10*sizeof(int));	// added		
		for (j = 0; j < 10; j++)
			values[i][j] = i * j;
	}
	for (i = 0; i < 10; i++)
		printf("%d ", values[i][i]);
	printf("\n");

	for(i=0;i<10;i++)	//added
	free(values[i]);

	free(values);		//added
}


/**
 * Prints out a specific string based on the input parameter (s).
 *
 * @param s
 *     Input parameter, used to determine which string is printed.
 */
void fourteen(const char *s)
{
/*	switch (*s)		//changed
	{
		case "blue":
			printf("Orange and BLUE!\n");
			break;

		case "orange":
			printf("ORANGE and blue!\n");
			break;

		default:
			printf("orange and blue!\n");
			break;
	}*/
	if(!strcmp(s,"blue"))
		printf("Orange and BLUE!\n");

	else if(!strcmp(s,"orange"))
		printf("ORANGE and blue!\n");
	else 
		printf("orange and blue!\n");	
}


/**
 * Prints out a specific string based on the input parameter (value).
 *
 * @param value
 *     Input parameter, used to determine which string is printed.
 */
void fifteen(const int value)	//dont know
{
	/*switch (value)
	{
		case 1:
			printf("You passed in the value of one!\n");

		case 2:
			printf("You passed in the value of two!\n");

		default:
			printf("You passed in some other value!\n");
	}*/
	if(value==1)
		printf("You passed in the value of one!\n");
	
	else if(value==2)
		printf("You passed in the value of two!\n");

	else				
		printf("You passed in some other value!\n");




}
