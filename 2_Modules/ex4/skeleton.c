/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/list.h>

// Fisrt argument as string
static char* text= "dummy help";
module_param(text, charp, 0);

// Second argument as int
static int elements= 1;
module_param(elements, int, 0);

// One element of the linked list
struct element {
	char character;
	int number;
	struct list_head node;
};

// List containing the element
static LIST_HEAD (my_list);

// Create a linked list with the arguments and save it dynamically
static void create_linked_list(void)
{
	int i;
	
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
}

// Remove each element of the linked list and free the memory
static void destroy_linked_list(void)
{
	struct element* ele;
	list_for_each_entry (ele, &my_list, node)
	{
		pr_info ("element[text: %c ; number: %d] removed.\n", ele->character, ele->number);
		kfree(ele);
	}
}

// Method called when the module is created
static int __init skeleton_init(void)
{
	// Display the given arguments
	pr_info ("Linux module skeleton loaded\n");
	pr_info ("text: %s --- elements: %d\n", text, elements);
	
	// Create linked list
	create_linked_list();

	return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	// Remove the list
	destroy_linked_list();

	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
