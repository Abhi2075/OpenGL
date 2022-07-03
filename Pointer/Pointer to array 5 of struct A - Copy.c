/* This code is use for Pointer to address of array of 5 element of type of struct A */

#include <stdio.h>
#include <stdlib.h>

struct A
{
	int a;
	char b;
	float c;
};

struct A arr[5] = 	{
						{30,'a',3.14f},
						{20, 'b',3.15f},
						{10, 'c',3.16f}
					};
					
struct A *p = NULL;

int main(void)
{
	int i;
	int copy_a;
	char copy_b;
	float copy_c;
	
	p = arr;
	
	for (i = 0; i < 5; i++)
	{
		copy_a = p[i].a;
		copy_b = p[i].b;
		copy_c = p[i].c;
		
		printf ("Index : %d\n copy_a : %d\n, copy_b : %c\n copy_c : %f\n", i, copy_a, copy_b, copy_c);
		
		puts ("--------------------------------------");
	}
	
	exit (EXIT_SUCCESS);
}
