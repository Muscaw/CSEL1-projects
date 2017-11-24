/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>

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
	pr_info ("Linux module skeleton loaded\n");

	// Launch the thread "counter_thread"
	thread = kthread_run(thread_counter, NULL, "Counter thread");

	return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	// Stop the running thread
	kthread_stop(thread);

	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
