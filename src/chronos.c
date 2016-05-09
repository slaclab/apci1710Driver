/** @file inc_cpt.c

   Contains chronos ioctl functions.

   @par CREATION
   @author Krauth Julien
   @date   15.03.2010

   @par VERSION
   @verbatim
   $LastChangedRevision:$
   $LastChangedDate:$
   @endverbatim

   @par LICENCE
   @verbatim
    Copyright (C) 2009  ADDI-DATA GmbH for the source code of this module.

    ADDI-DATA GmbH
    Airpark Business Center
    Airport Boulevard B210
    77836 Rheinmï¿½nster
    Germany
    Tel: +49(0)7229/1847-0
    Fax: +49(0)7229/1847-200
    http://www.addi-data-com
    info@addi-data.com

   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with
   this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   You shoud also find the complete GPL in the COPYING file
   accompanying this source code.
   @endverbatim
 */

#include "apci1710-private.h"

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user
#endif


//------------------------------------------------------------------------------

/** Initialize chronos functionality.
 *
 * Configure the chronometer operating mode (b_ChronoMode)
 * from selected module (b_ModuleNbr).
 * The ul_TimingInterval and ul_TimingUnit determine the
 * timing base for the measurement.
 * The pul_RealTimingInterval return the real timing
 * value. You must calling this function be for you call
 * any other function witch access of the chronometer.
 *
 * Witch this functionality from the APCI-1710 you have
 * the possibility to measure the timing witch two event.
 *
 * The mode 0 and 1 is appropriate for period measurement.
 * The mode 2 and 3 is appropriate for frequent measurement.
 * The mode 4 to 7 is appropriate for measuring the timing
 * between  two event.
 *
 * @param [in] struct pci_dev *pdev                                 : The device to use.
 * @param [in] uint32_t arg[0] (b_ModuleNbr)       : Module number to configure (0 to 3).
 * @param [in] uint32_t arg[1] (b_ChronoMode)      : Chronometer action mode (0 to 7).
 * @param [in] uint32_t arg[2] (b_PCIInputClock)   : Selection from PCI bus clock
 *                                                   - APCI1710_30MHZ :
 *                                                     The PC have a PCI bus
 *                                                     clock from 30 MHz
 *                                                     Not available with the APCIe-1711
 *                                                   - APCI1710_33MHZ :
 *                                                     The PC have a PCI bus
 *                                                     clock from 33 MHz
 *                                                     Not available with the APCIe-1711
 *                                                   - APCI1710_40MHZ
 *                                                     The APCI-1710 have a
 *                                                     integrated 40Mhz quartz.
 * @param [in] uint32_t arg[3] (b_TimingUnit)      : Base timing unity (0 to 4)
 *                                                   0 : ns
 *                                                   1 : µs
 *                                                   2 : ms
 *                                                   3 : s
 *                                                   4 : mn
 * @param [in] uint32_t arg[4] (ul_TimingInterval  : Base timing value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer mode selection is wrong.
 * @retval 5: The selected PCI input clock is wrong.
 * @retval 6: Timing unity selection is wrong.
 * @retval 7: Base timing selection is wrong.
 * @retval 8: You can not used the 40MHz clock selection wich this board.
 * @retval 9: You can not used the 40MHz clock selection wich this CHRONOS version.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_InitChrono (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	uint32_t dw_ArgArray[5];

	if ( copy_from_user( dw_ArgArray, (uint32_t __user *)arg, sizeof(dw_ArgArray) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_InitChrono (pdev,
												(uint8_t) dw_ArgArray[0],   // b_ModuleNbr
												(uint8_t) dw_ArgArray[1],   // b_ChronoMode
												(uint8_t) dw_ArgArray[2],   // b_PCIInputClock
												(uint8_t) dw_ArgArray[3],   // b_TimingUnit
												(uint32_t) dw_ArgArray[4]); // ul_TimingInterval
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Enable the chronometer.
 *
 * Enable the chronometer from selected module
 * (b_ModuleNbr). You must calling the
 * "i_APCI1710_InitChrono" function be for you call this
 * function.
 * If you enable the chronometer interrupt, the
 * chronometer generate a interrupt after the stop signal.
 * See function "i_APCI1710_SetBoardIntRoutineX" and the
 * Interrupt mask description chapter from this manual.
 * The b_CycleMode parameter determine if you will
 * measured a single or more cycle.
 *
 * @param [in] struct pci_dev *pdev                                : The device to use.
 * @param [in] uint8_t arg[0] (b_ModuleNbr)       : Module number to configure (0 to 3).
 * @param [in] uint8_t arg[1] (b_CycleMode)       : Selected the chronometer acquisition mode.
 * @param [in] uint8_t arg[2] (b_InterruptEnable) : Enable or disable the chronometer interrupt.
 *                                                  - APCI1710_ENABLE: Enable the chronometer interrupt.
 *                                                  - APCI1710_DISABLE:Disable the chronometer interrupt.
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval 5: Chronometer acquisition mode cycle is wrong.
 * @retval 6: Interrupt parameter is wrong.
 * @retval 7: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutineX".
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_EnableChrono (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	uint8_t b_ArgArray[3];

	if ( copy_from_user(b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_EnableChrono (pdev,
												   (uint8_t) b_ArgArray[0],  // b_ModuleNbr
												   (uint8_t) b_ArgArray[1],  // b_CycleMode
												   (uint8_t) b_ArgArray[2]); // b_InterruptEnable
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Disable the chronometer.
 *
 * Disable the chronometer from selected module
 * (b_ModuleNbr). If you disable the chronometer after a
 * start signal occur and you restart the chronometer
 * witch the " i_APCI1710_EnableChrono" function, if no
 * stop signal occur this start signal is ignored.
 *
 * @param [in] struct pci_dev *pdev                             : The device to use.
 * @param [in] uint8_t arg (b_ModuleNbr)       : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 */
