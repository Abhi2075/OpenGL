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
						{10, 'a', 3.14f},
						{20, 'b', 3.15f},
						{30, 'c', 3.16f}
					};

struct A (*pa)[5];

int main(void)
{
	int i;
	int copy_a;
	char copy_b;
	float copy_c;
	
	pa = &arr;
	
	for (i = 0; i < 5; i++)
	{
		copy_a = (*pa)[i].a;
		copy_b = (*pa)[i].b;
		copy_c = (*pa)[i].c;
		
		printf ("index : %d\n copy_a : %d\n copy_b : %c\n copy_c : %f\n", i, copy_a, copy_b, copy_c);
		puts ("--------------------------------------");
	}
	
	exit(EXIT_SUCCESS);
}