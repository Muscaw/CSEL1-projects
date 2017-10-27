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

#define SIZE 0x100
#define ADDRESS 0x10000000

int main()
{
	int filedesc;
	int* page;

	// Open the file /dev/mem
	filedesc = open("/dev/mem", O_RDONLY);
	if (filedesc < 0)
	{
		printf("Error when opening the file descriptor");
		return 0;	
	}

	// Add the pilote's registers to the virtual memory
	page = (int*)mmap(NULL, SIZE, PROT_READ, MAP_PRIVATE, filedesc, ADDRESS);

	// Read the informations of the chip
	printf("All : %u.\n", *page);
	printf("Minor revision : %u.\n", *page & 0x0000000F);
	printf("Major revision : %u.\n", (*page >> 4) & 0x0000000F);
	printf("Package ID : %u.\n", (*page >> 8) & 0x0000000F);
	printf("Product ID : %u.\n", (*page >> 12) & 0x000FFFFF);

	// Free the virtual memory and close the file
	if (munmap(page, SIZE) < 0)
	{
		printf("Error when freeing the virtual memory");
	}
	close(filedesc);

	return 0;
}
