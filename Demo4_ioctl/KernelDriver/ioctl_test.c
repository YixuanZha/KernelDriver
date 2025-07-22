#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include "ioctl_test.h"

static dev_t new_dev;
static struct cdev my_cdev;
static struct class* myclass;
static struct device* my_device;
static int shared_variable = 0;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yixuan Zha");
MODULE_DESCRIPTION("Simple test of ioctl");

static int moduleOpen(struct inode* file_inode , struct file* instance)
{
    printk(KERN_INFO "The simple ioctl module is opened");
    return 0;
}

static int moduleRelease(struct inode* file_inode, struct file* instance)
{
    printk(KERN_INFO "The simple ioctl module is released");
    return 0;
}

static long moduleIoctl(struct file* instance, unsigned int cmd, unsigned long arg)
{
    printk(KERN_INFO "ioctl kernel: moduleIoctl is called, cmd = %u\n",cmd);

    if(_IOC_TYPE(cmd) != IOCTL_MAGIC) return -ENOTTY;
    if(_IOC_NR(cmd) > 2) return -ENOTTY;

    switch(cmd)
    {
        case IOCTL_SET_VAL:
        if(copy_from_user(&shared_variable,(int*)arg,sizeof(int)))
        {
            return EFAULT;
        }
        printk(KERN_INFO "ioctl_kernel: Set shared_variabel to: %d\n",shared_variable);
        break;

        case IOCTL_GET_VAL:
        if(copy_to_user((int*)arg,&shared_variable,sizeof(int)))
        {
            return -EFAULT;
        }
        printk(KERN_INFO "ioctl kernel: Got shared_variabel of: %d\n",shared_variable);
        break;
        default:
        return -EINVAL;
    }
    return 0;
}

static const struct file_operations fops = {

    .owner = THIS_MODULE,
    .open =  moduleOpen,
    .release = moduleRelease,
    .unlocked_ioctl = moduleIoctl
};

static int __init moduleInit(void)
{
    int retval;
    printk(KERN_INFO "Hi Kernel, this is ioctl test");

    retval = alloc_chrdev_region(&new_dev,0,1,"ioctl_test");
    if(retval < 0)
    {
        printk(KERN_ERR "Failed to alloc device number");
        return retval;
    }

    printk(KERN_INFO "Device number is allocated, Major: %d, Minor: %d\n",MAJOR(new_dev),MINOR(new_dev));

    myclass = class_create("ioctl_test_class");
    if(IS_ERR(myclass))
    {
        printk(KERN_ERR "Failed to create device class");
        retval = PTR_ERR(myclass);
        goto unregister_chrdev;
    }

    cdev_init(&my_cdev,&fops);
    retval = cdev_add(&my_cdev,new_dev,1);
    if(retval < 0)
    {
        printk(KERN_ERR "Failed to add cdev to the system");
        goto destory_class;
    }

    my_device = device_create(myclass,NULL,new_dev,NULL,"ioctldevice");
    if(IS_ERR(my_device))
    {
        printk(KERN_ERR "Failed to create the device file");
        retval = PTR_ERR(my_device);
        goto destory_cdev;
    }

    printk(KERN_INFO "Device 'ioctldeice' created successfully\n");
    return 0;

destory_cdev:
    cdev_del(&my_cdev);
destory_class:
    class_destroy(myclass);    
unregister_chrdev:
    unregister_chrdev_region(new_dev,1);
    return retval;
}

static void __exit moduleExit(void)
{
    device_destroy(myclass,new_dev);
    cdev_del(&my_cdev);
    class_destroy(myclass);
    unregister_chrdev_region(new_dev,1);
    printk(KERN_INFO "Module is unloaded\n");
}

module_init(moduleInit);
module_exit(moduleExit);