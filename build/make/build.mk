ARCH := arm
CROSS_COMPILE := $(DBG_CROSS_COMPILE)
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
KERNELDIR := $(DBG_KERNELDIR)
INSTALLDIR := $(DBG_COPY_DIR)
PWD := $(shell pwd)
KBUILD_EXTRA_SYMBOLS := $(DBG_SOC_PATH)/$(DBG_SOC)/Module.symvers $(DBG_COM_API_PATH)/Module.symvers

include $(DBG_SOC_PATH)/$(DBG_SOC)/conf/config_driver

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=arm  CROSS_COMPILE=$(DBG_CROSS_COMPILE) modules

clean:
	rm -rf *.o *~ *.depend *.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers *.orig *.tmp

