/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/kthread.h>
#include <linux/delay.h>

// Fisrt argument as string
static char* text= "dummy help";
module_param(text, charp, 0);

// Second argument as int
static int elements= 1;
module_param(elements, int, 0);

// One element of the linked list
static struct element {
	char character;
	int number;
	struct list_head node;
};

// List containing the element
static LIST_HEAD (my_list);


// Addresses to reserve the IO Memory
volatile unsigned long* paddress = (volatile unsigned long*)0x10000000;
static unsigned long address = 0x10000000;
static unsigned long size = 0x100;

// Reference on the thread
static struct task_struct *thread;

// Body of the thread "thread_counter"
static int thread_counter (void* data)
{
	int counter = 0;

	pr_info ("Counter thread launched.\n");

	while (!kthread_should_stop())
	{
		pr_info ("Thread awake for the %d time(s)\n", counter);
		ssleep(5);
		counter++;
	}

	pr_info ("Counter thread stopped.\n");
	return 0;
}

// Method called when the module is created
static int __init skeleton_init(void)
{
	int i;
	char* name = "IO Memory";

	// Display the given arguments
	pr_info ("Linux module skeleton loaded\n");
	pr_info ("text: %s --- elements: %d\n", text, elements);

	// Create a linked list with the arguments and save it dynamically
	for (i = 0; i < elements; i++)
	{
		struct element* ele;
		ele = kzalloc (sizeof*(ele), GFP_KERNEL);
		
		if (ele != NULL)
		{
			list_add_tail (&ele->node, &my_list);
			ele->character = *(text + i);
			ele->number = i;
		}

		pr_info ("element[text: %c ; number: %d] added.\n", ele->character, ele->number);
	}

	// Reserve a region of IO Memory and red the uc information from it
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

	// Launch the thread "counter_thread"
	thread = kthread_run(thread_counter, NULL, "Counter thread");

	return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	// Remove each element of the linked list and free the memory
	struct element* ele;
	list_for_each_entry (ele, &my_list, node)
	{
		pr_info ("element[text: %c ; number: %d] removed.\n", ele->character, ele->number);
		kfree(ele);
	}

	// Release the reserved IO Memory
	//iounmap(paddress);
	release_mem_region(address, size);
	pr_info ("IO Memory unreserved.\n");

	// Stop the running thread
	kthread_stop(thread);

	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
