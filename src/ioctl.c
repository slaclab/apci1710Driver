/** @file ioctl.c
*
* @author Sylvain Nahas
* @author Krauth Julien
*
* This module implements the apci1710_do_ioctl function
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

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <asm/io.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)
	#include <asm/system.h>
#endif
#include <asm/uaccess.h>
#include <asm/bitops.h>
#include <linux/sched.h>

#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#include "apci1710-private.h"
#include "vtable.h"

EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user
#endif
//------------------------------------------------------------------------------
/** dummy command to be called when a ioctl command is incorrect */
static int apci1710_do_dummy(struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	printk (KERN_WARNING "%s: %d: invalid ioctl\n",__DRIVER_NAME,_IOC_NR(cmd));
	return -EINVAL;
}
//------------------------------------------------------------------------------
/** add new ioctl handlers here */
void apci1710_init_vtable(vtable_t vtable)
{
	apci1710_dummyze_vtable(vtable, apci1710_do_dummy);


	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_CheckAndGetPCISlotNumber,do_CMD_APCI1710_CheckAndGetPCISlotNumber);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_GetHardwareInformation,do_CMD_APCI1710_GetHardwareInformation);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetBoardInformation,do_CMD_APCI1710_SetBoardInformation);

	/* Incremental counter */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_InitCounter,do_CMD_APCI1710_InitCounter);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_ClearCounterValue,do_CMD_APCI1710_ClearCounterValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_ClearAllCounterValue,do_CMD_APCI1710_ClearAllCounterValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetInputFilter,do_CMD_APCI1710_SetInputFilter);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_InitCompareLogic,do_CMD_APCI1710_InitCompareLogic);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_EnableCompareLogic,do_CMD_APCI1710_EnableCompareLogic);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_DisableCompareLogic,do_CMD_APCI1710_DisableCompareLogic);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_Read16BitCounterValue,do_CMD_APCI1710_Read16BitCounterValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_Read32BitCounterValue,do_CMD_APCI1710_Read32BitCounterValue);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetDigitalChlOn,do_CMD_APCI1710_SetDigitalChlOn);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetDigitalChlOff,do_CMD_APCI1710_SetDigitalChlOff);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_EnableLatchInterrupt,do_CMD_APCI1710_EnableLatchInterrupt);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_DisableLatchInterrupt,do_CMD_APCI1710_DisableLatchInterrupt);

	/* Interrupt */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetBoardIntRoutine,do_CMD_APCI1710_SetBoardIntRoutine);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_ResetBoardIntRoutine,do_CMD_APCI1710_ResetBoardIntRoutine);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_TestInterrupt,do_CMD_APCI1710_TestInterrupt);

	/* Digital I/O */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_InitDigitalIO,do_CMD_APCI1710_InitDigitalIO);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_ReadDigitalIOChlValue,do_CMD_APCI1710_ReadDigitalIOChlValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_ReadDigitalIOPortValue,do_CMD_APCI1710_ReadDigitalIOPortValue);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetDigitalIOMemoryOn,do_CMD_APCI1710_SetDigitalIOMemoryOn);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetDigitalIOMemoryOff,do_CMD_APCI1710_SetDigitalIOMemoryOff);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetDigitalIOChlOn,do_CMD_APCI1710_SetDigitalIOChlOn);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetDigitalIOChlOff,do_CMD_APCI1710_SetDigitalIOChlOff);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetDigitalIOPortOn,do_CMD_APCI1710_SetDigitalIOPortOn);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetDigitalIOPortOff,do_CMD_APCI1710_SetDigitalIOPortOff);

	/* TTL I/O */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_InitTTLIODirection,do_CMD_APCI1710_InitTTLIODirection);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetTTLIOChlOn,do_CMD_APCI1710_SetTTLIOChlOn);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetTTLIOChlOff,do_CMD_APCI1710_SetTTLIOChlOff);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_Write16BitCounterValue,do_CMD_APCI1710_Write16BitCounterValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_Write32BitCounterValue,do_CMD_APCI1710_Write32BitCounterValue);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_GetInterruptUDLatchedStatus,do_CMD_APCI1710_GetInterruptUDLatchedStatus);

	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_InitIndex , do_CMD_APCI1710_InitIndex);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_EnableIndex, do_CMD_APCI1710_EnableIndex);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_DisableIndex, do_CMD_APCI1710_DisableIndex);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_GetIndexStatus, do_CMD_APCI1710_GetIndexStatus);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_SetIndexAndReferenceSource, do_CMD_APCI1710_SetIndexAndReferenceSource);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_InitReference, do_CMD_APCI1710_InitReference);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_GetReferenceStatus, do_CMD_APCI1710_GetReferenceStatus);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_GetCounterInitialisationStatus, do_CMD_APCI1710_GetCounterInitialisationStatus);


	/* Impuls counter */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_InitPulseEncoder,                do_CMD_APCI1710_InitPulseEncoder);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_EnablePulseEncoder,              do_CMD_APCI1710_EnablePulseEncoder);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_DisablePulseEncoder,             do_CMD_APCI1710_DisablePulseEncoder);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadPulseEncoderStatus,          do_CMD_APCI1710_ReadPulseEncoderStatus);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadPulseEncoderValue,           do_CMD_APCI1710_ReadPulseEncoderValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_WritePulseEncoderValue,          do_CMD_APCI1710_WritePulseEncoderValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_PulseEncoderSetDigitalOutputOn,  do_CMD_APCI1710_PulseEncoderSetDigitalOutputOn);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_PulseEncoderSetDigitalOutputOff, do_CMD_APCI1710_PulseEncoderSetDigitalOutputOff);

	/* Chronos */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_InitChrono, do_CMD_APCI1710_InitChrono);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_EnableChrono, do_CMD_APCI1710_EnableChrono);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_DisableChrono, do_CMD_APCI1710_DisableChrono);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_GetChronoProgressStatus, do_CMD_APCI1710_GetChronoProgressStatus);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadChronoValue, do_CMD_APCI1710_ReadChronoValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_SetChronoChlOn, do_CMD_APCI1710_SetChronoChlOn);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_SetChronoChlOff, do_CMD_APCI1710_SetChronoChlOff);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadChronoChlValue, do_CMD_APCI1710_ReadChronoChlValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadChronoPortValue, do_CMD_APCI1710_ReadChronoPortValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadChronoPortValue, do_CMD_APCI1710_ReadChronoPortValue);

	/* BiSS */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_BissMasterInitSingleCycle, do_CMD_APCI1710_BissMasterInitSingleCycle);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_BissMasterSingleCycleDataRead, do_CMD_APCI1710_BissMasterSingleCycleDataRead);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_BissMasterSingleCycleRegisterRead, do_CMD_APCI1710_BissMasterSingleCycleRegisterRead);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_BissMasterSingleCycleRegisterWrite, do_CMD_APCI1710_BissMasterSingleCycleRegisterWrite);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_BissMasterReleaseSingleCycle, do_CMD_APCI1710_BissMasterReleaseSingleCycle);

	/* SSI */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_InitSSI, do_CMD_APCI1710_InitSSI);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_InitSSIRawData, do_CMD_APCI1710_InitSSIRawData);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_Read1SSIValue, do_CMD_APCI1710_Read1SSIValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_Read1SSIRawDataValue, do_CMD_APCI1710_Read1SSIRawDataValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadAllSSIValue, do_CMD_APCI1710_ReadAllSSIValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadAllSSIRawDataValue, do_CMD_APCI1710_ReadAllSSIRawDataValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_StartSSIAcquisition, do_CMD_APCI1710_StartSSIAcquisition);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_GetSSIAcquisitionStatus, do_CMD_APCI1710_GetSSIAcquisitionStatus);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_GetSSIValue, do_CMD_APCI1710_GetSSIValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_GetSSIRawDataValue, do_CMD_APCI1710_GetSSIRawDataValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadSSI1DigitalInput, do_CMD_APCI1710_ReadSSI1DigitalInput);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ReadSSIAllDigitalInput, do_CMD_APCI1710_ReadSSIAllDigitalInput);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_SetSSIDigitalOutputOn, do_CMD_APCI1710_SetSSIDigitalOutputOn);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_SetSSIDigitalOutputOff, do_CMD_APCI1710_SetSSIDigitalOutputOff);

	/* Endat */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatInitialiseSensor, do_CMD_APCI1711_EndatInitialiseSensor);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatSensorReceiveReset, do_CMD_APCI1711_EndatSensorReceiveReset);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatResetErrorBits, do_CMD_APCI1711_EndatResetErrorBits);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatGetErrorSources, do_CMD_APCI1711_EndatGetErrorSources);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatSelectMemoryArea, do_CMD_APCI1711_EndatSelectMemoryArea);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatSensorSendPositionAndRecvMemArea, do_CMD_APCI1711_EndatSensorSendPositionAndRecvMemArea);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatSensorSendParameter, do_CMD_APCI1711_EndatSensorSendParameter);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatSensorReceiveParameter, do_CMD_APCI1711_EndatSensorReceiveParameter);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatSensorSendPositionValue, do_CMD_APCI1711_EndatSensorSendPositionValue);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatSelectAdditionalData, do_CMD_APCI1711_EndatSelectAdditionalData);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1711_EndatSensorSendPositionValueWithAdditionalData, do_CMD_APCI1711_EndatSensorSendPositionValueWithAdditionalData);

	/* Utils */
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_GetModulesId, do_CMD_APCI1710_GetModulesId);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_GetBoardType, do_CMD_APCI1710_GetBoardType);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_WRITE, do_CMD_APCI1710_WRITE);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_READ, do_CMD_APCI1710_READ);
	
	/* Balise */
