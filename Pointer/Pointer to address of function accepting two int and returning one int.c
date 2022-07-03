/* This code is use for Pointer to address of function accepting two int and returning an int */

#include <stdio.h>
#include <stdlib.h>

int my_add (int num1 ,int num2);

int main(void)
{
	int num1 = 10;
	int num2 = 20;
	int ret;
	
	int (*pf)(int, int);
	
	pf = my_add;
	
	ret = pf(num1, num2);
	
	printf ("num1 : %d\nnum2 : %d\nnum3 : %d\n", num1, num2, ret);
	
	exit (EXIT_SUCCESS);
}

int my_add (int num1, int num2)
{
	return (num1 + num2);
}
