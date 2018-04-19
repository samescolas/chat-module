To run, first create driver device with "make -C driver".
Next, insert the new module into kernel with "make test -C driver" (runs insmod, etc.)
Using output of previous command, find major number for created device. Run "mknod /dev/sec412a c MAJOR_NUM 0" and "mknod /dev/sec412b c MAJOR_NUM 1".
Finally, we can start up our chat program. Compile it with "make -C usermode".
Run in two separate terminals with "./usermode/chat a" and "./usermode/chat b".
You should be able to type and the messages should be encrypted, written to the driver, read by the other process, decrypted, and printed.
