/** @file fs.c
*
* @author Sylvain Nahas
*
* This module provides FS related function implementation.
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

#include "apci1710-private.h"

EXPORT_NO_SYMBOLS;

//------------------------------------------------------------------------------

/** Asynchronous signal.
 *
 */
int apci1710_fasync_lookup(int fd, struct file *filp, int mode)
{
	return fasync_helper(fd, filp, mode, & (APCI1710_PRIVDATA(filp->private_data)->async_queue) );
}

//------------------------------------------------------------------------------

/** open() function of the module for the APCI-XXXX, with lookup though global OS PCI list 
* this is for files unmanaged by the driver itself and use the minor device number to identify the board.
*
* When opening, the pci_dev associated to the minor number is looked up
* and associated with the file structure. It avoid further lookup when calling ioctl()
* 
*
*/
int apci1710_open_lookup (struct inode *inode, struct file *filp)
{
   if ( apci1710_INDEX_NOT_VALID(&apci1710_count, MINOR(inode->i_rdev) ) )
   {
   	return -ENODEV;
   }
      
   filp->private_data = apci1710_lookup_board_by_index(MINOR(inode->i_rdev) );  
   
   MOD_INC_USE_COUNT;
   return 0;   
}
//------------------------------------------------------------------------------
/** release() function of the module for the APCI-XXXX, with lookup though global OS PCI list 
* this is for files unmanaged by the driver itself and use the minor device number to identify the board
*/
int apci1710_release_lookup (struct inode *inode,struct file *filp)
{

   if ( apci1710_INDEX_NOT_VALID(&apci1710_count, MINOR(inode->i_rdev) ) )
   {
   	return -ENODEV;
   }

   MOD_DEC_USE_COUNT;
   return 0;
}
//------------------------------------------------------------------------------
/** ioctl() function of the module for the APCI-XXXX, with lookup though global OS PCI list 
* this is for files unmanaged by the driver itself and use the minor device number to identify the board
*/

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	int apci1710_ioctl_lookup (struct inode *inode,
							   struct file *filp,
							   unsigned int cmd,
							   unsigned long arg)
#else
	long apci1710_ioctl_lookup (struct file *filp,
								unsigned int cmd,
								unsigned long arg)
#endif
	{
		int ret = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)
		struct inode *inode = filp->f_path.dentry->d_inode;
	#else
		struct inode *inode = filp->f_dentry->d_inode;
	#endif
#endif

   if ( apci1710_INDEX_NOT_VALID(&apci1710_count, MINOR(inode->i_rdev) ) )
   {
   	return -ENODEV;
   }  
     
   if (!filp->private_data) // private data is initialised to NULL
   {
   	printk(KERN_CRIT "%s: %s: board data should be affected but is not (did you call open() before ioctl() ?) \n",__DRIVER_NAME, __FUNCTION__);
	return -EBADFD;
   }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
   lock_kernel();
#endif

	   ret = apci1710_do_ioctl( (struct pci_dev*) filp->private_data, cmd, arg);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
   unlock_kernel();
#endif

   return ret;
}

