#include <stdio.h>
#include <stdlib.h>

/**
 * Returns a new pointer (float *p) which contains the value of the
 * input pointer (int *x).
 *
 * @param x
 *     Input parameter, whose value will be returned as a (float *).
 *
 * @returns
 *     A new pointer, allocated on the heap and not freed, that
 *     contains the value of the input parameter.
 */
float * six(const int *x)	//fixed
{
	//float *p = *x;
	float *p = malloc(sizeof(int));
	*p = *x;
	return p;
}


/**
 * Takes two inputs, a and b such that (a < b), and makes a and b equal
 * via a while-loop.  Prints out their value before returning.
 * 
 * @param a
 *     Input parameter a, where a < b.
 *
 * @param b
 *     Input parameter b, where b > a.
 */
void seven(int a, const int b)	//fixed, might need to implement call by pointers
{
	while (a != b)
	{
		a++;
	}

	printf("%d is now equal to %d\n", a, b);
}


/**
 * Constructs a C-string, character by character, and prints out the full
 * string "Hello".
 */
void eight()
{
	char *s = malloc(6*sizeof(char));//changed, might not have been necessary

	s[0] = 'H';
	s[1] = 'e';
	s[2] = 'l';
	s[3] = 'l';
	s[4] = 'o';
	s[5] = '\0';
	printf("%s\n", s);

	free(s);
}


/**
 * Assigns a pointer (float *p) a numeric value (12.5).
 */
void nine()
{
	float *p = malloc(sizeof(float)); //changed
	*p = 12.5;

	printf("The value of p is: %f\n", *p);

	free(p); //added
}


/**
 * Reset the value of x to zero.
 *
 * @param x
 *     Pointer to reset to 0.
 */
void ten(int *x)
{
	*x = 0;
}
