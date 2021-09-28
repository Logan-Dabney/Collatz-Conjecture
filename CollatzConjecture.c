//==============================================================================================
// project1.c - Collatz Conjecture implementation with fork() and pipe()
//
// This program implements Collatz conjecture using fork to create a parent and child process. 
// The child process will do the conjecture and the parent will wait for the child to finish
// then the parent will print the results to the console.
// 
// Author: 	Logan Dabney, University of Toledo
// Date:	Feb 23, 2021
// Copyright:	Copyright 2020 by Logan Dabney. All rights reserved.
//==============================================================================================

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER 100000

int isNumeric(char* str)
{
	for (int i = 0; i < strlen(str); i++)
	{
		if(str[i] < '0' || str[i] > '9') return 0;
	}
	return 1;
}

int main(int argc, char* argv[])
{
	if (argc == 2 && isNumeric(argv[1]) == 1)	// If the user only enters 1 # and is a 
	{						// valid number
		int p[2];
		pid_t pid;
	
		if(pipe(p) < 0)				// If the pipe isn't created send error
		{
			fprintf(stderr, "Pipe Failed");
			return 1;
		}

		pid = fork();				// fork into child and parent
		if (pid < 0) 				// if fork fails send error
		{
			fprintf(stderr, "Fork Failed");
			return 1;
		}
		else if (pid == 0)	// child process
		{
			unsigned long long val = strtoull(argv[1], NULL, 10);// get the value 
			unsigned long long peak = val;	// peak value is starting val
			int  counter;			// count the interations
			char msg[BUFFER] = "";		// message for parent

			sprintf(msg, "%llu", val);	// put val into msg
			write(p[1], msg, strlen(msg));
			close(p[0]);
			
			if(val != 1)			// val can't be 1, no steps to 1 from 1
			{	
				while (val != 1)	// while val isn't 1 loop
				{
					if((val%2) == 0)// if val is even divide by 2
					   val = val / 2;
					else		// if odd multiple by 3 and add 1 
					   val = (val*3) + 1;

					if (val > peak) peak = val;	
					sprintf(msg, ", %llu", val);	// send the val thru p
					write(p[1], msg, strlen(msg)); 
					counter++;			// add to counter
				}
									// create a temp message									   from counter and peak
				sprintf(msg, " (%d, %llu)\n", counter, peak);

				write(p[1], msg, strlen(msg) + 1); // write msg to pipe
				close(p[1]);			// close write pipe
			}
			else
				write(p[1], " (0, 1)\n", 8); 	// can't do one message
		}
		else 	// parent process
		{
			char readmsg[1];		// create a readmsg
			close(p[1]);			// close write pipe

			wait(NULL);
			while (read(p[0], &readmsg, 1) > 0)	// read from pipe while there
				printf("%s", readmsg);		// are still char in it
			close(p[0]);			// close read pipe
		}
	}
	else	
	{
		printf("Entered a wrong or to many values!\n");	// wrong value error message
	}
	return 0;
}
