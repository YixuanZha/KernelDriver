#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>

#define SYS_LED_GPIO 272

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yixuan Zha");
MODULE_DESCRIPTION("GPIO LED test");

static int __init simple_led_init(void)
{
    int ret = 0;
    printk(KERN_INFO "My simple LED test, initing....\n");
    printk(KERN_INFO "Attempting to contril GPIO %d\n",SYS_LED_GPIO);

    if(!gpio_is_valid(SYS_LED_GPIO))
    {
        printk(KERN_ERR "gpio %d is not valid on this system",SYS_LED_GPIO);
        return -ENODEV;
    }

    ret = gpio_request(SYS_LED_GPIO,"my_led");
    if(ret)
    {
        printk(KERN_ERR "Failed to request GPIO %d",SYS_LED_GPIO);
        return ret;
    }

    ret = gpio_direction_output(SYS_LED_GPIO,1);
    if(ret)
    {
        printk(KERN_ERR "Failed to set GPIO direction");
        gpio_free(SYS_LED_GPIO);
        return ret;
    }

    printk(KERN_INFO "Successfully took control of GPIO %d, LED should be ON\n",SYS_LED_GPIO);
    return 0;
}

static void __exit simple_led_exit(void)
{
    printk(KERN_INFO "My simple led demo, exitint.....\n");

    gpio_set_value(SYS_LED_GPIO,0);
    gpio_free(SYS_LED_GPIO);

    printk(KERN_INFO "GPIO %d released\n",SYS_LED_GPIO);
}

module_init(simple_led_init);
module_exit(simple_led_exit);