int do_CMD_APCI1710_DisableChrono (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_DisableChrono (pdev, (uint8_t)arg);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Get the chronometer status.
 *
 * Return the chronometer status (pb_ChronoStatus) from
 * selected chronometer module (b_ModuleNbr).
 *
 * @param [in] struct pci_dev *pdev                        : The device to use.
 * @param [in] uint8_t* arg (b_ModuleNbr) : Module number to configure (0 to 3).
 *
 * @param [out] uint8_t* arg (b_ChronoStatus) : Return the chronometer status.
 *                                0 : Measurement not started.
 *                                    No start signal occur.
 *                                1 : Measurement started.
 *                                    A start signal occur.
 *                                2 : Measurement stopped.
 *                                    A stop signal occur.
 *                                    The measurement is terminate.
 *                                3: A overflow occur. You must change the base
 *                                   timing witch the function "i_APCI1710_InitChrono"
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval -EFAULT : Fail to return user data.
 */
int do_CMD_APCI1710_GetChronoProgressStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	uint8_t b_ChronoStatus = 0;

	i_ErrorCode = i_APCI1710_GetChronoProgressStatus (pdev, (uint8_t) arg, &b_ChronoStatus);

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint8_t __user *)arg , &b_ChronoStatus, sizeof(b_ChronoStatus) ) )
		return -EFAULT;

	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Read the chronometer value.
 *
 * Return the chronometer status (pb_ChronoStatus) and the
 * timing value (pul_ChronoValue) after a stop signal
 * occur from selected chronometer module (b_ModuleNbr).
 * This function are only avaible if you have disabled
 * the interrupt functionality. See function
 * "i_APCI1710_EnableChrono" and the Interrupt mask
 * description chapter.
 * You can test the chronometer status witch the
 * "i_APCI1710_GetChronoProgressStatus" function.
 *
 * The returned value from pul_ChronoValue parameter is
 * not real measured timing.
 * You must used the "i_APCI1710_ConvertChronoValue"
 * function or make this operation for calculate the
 * timing:
 *
 * Timing = pul_ChronoValue * pul_RealTimingInterval.
 *
 * pul_RealTimingInterval is the returned parameter from
 * "i_APCI1710_InitChrono" function and the time unity is
 * the b_TimingUnit from "i_APCI1710_InitChrono" function
 *
 * @param [in] struct pci_dev *pdev                           : The device to use.
 * @param [in] uint32_t arg[0] (b_ModuleNbr) : Module number to configure (0 to 3).
 * @param [in] uint32_t arg[1] (ul_TimeOut)  : 0 not used, >0 number of loop to to before timeout.
 *
 * @param [out] uint32_t arg[0] (b_ChronoStatus)  : Return the chronometer status.
 *                                                  0 : Measurement not started.
 *                                                      No start signal occur.
 *                                                  1 : Measurement started.
 *                                                      A start signal occur.
 *                                                  2 : Measurement stopped.
 *                                                      A stop signal occur.
 *                                                      The measurement is terminate.
 *                                                  3: A overflow occur. You must change the base
 *                                                     timing witch the function "i_APCI1710_InitChrono"
 *
 * @param [out] uint32_t arg[1] (ul_ChronoValue) : Chronometer timing value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval 5: Timeout parameter is wrong (0 to 65535).
 * @retval 6: Interrupt routine installed.
 *            You can not read directly the chronometer measured timing.
 * @retval -EFAULT : Fail to retrieve / return user data.
 */
