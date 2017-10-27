/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define SIZE 20

int main()
{
	int i, j, filedesc[3];
	char* text[3] = {"Bonjour", "Salut", "Hello"};
	char buffer[3][SIZE] = {{0}, {0}, {0}};

	for (i = 0; i < 3; i++)
	{
		// Open the file according to the pilote number (minor)
		char filename[SIZE];
		sprintf(filename, "/dev/mymodule.%d", i);
		filedesc[i] = open(filename, O_WRONLY);
		if (filedesc[i] < 0)
		{
			printf("Error when opening the file descriptor %d", i);
			return 1;	
		}
		
		// Write into the opened file
		if (write(filedesc[i], text[i], strlen(text[i])) < 0)
		{
			printf("Error when writing the file descriptor %d", i);
			return 1;
		}
		
		// Close and reopen the same file but for reading
		close(filedesc[i]);
		sprintf(filename, "/dev/mymodule.%d", i);
		filedesc[i] = open(filename, O_RDONLY);
		if (filedesc[i] < 0)
		{
			printf("Error when opening the file descriptor %d", i);
			return 1;	
		}
		
		// Read the content of the file
		if (read(filedesc[i], buffer[i], SIZE) < 0)
		{
			printf("Error when writing the file descriptor %d", i);
			return 1;
		}
		
		// Display the read data and close the file
		printf("Buffer %d = %s\n", i, buffer[i]);
		close(filedesc[i]);
	}

	return 0;
}
