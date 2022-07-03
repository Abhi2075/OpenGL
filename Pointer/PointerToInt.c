/* This code is for use Poiter to address of int */

#include <stdio.h>
#include <stdlib.h>

int *p = NULL;
int n = 100;

int main(void)
{
	p = &n;
	
	printf ("*p : %d\n", *p);
	
	*p = 500;
	printf ("*p : %d\n", *p);
	
	exit (EXIT_SUCCESS);
}

