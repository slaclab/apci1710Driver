/** @file procfs.c
*
* @author Sylvain Nahas
* @author Krauth Julien
*
* This module provides procfs related functions
*/

/** @par LICENCE
* @verbatim
    Copyright (C) 2009  ADDI-DATA GmbH for the source code of this module.
        
    ADDI-DATA GmbH
    Airpark Business Center
    Airport Boulevard B210
    77836 Rheinmünster
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

#include <linux/proc_fs.h>

EXPORT_NO_SYMBOLS;

// Disable proc
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
static struct proc_dir_entry * apci1710_proc_root = NULL;
#endif

//----------------------------------------------------------------------------------------------------------------------

// Disable proc
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)

/** Recursively delete a /proc tree whom top entry is given as parameter.
 * 
 * Release previously allocated ressources and delete entries in /proc.
 * This function implement a simple a go-left-first, go-down-then recursive 
 * algorithm through the given proc entry.
 * It frees driver data associated to proc entry and then removes proc entry
 * If top is true, the "next" pointer is not gone though.
 * 
 * @arg[in] item A pointer to the proc entry to remove
 * @arg[in] top A flag indicating if this entry is the top of the hierarchy 
 * @arg[in] clean A flag indicating if we want to kfree() the data associated as well 
 * 
 */

static void proc_release_under(struct proc_dir_entry * item, unsigned int top, unsigned int clean)
{
                
        if (!item) return;
                
        // next first
        if ( (!top) && item->next)
                proc_release_under(item->next,0,clean);

        // sub dir then
        if (item->subdir)
                proc_release_under(item->subdir, 0,clean);
                
        // release data
        if ( (item->data) && (clean) )
        {
                kfree(item->data);
                item->data = NULL;
        }
        // release proc item itself
        remove_proc_entry(item->name,item->parent);
        
}

//----------------------------------------------------------------------------------------------------------------------
/** Search an entry by its name in the sub-directory. */
static struct proc_dir_entry * proc_find_entry_by_name(struct proc_dir_entry * top, const char * name)
{
        struct proc_dir_entry * tmp = NULL;     

        if (!top) return NULL;
        if (!name) return NULL;
                                
        tmp=top->subdir;
        
        while(tmp)
        {
                if ( strcmp(tmp->name,name) == 0 ) /* same name, we found it*/
                        return tmp;
                tmp = tmp->next;
        }
        
        /* arrived here, found nothing */
        return NULL;
}

#endif // #if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)

#ifdef CONFIG_APCI_PROC_FS_SIMPLE 

#error "Sorry, CONFIG_APCI_PROC_FS_SIMPLE option is deprecated and shouldn't be used!"

	//----------------------------------------------------------------------------------------------------------------------
	/** Create a simple "device" entry in the /proc/apci1710/ root directory. 
	* 
	* @param dev Pointer to the device entry to be associated to the node.
	* @param major_number The minor number to be associated to this device.
	* 
	* @note this would not work with 2.6 - in any way this function was for compatibility with early devices
	* 
	*
	*/
	void apci1710_proc_create_device(struct pci_dev * dev, unsigned int minor_number)
	{
		struct proc_dir_entry * entry = NULL;

			if (!apci1710_proc_root) return;
			// apci1710_proc_create_device is available only for 2.4
			#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
			{
				entry = proc_mknod(	pci_name(dev), 
					S_IFCHR | 0666,
					apci1710_proc_root, 
					MKDEV(apci1710_majornumber, minor_number));
			}
			#else // 2.6 : special device are no more available :( - create a normal proc entry
			{
				entry = create_proc_entry("ctl",0,NULL,NULL);		
			}		
			#endif 
			
			if (!entry) 
			{
				printk(KERN_WARNING "%s: can't create proc entry for board %s(ENOMEM)\n",__DRIVER_NAME,pci_name(dev));
				return;
			}

			#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
				entry->owner = THIS_MODULE;
			#endif
			entry->data = dev;
		
			return;
	}

#else // !CONFIG_APCI_PROC_FS_SIMPLE
	
//----------------------------------------------------------------------------------------------------------------------
/** Create a directory in the /proc/apci1710/ root directory named after the board slot.
*
* @param dev Pointer to the device entry to be associated to the node.
* @param major_number The minor number to be associated to this device.
*
* @note The file "ctl" that is created there is a special device file - work only with kernel 2.4 :)
*/
void apci1710_proc_create_device(struct pci_dev * dev, unsigned int minor_number)
{
	// Disable proc
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
		return;
	#else

	struct proc_dir_entry * entry = NULL;

	if (!apci1710_proc_root) return ;

	/* create the board directory */
	{
		entry = proc_mkdir(	pci_name(dev), apci1710_proc_root);
	
		if (!entry)
		{
			printk(KERN_WARNING "%s: can't create proc entry for board %s(ENOMEM)\n",__DRIVER_NAME,pci_name(dev));
			return;
		}

		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
			entry->owner = THIS_MODULE;
		#endif
		entry->data = dev;
	}
	/* create a control special device file */
	{
		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		{
			entry = proc_mknod(	"ctrl",
					S_IFCHR | 0666,
					entry,
					MKDEV(apci1710_majornumber, minor_number));

			entry->owner = THIS_MODULE;
			entry->data = dev;
		}
		#else // 2.6 : special device are no more available :( - create a normal proc entry
		{	
			 /*
			  * Devices files are not allowed under /proc with kernel > 2.6.
			  * So a link on a device node is set.
			  * mkdevs.sh or udev script can be used to set the
			  * /dev/apci1710/minor_number file.
			  */
			 char path[100];
			 sprintf(path, "/dev/apci1710/%i", minor_number);
			 entry = proc_symlink("ctrl",entry,path);
			 #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
				entry->owner = THIS_MODULE;
			 #endif

		}
		#endif

		if (!entry)
		{
			printk(KERN_WARNING "%s: can't create proc entry for board %s(ENOMEM)\n",__DRIVER_NAME,pci_name(dev));
			return;
		}
	}
	return;
#endif
	}
#endif //CONFIG_APCI_PROC_FS_SIMPLE



//----------------------------------------------------------------------------------------------------------------------
/** Remove a "device" entry in the /proc/apci1710/ root directory. 
 */
void apci1710_proc_release_device(struct pci_dev * dev)
{
// Disable proc
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
	return;
#else
	struct proc_dir_entry * entry = NULL;

	entry = proc_find_entry_by_name(apci1710_proc_root, pci_name(dev));
	if (!entry) return;

	proc_release_under(entry,1,0);
	
	return;
#endif
}



//------------------------------------------------------------------------------
/** Create the main root directory in /proc.
 * 
 * @param[in] root Pointer to the root directory (/proc/sys/apci3701, globally owned by the module)
 * @arg[in] dev Pointer to the system card structure
 * 
 * @return 
 * */
void apci1710_proc_init(void)
{
// Disable proc
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
	return;
#else
	apci1710_proc_root = proc_mkdir("sys/apci1710", NULL );
        
	if (!apci1710_proc_root) 
	{
		printk(KERN_WARNING "%s: can't create /proc/sys/apci1710\n", __DRIVER_NAME);
		return;
	}

	{
		
	}	
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
		apci1710_proc_root->owner = THIS_MODULE;
	#endif
	apci1710_proc_root->data = NULL;
#endif
}

//------------------------------------------------------------------------------
/** Delete all entries in /proc.
 * 
 * @return 
 * */
void apci1710_proc_release(void)
{	
	// Disable proc
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
	return;
#else
	proc_release_under(apci1710_proc_root,1,0);
#endif
}
