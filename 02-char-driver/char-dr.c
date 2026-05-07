#include <linux/module.h> // metadata , module_init and exit
#include <linux/kernel.h> // printk
#include <linux/init.h>   // __init , __exit
#include <linux/fs.h>     // char driver operations
#include <linux/uaccess.h> // copy_to_user, copy_from_user

MODULE_LICENSE("GPL");
MODULE_AUTHOR("XYZ");
MODULE_DESCRIPTION("A basic character driver");

static int __init my_driver_init(void){
	printk(KERN_INFO "Hello char!\n");
	return 0;
}
module_init(my_driver_init);

static void __exit my_driver_exit(void){
	printk(KERN_INFO "Good Bye char !\n");
}
module_exit(my_driver_exit);
