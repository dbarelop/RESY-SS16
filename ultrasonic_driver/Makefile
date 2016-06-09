ifneq ($(KERNELRELEASE),)
obj-m := ultrasonic_driver.o

else
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
endif

clean:
	rm -rf *.ko *.o *.mod.c modules.order Module.symvers .tmp_versions .*.cmd *.dwo ultrasonic_driver
