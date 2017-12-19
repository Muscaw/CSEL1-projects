/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/wait.h>

// Déclaration d'une queue et d'une condition
static DECLARE_WAIT_QUEUE_HEAD(wq);
static int condition = 0;

// Reference on the thread
static struct task_struct *thread1;
static struct task_struct *thread2;

// Body of the thread "thread_counter"
static int thread_self_waked_up (void* data)
{
	int counter = 0;

	pr_info ("Self waked up thread (2) launched.\n");

	while (!kthread_should_stop())
	{
		pr_info ("Thread 2 awake for the %d time(s)\n", counter);
		ssleep(5);
		counter++;
		condition = 1;
		wake_up_interruptible(&wq);
	}

	pr_info ("Self waked up thread (2) stopped.\n");
	return 0;
}

// Body of the thread "thread_counter"
static int thread_waked_up (void* data)
{
	int counter = 0;

	pr_info ("Waked up thread (1) launched.\n");

	while (!kthread_should_stop())
	{
		pr_info ("Thread 1 awake for the %d time(s)\n", counter);
		wait_event_interruptible(wq, condition != 0);
		counter++;
		condition = 0;
	}

	pr_info ("Waked up thread (1) stopped.\n");
	return 0;
}

// Method called when the module is created
static int __init skeleton_init(void)
{
	pr_info ("Linux module skeleton loaded\n");

	// Launch the thread "counter_thread"
	thread1 = kthread_run(thread_waked_up, NULL, "Waked up thread");
	thread2 = kthread_run(thread_self_waked_up, NULL, "Self waked up thread");

	return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	// Stop the running thread
	kthread_stop(thread1);
	kthread_stop(thread2);

	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
