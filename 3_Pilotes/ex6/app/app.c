/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>

int main()
{
	int counter = 0;
	int filedesc;
	fd_set set;

	filedesc = open("/dev/mymodule_block", O_RDONLY);
	if (filedesc < 0)
	{
		printf("Error when opening the file descriptor");
		return 0;	
	}

	FD_ZERO(&set);
	FD_SET(filedesc, &set);
	
	while(counter < 10)
	{
		if (select(FD_SETSIZE, &set, NULL, NULL, NULL) == 1)
		{
			counter++;
			printf("Counter = %d\n", counter);
		}
		else
		{
			printf("Error when selecting the file descriptor");
		}
		
	}
	
	close(filedesc);

	return 0;
}
