CURRENT_PATH := $(shell pwd)
KERNELDIR := $(CURRENT_PATH)/../../linux-imx-rel_imx_4.1.15_2.1.0_ga

obj-m := chr_dev.o

build:kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean