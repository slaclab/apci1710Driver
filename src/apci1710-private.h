#ifndef __apci1710_PRIVATE__
#define __apci1710_PRIVATE__

/** @file apci1710-private.h
*
* @author Krauth Julien
*
* This header contains private definitions (not to be seen by external code)
*/

/** @par LICENCE
* @verbatim
    Copyright (C) 2009  ADDI-DATA GmbH for the source code of this module.

    ADDI-DATA GmbH
    Airpark Business Center
    Airport Boulevard B210
    77836 Rheinmuenster
    Germany
    Tel: +49(0)7229/1847-0
    Fax: +49(0)7229/1847-200
    http://www.addi-data-com
    info@addi-data.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You shoud find the complete GPL in the COPYING file accompanying
    this source code.
* @endverbatim
*/

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
	#include <linux/config.h>
#else
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
		#include <linux/autoconf.h>
	#else
		#include <generated/autoconf.h>
	#endif
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
	#include <linux/smp_lock.h>
#endif

#include <linux/spinlock.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/pci.h> // struct pci_dev
#include <asm/io.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)
	#include <asm/system.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 139)
    #include <linux/uaccess.h>
    #include <linux/uaccess.h>
#else
    #include <asm/uaccess.h>
    #include <asm/bitops.h>
#endif

#include <linux/sched.h>

#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

#include "apci1710.h"
#include "apci1710-kapi.h"

#ifndef __GNUC__
#define __inline__ inline
#endif

/** ressources of a board */

static __inline__ unsigned long GET_BAR0(struct pci_dev * dev)
{
	return dev->resource[0].start;
}

static __inline__ unsigned long GET_BAR1(struct pci_dev * dev)
{
	return dev->resource[1].start;
}

static __inline__ unsigned long GET_BAR2(struct pci_dev * dev)
{
	return dev->resource[2].start;
}

static __inline__ unsigned long GET_BAR3(struct pci_dev * dev)
{
	return dev->resource[3].start;
}

/** Utility fonction that returns 0 if the given index corresponds to a card already configured.
 *
 * @param count Number of board
 * @param index Minor number to check
 *
 * @return 0 if minor number valid, not 0 otherwise
 *
 * count ranges from 1 to ...
 * index ranges from 0 to ...
 *
 * */

static __inline__ int apci1710_INDEX_NOT_VALID(atomic_t * count , unsigned int index )
{
	volatile unsigned int __count =  atomic_read(count);
	if (index >= __count) return 1;
	return 0;
}

//------------------------------------------------------------------------------
static __inline__ void OUTP (unsigned long ui_Address, uint32_t dw_Offset, uint8_t b_ByteValue)
{
	outb (b_ByteValue, (ui_Address + dw_Offset));
}

static __inline__ void OUTPW (unsigned long ui_Address, uint32_t dw_Offset, uint32_t ui_WordValue)
{
	outw (ui_WordValue, (ui_Address + dw_Offset));
}

static __inline__ void OUTPDW (unsigned long ui_Address, uint32_t dw_Offset, uint32_t ul_LongValue)
{
	outl (ul_LongValue, (ui_Address + dw_Offset));
}

static __inline__ void INP (unsigned long ui_Address, uint32_t dw_Offset, uint8_t * pb_ByteValue)
{
	*pb_ByteValue = inb ((ui_Address + dw_Offset));
}

static __inline__ void INPW (unsigned long ui_Address, uint32_t dw_Offset, uint32_t * pui_WordValue)
{
	*pui_WordValue = inw ((ui_Address + dw_Offset));
}

static __inline__ void INPDW (unsigned long ui_Address, uint32_t dw_Offset, uint32_t * pul_LongValue)
{
	*pul_LongValue = inl ((ui_Address + dw_Offset));
}
//------------------------------------------------------------------------------

/* configuration flags */
#define APCI1710_SAVE_INTERRUPT		1000

//------------------------------------------------------------------------------

/* number of board detected by the kernel */
extern atomic_t apci1710_count;

#define __DRIVER_NAME "apci1710"

/* major number (attributed by the OS) */
extern unsigned int apci1710_majornumber;



/* /dev function */
int apci1710_fasync_lookup(int fd, struct file *filp, int mode);
int apci1710_open_lookup (struct inode *inode, struct file *filp);
int apci1710_release_lookup (struct inode *inode,struct file *filp);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	int apci1710_ioctl_lookup (struct inode *inode,
							   struct file *filp,
							   unsigned int cmd,
							   unsigned long arg);
#else
	long apci1710_ioctl_lookup (struct file *filp,
								unsigned int cmd,
								unsigned long arg);
#endif

/*/proc functions  */
void apci1710_proc_init(void);
void apci1710_proc_release(void);
void apci1710_proc_create_device(struct pci_dev * dev, unsigned int minor_number);
void apci1710_proc_release_device(struct pci_dev * dev);

/* interrupt related function */
int apci1710_register_interrupt(struct pci_dev * pdev);
int apci1710_deregister_interrupt(struct pci_dev * pdev);

#include "api.h"
#include "privdata.h"


#endif //__apci1710_PRIVATE__
