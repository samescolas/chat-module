/*********************************************************************
*
* This is a skeleton character device driver.
*
*
*********************************************************************/
#include <linux/init.h>           

// Contains types, macros, functions for the kernel
#include <linux/kernel.h>         

// The header for loadable kernel modules
#include <linux/module.h>         

// The header for kernel device structures 
#include <linux/device.h>         

// The header for the Linux file system support
#include <linux/fs.h>             

// Required for the copy to user function
#include <linux/uaccess.h>        

#include <linux/semaphore.h>

// Using a common header file for usermode/kernel mode code
#include "../common/char_ioctl.h"


#define  CLASS_NAME  "sec412"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mario");
MODULE_DESCRIPTION("Linux character device driver for OSS");
MODULE_VERSION("4.10.2018");

//
// Setup some global variables for your device
// Note: Below is not an accurate/complete list for the assignment
// You will need to modify these in order to complete the assignment
//

#define MAX_ALLOWED_LEN 1024
#define TYPE(dev) (MINOR(dev) >> 4) /* high nibble */
#define NUM(dev) (MINOR(dev) & 0xf) /* low nibble */

#define MSG0 "hey there\n"
#define MSG1 "sup dood\n"

static int    g_majornum;
static char   g_buffer[MAX_ALLOWED_LEN] = {0};

/*
	These will represent the inboxes for the devices.
	We will use minor numbers to find the appropriate inbox.
*/

typedef struct inbox {
	char	data[MAX_ALLOWED_LEN];
	struct	semaphore sem;
} inbox;

static inbox	g_inboxes[2];
static int		g_minor = 0;

static struct class*  sec412_class  = NULL;
static struct device* sec412a_device = NULL;
static struct device* sec412b_device = NULL;

//
// Relevant function prototypes that will be used
// for the file_operations structure
//
static int     dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long    dev_ioctl(struct file *, unsigned int, unsigned long );
static int     dev_release(struct inode *, struct file *);


//
// Setup the relevant fields within the file_operations structure
// Any non-filled in values will use the system defaults
//
static struct file_operations fops =
{
   owner: THIS_MODULE,
   open: dev_open,
   read: dev_read,
   write: dev_write,
   compat_ioctl: dev_ioctl,
   unlocked_ioctl: dev_ioctl,
   release: dev_release,
};

// 
// This path is called when the module is being loaded
//
static int __init sec412_char_init(void)
{

   //
   // Register the device dynamically
   //
   g_majornum = register_chrdev(0, DEVICE_NAME, &fops);
   memset(g_inboxes[0].data, 0, MAX_ALLOWED_LEN);
   memset(g_inboxes[1].data, 0, MAX_ALLOWED_LEN);
   strncpy(g_inboxes[0].data, MSG0, MAX_ALLOWED_LEN);
   strncpy(g_inboxes[1].data, MSG1, MAX_ALLOWED_LEN);

   //sema_init(&g_inboxes[0].sem, 0);
   //sema_init(&g_inboxes[1].sem, 0);

   if( g_majornum < 0 )
   {
	  printk(KERN_ALERT "Could not register device: %d\n", g_majornum);
      return g_majornum;
   }

   printk("[+] Successfully registered device with major number %d\n", g_majornum);

   //
   // Create the device class
   // 
   sec412_class = class_create(THIS_MODULE, CLASS_NAME);

   if( IS_ERR(sec412_class) )
   { 

      unregister_chrdev(g_majornum, DEVICE_NAME);

      printk("[-] Failed to create device class\n");

      return PTR_ERR(sec412_class);          
   }

   printk("[+] Successfully created the device class\n");

   // 
   // create the device now
   //

   // 
   // NOTE:
   // The MKDEV takes a major/minor pair and creates an appropriate device number
   //
   sec412a_device = device_create(sec412_class, NULL, MKDEV(g_majornum, 0), NULL, DEVICE_NAME);
   if( IS_ERR(sec412a_device))
   {
      class_destroy(sec412_class);

      unregister_chrdev(g_majornum, DEVICE_NAME);

      printk("[-] Failed to create device class\n");

      return PTR_ERR(sec412a_device);
   }

   //sec412b_device = device_create(sec412_class, NULL, MKDEV(g_majornum, 1), NULL, DEVICE_NAME);

   //if( IS_ERR(sec412b_device))
   //{
	  //device_destroy(sec412_class, MKDEV(g_majornum, 0));     
      //class_destroy(sec412_class);
//
      //unregister_chrdev(g_majornum, DEVICE_NAME);
//
      //printk("[-] Failed to create device class\n");
//
      //return PTR_ERR(sec412b_device);
   //}

   printk("[+] Module successfully initialized\n");

   return 0;

}

