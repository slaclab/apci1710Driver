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
#include "vtable.h"

#include "knowndev.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <linux/sysfs.h>
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ADDI-DATA GmbH <info@addi-data.com>");
MODULE_DESCRIPTION("APCI-1710 IOCTL driver");

EXPORT_SYMBOL(apci1710_get_lock);

EXPORT_NO_SYMBOLS;

#ifndef __devinit
#define __devinit
#define __devexit
#define __devinitdata
#define __devexit_p
#endif

//------------------------------------------------------------------------------
spinlock_t * apci1710_get_lock(struct pci_dev *pdev)
{
        return &(APCI1710_PRIVDATA(pdev)->lock);
}
//------------------------------------------------------------------------------
atomic_t apci1710_count = ATOMIC_INIT(0);
unsigned int apci1710_majornumber = 0;

//------------------------------------------------------------------------------
static int __devinit apci1710_probe_one(struct pci_dev *dev,const struct pci_device_id *ent);
static void __devexit apci1710_remove_one(struct pci_dev *dev);

//------------------------------------------------------------------------------
/** The ID table is an array of struct pci_device_id ending with a all-zero entry.
Each entry consists of:

	- vendor, device	Vendor and device ID to match (or PCI_ANY_ID)
	- subvendor,	Subsystem vendor and device ID to match (or PCI_ANY_ID)
	- subdevice class,		Device class to match. The class_mask tells which bits
	- class_mask	of the class are honored during the comparison.
	- driver_data	Data private to the driver.
*/

static struct pci_device_id apci1710_pci_tbl[] __devinitdata = {
		#include "devices.ids"
		{ 0 },	/* terminate list */
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
static struct class * apci1710_class = NULL;
#endif

MODULE_DEVICE_TABLE (pci, apci1710_pci_tbl);

//----------------------------------------------------------------------------------------------------------------------
static struct pci_driver apci1710_pci_driver = {
	.name		= __DRIVER_NAME,
	.probe		= apci1710_probe_one,
	.remove		= __devexit_p(apci1710_remove_one),
	.id_table	= apci1710_pci_tbl,
};
//------------------------------------------------------------------------------
static struct file_operations apci1710_fops =
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	.ioctl				= apci1710_ioctl_lookup,
#else
	.unlocked_ioctl		= apci1710_ioctl_lookup,
#endif
	.open		= apci1710_open_lookup,
	.release	= apci1710_release_lookup,
	.fasync		= apci1710_fasync_lookup,
};

//------------------------------------------------------------------------------
/** when module is unloaded, stop all board activities (cf interrupt)*/
static void apci1710_stop_board(struct pci_dev * pdev)
{
	i_APCI1710_ResetBoardIntRoutine (pdev);
}

//-------------------------------------------------------------------
/** event: a new card is detected.
 *
 * Historically - in former ADDI-DATA drivers - data about a board has been stored
 * in a static structure.
 * This led to huge duplication of information since most of these information
 * are already present in the pci_dev struct.
 *
 * Now we manipulate this standard structure provided by the OS and we attach
 * private data using the field driver_data, if necessary.
 *
 *  */
