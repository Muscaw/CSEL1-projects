/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

//Method called when the open method is called on a file
static int skeleton_open(struct inode *i, struct file *f)
{
    pr_info ("skeleton: open operation... major %d minor %d \n", imajor(i), iminor(i));

    return 0;
}

//Function called when the file is closed
static int skeleton_release(struct inode *i, struct file *f)
{
    pr_info("skeleton: release op\n");
    return 0;
}

static char s_buffer[10000];

static ssize_t skeleton_read(struct file *f, char __user *buf, size_t count, loff_t *off)
{
    ssize_t remaining = strlen(s_buffer) - (ssize_t)(*off);
    char* ptr = s_buffer + *off;
    if (count > remaining) count = remaining;
    *off += count;

    if(copy_to_user (buf, ptr, count) != 0) count = -EFAULT;

    pr_info("skeleton: read operation... read=%d\n", count);

    return count;
}


static ssize_t skeleton_write(struct file *f, const char __user *buf, size_t count, loff_t *off)
{
    ssize_t remaining = sizeof(s_buffer) - (ssize_t)(*off);
    char* ptr = s_buffer + *off;
    *off += count;

    if(count >= remaining) count = -EIO;

    if(count > 0) {
        ptr[count] = 0;
        if(copy_from_user(ptr, buf, count)) count = -EFAULT;
    }
    pr_info ("skeleton: write op.. written=%d\n", count);
    return count;
}

static dev_t skeleton_dev;
static struct cdev skeleton_cdev;

static struct file_operations skeleton_fops = {
    .owner = THIS_MODULE,
    .open = skeleton_open,
    .read = skeleton_read,
    .write = skeleton_write,
    .release = skeleton_release,
};

// Method called when the module is created
static int __init skeleton_init(void)
{
    int status = alloc_chrdev_region(&skeleton_dev, 0, 1, "skeleton");
    if(status == 0) {
        cdev_init(&skeleton_cdev, &skeleton_fops);
        skeleton_cdev.owner = THIS_MODULE;
        status = cdev_add(&skeleton_cdev, skeleton_dev, 1);
    }
    pr_info ("skeleton device driver loaded (%d)\n", status);
	return status;
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

MODULE_AUTHOR ("Kevin Grandjean <kevin.grandjean@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