// 
// This path is called when the module is being unloaded
//
static void __exit sec412_char_exit(void)
{

   printk("[*] Unloading the module\n");

   // destroy the created device
   device_destroy(sec412_class, MKDEV(g_majornum, 0));     
   //device_destroy(sec412_class, MKDEV(g_majornum, 1));     

   // unregister the class
   class_unregister(sec412_class);                          

   // unregister the character device
   unregister_chrdev(g_majornum, DEVICE_NAME);            


}

static int dev_open(struct inode *inode, struct file *filp)
{
   inbox *i = NULL;
   g_minor = NUM(inode->i_rdev);

   /*
   if (!capable(CAP_SECRET_FOURONETWO))
   {
   	 return -ENOPERM;
   }
   */
   if (g_minor >= 0)
   {
     i = &g_inboxes[g_minor];
   }

   // Assign correct inbox to file.
   filp->private_data = i;
   return 0;
}


//
// This path is called when read() is made on the file descriptor
// That is, the user mode program is expected to read data from
// this device
//
static ssize_t dev_read(struct file *filp, char *buffer, size_t len, loff_t *offset)
{

   //int error = -1;
   inbox	*inbox = filp->private_data;

   //if (down_interruptible(&inbox->sem))
   //{
   	 //return -ERESTARTSYS;
   //}
   if (*inbox->data == 0)
   {
	 //up(&inbox->sem);
     return (0);
   }
   // Make sure you are only reading the requested amount!
   //error= copy_to_user(buffer, KERNEL_SOURCE, KERNEL_SOURCE_SIZE);
   copy_to_user(buffer, inbox->data, MAX_ALLOWED_LEN);
   memset(inbox->data, 0, MAX_ALLOWED_LEN);
   //while (len && *g_inboxp)
   //{
     //put_user(g_inboxp[bytes_read], buffer++);
	 //g_inboxp[bytes_read++] = '\0';
	 //len--;
   //}

   //up(&inbox->sem);
   return (MAX_ALLOWED_LEN);
}

//
// This path is called when write() is made on the file descriptor
// That is, the user mode program is passing data to this function
//
static ssize_t dev_write(struct file *filp, const char *buffer, size_t len, loff_t *offset)
{
   int		bytes_read = 0;
   inbox	*inbox = filp->private_data;

   //if (down_interruptible(&inbox->sem))
   //{
   		//return -ERESTARTSYS;
   //}
   for (bytes_read=0; bytes_read < len && bytes_read < MAX_ALLOWED_LEN; bytes_read++)
   {
   		get_user(inbox->data[bytes_read], buffer + bytes_read);
   }
   inbox->data[bytes_read] = '\0';
   //up(&inbox->sem);

   return (bytes_read);
}

long dev_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    int i = 0;
    int error = 0;
    char *temp = NULL;
    char ch;

    printk("[*] Usermode is requesting %08x ioctl\n", ioctl_num);

    switch (ioctl_num) 
    {
        case IOCTL_READ_FROM_KERNEL:
            printk("[*]    IOCTL_READ_FROM_KERNEL\n");
            // 
            //  The code below is not safe..be sure to fix it properly
            //  if you use it
            // 
            temp = (char *)ioctl_param;
            error= copy_to_user(temp, g_buffer, MAX_ALLOWED_LEN);

            printk("[+]    The message is %s\n", g_buffer );

            break;

        case IOCTL_WRITE_TO_KERNEL:
            printk("[*]    IOCTL_WRITE_TO_KERNEL\n");
            temp = (char *)ioctl_param;
            get_user(ch, temp);
            for (i = 0; ch && i < MAX_ALLOWED_LEN; i++, temp++)
                get_user(ch, temp);

            // 
            //  The code below is not safe..be sure to fix it properly
            //  if you use it
            // 
            memset( g_buffer, 0, MAX_ALLOWED_LEN );
            error= copy_from_user(g_buffer, (char*)ioctl_param, i);

            printk("[+]    The length passed in is %d\n", i );
            printk("[+]    The message is %s\n", g_buffer );

            break;

        default:
            break;

    }

    return 0;
}


static int dev_release(struct inode *inode, struct file *filp){
   //
   // This path is called when the file descriptor is closed
   //

   printk("[*] Releasing the file\n");

   return 0;
}

module_init(sec412_char_init);
module_exit(sec412_char_exit);
