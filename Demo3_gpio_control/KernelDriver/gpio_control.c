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

    return 0;
}

static void __exit simple_led_exit(void)
{

}

module_init(simple_led_init);
module_exit(simple_led_exit);