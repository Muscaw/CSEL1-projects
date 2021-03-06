/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>

// Argument for the number of instances
static int instances = 1;
module_param(instances, int, 0);

static char** s_buffer;

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

// Methode called when the file is read
static ssize_t skeleton_read(struct file* f, char* __user buf, size_t count, loff_t* off)
{
	// compute remaining bytes to copy, update count and pointers
	ssize_t remaining = strlen(s_buffer[iminor(f->f_path.dentry->d_inode)]) - (ssize_t)(*off);
	char* ptr = s_buffer[iminor(f->f_path.dentry->d_inode)] + *off;
	if (count > remaining)
	{
		count = remaining;
	}
	*off += count;
	
	// copy required number of bytes
	if (copy_to_user(buf, ptr, count) != 0)
	{
		count = -EFAULT;
	}
	
	pr_info("skeleton: read operation... read=%d\n", count);
	return count;
}

// Methode called when the file is written
static ssize_t skeleton_write(struct file* f, const char* __user buf, size_t count, loff_t* off)
{
	// compute remaining space in buffer and update pointers
	ssize_t remaining = 10000 * sizeof(char) - (ssize_t)(*off);
	char* ptr = s_buffer[iminor(f->f_path.dentry->d_inode)] + *off;
	*off += count;
	
	// check if still remaining space to store additional bytes
	if (count >= remaining)
	{
		count = -EIO;
	}
	
	// store additional bytes into internal buffer
	if (count > 0)
	{
		ptr[count] = 0;
	}
	
	if (copy_from_user(ptr, buf, count))
	{
		count = -EFAULT;
	}
	
	pr_info("skeleton: write operation... written=%d\n", count);
	return count;
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
		.read = skeleton_read,
		.write = skeleton_write,
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
	
	s_buffer = (char**) kcalloc((size_t)instances, (size_t)sizeof(char*), GFP_KERNEL);
	for (i = 0; i < instances; i++)
	{
		s_buffer[i] = (char*) kcalloc((size_t)instances, (size_t)(10000*sizeof(char)), GFP_KERNEL);
	}
	
	if (alloc_chrdev_region(&skeleton_dev, 0, instances, "skeleton") != 0)
	{
		pr_info ("Error when reserving the pilote number\n");
		return 1;
	}
	
	cdev_init(&skeleton_cdev, &skeleton_fops);
	skeleton_cdev.owner = THIS_MODULE;
	if (cdev_add(&skeleton_cdev, skeleton_dev, instances) != 0)
	{
		pr_info ("Error when registering the pilote\n");
		return 1;
	}

	return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
	int i = 0;
	
	cdev_del(&skeleton_cdev);
	unregister_chrdev_region(skeleton_dev, instances);
	
	for (i = 0; i < instances; i++)
	{
		kfree(s_buffer[i]);
	}
	kfree(s_buffer);
	
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Nicolas Zurbuchen <nicolas.zurbuchen@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
