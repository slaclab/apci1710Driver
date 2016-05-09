/** @file biss.c

   Contains SSI ioctl functions.

   @par CREATION
   @author Laaziz ElBakali
   @date   1.12.2015

   @par VERSION
   @verbatim
   $LastChangedRevision:$
   $LastChangedDate:$
   @endverbatim

   @par LICENCE
   @verbatim
    Copyright (C) 2013  ADDI-DATA GmbH for the source code of this module.

    ADDI-DATA GmbH
    Airpark Business Center
    Airport Boulevard B210
    77836 Rheinm�nster
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

EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user
#endif

/** Initialize SSI.
 *
 * Configure the SSI operating mode from selected module
 * (b_ModulNbr). You must calling this function be for you
 * call any other function witch access of SSI.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3)
 * @param [in] b_SSIProfile          : Selection from SSI profile length (2 to 32).
 * @param [in] b_PositionTurnLength  : Selection from SSI position data length (1 to 31).
 * @param [in] b_TurnCptLength       : Selection from SSI turn counter data length (1 to 31).
 * @param [in] b_PCIInputClock       : Selection from PCI bus clock:
 *                                    - APCI1710_30MHZ: The PC have a PCI bus clock from 30 MHz
 *                                    - APCI1710_33MHZ: The PC have a PCI bus clock from 33 MHz
 * @param [in] ul_SSIOutputClock     : Selection from SSI output clock:
 *                                    From  229 to 5 000 000 Hz for 30 MHz selection.
 *                                    From  252 to 5 000 000 Hz for 33 MHz selection.
 * @param [in] b_SSICountingMode    : SSI counting mode selection:
 *                                   - APCI1710_BINARY_MODE: Binary counting mode.
 *                                   - APCI1710_GRAY_MODE: Gray counting mode.
 *
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: The module parameter is wrong
 * @retval 3: The module is not a SSI module
 * @retval 4: The selected SSI profile length is wrong
 * @retval 5: The selected SSI position data length is wrong
 * @retval 6: The selected SSI turn counter data length is wrong
 * @retval 7: The selected PCI input clock is wrong
 * @retval 8: The selected SSI output clock is wrong
 * @retval 9: The selected SSI counting mode parameter is wrong
 */
int do_CMD_APCI1710_InitSSI(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[7]= {0};
	int i_ErrorCode = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);

		i_ErrorCode = i_APCI1710_InitSSI(pdev,
									  (uint8_t)argArray[0],  // ModulNbr
									  (uint8_t)argArray[1],  // SSIProfile
									  (uint8_t)argArray[2],  // PositionTurnLength
									  (uint8_t)argArray[3],  // TurnCptLength
									  (uint8_t)argArray[4],  // PCIInputClock
									  (uint32_t)argArray[5], // SSIOutputClock
									  (uint8_t)argArray[6]); // SSICountingMode

		APCI1710_UNLOCK(pdev,irqstate);
	}
	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	return 0;
}


/** Initialize raw SSI.
 *
 * Configure the SSI operating mode from selected module
 * (b_ModulNbr). You must calling this function be for you
 * call any other function witch access of SSI.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_SSIProfile          : Selection from SSI profile length (2 to 32).
 * @param [in] b_PCIInputClock       : Selection from PCI bus clock:
 *                                      - APCI1710_30MHZ: The PC have a PCI bus clock from 30 MHz
 *                                      - APCI1710_33MHZ: The PC have a PCI bus clock from 33 MHz
 * @param [in] ul_SSIOutputClock    : Selection from SSI output clock:
 *                                    From  229 to 5 000 000 Hz for 30 MHz selection.
 *                                    From  252 to 5 000 000 Hz for 33 MHz selection.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: The module does not support this profile length.
 * @retval 5: The selected SSI profile length is wrong.
 * @retval 6: The selected PCI input clock is wrong.
 * @retval 7: The selected SSI output clock is wrong.
 */
int do_CMD_APCI1710_InitSSIRawData(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[4]= {0};
	int i_ErrorCode = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);

		i_ErrorCode = i_APCI1710_InitSSIRawData(pdev,
		                          (uint8_t)argArray[0],   // ModulNbr
		                          (uint8_t)argArray[1],   // SSIProfile
		                          (uint8_t)argArray[2],   // PCIInputClock
		                          (uint32_t)argArray[3]); // SSIOutputClock


		APCI1710_UNLOCK(pdev,irqstate);
	}
	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	return 0;
}

