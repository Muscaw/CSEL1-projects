/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>

// Addresses to reserve the IO Memory
volatile unsigned long* paddress = (volatile unsigned long*)0x10000000;
static unsigned long address = 0x10000000;
static unsigned long size = 0x100;

// Reserve a region of IO Memory and red the uc information from it
static void request_io(void)
{
	char* name = "IO Memory";
	
	if (request_mem_region(address, size, name) != NULL)
	{
		unsigned int id = ioread32 (ioremap(address, size));
		pr_info ("IO Memory reserved.\n");
		pr_info ("All : %u.\n", id);
		pr_info ("Minor revision : %u.\n", id & 0x0000000F);
		pr_info ("Major revision : %u.\n", (id >> 4) & 0x0000000F);
		pr_info ("Package ID : %u.\n", (id >> 8) & 0x0000000F);
		pr_info ("Product ID : %u.\n", (id >> 12) & 0x000FFFFF);
	}
}

// Release the reserved IO Memory
static void release_io(void)
{
	//iounmap(paddress);
	release_mem_region(address, size);
	pr_info ("IO Memory unreserved.\n");
}

// Method called when the module is created
static int __init skeleton_init(void)
{
	pr_info ("Linux module skeleton loaded\n");
	
	// Access to IO
	request_io();
	
	return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	// Release the reserved IO Memory
	release_io();

	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
