#ifndef _TEST_FRAMEWORK_H_
#define _TEST_FRAMEWORK_H_

#include <stdio.h>

static void print_result(char* part, int isPass, int line)
{
	if(isPass) 
		printf("Test-%-32s\033[0;34m\t%-16s\033[0m[Line:%-3d]\n", part,"Pass!",line); 
	else 
		printf("Test-%-32s\033[0;31m\t%-16s\033[0m[Line:%-3d]\n", part,"Failure!",line); 
}

static void print_output_border(char*prog, int isBegin)
{
	if(isBegin)
		printf("----------Test %-16s----------\n", prog);
	else
		printf("----------Test %-16s----------\n", "Done"); 
}	

#endif