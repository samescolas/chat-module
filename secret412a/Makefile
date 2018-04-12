obj-m += secret412a.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	-sudo rmmod secret412a
	sudo dmesg -C
	sudo insmod secret412a.ko
	dmesg
