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
static loff_t my_llseek(struct file *file, loff_t off, int whence);

static struct file_operations fops = {
	.owner = THIS_MODULE,
        .open = my_open,
        .read = my_read,
        .write = my_write,
	.release = my_release,
	.llseek = my_llseek,
};

#define MAX_NUMBER 100
static struct cdev my_cdev;
static struct class *my_class;
static dev_t dev;
struct my_state {
	char buf[MAX_NUMBER];
	size_t curr;
	struct mutex lock;
	wait_queue_head_t read_queue;
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
		goto err_cdev_add;

	my_class = class_create("my_class");
	if (IS_ERR(my_class)){
		ret = PTR_ERR(my_class);
		goto err_class_create;
	}

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
	class_destroy(my_class); //Device creation fails only after class creation succeeds, class_destroy needed here

err_class_create:
	//class_destroy(my_class); will lead to crash/undefined behavior, cant destroy what is not created
err_cdev_add:
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
	mutex_init(&state->lock);
	file->private_data = state;
	init_waitqueue_head(&state->read_queue);

	return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
	struct my_state *state = file->private_data;
	char tmp[MAX_NUMBER];

	mutex_lock(&state->lock);

	size_t msg_len = state->curr;
	pr_info(" %s, state ptr = %px\n", __func__, state);

	while (*offset >= msg_len) {
		mutex_unlock(&state->lock);

		if (wait_event_interruptible(state->read_queue, *offset < state->curr))
			return -ERESTARTSYS;

		mutex_lock(&state->lock);
	}

	size_t remaining = msg_len - *offset;
	size_t bytes_to_copy = min(remaining, len);
	bytes_to_copy = min(bytes_to_copy, (size_t)MAX_NUMBER);

	memcpy(tmp, state->buf + *offset, bytes_to_copy);

	mutex_unlock(&state->lock);

	if (copy_to_user(buf, tmp, bytes_to_copy))
		return -EFAULT;
	*offset += bytes_to_copy;

	return bytes_to_copy;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset){
	struct my_state *state = file->private_data;
	char tmp[MAX_NUMBER];

	size_t bytes_to_copy = min(sizeof(state->buf) - state->curr, len);
	if (copy_from_user(tmp, buf, bytes_to_copy))
		return -EFAULT;

	mutex_lock(&state->lock);

	pr_info(" %s, state ptr = %px\n", __func__, state);

	if (state->curr >= MAX_NUMBER) {
		mutex_unlock(&state->lock);
		return -ENOSPC;
	}

	bytes_to_copy = min(bytes_to_copy, sizeof(state->buf) - state->curr);
	memcpy(state->buf + state->curr, tmp, bytes_to_copy);

	state->curr += bytes_to_copy;

	/* Print buffer using length, avoids need for null termination */
	pr_info("Bytes written: %.*s\n", (int)state->curr, state->buf);

	mutex_unlock(&state->lock);

	wake_up_interruptible(&state->read_queue);

	return bytes_to_copy;
}

static int my_release(struct inode *inode, struct file *file){
	struct my_state *state = file->private_data;
	kfree(state);
	return 0;
}

static loff_t my_llseek(struct file *file, loff_t off, int whence)
{
	struct my_state *state = file->private_data;
	loff_t new_pos;

	mutex_lock(&state->lock);

	switch(whence){
		case(SEEK_SET):
			new_pos = off;
			break;

		case(SEEK_CUR):
			new_pos = file->f_pos + off;
			break;

		case(SEEK_END):
			new_pos = state->curr + off;
			break;

		default:
			return -EINVAL;
	}

	if(new_pos < 0 || new_pos > state->curr)
		return -EINVAL;

	file->f_pos = new_pos;

	mutex_unlock(&state->lock);
	return new_pos;
}
