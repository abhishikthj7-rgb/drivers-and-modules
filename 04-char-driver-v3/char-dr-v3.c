#include <linux/module.h> // metadata , module_init and exit
#include <linux/kernel.h> // printk
#include <linux/init.h>   // __init , __exit
#include <linux/fs.h>     // char driver operations , alloc_chrdev_region
#include <linux/uaccess.h> // copy_to_user, copy_from_user
#include <linux/slab.h> // kmalloc , kfree
#include <linux/cdev.h> // cdev, cdev_init, cdev_add
#include <linux/device.h> // class create , device create 


MODULE_LICENSE("GPL");
MODULE_AUTHOR("XYZ");
MODULE_DESCRIPTION("A basic character driver");

static int major;
static int my_open(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset);
static int my_release(struct inode *inode, struct file *file);
static struct file_operations fops = {
	.owner = THIS_MODULE,
        .open = my_open,
        .read = my_read,
        .write = my_write,
	.release = my_release,
};
#define MAX_NUMBER 100
//static char kbuf[MAX_NUMBER];
//static size_t curr = 0;
static struct cdev my_cdev;
static struct class *my_class;
static dev_t dev;
struct my_state{
       char buf[MAX_NUMBER];
       size_t curr;
};

static int __init my_driver_init(void){
	if(alloc_chrdev_region(&dev, 0, 3, "mychar_driver") <0)
		return -1;
	//major = register_chrdev(0, "mychar_driver", &fops);
	major = MAJOR(dev);
	if(major<0){
		printk(KERN_ERR "Device allocation failed !!\n");
		return major;
	}

	cdev_init(&my_cdev,&fops);

	if(cdev_add(&my_cdev, dev, 3) <0)
		return -1;

	my_class = class_create("my_class");
	if(IS_ERR(my_class))
		return PTR_ERR(my_class);

	for(int i=0; i<3; i++)
		if(IS_ERR(device_create(my_class, NULL, MKDEV(major, i), NULL, "mychar%d", i)))
			return -1;

	printk(KERN_INFO "Devices registered successfully, major number = %d\n", major);
	printk(KERN_INFO "Module insertion success!\n");
	return 0;
}
module_init(my_driver_init);

static void __exit my_driver_exit(void){
	//unregister_chrdev(major, "mychar_driver");

	for(int i=0; i<3; i++)
		device_destroy(my_class, MKDEV(major,i));

	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 3);

	printk(KERN_INFO "device_destroy success!\n");
	printk(KERN_INFO "Module removal success!\n");
}
module_exit(my_driver_exit);

static int my_open(struct inode *inode, struct file *file){
	printk(KERN_INFO "Device opened\n");
	struct my_state *state;

	state = kmalloc(sizeof(*state), GFP_KERNEL);
	if(!state)
		return -ENOMEM;

	state->curr = 0;
	file->private_data = state;

	return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset){
	//const char msg[] = "Hello from kernel\n";
	// sizeof includes the null terminator : '\0' for length
	// cat does not expect the extra '\0', leads to undefined behavior
	//int msg_len = sizeof(msg);
	// replace sizeof with strlen
	// avoid the use of strlen wherever possible, kernel prefers explicit memory control
	//size_t msg_len = strlen(kbuf);
	//size_t msg_len = curr;

	struct my_state *state = file->private_data;
	size_t msg_len = state->curr;
	printk(KERN_INFO "state ptr = %px\n", state);

	printk(KERN_INFO "Device being read\n");

	if(*offset >= msg_len)
		return 0;

	//int bytes_to_copy = min(msg_len - *offset, len);
	// kernel min is type strict, both args must be of same type
	// otherwise, will get "signedness" error
	size_t remaining = msg_len - *offset;
	size_t bytes_to_copy = min(remaining, len);

	//if(copy_to_user(buf, kbuf + *offset, bytes_to_copy))
	if(copy_to_user(buf, state->buf + *offset, bytes_to_copy))
		return -EFAULT;

	*offset += bytes_to_copy;

	return bytes_to_copy;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset){
	//char kbuf[64];

	printk(KERN_INFO "Write entry\n");
	//struct my_state *state = (struct my_state *)file->private_data;
	struct my_state *state = file->private_data;
	printk(KERN_INFO "state ptr = %px\n", state);
	// each write is called using a fresh buffer, adding offset not needed for "stateless" char driver
	//size_t bytes_to_copy = min(sizeof(kbuf) - offset, len);
	// !! -1 in below line for '\0'
	if(state->curr >= MAX_NUMBER)
		return -ENOSPC;
	//size_t bytes_to_copy = min(sizeof(state->buf)-1-state->curr , len);
	// kernel , string is treated as a set of raw bytes, remove -1 while calculating the remaining size
	size_t bytes_to_copy = min(sizeof(state->buf)-state->curr , len);
	//if(copy_from_user(kbuf + *offset, buf, bytes_to_copy))
	//if(copy_from_user(kbuf+curr, buf, bytes_to_copy))
	if(copy_from_user(state->buf+state->curr, buf, bytes_to_copy))
		return -EFAULT;
	state->curr += bytes_to_copy;
	// for bad memory access/copy failure return -EFAULT

	// !! %s expects null terinated string
	//if(state->curr == MAX_NUMBER)
	//kbuf[bytes_to_copy] = '\0';
	//	state->buf[state->curr] = '\0';
	//printk(KERN_INFO "Bytes written : %s\n", state->buf);
	//Below line prints exact number of bytes, no need for '\0', safe even when the buffer is full
	printk(KERN_INFO "Bytes written: %.*s\n", (int)state->curr, state->buf);
	return bytes_to_copy;
}

static int my_release(struct inode *inode, struct file *file){
	struct my_state *state = file->private_data;
	kfree(state);
	return 0;
}

// !! node can be created manullay like this : mknod /dev/mychar c majornumber 0

