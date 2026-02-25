#include <linux/init.h>
#include <linux/module.h>

static int __init chr_dev_init(void){
    printk("chr_dev_init\r\n");
    return 0;
}

static void __exit chr_dev_exit(void){
    printk("chr_dev_exit\r\n");
    return 0;
}

module_init(chr_dev_init);
module_exit(chr_dev_exit);
MODULE_LICENSE("GPL");