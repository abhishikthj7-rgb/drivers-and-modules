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
static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset);
static struct file_operations fops = {
        .open = my_open,
	.read = my_read,
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

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset){
	const char msg[] = "Hello from kernel\n";
	// sizeof includes the null terminator : '\0' for length
	// cat does not expect the extra '\0', leads to undefined behavior
	//int msg_len = sizeof(msg);
	// replace sizeof with strlen
	size_t msg_len = strlen(msg);


	printk(KERN_INFO "Device being read\n");

	if(*offset >= msg_len)
		return 0;

	//int bytes_to_copy = min(msg_len - *offset, len);
	// kernel min is type strict, both args must be of same type
	// otherwise, will get "signedness" error
	size_t remaining = msg_len - *offset;
	size_t bytes_to_copy = min(remaining, len);

	if(copy_to_user(buf, msg + *offset, bytes_to_copy))
		return -EFAULT;

	*offset += bytes_to_copy;

	return bytes_to_copy;
}
// !! node can be created manullay like this : mknod /dev/mychar c majornumber 0

