/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

// Fisrt argument as string
static char* text = "dummy help";
module_param(text, charp, 0);

// Second argument as int
static int elements = 1;
module_param(elements, int, 0);

// Method called when the module is created
static int __init skeleton_init(void)
{
	// Display the given arguments
	pr_info ("Linux module skeleton loaded\n");
	pr_info ("text: %s --- elements: %d\n", text, elements);
	return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
