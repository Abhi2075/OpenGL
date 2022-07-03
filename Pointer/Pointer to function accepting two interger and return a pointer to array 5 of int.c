/* This code is use for Pointer to function accepting two int and return a pointer to array 5 of int */

#include <stdio.h>
#include <stdlib.h>

/* 
	my_function is a function
	my_function()
	
	my_function is a function accepting two int
	my_function (int, int)
	
	my_function is a function accepting two int and return a pointer
	*my_function (int, int)
	
	my_function is a function acceptiong two int and return a pointer to address of array
	*my_function (int,int)[5]
	
	my_function is a function accepting two int and return a pointer to address of array of 5 elemtnt of type int
	int *my_function (int,int) [5]
*/

/*
	pfn is a pointer
	*pfn
	
	pfn is a pointer to a function
	(*pfn)()
	
	pfn is a pointer to a function accepting two int
	(*pfn)(int, int)
	
	pfn is a pointer to a function accepting two int and return a pointer
	*(*pfn)(int, int)
	
	pfn is a pointer to a function accepting two int and return a pointer to array of 5 element
	(*(*pfn)(int, int))[5]
	
	pfn is a pointer to a function accepting tw0 int and return a pointer to array of 5 element of type of int
	int (*(*pfn)(int, int))[5]
*/

/* Function declation */
int (*my_function (int, int))[5];

/* Pointer to a function declaration */
int (*(*pfn)(int,int))[5];

int main(void)
{
	pfn = my_function;
	int (*pa)[5];
	int i;
	int m;
	
	pa = pfn(12,4);
	
	for (i = 0; i < 5; i++)
	{
		m = (*pa)[i];
		printf ("member of a array : %d\n",m);
	}
	
	return (EXIT_SUCCESS);
}

int (*my_function(int a, int b))[5]
{
	 static int A[5];
	
	A[0] = a + b;
	A[1] = a - b;
	A[2] = a * b;
	A[3] = a / b;
	A[4] = a * a;
	
	return (&A);
}

