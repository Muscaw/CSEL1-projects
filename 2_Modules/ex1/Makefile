# Part executed when called from kernel build system
ifneq ($(KERNELRELEASE),)
obj-m += mymodule.o
mymodule-objs := skeleton.o

#Part exectued when called from standard make in the module source directory
else
CPU := arm
KDIR := /home/lmi/workspace/xu3/buildroot/output/build/linux-4.6.4/
TOOLS := /home/lmi/workspace/xu3/buildroot/output/host/usr/bin/arm-linux-gnueabihf-
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(CPU) CROSS_COMPILE=$(TOOLS) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

endif
