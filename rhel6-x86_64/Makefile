# Location of Kernel source:
KERNELDIR := /lib/modules/$(shell uname -r)/build

obj-m := apci1710.o

apci1710-objs := Endat_1711-kapi.o
apci1710-objs += Endat_1711.o
apci1710-objs += biss.o
apci1710-objs += biss_1711-kapi.o
apci1710-objs += chronos-kapi.o
apci1710-objs += chronos.o
apci1710-objs += dig_io-kapi.o
apci1710-objs += dig_io.o
apci1710-objs += etm-kapi.o
apci1710-objs += fs.o
apci1710-objs += imp_cpt-kapi.o
apci1710-objs += imp_cpt.o
apci1710-objs += inc_cpt-kapi.o
apci1710-objs += inc_cpt.o
apci1710-objs += interrupt.o
apci1710-objs += ioctl.o
apci1710-objs += irq-kapi.o
apci1710-objs += irq.o
apci1710-objs += knowndev.o
apci1710-objs += main.o
apci1710-objs += procfs.o
apci1710-objs += reset_board-kapi.o
apci1710-objs += ssi.o
apci1710-objs += ssi-kapi.o
apci1710-objs += ttl-kapi.o
apci1710-objs += ttl.o
apci1710-objs += utils-kapi.o
apci1710-objs += utils.o
apci1710-objs += vtable.o

all: 
# Copy source code for compiling
	cp -r ../src/*.{c,h,ids} .
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
# Erase all files but Makefile
	find . ! -name 'Makefile' -type f -exec rm -f {} +
# Erase all directory but the root one
	find . ! -name '.' -type d -exec rm -rf {} +
