/** @file dig_io.c
 
   Contains ttl I/O ioctl functions.
 
   @par CREATION  
   @author Krauth Julien
   @date   26.10.2007
   
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
    77836 Rheinmünster
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

//----------------------------------------------------------------------------

/** Configure the TTL I/O operating mode on the selected module.
 *  You must call this function before you call any other TTL function.
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PortAMode)     : Port A mode selection <br>
 *                                        0 : digital input <br>
 *                                        1 : digital output
 * @param [in] arg[2] (b_PortBMode)     : Port B mode selection <br>
 *                                        0 : digital input <br>
 *                                        1 : digital output
 * @param [in] arg[3] (b_PortCMode)     : Port C mode selection <br>
 *                                        0 : digital input <br>
 *                                        1 : digital output
 * @param [in] arg[4] (b_PortDMode)     : Port D mode selection <br>
 *                                        0 : digital input <br>
 *                                        1 : digital output
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 * @retval 4: Function not available for this version.
 * @retval 5: Port A mode selection is wrong.
 * @retval 6: Port B mode selection is wrong.
 * @retval 7: Port C mode selection is wrong.
 * @retval 8: Port D mode selection is wrong.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_InitTTLIODirection (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[5];
	     
	if ( copy_from_user(b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;									
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_InitTTLIODirection (pdev,
																		b_ArgArray[0],  // b_ModulNbr
																		b_ArgArray[1],  // b_PortAMode
																		b_ArgArray[2],  // b_PortBMode
																		b_ArgArray[3],  // b_PortCode
																		b_ArgArray[4]); // b_PortDMode
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}
	
	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Set the output witch has been passed with the
 *  parameter b_Channel. Setting an output means setting
 *  an ouput high.
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_OutputChannel) : Selection from digital output channel (0 or 1):<br>
 *                                        0       : PD0<br>
 *                                        1       : PD1<br>
 *                                        2 to 9  : PA<br>
 *                                        10 to 17: PB<br>
 *                                        8 to 25 : PC
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 * @retval 4: The selected digital output is wrong. 
 * @retval 5: TTL I/O not initialised see function i_APCI1710_InitTTLIO.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetTTLIOChlOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[2];
    			        
	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;									
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetTTLIOChlOn (pdev,
													b_ArgArray[0],   // b_ModulNbr
													b_ArgArray[1]);  // b_OutputChannel
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}									
	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Reset the output witch has been passed with the
 *  parameter b_Channel. Resetting an output means setting
 *  an ouput low.
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_OutputChannel) : Selection from digital output channel (0 or 1):<br>
 *                                        0       : PD0<br>
 *                                        1       : PD1<br>
 *                                        2 to 9  : PA<br>
 *                                        10 to 17: PB<br>
 *                                        8 to 25 : PC
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 * @retval 4: The selected digital output is wrong. 
 * @retval 5: TTL I/O not initialised see function i_APCI1710_InitTTLIO.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetTTLIOChlOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[2];
    			        
	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;									
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetTTLIOChlOff (pdev,
													b_ArgArray[0],   // b_ModulNbr
													b_ArgArray[1]);  // b_OutputChannel
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}									
	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