#ifdef WITH_BALISE_OPTION	
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_SetBaliseOff, do_CMD_APCI1710_SetBaliseOff);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_SetBaliseTelegram, do_CMD_APCI1710_SetBaliseTelegram);
#endif

	/* IDV */
#ifdef WITH_IDV_OPTION
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_InitIdv,do_CMD_APCI1710_InitIdv);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_GetInterruptIdvStatus,do_CMD_APCI1710_GetInterruptIdvStatus);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable,CMD_APCI1710_SetIdvDigitalH,do_CMD_APCI1710_SetIdvDigitalH);
#endif

	/* EL Timers */
#ifdef WITH_EL_TIMERS_OPTION
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ELInitDelayAndPulseWidth, do_CMD_APCI1710_ELInitDelayAndPulseWidth);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ELEnableTimers, do_CMD_APCI1710_ELEnableTimers);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ELDisableTimers, do_CMD_APCI1710_ELDisableTimers);
	__APCI_1710_DECLARE_IOCTL_HANDLER(vtable, CMD_APCI1710_ELSetNewTimerValue, do_CMD_APCI1710_ELSetNewTimerValue);
#endif
}

//------------------------------------------------------------------------------
int apci1710_do_ioctl(struct pci_dev * pdev,unsigned int cmd,unsigned long arg)
{

	/* boundaries check
	 *
	 * VTABLE_ELEMENT_NB(vtable_t) = __APCI1710_UPPER_IOCTL_CMD +1
	 * maximum index value = __APCI1710_UPPER_IOCTL_CMD = VTABLE_ELEMENT_NB(vtable_t) -1
	 *
	 * the idea here is to favorize compilation-time
	 *
	 * */

	if (_IOC_NR(cmd) > (VTABLE_ELEMENT_NB(vtable_t)-1) )
		return apci1710_do_dummy(pdev,cmd,arg);

	/* call actual ioctl handler - should be safe now */
	return (apci1710_vtable[_IOC_NR(cmd)]) (pdev, cmd, arg);
}
//------------------------------------------------------------------------------
int do_CMD_APCI1710_CheckAndGetPCISlotNumber(struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	int retval = 0; /* return value of this call is the number of boards */

    /* this command returns an area of size CONFIG_apci1710_MAX_BOARD_NBR+1 filled with the PCI_SLOT() value and as last field the major number associated to the device - this is deprecated and should not be used anymore.
	The call returns 0 when memory is too low or the number of boards  */

	char * b_SlotArray = kmalloc(CONFIG_apci1710_MAX_BOARD_NBR+1 * sizeof(char),GFP_KERNEL);
    if (!b_SlotArray)
	{
		/* bad param. Let output some information for debug */
		printk("%s: CMD_APCI1710_CheckAndGetPCISlotNumber: -EINVAL\n",__DRIVER_NAME);
		return 0;
	}

	memset(b_SlotArray,0,CONFIG_apci1710_MAX_BOARD_NBR+1 );

	printk("%s: the use of CMD_APCI1710_CheckAndGetPCISlotNumber is deprecated\n",__DRIVER_NAME);

	/* record the PCI_SLOT for each device from 0 to CONFIG_apci1710_MAX_BOARD_NBR  */
	{
		int i;
		for(i = 0; ( ( i < atomic_read(&apci1710_count) ) || ( i< CONFIG_apci1710_MAX_BOARD_NBR ) ); i++)
		{

			struct pci_dev * dev =  apci1710_lookup_board_by_index(i);

			if (!dev) break;

			b_SlotArray[i] = PCI_SLOT(dev->devfn);

		}
    }
    b_SlotArray[CONFIG_apci1710_MAX_BOARD_NBR] = apci1710_majornumber;

    if ( copy_to_user ( (char __user *)arg, b_SlotArray, sizeof (uint8_t[CONFIG_apci1710_MAX_BOARD_NBR+1])) )
	{
    	/* bad address. Let output some information for debug */
    	printk("%s: CMD_APCI1710_CheckAndGetPCISlotNumber: -EFAULT\n",__DRIVER_NAME);
    	kfree(b_SlotArray);
    	return 0;
	}
	/* return the smaller value between CONFIG_apci1710_MAX_BOARD_NBR and number of board - Note: apci1710_count is assumed to to be always consistent with the system PCI devices list */
    retval = ( atomic_read(&apci1710_count) < CONFIG_apci1710_MAX_BOARD_NBR ? atomic_read(&apci1710_count) : CONFIG_apci1710_MAX_BOARD_NBR );

	kfree(b_SlotArray);

	return retval;

}



//------------------------------------------------------------------------------
/** Returns the informations of base address, IRQ to the user.
 * @deprecated: use-space doesn't need it and kernel-space has better means to get it.
 *  */
int do_CMD_APCI1710_GetHardwareInformation(struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	str_BaseInformations s_BaseInformations;
	s_BaseInformations.ui_BaseAddress[0] = GET_BAR0(pdev);
	s_BaseInformations.ui_BaseAddress[1] = GET_BAR1(pdev);
	s_BaseInformations.ui_BaseAddress[2] = 0;
	s_BaseInformations.ui_BaseAddress[3] = 0;
	s_BaseInformations.ui_BaseAddress[4] = 0;
	s_BaseInformations.b_Interrupt = pdev->irq;
	s_BaseInformations.b_SlotNumber = PCI_SLOT(pdev->devfn);

    if ( copy_to_user ((str_BaseInformations __user *)arg,&s_BaseInformations, sizeof(s_BaseInformations)) )
		return -EFAULT;

	return 0;
}

//------------------------------------------------------------------------------
int do_CMD_APCI1710_SetBoardInformation(struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	return 0;
}
