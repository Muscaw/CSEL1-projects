/* skeleton.c */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>

int skeleton_mmap (struct file* f, struct vm_area_struct *vma)
{
    if(remap_pfn_range(
        vma, //vma struct pointer
        vma->vm_start, // start address
        0x10000000 >> PAGE_SHIFT, // numéro de page (adresse physique right shifted de PAGE_SHIFT)
        0x100, //taille de la zone
        vma->vm_page_prot //protection de la page
     )) return -EAGAIN;
     return 0;
}

//static dev_t skeleton_dev;
//static struct cdev skeleton_cdev;

/*static struct file_operations skeleton_fops = {
    .owner = THIS_MODULE,
    .mmap = skeleton_mmap,
};*/

// Method called when the module is created
static int __init skeleton_init(void)
{
    /*int status = alloc_chrdev_region(&skeleton_dev, 0, 1, "skeleton");
    if(status == 0) {
        cdev_init(&skeleton_cdev, &skeleton_fops);
        skeleton_cdev.owner = THIS_MODULE;
        status = cdev_add(&skeleton_cdev, skeleton_dev, 1);
    }
    pr_info ("skeleton device memory driver loaded (%d)\n", status);
    return status;*/
    return 0;
}

// Method called when the module is destroyed
static void __exit skeleton_exit(void)
{
    //cdev_del(&skeleton_cdev);
    //unregister_chrdev_region(skeleton_dev, 1);
	pr_info ("Linux module memory skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Kevin Grandjean <kevin.grandjean@master.hes-so.ch>");
MODULE_DESCRIPTION ("Module memory map");
MODULE_LICENSE ("GPL");