/** Read the selected SSI counter.
 *
 * @param [in] pdev          : The device to initialize.
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 * @param [in] b_SelectedSSI : Selection from SSI counter (0 to 2).
 *
 * @param [out] pul_Position : SSI position in the turn.
 * @param [out] pul_TurnCpt  : Number of turns.

 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: SSI not initialised see function "i_APCI1710_InitSSI".
 * @retval 5: The selected SSI is wrong.
 * @retval 6: This function does not support more than 32 bits profile length.
 */
int do_CMD_APCI1710_Read1SSIValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[2]= {0};
	uint32_t tmp[2]= {0};
	int i_ErrorCode = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_Read1SSIValue(pdev,
		                          argArray[0],  // ModulNbr
		                          argArray[1],  // b_SelectedSSI
		                          &tmp[1],  	// *pul_Position
		                          &tmp[0]); 	// *pul_TurnCpt

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint32_t __user *)arg , tmp, sizeof(tmp) ) )
		return -EFAULT;

	return 0;

}

/** Read the selected raw SSI counter.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_SelectedSSI     : Selection from SSI counter (0 to 2).
 * @param [in] b_ValueArraySize  : Size of the pul_ValueArray in dword.
 *
 * @param [out] pul_ValueArray   : Array of the raw data from the SSI counter
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: SSI not initialised see function "i_APCI1710_InitSSI".
 * @retval 5: The selected SSI is wrong.
 * @retval 6: The b_ValueArraySize parameter is wrong.
 */
int do_CMD_APCI1710_Read1SSIRawDataValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[4]= {0};
	uint32_t tmp[1]= {0};
	int i_ErrorCode = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_Read1SSIRawDataValue(pdev,
		                          argArray[0],   // ModulNbr
		                          argArray[1],   // b_SelectedSSI
		                          &tmp[0],  // * pul_ValueArray
		                          argArray[3]);  // b_ValueArraySize

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint32_t __user *)arg , tmp, sizeof(tmp) ) )
		return -EFAULT;

	return 0;

}


/** Read all SSI counter.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @param [out] pul_Position     : SSI position in the turn.
 * @param [out] pul_TurnCpt      : Number of turns.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: SSI not initialised see function "i_APCI1710_InitSSI".
 * @retval 5: This function does not support more than 32 bits profile length.
 */
int do_CMD_APCI1710_ReadAllSSIValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[3]= {0};
	uint32_t tmp[2]= {0};
	int i_ErrorCode = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_ReadAllSSIValue(pdev,
		                          argArray[0],   // ModulNbr
		                          &tmp[0],   		  // * pul_Position
		                          &tmp[1]);  		  // * pul_TurnCpt

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint32_t __user *)arg , tmp, sizeof(tmp) ) )
		return -EFAULT;

	return 0;
}


/** Read all raw SSI counter.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_ValueArraySize  : Size of the pul_ValueArray in dword.
 *
 * @param [out] pul_ValueArray   : Array of the raw data from the SSI counter.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: SSI not initialised see function "i_APCI1710_InitSSI".
 * @retval 5: The b_ValueArraySize parameter is wrong.
 */
int do_CMD_APCI1710_ReadAllSSIRawDataValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[3]= {0};
	uint32_t tmp[1]= {0};
	int i_ErrorCode = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_ReadAllSSIRawDataValue(pdev,
												  argArray[0],  	// ModulNbr
												  &tmp[0],  		// *pul_ValueArray
												  argArray[2]);  	// b_ValueArraySize

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint32_t __user *)arg , tmp, sizeof(tmp) ) )
		return -EFAULT;

	return 0;

}

/** Start the SSI acquisition.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: SSI not initialised see function "i_APCI1710_InitSSI".
 * @retval 5: Acquisition already in progress.
 */

int do_CMD_APCI1710_StartSSIAcquisition(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[1]= {0};
	int i_ErrorCode = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode =  i_APCI1710_StartSSIAcquisition(pdev,
		                                      argArray[0]);  // ModulNbr
	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	return 0;

}


/** Return the SSI acquisition status.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @param [out] pb_Status        : 1: Acquisition in progress.
 *                                 0: Acquisition stopped.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: SSI not initialised see function "i_APCI1710_InitSSI".
 */

