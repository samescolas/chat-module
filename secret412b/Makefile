obj-m += secret412b.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	-sudo rmmod secret412b
	sudo dmesg -C
	sudo insmod secret412b.ko
	dmesg
