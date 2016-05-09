/** @file imp_cpt.c
 
   Contains impuls counter ioctl functions.
 
   @par CREATION  
   @author Krauth Julien
   @date   06.10.08
   
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

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user 
#endif

//------------------------------------------------------------------------------

/** Initialize the impuls counter.
 * 
 * Configure the pulse encoder operating mode selected via b_ModulNbr and b_PulseEncoderNbr. 
 * The pulse encoder after each pulse decrement the counter value from 1.
 * You must calling this function be for you call any
 * other function witch access of pulse encoders.
 *  
 *   
 * @param [in] pdev                           : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)            : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr)     : Pulse encoder selection (0 to 3)
 * @param [in] arg[2] (b_InputLevelSelection) : Input level selection (0 or 1) <br>
 *                                                0 : Set pulse encoder count the the low level pulse. <br>
 *                                                1 : Set pulse encoder count the the high level pulse.
 * @param [in] arg[3] (b_TriggerOutputAction) : Digital TRIGGER output action <br>
 *                                                0 : No action <br>
 *                                                1 : Set the trigger output to "1" <br>
 *                                                    (high) after the passage from 1 to 0 <br>
 *                                                    from pulse encoder. <br>
 *                                                2 : Set the trigger output to "0" <br>
 *                                                    (low) after the passage from 1 to 0 <br>
 *                                                    from pulse encoder 
 * @param [in] arg[4] (ul_StartValue)        : Pulse encoder start value (1 to 4294967295)
 *
 * @retval 0: No error.                                            
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module is not a pulse encoder module.
 * @retval 3: Pulse encoder selection is wrong.
 * @retval 4: Input level selection is wrong.
 * @retval 5: Digital TRIGGER output action selection is wrong.
 * @retval 6: Pulse encoder start value is wrong.
 * @retval -EFAULT : Fail to retrieve user data.                                            
 */
