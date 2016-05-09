/** @file dig_io.c
 
   Contains digital I/O ioctl functions.
 
   @par CREATION  
   @author Krauth Julien
   @date   30.05.06
   
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

/** Configure the digital I/O operating mode.
 * 
 * Configure the digital I/O operating mode from selected
 * module  (b_ModulNbr). You must calling this function be
 * for you call any other function witch access of digital I/O.                                                   
 * 
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_ChannelAMode)  : Channel A mode selection <br>
 *                                        0 : Channel used for digital input <br>
 *                                        1 : Channel used for digital output
 * @param [in] arg[2] (b_ChannelBMode)  : Channel B mode selection <br>
 *                                        0 : Channel used for digital input
 *                                        1 : Channel used for digital output
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Bi-directional channel A configuration error.
 * @retval 5: Bi-directional channel B configuration error.
 * @retval -EFAULT : Fail to retrieve user data.     
 */				
int do_CMD_APCI1710_InitDigitalIO (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[3];
	     
	if ( copy_from_user(b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;									
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_InitDigitalIO (pdev,
													b_ArgArray[0],  // b_ModulNbr
													b_ArgArray[1],  // b_ChannelAMode
													b_ArgArray[2]); // b_ChannelBMode
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}
	
	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Read the status from selected digital I/O digital input (b_InputChannel).
 *
 * @param [in] pdev                    : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)     : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_InputChannel) : Selection from digital input (0 to 6) <br>
 *                                       0 : Channel C <br>         
 *                                       1 : Channel D <br>         
 *                                       2 : Channel E <br>         
 *                                       3 : Channel F <br>         
 *                                       4 : Channel G <br>         
 *                                       5 : Channel A <br>         
 *                                       6 : Channel B         
 *
 * @param [out] arg[0] (pb_ChannelStatus) : Digital input channel status <br>
 *                                          0 : Channle is not active <br>
 *                                          1 : Channle is active    
 *
 * @retval 0: No error.                                            
 * @retval 1: The handle parameter of the board is wrong.          
 * @retval 2: The module parameter is wrong.                       
 * @retval 3: The module is not a digital I/O module.              
 * @retval 4: The selected digital I/O digital input is wrong.     
 * @retval 5: Digital I/O not initialised.                         
 * @retval 6: The digital channel A is used for output.            
 * @retval 7: The digital channel B is used for output.
 * @retval -EFAULT : Fail to retrieve user data.             
 */
int do_CMD_APCI1710_ReadDigitalIOChlValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
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
			i_ErrorCode = i_APCI1710_ReadDigitalIOChlValue (pdev,
														b_ArgArray[0],  // b_ModulNbr
														b_ArgArray[1],  // b_InputChannel
														&b_Tmp);        // pb_ChannelStatus
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}									
	if (i_ErrorCode != 0)
		return (i_ErrorCode);		
			
	if ( copy_to_user( (uint8_t __user *)arg , &b_Tmp, sizeof(b_Tmp) ) )
		return -EFAULT;													
												
	return (i_ErrorCode);
}			

//------------------------------------------------------------------------------

/** Read the status from digital input port.               
 * 
 * Read the status from digital input port from selected 
 * digital I/O module (b_ModulNbr).
 *
 * @param [in] pdev                : The device to initialize.
 * @param [in] arg (b_ModulNbr)    : Module number to configure (0 to 3).
 *
 * @param [out] arg (pb_PortValue) : Digital I/O inputs port status.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Digital I/O not initialised.
 */
int do_CMD_APCI1710_ReadDigitalIOPortValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint8_t b_Tmp = 0;
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_ReadDigitalIOPortValue (pdev,
														arg,     // b_ModulNbr
														&b_Tmp); // pb_PortValue
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}									
	if (i_ErrorCode != 0)
		return (i_ErrorCode);		
			
	if ( copy_to_user( (uint8_t __user *)arg , &b_Tmp, sizeof(b_Tmp) ) )
		return -EFAULT;													
												
	return (i_ErrorCode);
}		

//------------------------------------------------------------------------------																	

/** Activates the digital output memory. 
 * 
 * After having called up this function, the output you have previously
 * activated with the function "i_APCI1710_SetDigitalIOOutputXOn" are not reset.
 * You can reset them with the function "i_APCI1710_SetDigitalIOOutputXOff".                  
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg (b_ModulNbr)      : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Digital I/O not initialised.
 */
