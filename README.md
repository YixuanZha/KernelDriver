# KernelDriver
## How to write a simple char driver
```c
struct file_operations
{
    .owner = THISMOUDLE,
    .open = moduleOpen,
    .release = moduleRelease,
    .read = moduleRead,
    .write = moduleWrite
};

module_init(moduleInit);
module_exit(moduleExit);

```

Useful data structure
```c
static dev_t new_dev;
static struct cdev my_cdev;
static struct class* my_class;
static struct device* my_device;
```
How to register a device
```c
int retval;
retval = alloc_chrdev_region(&new_dev,0,1,"my_new_dev");
my_class = class_create("my_device_class");
cdev_init(&my_dev,&fops);
retval = cdev_add(&my_dev, new_dev);
my_device = device_create(my_class,NULL,new_dev,NULL,"mynewdev");
```