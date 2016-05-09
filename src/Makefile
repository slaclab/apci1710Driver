# the ADDIDATAINCLUDE variable may hold the path to the addidata-specific include directory
ifeq ($(ADDIDATAINC),)
# include directory has not been specified. suppose we are located inside the Linux kernel source tree
EXTRA_CFLAGS:=-I$(TOPDIR)/include/linux/addidata/ 
else
# use specified location
EXTRA_CFLAGS:=-I$(ADDIDATAINC) -Werror
#EXTRA_CFLAGS:=-I$(ADDIDATAINC) -Wno-unused-but-set-variable -Werror
endif

# no ADDI-DATA legacy types
EXTRA_CFLAGS+=-D_ADDI_NO_WIN32_COMPAT

# Each configuration option enables a list of files.
obj-$(CONFIG_apci1710_IOCTL) += apci1710.o

# list of objects that make the module
apci1710-objs := knowndev.o main.o fs.o procfs.o ioctl.o interrupt.o vtable.o inc_cpt.o dig_io.o irq.o inc_cpt-kapi.o utils-kapi.o irq-kapi.o dig_io-kapi.o utils.o reset_board-kapi.o ttl.o ttl-kapi.o ssi.o ssi-kapi.o imp_cpt.o imp_cpt-kapi.o chronos.o chronos-kapi.o etm-kapi.o biss.o biss_1711-kapi.o Endat_1711.o Endat_1711-kapi.o

ifneq ($(WITH_BALISE_OPTION),)
apci1710-objs += customer/balise/balise-kapi.o customer/balise/balise.o

EXTRA_CFLAGS+=-DWITH_BALISE_OPTION
endif

ifneq ($(WITH_EL_TIMERS_OPTION),)
apci1710-objs += customer/el_timers/el_timers-kapi.o customer/el_timers/el_timers.o

EXTRA_CFLAGS+=-DWITH_EL_TIMERS_OPTION
endif

ifneq ($(WITH_IDV_OPTION),)
apci1710-objs += customer/idv/idv-irq-kapi.o customer/idv/idv-kapi.o customer/idv/idv.o

EXTRA_CFLAGS+=-DWITH_IDV_OPTION
endif


# KERNEL 2.4 ------------------------------------------
ifeq ($(VERSION),2)
ifeq ($(PATCHLEVEL),4)

# The target object and module list name.
O_TARGET	:= driver.o

# Objects that export symbols.
export-objs	:= knowndev.o main.o fs.o procfs.o ioctl.o interrupt.o vtable.o inc_cpt.o dig_io.o irq.o inc_cpt-kapi.o utils-kapi.o irq-kapi.o dig_io-kapi.o utils.o reset_board-kapi.o ttl.o ttl-kapi.o ssi.o ssi-kapi.o imp_cpt.o imp_cpt-kapi.o chronos.o chronos-kapi.o etm-kapi.o biss.o biss_1711-kapi.o Endat_1711.o Endat_1711-kapi.o
    

# The global Rules.make.
include $(TOPDIR)/Rules.make

# Link rules
apci1710.o: $(apci1710-objs)
	$(LD) -r -o $@ $(apci1710-objs)
	
endif
endif
