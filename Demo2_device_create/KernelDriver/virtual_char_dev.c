#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define MYMAJOR 92

static dev_t new_dev;
static struct cdev my_cdev; // cdev -> character device 将一个设备号（dev_t，由主/次设备号构成）和你的驱动所提供的文件操作函数集（struct file_operations）绑定在一起
static struct class *myclass; // class -> device class class 是一个更高层次的抽象，它把功能类似的设备归为一类。它的主要目的是为了方便用户空间的设备管理器（如 udev）工作
static struct device *my_device; // 代表一个具体的、独立的设备实例，并触发 udev 在 /dev 目录下创建对应的设备文件。

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yixuan Zha");
MODULE_DESCRIPTION("Install the kernel driver and register the dev automatically");

static int driverOpen(struct inode *file_inode, struct file *instance)
{
    printk("new_dev -- open was called\n");
    return 0;
}

static int driverClose(struct inode *file_inode, struct file *instance)
{
    printk("new_dev -- close was called\n");
    return 0;
}

static ssize_t driverRead(struct file *instance, char __user *buf, size_t count, loff_t *fpos)
{
    // read from kernel to user space
    char kbuf[] = "Hi, this is kernel, are you user space\n";
    int to_copy = strlen(kbuf);

    printk(KERN_INFO "Read is called\n");
    if (to_copy > count)
    {
        to_copy = count;
    }

    if (copy_to_user(buf, kbuf, to_copy))
    {
        printk(KERN_INFO "Failed to copy to user\n");
        return -EFAULT;
    }

    printk(KERN_INFO "Copy to user successfully\n");
    return to_copy;
}

static ssize_t driverWrite(struct file *instance, const char __user *buf, size_t count, loff_t *fpos)
{
    // write is from user space to kernel space
    char kbuf[256];
    int max_bytes = sizeof(kbuf) - 1;

    printk(KERN_INFO "Write was caled\n");

    if (count > max_bytes)
    {
        count = max_bytes;
    }

    if (copy_from_user(kbuf, buf, count))
    {
        printk(KERN_INFO "Failed to copy from user\n");
        return -EFAULT;
    }

    printk(KERN_INFO "Copy from user successfully\n");
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driverOpen,
    .release = driverClose,
    .read = driverRead,
    .write = driverWrite};

static int __init moduleInit(void)
{
    int retval;
    printk(KERN_INFO "Hello Kernel\n");

    // 1. allocate the device number
    retval = alloc_chrdev_region(&new_dev, 0, 1, "my_dev_driver"); // 向内核动态申请一个可用的设备号（主设备号和次设备号）---（动态）！！！！
    if (retval < 0)
    {
        printk(KERN_ERR "Failed to allocate device number\n");
        return retval;
    }

    printk(KERN_INFO "Device number allocated, Major: %d, Minor: %d \n", MAJOR(new_dev), MINOR(new_dev));
    // 2. create device class
    myclass = class_create(THIS_MODULE,"my_dev_class"); //在 /sys/class 下创建一个目录，用于存放后续创建的设备
    if (IS_ERR(myclass))
    {
        printk(KERN_ERR "Failed to create device class\n");
        retval = PTR_ERR(myclass);
        goto unregister_chrdev;
    }

    // 3. init and add cdev
    cdev_init(&my_cdev, &fops); // 初始化一个cdev结构体，与文件操作函数集绑定起来
    retval = cdev_add(&my_cdev, new_dev, 1); // 将初始化好的cdev注册到内核中
    if (retval < 0)
    {
        printk(KERN_ERR "Failed to add cdev to the system\n");
        goto destory_class;
    }

    // 4. Create device file
    my_device = device_create(myclass, NULL, new_dev, NULL, "mynewdev"); // 触发udev服务，在/dev目录下创建设备文件， 在/sys/class/my_dev_class/目录下创建一个mynewdev的条目
    if (IS_ERR(my_device))
    {
        printk(KERN_ERR "Failed to create the device file\n");
        retval = PTR_ERR(my_device);
        goto destory_cdev;
    }

    printk(KERN_INFO "Device 'mynewdev' created successfully\n");
    return 0;

destory_cdev:
    cdev_del(&my_cdev);
destory_class:
    class_destroy(myclass);
unregister_chrdev:
    unregister_chrdev_region(new_dev, 1);
    return retval;
}

static void __exit moduleExit(void)
{
    device_destroy(myclass,new_dev); // 删除创建的具体的设备实例
    cdev_del(&my_cdev); // 删除cdev结构体， 删除设备号与文件操作集的内核链接
    class_destroy(myclass); // 删除设备类
    unregister_chrdev_region(new_dev,1); // 释放动态分配的设备号
    printk(KERN_INFO "Goodbye Kernel, module is unloaded\n");
}

module_init(moduleInit);
module_exit(moduleExit);
