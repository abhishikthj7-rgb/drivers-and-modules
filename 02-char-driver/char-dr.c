#include <linux/module.h> // metadata , module_init and exit
#include <linux/kernel.h> // printk
#include <linux/init.h>   // __init , __exit
#include <linux/fs.h>     // char driver operations
#include <linux/uaccess.h> // copy_to_user, copy_from_user

MODULE_LICENSE("GPL");
MODULE_AUTHOR("XYZ");
MODULE_DESCRIPTION("A basic character driver");

static int major;
static int my_open(struct inode *inode, struct file *file);
static struct file_operations fops = {
        .open = my_open,
};

static int __init my_driver_init(void){

	major = register_chrdev(0, "mychar_driver", &fops);
	if(major<0){
		printk(KERN_ERR "Device allocation failed !!\n");
		return major;
	}
	printk(KERN_INFO "Device registered successfully, major number = %d", major);

	printk(KERN_INFO "Hello char!\n");
	return 0;
}
module_init(my_driver_init);

static void __exit my_driver_exit(void){
	printk(KERN_INFO "Good Bye char !\n");
}
module_exit(my_driver_exit);

static int my_open(struct inode *inode, struct file *file){

	printk(KERN_INFO "Device opened\n");
	return 0;
}

// !! node can be created manullay like this : mknod /dev/mychar c majornumber 0

