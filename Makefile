
obj-m := usb_driver.o

CC = gcc
CCFLAGS = -g -Wall -std=c99

KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules M=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~
