# Part executed when called from kernel build system
ifneq ($(KERNELRELEASE),)
obj-m += mymodule.o
mymodule-objs := skeleton.o

#Part exectued when callaed from standard make in the module source directory
else
CPU := arm
KDIR := /home/lmi/workspace/xu3/buildroot/output/build/linux-4.6.4/
TOOLS := /home/lmi/workspace/xu3/buildroot/output/host/usr/bin/arm-linux-gnueabihf-
PWD := $(shell pwd)
#MODPATH := $(HOME)/workspace/buidroot/output/target # PAS NFS DEV
MODPATH := /var/lib/tftpboot/odroidxu3/rootfs

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(CPU) CROSS_COMPILE=$(TOOLS) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	$(MAKE) -C $(KDIR) M=$(PWD) INSTALL_MOD_PATH=$(MODPATH)  modules_install

endif