int do_CMD_APCI1710_SetDigitalIOMemoryOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
			
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetDigitalIOMemoryOn (pdev, arg); // b_ModulNbr								
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}								
		return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Deactivates the digital output memory.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg (b_ModulNbr)      : Module number to configure (0 to 3).
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Digital I/O not initialised.
 */
int do_CMD_APCI1710_SetDigitalIOMemoryOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetDigitalIOMemoryOff (pdev, arg); // b_ModulNbr								
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}									
	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Sets the output. 
 * 
 * Sets the output witch has been passed with the
 * parameter b_Channel. Setting an output means setting
 * an ouput high.                                         
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_OutputChannel) : Selection from digital output <br>
 *                                        channel (0 to 2) <br>               
 *                                        0 : Channel H <br>
 *                                        1 : Channel A <br>
 *                                        2 : Channel B
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".
 * @retval 6: The digital channel A is used for input.
 * @retval 7: The digital channel B is used for input.
 * @retval -EFAULT : Fail to retrieve user data. 
 */
int do_CMD_APCI1710_SetDigitalIOChlOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[2];
    			        
	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;									
			
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetDigitalIOChlOn (pdev,
													b_ArgArray[0],   // b_ModulNbr
													b_ArgArray[1]);  // b_OutputChannel
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}									
	return (i_ErrorCode);
}	

//------------------------------------------------------------------------------	

/** Resets the output. 
 * 
 * Resets the output witch has been passed with the
 * parameter b_Channel. Resetting an output means setting 
 * an ouput low.                                          
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_OutputChannel) : Selection from digital output channel (0 to 2) <br>              
 *                                        0 : Channel H <br>              
 *                                        1 : Channel A <br>              
 *                                        2 : Channel B               
 * 
 * @retval 0: No error.                                            
 * @retval 1: The handle parameter of the board is wrong.          
 * @retval 2: The module parameter is wrong.                       
 * @retval 3: The module is not a digital I/O module.              
 * @retval 4: The selected digital output is wrong.                
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".                         
 * @retval 6: The digital channel A is used for input.             
 * @retval 7: The digital channel B is used for input.             
 * @retval 8: Digital Output Memory OFF Use previously the function "i_APCI1710_SetDigitalIOMemoryOn".
 * @retval -EFAULT : Fail to retrieve user data.                   
 */
int do_CMD_APCI1710_SetDigitalIOChlOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[2];
    			        
	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;									
			
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetDigitalIOChlOff (pdev,
													b_ArgArray[0],   // b_ModulNbr
													b_ArgArray[1]);  // b_OutputChannel
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}										
	return (i_ErrorCode);
}	

//------------------------------------------------------------------------------
					
/** Sets one or several outputs from port.
 *
 * Setting an output means setting an output high.        
 * If you have switched OFF the digital output memory     
 * (OFF), all the other output are set to "0".            
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)   : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PortValue)  : Output Value ( 0 To 7 ).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Output value wrong.
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".
 * @retval 6: The digital channel A is used for input.
 * @retval 7: The digital channel B is used for input.
 * @retval -EFAULT : Fail to retrieve user data. 
 */
int do_CMD_APCI1710_SetDigitalIOPortOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[2];
    			        
	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;									
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetDigitalIOPortOn (pdev,
													b_ArgArray[0],   // b_ModulNbr
													b_ArgArray[1]);  // b_PortValue
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}									
	return (i_ErrorCode);
}	

//------------------------------------------------------------------------------
		
/** Resets one or several output from port.
 *
 * Resetting means setting low.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)   : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PortValue)  : Output Value ( 0 To 7 ).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Output value wrong.
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".
 * @retval 6: The digital channel A is used for input.
 * @retval 7: The digital channel B is used for input.
 * @retval 8: Digital Output Memory OFF. Use previously the function "i_APCI1710_SetDigitalIOMemoryOn".      
 * @retval -EFAULT : Fail to retrieve user data.            
 */
int do_CMD_APCI1710_SetDigitalIOPortOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[2];
    			        
	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;									
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetDigitalIOPortOff (pdev,
													b_ArgArray[0],   // b_ModulNbr
													b_ArgArray[1]);  // b_PortValue
		}
		APCI1710_UNLOCK(pdev, irqstate);
	}									
	return (i_ErrorCode);
}	
																								
//------------------------------------------------------------------------------

