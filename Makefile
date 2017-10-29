ifneq ($(KERNELRELEASE),)
    export-objs = hello.o
    obj-m = hello.o
else
	KERNELDIR :=  /lib/modules/$(shell uname -r)/build
	PWD       := $(shell pwd)
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(RM) -r modules.order Module.symvers .tmp_versions .*.cmd *.o *.ko *.mod.c
endif
