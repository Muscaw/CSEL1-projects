/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/poll.h>

// Declaration of the constants and variables
static const int GPIO_NR[4] = {2, 2, 1, 1};
static const int PIN_NR[4] = {5, 6, 6, 2};
static const int IO_NR[4] = {29, 30, 22, 18};
static const char* GPIO_NAME[4] = {"SW1_GPIO", "SW2_GPIO", "SW3_GPIO", "SW4_GPIO"};
static const char* IRQ_NAME[4] = {"SW1_IRQ", "SW2_IRQ", "SW3_IRQ", "SW4_IRQ"};
static int irqs_number[4];

// Déclaration d'une queue et d'une condition
static DECLARE_WAIT_QUEUE_HEAD(wq);
static int condition = 0;

// Method to handle the interruption
static irqreturn_t switch_pressed(int irq, void *dev_id)
{
	if (irq == irqs_number[0])
	{
		pr_info ("Interrupt raised by the switch 1.\n");
	}
	else if (irq == irqs_number[1])
	{
		pr_info ("Interrupt raised by the switch 2.\n");
	}
	else if (irq == irqs_number[2])
	{
		pr_info ("Interrupt raised by the switch 3.\n");
	}
	else if (irq == irqs_number[3])
	{
		pr_info ("Interrupt raised by the switch 4.\n");
	}

	condition = 1;
	wake_up_interruptible(&wq);
	
	return IRQ_HANDLED;
}

// Methode called when the file is opened
static int skeleton_open(struct inode* i, struct file* f)
{
	pr_info ("skeleton : open operation... major:%d, minor:%d\n", imajor(i), iminor(i));
	
	if ((f->f_mode & (FMODE_READ | FMODE_WRITE)) != 0)
	{
		pr_info("skeleton : opened for reading & writing...\n");
	}
	else if ((f->f_mode & FMODE_READ) != 0)
	{
		pr_info("skeleton : opened for reading...\n");
	}
	else if ((f->f_mode & FMODE_WRITE) != 0)
	{
		pr_info("skeleton : opened for writing...\n");
	}

	pr_info("skeleton : opened\n");
	return 0;
}

// Method called when the 
static unsigned int skeleton_poll(struct file* f, poll_table *wait)
{
	unsigned mask = 0;
	
	poll_wait(f, &wq, wait);
	if (condition == 1)
	{
		mask |= POLLIN | POLLRDNORM;
	}
	condition = 0;
	
	return mask;
}

// Methode called when the file is released
static int skeleton_release(struct inode* i, struct file* f)
{
	pr_info ("skeleton : closed\n");
	return 0;
}

// Structure defining 
static struct file_operations skeleton_fops =
{
		.owner = THIS_MODULE,
		.open = skeleton_open,
		.poll = skeleton_poll,
		.release = skeleton_release,
};

// Driver numbers
static struct cdev skeleton_cdev;
static dev_t skeleton_dev;

// Method called when the module is created
static int __init skeleton_init(void)
{
	int i;
	pr_info ("Linux module skeleton loaded\n");
	
	if (alloc_chrdev_region(&skeleton_dev, 0, 1, "skeleton") != 0)
	{
		pr_info ("Error when reserving the pilote number\n");
		return 1;
	}
	
	cdev_init(&skeleton_cdev, &skeleton_fops);
	skeleton_cdev.owner = THIS_MODULE;
	if (cdev_add(&skeleton_cdev, skeleton_dev, 1) != 0)
	{
		pr_info ("Error when registering the pilote\n");
		return 1;
	}
	
	for (i = 0; i < 4; i++)
	{
		if (gpio_request(IO_NR[i], GPIO_NAME[i]) == 0)
		{
			irqs_number[i] = gpio_to_irq(IO_NR[i]);
			if (request_irq(irqs_number[i], switch_pressed, IRQF_TRIGGER_FALLING, IRQ_NAME[i], NULL) == 0)
			{
				pr_info ("Interrupt for switch %d is ready.\n", (i+1));
			}
		}
	}

	return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		free_irq(irqs_number[i], NULL);
		gpio_free(IO_NR[i]);
		pr_info ("Interrupt for switch %d is removed.\n", (i+1));
	}
		
	cdev_del(&skeleton_cdev);
	unregister_chrdev_region(skeleton_dev, 1);
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
