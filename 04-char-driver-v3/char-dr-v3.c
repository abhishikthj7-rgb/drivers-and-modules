#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>

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
static struct cdev my_cdev;
static struct class *my_class;
static dev_t dev;
struct my_state {
       char buf[MAX_NUMBER];
       size_t curr;
};

static int __init my_driver_init(void) {
	int ret;
	struct device *device;

	ret = alloc_chrdev_region(&dev, 0, 3, "mychar_driver");
	if (ret)
		return ret;

	major = MAJOR(dev);

	cdev_init(&my_cdev, &fops);

	ret = cdev_add(&my_cdev, dev, 3);
	if (ret)
		return ret;

	my_class = class_create("my_class");
	if (IS_ERR(my_class))
		return PTR_ERR(my_class);

	int i;
	for (i = 0; i < 3; i++) {
		device = device_create(my_class, NULL, MKDEV(major, i), NULL, "mychar%d", i);
		if (IS_ERR(device)) {
			ret = PTR_ERR(device);
			goto err_device_create;
		}
	}

	pr_info("Device registration success, major number = %d\n", major);
	return 0;


err_device_create:
	while (--i >= 0)
		device_destroy(my_class, MKDEV(major, i));
	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 3);

	return ret;
}
module_init(my_driver_init);

static void __exit my_driver_exit(void) {
	int i;
	for (i = 0; i < 3; i++)
		device_destroy(my_class, MKDEV(major, i));

	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 3);

	pr_info("module unloaded\n");
}
module_exit(my_driver_exit);

static int my_open(struct inode *inode, struct file *file) {
	pr_debug("Device opened\n");
	struct my_state *state;

	state = kmalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return -ENOMEM;

	state->curr = 0;
	file->private_data = state;

	return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
	struct my_state *state = file->private_data;
	size_t msg_len = state->curr;
	pr_info(" %s, state ptr = %px\n", __func__, state);

	if (*offset >= msg_len)
		return 0;

	size_t remaining = msg_len - *offset;
	size_t bytes_to_copy = min(remaining, len);

	if (copy_to_user(buf, state->buf + *offset, bytes_to_copy))
		return -EFAULT;

	*offset += bytes_to_copy;

	return bytes_to_copy;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset){
	struct my_state *state = file->private_data;
	pr_info(" %s, state ptr = %px\n", __func__, state);

	if (state->curr >= MAX_NUMBER)
		return -ENOSPC;
	size_t bytes_to_copy = min(sizeof(state->buf) - state->curr, len);

	if (copy_from_user(state->buf + state->curr, buf, bytes_to_copy))
		return -EFAULT;
	state->curr += bytes_to_copy;

	/* Print buffer using length, avoids need for null termination */
	pr_info("Bytes written: %.*s\n", (int)state->curr, state->buf);
	return bytes_to_copy;
}

static int my_release(struct inode *inode, struct file *file){
	struct my_state *state = file->private_data;
	kfree(state);
	return 0;
}