int do_CMD_APCI1710_GetSSIAcquisitionStatus(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[2]= {0};
	int i_ErrorCode = 0;
	uint32_t tmp[1]= {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_GetSSIAcquisitionStatus(pdev,
		        		                          argArray[0],     // ModulNbr
		        		                          (uint8_t*) &tmp[0]);   // * pb_Status

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint32_t __user *)arg , tmp, sizeof(tmp) ) )
		return -EFAULT;

	return 0;

}

/** Read the selected SSI counter.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_SelectedSSI     : Selection from SSI counter (0 to 2).
 *
 * @param [out] pul_Position     : SSI position in the turn.
 * @param [out] pul_TurnCpt      : Number of turns.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: SSI not initialised see function "i_APCI1710_InitSSI".
 * @retval 5: The selected SSI is wrong.
 * @retval 6: Acquisition in progress.
 * @retval 7: This function does not support more than 32 bits profile length.
 */

int do_CMD_APCI1710_GetSSIValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[4]= {0};
	int i_ErrorCode = 0;
	uint32_t tmp[2]= {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_GetSSIValue(pdev,
		        		                          argArray[0],   // ModulNbr
		        		                          argArray[1],   // b_SelectedSSI
		        		                          &tmp[0],  // * pul_Position
												  &tmp[1]); // * pul_TurnCpt

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint32_t __user *)arg , tmp, sizeof(tmp) ) )
		return -EFAULT;

	return 0;

}


/** Read the selected raw SSI counter.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_SelectedSSI     : Selection from SSI counter (0 to 2).
 * @param [in] b_ValueArraySize  : Size of the pul_ValueArray in dword.
 *
 * @param [out] pul_ValueArray   : Array of the raw data from the SSI counter.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: SSI not initialised see function "i_APCI1710_InitSSI".
 * @retval 5: The selected SSI is wrong.
 * @retval 6: Acquisition in progress.
 * @retval 7: The b_ValueArraySize parameter is wrong.
 */


int do_CMD_APCI1710_GetSSIRawDataValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[4]= {0};
	int i_ErrorCode = 0;
	uint32_t tmp[1]= {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_GetSSIRawDataValue(pdev,
		        		                          argArray[0],     // ModulNbr
		        		                          argArray[1],     // b_SelectedSSI
		        		                          &tmp[0],  	   // * pul_ValueArray
												  argArray[3]);    // b_ValueArraySize

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint32_t __user *)arg , tmp, sizeof(tmp) ) )
		return -EFAULT;

	return 0;

}



/** Read the selected SSI input.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_InputChannel    : Selection from digital input (0 to 2).
 *
 * @param [out] pb_ChannelStatus : Digital input channel status
 *                                 0 : Channle is not active
 *                                 1 : Channle is active
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 * @retval 4: The selected SSI digital input is wrong.
 */


int do_CMD_APCI1710_ReadSSI1DigitalInput(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[3]= {0};
	int i_ErrorCode = 0;
	uint32_t tmp[1]= {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_ReadSSI1DigitalInput(pdev,
		        		                          argArray[0],   // ModulNbr
		        		                          argArray[1],   // b_InputChannel
		        		                          (uint8_t*) &tmp[0]);   		  // * pb_ChannelStatus


	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint32_t __user *)arg , tmp, sizeof(tmp) ) )
		return -EFAULT;

	return 0;

}


/** Read the status from all SSI digital inputs.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @param [out] pb_InputStatus : Digital inputs channel status.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 */

int do_CMD_APCI1710_ReadSSIAllDigitalInput(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[2]= {0};
	int i_ErrorCode = 0;
	uint32_t tmp[1]= {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_ReadSSIAllDigitalInput(pdev,
		        		                          argArray[0],    // ModulNbr
		        		                          (uint8_t*) &tmp[0]);   // pb_InputStatus

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	if ( copy_to_user( (uint32_t __user *)arg , tmp, sizeof(tmp) ) )
		return -EFAULT;

	return 0;

}

/** Set the digital output.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 */

int do_CMD_APCI1710_SetSSIDigitalOutputOn(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[1]= {0};
	int i_ErrorCode = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_SetSSIDigitalOutputOn(pdev,
		        		                        argArray[0]);    // ModulNbr


	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	return 0;
}


/** Reset the digital output.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a SSI module.
 */

int do_CMD_APCI1710_SetSSIDigitalOutputOff(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[1]= {0};
	int i_ErrorCode = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	i_ErrorCode = i_APCI1710_SetSSIDigitalOutputOff(pdev,
		        		                         argArray[0]);    // ModulNbr

	if (i_ErrorCode != 0)
		return (i_ErrorCode);

	return 0;
}