static int __devinit apci1710_probe_one(struct pci_dev *dev, const struct pci_device_id *ent)
{

	{
		int ret = pci_enable_device(dev);
		if (ret)
		{
			printk(KERN_ERR "%s: pci_enable_device failed\n",__DRIVER_NAME);
			return ret;
		}
	}

	// needed to enable PCI DMA
	// DMA not used with the APCI-1710.
	//pci_set_master(dev);

	/* 2.4 only : check_region is deprecated in 2.6 */
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		/* check if BAR 0 is free */
		if ( check_region(dev->resource[0].start, pci_resource_len(dev,0)) )
		{
			printk(KERN_WARNING "%s: BAR 0 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[0].start);
			return -EBUSY;
		}
		/* check if BAR 1 is free */
		if ( check_region(dev->resource[1].start, pci_resource_len(dev,1)) )
		{
			printk(KERN_WARNING "%s: BAR 1 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[1].start);
			return -EBUSY;
		}
		/* check if BAR 2 is free */
		if ( check_region(dev->resource[2].start, pci_resource_len(dev,2)) )
		{
			printk(KERN_WARNING "%s: BAR 2 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[2].start);
			return -EBUSY;
		}
		/* check if BAR 3 is free */
		if (dev->device == apcie1711_BOARD_DEVICE_ID)
		{
			if ( check_mem_region(dev->resource[3].start, pci_resource_len(dev,3)) )
			{
				printk(KERN_WARNING "%s: BAR 3 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[3].start);
				return -EBUSY;
			}
		}
	#endif // 2.4

	/* allocate a new data structure containing board private data */
	{
		struct apci1710_str_BoardInformations * newboard_data = NULL;
		newboard_data = kmalloc( sizeof( struct apci1710_str_BoardInformations) , GFP_ATOMIC);
		if (!newboard_data)
		{
			printk(KERN_CRIT "Can't allocate memory for new board %s\n",pci_name(dev));
			return -ENOMEM;
		}
		/* link standard data structure to the board's private one */
		pci_set_drvdata(dev,newboard_data);

		apci1710_init_priv_data(newboard_data);
	}

	/* lock BAR IO ports ressources */
	{
		int ret = pci_request_regions(dev,__DRIVER_NAME);
		if (ret)
		{
			printk(KERN_ERR "%s: pci_request_regions failed\n",__DRIVER_NAME);
			/* free all allocated ressources here*/
			kfree(APCI1710_PRIVDATA(dev));
			return ret;
		}
	}


	/* map BAR3 */
	if (dev->device == apcie1711_BOARD_DEVICE_ID)
	{
		APCI1710_PRIVDATA(dev)->memBaseAddress3 = ioremap(dev->resource[3].start, pci_resource_len(dev,3));
	}

	{
		/* increase the global board count */
		atomic_inc(&apci1710_count);
		printk(KERN_INFO "%s: board %s managed (minor number will be %d)\n",__DRIVER_NAME, pci_name(dev), atomic_read(&apci1710_count)-1);
	}

	/* create /proc entry */
	apci1710_proc_create_device(dev, atomic_read(&apci1710_count)-1);

	apci1710_known_dev_append(dev);

    /* Read the board configuration */
	i_APCI1710_ReadModulesConfiguration(dev);
	i_APCI1710_DumpModulesFunctionalities (dev);

	/* register interrupt */
	 if ( apci1710_register_interrupt(dev) )
	 {
	 	/* failed, clean previously allocated resources */
		if (dev->device == apcie1711_BOARD_DEVICE_ID)
			iounmap(APCI1710_PRIVDATA(dev)->memBaseAddress3);
	 	kfree(APCI1710_PRIVDATA(dev));
	 	pci_release_regions(dev);
	 }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	while(1)
	{
		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
			struct class_device *cdev;
		#else
			struct device *cdev;
		#endif

		int minor = (atomic_read(&apci1710_count)-1);

		/* don't execute if class not exists */
		if (IS_ERR(apci1710_class))
			break;

		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
			cdev = class_device_create (apci1710_class, NULL, MKDEV(apci1710_majornumber, minor), NULL, "%s_%d", __DRIVER_NAME, minor);
		#elif LINUX_VERSION_CODE > KERNEL_VERSION(2,6,26)
			cdev = device_create (apci1710_class, NULL, MKDEV(apci1710_majornumber, minor), NULL, "%s_%d", __DRIVER_NAME, minor);
		#else /* If kernel 2.6.26 */
			cdev = device_create (apci1710_class, NULL, MKDEV(apci1710_majornumber, minor), "%s_%d", __DRIVER_NAME, minor);
		#endif

		if (IS_ERR(cdev))
		{
			printk (KERN_WARNING "%s: class_device_create error\n", __DRIVER_NAME);
		}
		break;
	}
#endif

	return 0;
}


//------------------------------------------------------------------------------
/** This function registers the driver with register_chrdev.
 *
 * @todo For kernel 2.6, use udev
 *  */

static int apci1710_register(void)
{

	//Registration of driver
	apci1710_majornumber = register_chrdev(0, __DRIVER_NAME, &apci1710_fops);

	if (apci1710_majornumber < 0)
	{
		printk (KERN_ERR "%s: register_chrdev returned %d ... aborting\n",__DRIVER_NAME,apci1710_majornumber);
		return -ENODEV;
	}
	return 0;
}

//------------------------------------------------------------------------------
/** Called when module loads. */
static int __init apci1710_init(void)
{

	apci1710_init_vtable(apci1710_vtable);

	if( apci1710_register() )
	{
		return -ENODEV;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	apci1710_class = class_create (THIS_MODULE, __DRIVER_NAME);
	if (IS_ERR(apci1710_class))
 	{
 		printk (KERN_WARNING "%s: class_create error\n",__DRIVER_NAME );
 	}
#endif

	/* registred, now create root /proc entry */
	apci1710_proc_init();

	printk(KERN_INFO "%s: loaded\n",__DRIVER_NAME);

	/* now, subscribe to PCI bus subsystem  */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	/** @note The module will load anyway even if the call to pci_module_init() fails */
	if ( pci_module_init (&apci1710_pci_driver) != 0)
#else
	if ( pci_register_driver (&apci1710_pci_driver) != 0)
#endif
	{
		printk("%s: no device found\n",__DRIVER_NAME);
		apci1710_pci_driver.name = "";
	}

	return 0;
}

//-------------------------------------------------------------------
/** event: a card is removed (also called when module is unloaded) */
static void __devexit apci1710_remove_one(struct pci_dev *dev)
{
	/* stop board activities */
	apci1710_stop_board(dev);

	/* register interrupt */
	apci1710_deregister_interrupt(dev);

	/* deallocate BAR IO Ports ressources */
	pci_release_regions(dev);

	/* do OS-dependant thing we don't really want to know of :) */
	pci_disable_device(dev);

	/* unmap BAR3 */
	if (dev->device == apcie1711_BOARD_DEVICE_ID)
		iounmap(APCI1710_PRIVDATA(dev)->memBaseAddress3);

	apci1710_known_dev_remove(dev);

	/* free private device data*/
	if (APCI1710_PRIVDATA(dev))
		kfree(APCI1710_PRIVDATA(dev));

	/* delete associated /proc entry */
	apci1710_proc_release_device(dev);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	while(1)
	{
		int minor = (atomic_read(&apci1710_count)-1);

        /* don't execute if class not exists */
        if(IS_ERR(apci1710_class))
                break;

		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
			class_device_destroy (apci1710_class, MKDEV(apci1710_majornumber, minor));
		#else
			device_destroy (apci1710_class, MKDEV(apci1710_majornumber, minor));
		#endif

		break;
	}
#endif

	/* decrement global board count*/
	atomic_dec(&apci1710_count);

	printk("%s: board %s unloaded\n",__DRIVER_NAME,pci_name(dev));
}

//------------------------------------------------------------------------------
/** This function unregisters the driver with unregister_chrdev.
 *
 * @todo For kernel 2.6, use udev
 *  */

static void apci1710_unregister(void)
{
	unregister_chrdev(apci1710_majornumber, __DRIVER_NAME);
}

//-------------------------------------------------------------------
/** Called when module is unloaded. */
static void __exit apci1710_exit(void)
{

	/* unsubscribe to PCI bus subsystem */
	if (apci1710_pci_driver.name[0])
	{
		pci_unregister_driver (&apci1710_pci_driver);
	}

	/* unsubscribe to /dev VFS */
	apci1710_unregister();

	/* delete /proc root */
	apci1710_proc_release();

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	class_destroy (apci1710_class);
#endif

}
//------------------------------------------------------------------------------
module_exit(apci1710_exit);
module_init(apci1710_init);
