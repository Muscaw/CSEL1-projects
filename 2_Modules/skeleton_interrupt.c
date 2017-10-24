/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

// Declaration of the constants and variables
static const int GPIO_NR[4] = {2, 2, 1, 1};
static const int PIN_NR[4] = {5, 6, 6, 2};
static const int IO_NR[4] = {29, 30, 22, 18};
static const char* GPIO_NAME[4] = {"SW1_GPIO", "SW2_GPIO", "SW3_GPIO", "SW4_GPIO"};
static const char* IRQ_NAME[4] = {"SW1_IRQ", "SW2_IRQ", "SW3_IRQ", "SW4_IRQ"};
static int irqs_number[4];

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

	return IRQ_HANDLED;
}

// Method called when the module is created
static int __init skeleton_init(void)
{
	int i;
	pr_info ("Linux module skeleton loaded\n");
	
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
	
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