int do_CMD_APCI1710_InitPulseEncoder (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint32_t ui_ArgArray[5];

	if ( copy_from_user( ui_ArgArray, (uint32_t __user *)arg, sizeof(ui_ArgArray) ) )
		return -EFAULT;	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{		
			i_ErrorCode = i_APCI1710_InitPulseEncoder (pdev,
							           (uint8_t)  ui_ArgArray [0],  // b_ModulNbr
							           (uint8_t)  ui_ArgArray [1],  // b_PulseEncoderNbr
							           (uint8_t)  ui_ArgArray [2],  // b_InputLevelSelection
							           (uint8_t)  ui_ArgArray [3],  // b_TriggerOutputAction
							           (uint32_t) ui_ArgArray [4]); // ul_StartValue
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}												
		      
//------------------------------------------------------------------------------

/** Enable the pulse encoder
 * 
 * Enable the selected pulse encoder (b_PulseEncoderNbr)
 * from selected module (b_ModulNbr). Each input pulse
 * decrement the pulse encoder counter value from 1.
 * If you enabled the interrupt (b_InterruptHandling), a
 * interrupt is generated when the pulse encoder has run down.
 * 
 * @param [in] pdev                         : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr           : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr)   : Pulse encoder selection (0 to 3).
 * @param [in] arg[2] (b_CycleSelection)    : APCI1710_CONTINUOUS: <br>
 *                                              Each time the counting value is set <br>
 *                                              on "0", the pulse encoder load the <br>
 *                                              start value after the next pulse. <br>
 *                                            APCI1710_SINGLE:
 *                                              If the counter is set on "0", the pulse <br>
 *                                              encoder is stopped.
 * @param [in] arg[3] (b_InterruptHandling) : Interrupts can be generated, when the pulse <br>
 *                                            encoder has run down. <br>
 *                                            With this parameter the user decides if <br>
 *                                            interrupts are used or not. <br>
 *                                            APCI1710_ENABLE : Interrupts are enabled <br>
 *                                            APCI1710_DISABLE: Interrupts are disabled
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval 5: Cycle selection mode is wrong.
 * @retval 6: Interrupt handling mode is wrong.
 * @retval 7: Interrupt routine not installed. See function "i_APCI1710_SetBoardIntRoutine"
 * @retval -EFAULT : Fail to retrieve user data.                                            
 */
int do_CMD_APCI1710_EnablePulseEncoder (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[4];

	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{		
			i_ErrorCode = i_APCI1710_EnablePulseEncoder (pdev,
                                                                     (uint8_t) b_ArgArray [0],  // b_ModulNbr
                                                                     (uint8_t) b_ArgArray [1],  // b_PulseEncoderNbr
                                                                     (uint8_t) b_ArgArray [2],  // b_CycleSelection
                                                                     (uint8_t) b_ArgArray [3]); // b_InterruptHandling
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}												
		      
//------------------------------------------------------------------------------

/** Disable the pulse encoder.
 * 
 * Disable the selected pulse encoder (b_PulseEncoderNbr) from selected module (b_ModulNbr).
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr         : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr) : Pulse encoder selection (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_DisablePulseEncoder (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[2];

	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{		
			i_ErrorCode = i_APCI1710_DisablePulseEncoder (pdev,
                                                                     (uint8_t) b_ArgArray [0],  // b_ModulNbr
                                                                     (uint8_t) b_ArgArray [1]); // b_PulseEncoderNbr
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}												

//------------------------------------------------------------------------------

/** Reads the pulse encoder status
 * 
 * Reads the status from selected pulse encoder (b_PulseEncoderNbr) 
 * from selected module (b_ModulNbr).
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr         : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr) : Pulse encoder selection (0 to 3).
 *
 * @param [out] arg[0] (pb_Status)        : Pulse encoder status. <br>
 *                                            0 : No overflow occur <br>
 *                                            1 : Overflow occur
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_ReadPulseEncoderStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	uint8_t b_ArgArray[2];
	uint8_t b_Tmp = 0;
    			        
		if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
			return -EFAULT;	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_ReadPulseEncoderStatus (pdev,
                                                                         (uint8_t) b_ArgArray [0],  // b_ModulNbr
                                                                         (uint8_t) b_ArgArray [1], // b_PulseEncoderNbr
                                                                         &b_Tmp);		    // pb_Status
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	if (i_ErrorCode != 0)
		return (i_ErrorCode);		
			
	if ( copy_to_user( (uint8_t __user *)arg , &b_Tmp, sizeof(b_Tmp) ) )
		return -EFAULT;	
														
	return (i_ErrorCode);
}												

//------------------------------------------------------------------------------

/** Reads the pulse encoder value.
 * 
 * Reads the value from selected pulse encoder (b_PulseEncoderNbr) from selected module (b_ModulNbr).
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr         : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr) : Pulse encoder selection (0 to 3).
 *
 * @param [out] arg[0] (pul_ReadValue)    : Pulse encoder value
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_ReadPulseEncoderValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	uint32_t ui_ArgArray[2];
	uint32_t ui_Tmp = 0;
    			        
		if ( copy_from_user( ui_ArgArray, (uint32_t __user *)arg, sizeof(ui_ArgArray) ) )
			return -EFAULT;	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_ReadPulseEncoderValue (pdev,
                                                                        (uint8_t) ui_ArgArray [0],  // b_ModulNbr
                                                                        (uint8_t) ui_ArgArray [1], // b_PulseEncoderNbr
                                                                        &ui_Tmp);		    // pul_ReadValue
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	if (i_ErrorCode != 0)
		return (i_ErrorCode);		
			
	if ( copy_to_user( (uint32_t __user *)arg , &ui_Tmp, sizeof(ui_Tmp) ) )
		return -EFAULT;	
														
	return (i_ErrorCode);
}												
		      
//------------------------------------------------------------------------------

/** Writes a 32-bit value into the pulse encoder.
 * 
 * Writes a 32-bit value (ul_WriteValue) into the selected pulse encoder (b_PulseEncoderNbr) 
 * from selected module (b_ModulNbr). This operation set the new start pulse
 * encoder value.                                         |
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr         : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr) : Pulse encoder selection (0 to 3).
 * @param [in] arg[2] (ul_WriteValue)     : 32-bit value to be written.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_WritePulseEncoderValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint32_t ui_ArgArray[3];

	if ( copy_from_user( ui_ArgArray, (uint32_t __user *)arg, sizeof(ui_ArgArray) ) )
		return -EFAULT;	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{		
			i_ErrorCode = i_APCI1710_WritePulseEncoderValue (pdev,
                                                                         (uint8_t) ui_ArgArray [0],   // b_ModulNbr
                                                                         (uint8_t) ui_ArgArray [1],   // b_PulseEncoderNbr
									 (uint32_t) ui_ArgArray [2]); // ul_WriteValue
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}												
		      

/** Set the digital output H on.
 * 
 * Set the digital outp H on (High) from selected module (b_ModulNbr)
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg (b_ModulNbr)      : Module number (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Functionality not available
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_PulseEncoderSetDigitalOutputOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_PulseEncoderSetDigitalOutputOn (pdev, (uint8_t)arg);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	return (i_ErrorCode);
}

/** Set the digital output H off.
 * 
 * Set the digital outp H off (Low) from selected module (b_ModulNbr)
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg (b_ModulNbr)      : Module number (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Functionality not available
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_PulseEncoderSetDigitalOutputOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_PulseEncoderSetDigitalOutputOff (pdev, (uint8_t)arg);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	return (i_ErrorCode);
}

