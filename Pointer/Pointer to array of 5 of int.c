/* This Code is for use of pointer to address of Array of 5 element of type interger */

#include <stdio.h>
#include <stdlib.h>

int (*pa)[5];

int a[5] = {10,20,30,40,50};

int main (void)
{
	int i, m;
	pa = &a;
	
	for (i = 0; i < 5; i++)
	{
		m = (*pa)[i];
		printf (" m : %d\n", m);
	}
	
	exit (EXIT_SUCCESS);
}
