/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

// Method called when the module is created
static int __init skeleton_init(void)
{
	pr_info ("Linux module skeleton loaded\n");
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
