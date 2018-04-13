#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("");
MODULE_DESCRIPTION("A simple module to facilitate encrypted 2-way communication.");
MODULE_VERSION("0.42");

#define DEVICE_NAME "secret412b"
#define CLASS_NAME "sec412"
#define EXAMPLE_MSG "Hello, World!\n"
#define MSG_BUFFER_LEN 1024

/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_num;
static int device_is_open = 0;
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;
static struct class* device_class = NULL;

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
 .owner = THIS_MODULE,
 .read = device_read,
 .write = device_write,
 .open = device_open,
 .release = device_release
};

/* Called when a process opens our device */
static int device_open(struct inode *inode, struct file *file)
{
	/* If device is open, return busy */
	//if (device_is_open)
	//{
		//return (-EBUSY);
	//}
	device_is_open++;
	return (0);
}

/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset)
{
	int bytes_read = 0;
	/* If we’re at the end, loop back to the beginning */
	if (*msg_ptr == 0)
	{
		return (0);
	}
	/* Put data in the buffer */
	while (len && *msg_ptr)
	{
		/* Buffer is in user data, not kernel, so you can’t just reference
		* with a pointer. The function put_user handles this for us */
		put_user(*(msg_ptr++), buffer++);
		len--;
		bytes_read++;
	}
	return (bytes_read);
}

/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset)
{
	int		i = 0;

	for (i=0; i < len && i < MSG_BUFFER_LEN; i++)
	{
		get_user(msg_buffer[i], buffer + i);
	}
	msg_buffer[i] = '\0';
	msg_ptr = msg_buffer;

	return (i);
}

/* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file)
{
	/* Decrement the open counter and usage count. Without this, the module would not unload. */
	device_is_open--;
	return (0);
}

static int __init secret412bmodule_init(void)
{
	/* Fill buffer with our message */
	strncpy(msg_buffer, EXAMPLE_MSG, MSG_BUFFER_LEN);
	/* Set the msg_ptr to the buffer */
	msg_ptr = msg_buffer;
	/* Try to register character device */
	major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
	if (major_num < 0)
	{
		printk(KERN_ALERT "Could not register device: %d\n", major_num);
		return (major_num);
	} 
	else 
	{
		printk(KERN_INFO "module loaded with device major number %d\n", major_num);
	}
	device_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(device_class))
	{
		unregister_chrdev(major_num, DEVICE_NAME);
	}
	return (0);
}

static void __exit secret412bmodule_exit(void)
{
	/* Unregister the character device. */
	class_destroy(device_class);
	unregister_chrdev(major_num, DEVICE_NAME);
}

/* Register module functions */
module_init(secret412bmodule_init);
module_exit(secret412bmodule_exit);
