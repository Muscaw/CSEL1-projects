/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>

// Declaration of the data structure and a globale variable
struct struct_data
{
	int integer;
	char character;
	char string[100];
};
static struct struct_data data;

// Show method for the gobal variable
ssize_t skeleton_show_data(struct device* dev, struct device_attribute* attr, char* buf)
{
	pr_info ("Show...\n");
	sprintf(buf, "Integer = %d, Character = %c, String = %s\n", data.integer, data.character, data.string);
	return strlen(buf);
}

// Store method for the global variable
ssize_t skeleton_store_data(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	pr_info ("Store...\n");
	sscanf(buf, "%d %c %s", &(data.integer), &(data.character), data.string);
	return count;
}

// Maccro creating an attribute structure for the global variable
DEVICE_ATTR(attr, 0664, skeleton_show_data, skeleton_store_data);

// Release method for the device
static void sysfs_dev_release(struct device* dev)
{
	pr_info ("skeleton : released...\n");
}

// Structure of the device
static struct platform_device sysfs_device =
{
		.name = "skeleton_device",
		.id = -1,
		.dev.release = sysfs_dev_release,
};

// Structure of the driver
static struct platform_driver sysfs_driver = {
		.driver =
		{
				.name = "skeleton_driver",
		},
};

// Method called when the module is created
static int __init skeleton_init(void)
{
	int status = 0;
	pr_info ("Linux module skeleton loaded\n");
	
	data.integer = 0;
	data.character = 'c';
	strcpy(data.string, "Bonjour");
	
	if (status == 0)
	{
		status = platform_driver_register(&sysfs_driver);
		pr_info ("Driver ok...\n");
	}
	if (status == 0)
	{
		status = platform_device_register(&sysfs_device);
		pr_info ("Device ok...\n");
	}
	if (status == 0)
	{
		device_create_file(&sysfs_device.dev, &dev_attr_attr);
		pr_info ("Attr ok...\n");
	}
	
	return status;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	device_remove_file(&sysfs_device.dev, &dev_attr_attr);
	platform_device_unregister(&sysfs_device);
	platform_driver_unregister(&sysfs_driver);
	
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
