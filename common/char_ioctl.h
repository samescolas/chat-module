/*********************************************************************
*
* This is a skeleton character device driver header file.
*
*********************************************************************/
#ifndef SEC412_OSS_CHAR_H
#define SEC412_OSS_CHAR_H

// Both usermode and the driver need this information
// so it makes more sense to include it here.
#define MAX_ALLOWED_LEN 1024

//
// This header is required for ioctl functionality
//
#include <linux/ioctl.h>

#define DEVICE_NAME "sec412_char"

//
// The below comments are taken from Documentation/ioctl/ioctl-number.txt
// within the Linux v4.15.2 source tree
//
// The "identifying letter or number" table was not copied. You can look
// at the above file for the complete table. For this exercise, you can
// pick a simple value
//
// ---------------------------------------------------------------------
// 
// If you are adding new ioctl's to the kernel, you should use the _IO
// macros defined in <linux/ioctl.h>:
// 
//     _IO    an ioctl with no parameters
//     _IOW   an ioctl with write parameters (copy_from_user)
//     _IOR   an ioctl with read parameters  (copy_to_user)
//     _IOWR  an ioctl with both write and read parameters.
// 
// 'Write' and 'read' are from the user's point of view, just like the
// system calls 'write' and 'read'.  For example, a SET_FOO ioctl would
// be _IOW, although the kernel would actually read data from user space;
// a GET_FOO ioctl would be _IOR, although the kernel would actually write
// data to user space.
// 
// The first argument to _IO, _IOW, _IOR, or _IOWR is an identifying letter
// or number from the table below.  Because of the large number of drivers,
// many drivers share a partial letter with other drivers.
// 
// If you are writing a driver for a new device and need a letter, pick an
// unused block with enough room for expansion: 32 to 256 ioctl commands.
// You can register the block by patching this file and submitting the
// patch to Linus Torvalds.  Or you can e-mail me at <mec@shout.net> and
// I'll register one for you.
// 
// The second argument to _IO, _IOW, _IOR, or _IOWR is a sequence number
// to distinguish ioctls from each other.  The third argument to _IOW,
// _IOR, or _IOWR is the type of the data going into the kernel or coming
// out of the kernel (e.g.  'int' or 'struct foo').  NOTE!  Do NOT use
// sizeof(arg) as the third argument as this results in your ioctl thinking
// it passes an argument of type size_t.
// 

#define SEC412_IOCTL_MAGIC 208 // picking 0xd0 

// base sequence number
#define SEC412_IOCTL_BASE_SEQ 0 

// read from kernel sequence number is base + 1
//#define SEC412_IOCTL_RFK (SEC412_IOCTL_BASE_SEQ + 1)

// write to kernel sequence number is base + 2
//#define SEC412_IOCTL_WTK (SEC412_IOCTL_BASE_SEQ + 2)

//
// _IOR() creates an IOCTL that is reading from the kernel
//        SEC412_IOCTL_MAGIC is the magic byte
//        
// The last argument might be better suited as a structure!
// 
#define IOCTL_READ_KEY_FROM_KERNEL _IOR(SEC412_IOCTL_MAGIC, 1, char *)
#define IOCTL_WRITE_KEY_TO_KERNEL _IOWR(SEC412_IOCTL_MAGIC, 2, char *)
#define IOCTL_READ_IV_FROM_KERNEL _IOR(SEC412_IOCTL_MAGIC, 3, char *)
#define IOCTL_WRITE_IV_TO_KERNEL _IOWR(SEC412_IOCTL_MAGIC, 4, char *)
#define IOCTL_READ_MSGLEN_FROM_KERNEL _IOR(SEC412_IOCTL_MAGIC, 5, char *)
#define IOCTL_WRITE_MSGLEN_TO_KERNEL _IOWR(SEC412_IOCTL_MAGIC, 6, char *)


#endif // SEC412_OSS_CHAR_H
