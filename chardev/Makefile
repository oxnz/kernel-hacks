kernel-release = $(shell uname -r)
obj-m += chardev.o

all:
	make -C /lib/modules/${kernel-release}/build M=$(PWD) modules

clean:
	make -C /lib/modules/${kernel-release}/build M=$(PWD) clean