int do_CMD_APCI1710_ReadChronoValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	uint32_t dw_ArgArray[2];
	uint8_t b_ChronoStatus = 0;
	uint32_t ul_ChronoValue = 0;

	if ( copy_from_user(dw_ArgArray, (uint32_t __user *)arg, sizeof(dw_ArgArray) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_ReadChronoValue (pdev,
													  (uint8_t) dw_ArgArray[0],  // b_ModuleNbr
													  (uint32_t) dw_ArgArray[1], // ul_TimeOut
													  &b_ChronoStatus,
													  &ul_ChronoValue);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	dw_ArgArray[0] = b_ChronoStatus;
	dw_ArgArray[1] = ul_ChronoValue;

	if ( copy_to_user( (uint32_t __user *)arg , dw_ArgArray, sizeof(dw_ArgArray) ) )
		return -EFAULT;

	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Set the selected chronometer channel on.
 *
 * Sets the output witch has been passed with the
 * parameter b_Channel. Setting an output means setting an
 * output high.
 *
 * @param [in] struct pci_dev *pdev                              : The device to use.
 * @param [in] uint8_t arg[0] (b_ModuleNbr)     : Module number to configure (0 to 3).
 * @param [in] uint8_t arg[1] (b_OutputChannel) : Selection from digital output
 *                                                 channel (0 to 2)
 *                                                 0 : Channel H
 *                                                 1 : Channel A
 *                                                 2 : Channel B
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: Timeout parameter is wrong (0 to 65535).
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetChronoChlOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	uint8_t b_ArgArray[2];

	if ( copy_from_user(b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetChronoChlOn (pdev,
													 b_ArgArray[0],  // b_ModuleNbr
													 b_ArgArray[1]); // b_OutputChannel
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}

	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Set the selected chronometer channel off.
 *
 * Reset the output witch has been passed with the
 * parameter b_Channel. Setting an output means setting an
 * output high.
 *
 * @param [in] struct pci_dev *pdev                              : The device to initialize.
 * @param [in] uint8_t arg[0] (b_ModuleNbr)     : Module number to configure (0 to 3).
 * @param [in] uint8_t arg[1] (b_OutputChannel) : Selection from digital output
 *                                                channel (0 to 2)
 *                                                0 : Channel H
 *                                                1 : Channel A
 *                                                2 : Channel B
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetChronoChlOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	uint8_t b_ArgArray[2];

	if ( copy_from_user(b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetChronoChlOff (pdev,
													 b_ArgArray[0],  // b_ModuleNbr
													 b_ArgArray[1]); // b_OutputChannel
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}

	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Read the selected chronometer channel value.
 *
 * Return the status from selected digital input
 * (b_InputChannel) from selected chronometer
 * module (b_ModuleNbr).
 *
 * @param [in] struct pci_dev *pdev                             : The device to use.
 * @param [in] uint8_t arg[0] (b_ModuleNbr)    : Module number to configure (0 to 3).
 * @param [in] uint8_t arg[1] (b_InputChannel) : Selection from digital output
 *                                               channel (0 to 2)
 *                                               0 : Channel E
 *                                               1 : Channel F
 *                                               2 : Channel G
 *
 * @param [out] uint8_t arg[0] (b_ChannelStatus) : Digital input channel status.
 *                                                 0 : Channel is not active
 *                                                 1 : Channel is active
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: The selected digital input is wrong.
 * @retval 5: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval -EFAULT : Fail to retrieve / return user data.
 */
int do_CMD_APCI1710_ReadChronoChlValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	uint8_t b_ArgArray[2];
	uint8_t b_ChannelStatus = 0;


	if ( copy_from_user(b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;

	i_ErrorCode = i_APCI1710_ReadChronoChlValue (pdev,
												 b_ArgArray[0], // b_ModuleNbr
												 b_ArgArray[1], // b_InputChannel
												 &b_ChannelStatus);

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	b_ArgArray[0] = b_ChannelStatus;

	if ( copy_to_user( (uint8_t __user *)arg , b_ArgArray, sizeof(uint8_t) ) )
		return -EFAULT;

	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Read the selected chronometer port value.
 *
 * Return the status from digital inputs port from
 * selected  (b_ModuleNbr) chronometer module.
 *
 * @param [in] struct pci_dev *pdev                         : The device to use.
 * @param [in] uint8_t* arg (b_ModuleNbr)  : Module number to configure (0 to 3).
 *
 * @param [out] uint8_t* arg (b_PortValue) : Digital inputs port status.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval -EFAULT : Fail to return user data.
 */
int do_CMD_APCI1710_ReadChronoPortValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	uint8_t b_PortValue = 0;

	i_ErrorCode = i_APCI1710_ReadChronoPortValue (pdev,
												  (uint8_t) arg, // b_ModuleNbr
											      &b_PortValue);

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint8_t __user *)arg , &b_PortValue, sizeof(uint8_t) ) )
		return -EFAULT;

	return (i_ErrorCode);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


