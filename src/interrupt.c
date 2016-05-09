/** @file interrupt.c
*
* @author Krauth Julien
*
* This module implements the interrupt related functions.
*/

/** @par LICENCE
* @verbatim
    Copyright (C) 2009  ADDI-DATA GmbH for the source code of this module.

    ADDI-DATA GmbH
    Airpark Business Center
    Airport Boulevard B210
    77836 Rheinm�nster
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

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

//------------------------------------------------------------------------------

/** IRQ handler prototype has changed between 2.4 and 2.6.
 * in 2.4 the handler has not return value, in 2.6 it is of type irqreturn_t
 * IRQ_HANDLED means that the device did indeed need attention from the driver
 * IRQ_NONE means that the device didn't actually need attention
 *
 * NOTE: the change between 2.4 and 2.6 was not so important that it needed
 * two version of the function. BUT if in the course of the implementation you
 * notice the changes are so important that maintaining a code for both version
 * in one function is just a hassle, DON'T HESITATE and create two versions
 * of the same function.
 *
 */

/** IRQ Handler */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)	/* 2.4  */
	#define RETURN_HANDLED return
	#define RETURN_NONE return
#else
	#define RETURN_HANDLED return IRQ_HANDLED
	#define RETURN_NONE return IRQ_NONE
#endif


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)	/* 2.4  */
	static void apci1710_do_interrupt(int irq, void * dev_id, struct pt_regs *regs)
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)       /* 2.6.0 - 2.6.19  */
	static irqreturn_t apci1710_do_interrupt(int irq, void * dev_id, struct pt_regs *regs)
#else /* 2.6 */
	static irqreturn_t apci1710_do_interrupt(int irq, void * dev_id)
#endif
{
	uint8_t b_ModuleCpt = 0;
	uint8_t b_InterruptFlag = 0;
	uint8_t b_InterruptFlagCount = 0;

	{
		unsigned long irqstate;
		APCI1710_LOCK(VOID_TO_PCIDEV(dev_id), &irqstate);
		{
			/* Is the interrupt initialized */
			if (INTERRUPT_FUNCTION_NOT_INITIALISED(dev_id))
			{
					APCI1710_UNLOCK(VOID_TO_PCIDEV(dev_id), irqstate);
					RETURN_NONE;
			}
			/* Test if the interrupt occured on one of the modules */
			for (b_ModuleCpt = 0; b_ModuleCpt < NUMBER_OF_MODULE(VOID_TO_PCIDEV(dev_id)); b_ModuleCpt ++)
			{
				/* Test if for this functionality any interrupt function installed */
				if (APCI1710_PRIVDATA(VOID_TO_PCIDEV(dev_id))->s_InterruptFunctionality [b_ModuleCpt].v_InterruptFunction != NULL)
				{
					/* Call the interrupt function */
					APCI1710_PRIVDATA(VOID_TO_PCIDEV(dev_id))->s_InterruptFunctionality [b_ModuleCpt].v_InterruptFunction (VOID_TO_PCIDEV(dev_id), b_ModuleCpt, &b_InterruptFlag);
				}
				b_InterruptFlagCount = b_InterruptFlagCount + b_InterruptFlag;
			}
		}
		APCI1710_UNLOCK(VOID_TO_PCIDEV(dev_id), irqstate);
	}
	if ( b_InterruptFlagCount>0)
		RETURN_HANDLED;
	else
		RETURN_NONE;
}

//------------------------------------------------------------------------------

int apci1710_register_interrupt(struct pci_dev * pdev)
{
	uint32_t tmp = 0;

	/* Enable the interrupt on the PCI-Express controller */
	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
	{
		tmp = inl(GET_BAR1(pdev) + 0x68);
		outl((tmp | (1<<11) | (1 << 8)), GET_BAR1(pdev) + 0x68);
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	if ( request_irq( pdev->irq, apci1710_do_interrupt, SA_SHIRQ, __DRIVER_NAME, pdev) )
#else
	if ( request_irq( pdev->irq, apci1710_do_interrupt, IRQF_SHARED, __DRIVER_NAME, pdev) )
#endif
	{
		printk(KERN_ERR "%s: can't register interrupt handler\n", __DRIVER_NAME );
		return -EBUSY;
	}
	return 0;
}

//------------------------------------------------------------------------------

int apci1710_deregister_interrupt(struct pci_dev * pdev)
{
	free_irq( pdev->irq , pdev);
	return 0;
}

