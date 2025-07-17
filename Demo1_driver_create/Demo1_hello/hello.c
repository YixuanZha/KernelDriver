#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define MYMAJOR 91

// inode 表示文件， file表示打开的文件

// Meta information
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yixuan Zha");
MODULE_DESCRIPTION("Resgiter a device num");

static int DriverOpen(struct inode* device_file,struct file* instance)
{
    printk("dev_num - open was called\n");
    return 0;
}

static int DriverClose(struct inode* device_file,struct file* instance)
{
    printk("dev_num - close was called\n");
    return 0;
}

static ssize_t DriverRead(struct file *instance, char __user* buf, size_t count, loff_t *f_pos)
{
    char kbuf[] = "Hello from kernel\n";
    int to_copy = strlen(kbuf) + 1;

    printk(KERN_INFO "Read was called\n");

    if(to_copy > count)
    {
        to_copy = count;
    }

    if(copy_to_user(buf,kbuf,to_copy))
    {
        printk(KERN_INFO "Failed to copy to user\n");
        return -EFAULT;
    }

    printk(KERN_INFO "Copied %d bytes to user \n",to_copy);
    return to_copy;
}

static ssize_t DriverWrite(struct file* instance, const char __user* buf,size_t count, loff_t *f_pos)
{
    char kbuf[256];
    int max_bytes = sizeof(kbuf) -1;

    printk(KERN_INFO "Write was called\n");

    if(count > max_bytes)
    {
        count = max_bytes;
    }

    if(copy_from_user(kbuf,buf,count))
    {
        printk(KERN_INFO "Failed to read data from user space\n");
        return -EFAULT;
    }

    kbuf[count] = '\0';
    
    printk(KERN_INFO "Received %d bytes from user space\n",count);
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = DriverOpen,
    .release = DriverClose,
    .read = DriverRead,
    .write = DriverWrite
};

static int __init ModuleInit(void)
{
    int retval;
    printk("Hello kernel\n");
    retval = register_chrdev(MYMAJOR, "dev_num", &fops);
    if (retval == 0) {
        // 成功注册了指定的主设备号
        printk(KERN_INFO "dev_num - Registered device number Major: %d, Minor: %d\n", MYMAJOR, 0);
        return 0; // 成功时直接返回 0
    } else { // retval 是非零值，表示失败（无论是 > 0 的动态分配还是 < 0 的真正错误）
        printk(KERN_ERR "dev_num - Could not register device number with MYMAJOR %d. Error code: %d\n", MYMAJOR, retval); // 添加日志级别并打印错误码
        if (retval > 0) {
            // 如果内核动态分配了主设备号，但我们不想要这个行为，需要手动注销它
            unregister_chrdev(retval, "dev_num");
            // 此时可以返回更具体的错误码，如 -EEXIST
            return -EEXIST;
        }
        // 如果是负数（真正的错误码），直接返回该错误码
        return retval; // 或者返回一个通用的 -1 或 -ENODEV
    }

    return 0;
}

static void __exit ModuleExit(void)
{
    unregister_chrdev(MYMAJOR, "dev_num");
    printk("Goodbye, kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);