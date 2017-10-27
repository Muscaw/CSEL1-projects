/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/mm.h>

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

// Method called to map the address to the virtual addresses
static int skeleton_mmap(struct file* f, struct vm_area_struct* vma)
{
	if (remap_pfn_range(vma, vma->vm_start, 0x10000000 >> PAGE_SHIFT, 0x100, vma->vm_page_prot))
	{
		pr_info("skeleton : fail to remap...\n");
		return 1;
	}
	
	pr_info("skeleton : remapped...\n");
	return 0;
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
		.mmap = skeleton_mmap,
		.release = skeleton_release,
};

// Driver numbers
static struct cdev skeleton_cdev;
static dev_t skeleton_dev;

// Method called when the module is created
static int __init skeleton_init(void)
{
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

	return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	cdev_del(&skeleton_cdev);
	unregister_chrdev_region(skeleton_dev, 1);
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
