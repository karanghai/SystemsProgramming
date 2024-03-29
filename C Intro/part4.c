#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>	//added

/**
 * Returns a newly allocated string on the heap with the value of "Hello".
 * This should not be freed.
 *
 * @returns
 *     A newly allocated string, stored on the heap, with the value "Hello".
 */
char * sixteen()
{
	char *s = malloc(6*sizeof(char));//changed
	strcpy(s, "Hello");
	//printf("%s\n",s);	// added
	
	return s;	//added
}


/**
 * Prints out the radius of a circle, given its diameter.
 *
 * @param d
 *     The diameter of the circle.
 */
void seventeen(const int d)
{
	printf("The radius of the circle is: %f.\n",(float) d/2);
}


/**
 * Manipulates the input parameter (k) and prints the result.
 *
 * @param k
 *     The input parameter to manipulate.
 */
void eighteen( int k)	//removed const before int
{
	k = k * k;
	k += k;
	k *= k;
	k -= 1;

	printf("Result: %d\n", k);
}


/**
 * @brief
 *     Clears the bits in "value" that are set in "flag".
 *
 * This function will apply the following rules to the number stored
 * in the input parameter "value":
 * (1): If a specific bit is set in BOTH "value" and "flag", that
 *      bit is NOT SET in the result.
 * (2): If a specific bit is set ONLY in "value", that bit IS SET
 *      in the result.
 * (3): All other bits are NOT SET in the result.
 *
 * Examples:
 *    clear_bits(value = 0xFF, flag = 0x55): 0xAA
 *    clear_bits(value = 0x00, flag = 0xF0): 0x00
 *    clear_bits(value = 0xAB, flag = 0x00): 0xAB
 *
 * @param value
 *     The numeric value to manipulate.
 *
 * @param flag
 *     The flag (or mask) used in order to clear bits from "value".
 */
long int clear_bits(long int value, long int flag)
{
	
	int i;
	for(i=1;i<256;i=i*2)
	{
				
		if( ((value&i)==(flag&i) && (flag&i)!=0 ) )
			{
				value = (value & (~i));
			}
		else if( (value&i)!=0 && (flag&i)==0 )
			{
				//value remains unchanged			
			}
		
		else
			value = (value & (~i));
	}
	
	
	return value;


}


