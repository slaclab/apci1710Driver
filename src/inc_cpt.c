/** @file inc_cpt.c
 
   Contains incremental counter ioctl functions.
 
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

/** Initialize the counter.
 * 
 * Configure the counter operating mode from selected module (b_ModulNbr). 
 * You must calling this function be for you call any other function 
 * witch access of counters.
   @verbatim
+----------------------------------------------------------------------------+
|                          Counter range                                     |
|                          -------------                                     |
| +------------------------------------+-----------------------------------+ |
| | Parameter       Passed value       |        Description                | |
| |------------------------------------+-----------------------------------| |
| |b_ModulNbr   APCI1710_16BIT_COUNTER |  The module is configured for     | |
| |                                    |  two 16-bit counter.              | |
| |                                    |  - b_FirstCounterModus and        | |
| |                                    |    b_FirstCounterOption           | |
| |                                    |    configure the first 16 bit     | |
| |                                    |    counter.                       | |
| |                                    |  - b_SecondCounterModus and       | |
| |                                    |    b_SecondCounterOption          | |
| |                                    |    configure the second 16 bit    | |
| |                                    |    counter.                       | |
| |------------------------------------+-----------------------------------| |
| |b_ModulNbr   APCI1710_32BIT_COUNTER |  The module is configured for one | |
| |                                    |  32-bit counter.                  | |
| |                                    |  - b_FirstCounterModus and        | |
| |                                    |    b_FirstCounterOption           | |
| |                                    |    configure the 32 bit counter.  | |
| |                                    |  - b_SecondCounterModus and       | |
| |                                    |    b_SecondCounterOption          | |
| |                                    |    are not used and have no       | |
| |                                    |    importance.                    | |
| +------------------------------------+-----------------------------------+ |
|                                                                            |
|                      Counter operating mode                                |
|                      ----------------------                                |
|                                                                            |
| +--------------------+-------------------------+-------------------------+ |
| |    Parameter       |     Passed value        |    Description          | |
| |--------------------+-------------------------+-------------------------| |
| |b_FirstCounterModus | APCI1710_QUADRUPLE_MODE | In the quadruple mode,  | |
| |       or           |                         | the edge analysis       | |
| |b_SecondCounterModus|                         | circuit generates a     | |
| |                    |                         | counting pulse from     | |
| |                    |                         | each edge of 2 signals  | |
| |                    |                         | which are phase shifted | |
| |                    |                         | in relation to each     | |
| |                    |                         | other.                  | |
| |--------------------+-------------------------+-------------------------| |
| |b_FirstCounterModus |   APCI1710_DOUBLE_MODE  | Functions in the same   | |
| |       or           |                         | way as the quadruple    | |
| |b_SecondCounterModus|                         | mode, except that only  | |
| |                    |                         | two of the four edges   | |
| |                    |                         | are analysed per        | |
| |                    |                         | period                  | |
| |--------------------+-------------------------+-------------------------| |
| |b_FirstCounterModus |   APCI1710_SIMPLE_MODE  | Functions in the same   | |
| |       or           |                         | way as the quadruple    | |
| |b_SecondCounterModus|                         | mode, except that only  | |
| |                    |                         | one of the four edges   | |
| |                    |                         | is analysed per         | |
| |                    |                         | period.                 | |
| |--------------------+-------------------------+-------------------------| |
| |b_FirstCounterModus |   APCI1710_DIRECT_MODE  | In the direct mode the  | |
| |       or           |                         | both edge analysis      | |
| |b_SecondCounterModus|                         | circuits are inactive.  | |
| |                    |                         | The inputs A, B in the  | |
| |                    |                         | 32-bit mode or A, B and | |
| |                    |                         | C, D in the 16-bit mode | |
| |                    |                         | represent, each, one    | |
| |                    |                         | clock pulse gate circuit| |
| |                    |                         | There by frequency and  | |
| |                    |                         | pulse duration          | |
| |                    |                         | measurements can be     | |
| |                    |                         | performed.              | |
| +--------------------+-------------------------+-------------------------+ |
|                                                                            |
|                                                                            |
|       IMPORTANT!                                                           |
|       If you have configured the module for two 16-bit counter, a mixed    |
|       mode with a counter in quadruple/double/single mode                  |
|       and the other counter in direct mode is not possible!                |
|                                                                            |
|                                                                            |
|         Counter operating option for quadruple/double/simple mode          |
|         ---------------------------------------------------------          |
|                                                                            |
| +----------------------+-------------------------+------------------------+|
| |       Parameter      |     Passed value        |  Description           ||
| |----------------------+-------------------------+------------------------||
| |b_FirstCounterOption  | APCI1710_HYSTERESIS_ON  | In both edge analysis  ||
| |        or            |                         | circuits is available  ||
| |b_SecondCounterOption |                         | one hysteresis circuit.||
| |                      |                         | It suppresses each     ||
| |                      |                         | time the first counting||
| |                      |                         | pulse after a change   ||
| |                      |                         | of rotation.           ||
| |----------------------+-------------------------+------------------------||
| |b_FirstCounterOption  | APCI1710_HYSTERESIS_OFF | The first counting     ||
| |       or             |                         | pulse is not suppress  ||
| |b_SecondCounterOption |                         | after a change of      ||
| |                      |                         | rotation.              ||
| +----------------------+-------------------------+------------------------+|
|                                                                            |
|                                                                            |
|       IMPORTANT!                                                           |
|       This option are only avaible if you have selected the direct mode.   |
|                                                                            |
|                                                                            |
|               Counter operating option for direct mode                     |
|               ----------------------------------------                     |
|                                                                            |
| +----------------------+--------------------+----------------------------+ |
| |      Parameter       |     Passed value   |       Description          | |
| |----------------------+--------------------+----------------------------| |
| |b_FirstCounterOption  | APCI1710_INCREMENT | The counter increment for  | |
| |       or             |                    | each counting pulse        | |
| |b_SecondCounterOption |                    |                            | |
| |----------------------+--------------------+----------------------------| |
| |b_FirstCounterOption  | APCI1710_DECREMENT | The counter decrement for  | |
| |       or             |                    | each counting pulse        | |
| |b_SecondCounterOption |                    |                            | |
| +----------------------+--------------------+----------------------------+ |
+----------------------------------------------------------------------------+
   @endverbatim  
   
 * @param [in] pdev                           : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)            : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_CounterRange)        : Selection form counter range.
 * @param [in] arg[2] (b_FirstCounterModus)   : First counter operating mode.
 * @param [in] arg[3] (b_FirstCounterOption)  : First counter option.
 * @param [in] arg[4] (b_SecondCounterModus)  : Second counter operating mode.
 * @param [in] arg[5] (b_SecondCounterOption) : Second counter option.
 *
 * @retval 0: No error.                                            
 * @retval 1: The handle parameter of the board is wrong.          
 * @retval 2: The module is not a counter module.                  
 * @retval 3: The selected counter range is wrong.                
 * @retval 4: The selected first counter operating mode is wrong. 
 * @retval 5: The selected first counter operating option is wrong.
 * @retval 6: The selected second counter operating mode is wrong.
 * @retval 7: The selected second counter operating option is wrong.  
 * @retval -EFAULT : Fail to retrieve user data.                                            
 */
int do_CMD_APCI1710_InitCounter (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;											
	uint8_t b_ArgArray[6];

	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{		
			i_ErrorCode = i_APCI1710_InitCounter (pdev,
												b_ArgArray[0],	// b_ModulNbr
												b_ArgArray[1],	// b_CounterRange
												b_ArgArray[2],	// b_FirstCounterModus
												b_ArgArray[3],	// b_FirstCounterOption
												b_ArgArray[4],	// b_SecondCounterModus
												b_ArgArray[5]);	// b_SecondCounterOption
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}												
		      
//------------------------------------------------------------------------------

/** Clear the counter value.
 * 
 * Clear the counter value from selected module (b_ModulNbr)
 * 
 * @param [in] pdev                 : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)  : Module number to configure (0 to 3).
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module number parameter is wrong.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 */
int do_CMD_APCI1710_ClearCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_ClearCounterValue (pdev, (uint8_t)arg);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	return (i_ErrorCode);
}												
		      
//------------------------------------------------------------------------------ 

/** Clear all counter value.
 * 
 * @param [in] pdev          : The device to initialize.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_ClearAllCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_ClearAllCounterValue (pdev);			
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}

//------------------------------------------------------------------------------

/** Set / reset a filter on the selected module.
 *
 * @param [in] pdev             : The device to initialize.
 * @param [in] arg[0] b_ModulNbr       : Module number to configure (0 to 3).
 * @param [in] arg[1] b_ClockSelection : Selection from PCI bus clock
 *                                       - APCI1710_30MHZ:
 *                                         The PC has a PCI bus clock from 30 MHz
 *                                         Not available with the APCIe-1711
 *                                       - APCI1710_33MHZ:
 *                                         The PC has a PCI bus clock from 33 MHz
 *                                         Not available with the APCIe-1711
 *                                       - APCI1710_40MHZ
 *                                         The APCI-1710 has a integrated 40Mhz quartz.
 * @param [in] arg[2] b_Filter         : Filter selection
 *
 *				30 MHz
 *				------
 *					0:  Software filter not used
 *					1:  Filter from 133ns
 *					2:  Filter from 200ns
 *					3:  Filter from 267ns
 *					4:  Filter from 333ns
 *					5:  Filter from 400ns
 *					6:  Filter from 467ns
 *					7:  Filter from 533ns
 *					8:  Filter from 600ns
 *					9:  Filter from 667ns
 *					10: Filter from 733ns
 *					11: Filter from 800ns
 *					12: Filter from 867ns
 *					13: Filter from 933ns
 *					14: Filter from 1000ns
 *					15: Filter from 1067ns
 *
 *				33 MHz
 *				------
 *					0:  Software filter not used
 *					1:  Filter from 121ns
 *					2:  Filter from 182ns
 *					3:  Filter from 242ns
 *					4:  Filter from 303ns
 *					5:  Filter from 364ns
 *					6:  Filter from 424ns
 *					7:  Filter from 485ns
 *					8:  Filter from 545ns
 *					9:  Filter from 606ns
 *					10: Filter from 667ns
 *					11: Filter from 727ns
 *					12: Filter from 788ns
 *					13: Filter from 848ns
 *					14: Filter from 909ns
 *					15: Filter from 970ns
 *
 *				40 MHz
 *				------
 *					0:  Software filter not used
 *					1:  Filter from 100ns
 *					2:  Filter from 150ns
 *					3:  Filter from 200ns
 *					4:  Filter from 250ns
 *					5:  Filter from 300ns
 *					6:  Filter from 350ns
 *					7:  Filter from 400ns
 *					8:  Filter from 450ns
 *					9:  Filter from 500ns
 *					10: Filter from 550ns
 *					11: Filter from 600ns
 *					12: Filter from 650ns
 *					13: Filter from 700ns
 *					14: Filter from 750ns
 *					15: Filter from 800ns
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module number is wrong.
 * @retval 3: The module is not a counter module or not initialized.
 * @retval 4: The selected PCI input clock is wrong.
 * @retval 5: The selected filter value is wrong.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetInputFilter (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int i_ErrorCode = 0;
	unsigned long irqstate;
	uint8_t b_ArgArray[3];

	if (copy_from_user (b_ArgArray, (uint8_t __user *)arg, sizeof (b_ArgArray)))
		return -EFAULT;

	APCI1710_LOCK(pdev,&irqstate);
	{
		i_ErrorCode = i_APCI1710_SetInputFilter (pdev, b_ArgArray[0], b_ArgArray[1], b_ArgArray[2]);
	}
	APCI1710_UNLOCK(pdev,irqstate);

	return (i_ErrorCode);
}

//------------------------------------------------------------------------------
/** Enable the latch interrupt. 
 * 
 * Enable the latch interrupt from selected module
 * (b_ModulNbr). Each software or hardware latch occur a interrupt.                                             
 *
 * @param [in] pdev          : The device to initialize.
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: Interrupt routine not installed see function "i_APCI1710_SetBoardIntRoutine".
 */

int do_CMD_APCI1710_EnableLatchInterrupt (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_EnableLatchInterrupt (pdev, (uint8_t)arg);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	return (i_ErrorCode);
}												

//------------------------------------------------------------------------------
/** Disable the latch interrupt. 
 * 
 * Disable the latch interrupt from selected module (b_ModulNbr).                                          
 *
 * @param [in] pdev          : The device to initialize.
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: Interrupt routine not installed see function "i_APCI1710_SetBoardIntRoutine".
 */

int do_CMD_APCI1710_DisableLatchInterrupt (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_DisableLatchInterrupt (pdev, (uint8_t)arg);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	return (i_ErrorCode);
}												
		      
//------------------------------------------------------------------------------

/** Set the 32-Bit compare value. 
 * 
 * Set the 32-Bit compare value. At that moment that the
 * incremental counter arrive to the compare value       
 * (ui_CompareValue) a interrupt is generated.           
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (ui_CompareValue) : 32-Bit compare value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_InitCompareLogic (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	uint32_t ui_ArgArray[2];
    			        
	if ( copy_from_user( ui_ArgArray, (uint32_t __user *)arg, sizeof(ui_ArgArray) ) )
		return -EFAULT;
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_InitCompareLogic (pdev, 
													(uint8_t)ui_ArgArray[0],	// b_ModuleNbr
													ui_ArgArray[1]);		// ui_CompareValue
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}	

//------------------------------------------------------------------------------

/** Enable the 32-Bit compare logic. 
 * 
 * Enable the 32-Bit compare logic. At that moment that
 * the incremental counter arrive to the compare value a
 * interrupt is generated.
 * 
 * @param [in] pdev                : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr) : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: Compare logic not initialised. See function "i_APCI1710_InitCompareLogic".
 * @retval 5: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutine".
 */
int do_CMD_APCI1710_EnableCompareLogic (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{		
			i_ErrorCode = i_APCI1710_EnableCompareLogic (pdev, (uint8_t)arg);			
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}		

//------------------------------------------------------------------------------

/** Disable the 32-Bit compare logic.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr) : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found. 
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: Compare logic not initialised. See function "i_APCI1710_InitCompareLogic".
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_DisableCompareLogic (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{	
			i_ErrorCode = i_APCI1710_DisableCompareLogic (pdev, (uint8_t)arg);			
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}														     								
		      
//------------------------------------------------------------------------------

/** Latch the selected 16-Bit counter. 
 * 
 * Latch the selected 16-Bit counter (b_SelectedCounter)
 * from selected module (b_ModulNbr) in to the first
 * latch register and return the latched value.
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)        : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_SelectedCounter) : Selected 16-Bit counter (0 or 1).
 * 
 * @param [out] arg[0] (pui_CounterValue) : 16-Bit counter value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: The selected 16-Bit counter parameter is wrong.
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_Read16BitCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
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
			i_ErrorCode = i_APCI1710_Read16BitCounterValue (pdev,
														(uint8_t)ui_ArgArray[0],	// b_ModulNbr
														(uint8_t)ui_ArgArray[1],	// b_SelectedCounter
														&ui_Tmp);		// ui_CounterValue
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

/** Latch the 32-Bit counter. 
 * 
 * Latch the 32-Bit counter from selected module
 * (b_ModulNbr) in to the first latch register and return the latched value.
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)        : Module number to configure (0 to 3).
 *
 * @param [out] arg[0] (pui_CounterValue) : 32-Bit counter value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_Read32BitCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	uint32_t ul_Arg = 0;
	uint32_t ul_Tmp = 0;	
    			        
	if ( copy_from_user( &ul_Arg, (uint32_t __user *)arg, sizeof(ul_Arg) ) )
		return -EFAULT;	
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_Read32BitCounterValue (pdev,
														ul_Arg,		// b_ModulNbr
														&ul_Tmp);	// ul_CounterValue
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}                            														
												
	if (i_ErrorCode != 0)
		return (i_ErrorCode);		
			
	if ( copy_to_user( (uint32_t __user *)arg , &ul_Tmp, sizeof(ul_Tmp) ) )
		return -EFAULT;	
														
	return (i_ErrorCode);
}												
		      
//------------------------------------------------------------------------------ 

/** Sets the digital output H. 
 * 
 * Sets the digital output H. Setting an output means setting an ouput high.
 *
 * @param [in] pdev                 : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)  : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module number is wrong.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 */
int do_CMD_APCI1710_SetDigitalChlOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetDigitalChlOn (pdev, (uint8_t)arg);			
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}	

//------------------------------------------------------------------------------

/** Resets the digital output H. 
 * 
 * Resets the digital output H. Resetting an output means setting an ouput low.
 * 
 * @param [in] pdev                 : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)  : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module number is wrong.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 */
int do_CMD_APCI1710_SetDigitalChlOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetDigitalChlOff (pdev, (uint8_t)arg);			
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}	

//------------------------------------------------------------------------------
/** Write the selected 16-Bit counter. 
 * 
 * Write a 16-Bit value (ui_WriteValue) in to the selected
 * 16-Bit counter (b_SelectedCounter) from selected module (b_ModulNbr).
 *
 * @param [in] arg[0] (b_ModulNbr)        : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_SelectedCounter) : Selected 16-Bit counter (0 or 1).
 * @param [in] arg[3] (pui_CounterValue) : 16-Bit counter value : (0x0 - 0xFFFF)
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: The selected 16-Bit counter parameter is wrong ( must be 0 or 1 ).
 * @retval 5: The given ui_WriteValue value is not in the correct range. 
 * @retval -EFAULT : Failed to retrieve user data.
 */
int do_CMD_APCI1710_Write16BitCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int ret = 0;
	unsigned long ul_ArgArray[3];
    			        
	if ( copy_from_user( ul_ArgArray, (unsigned long __user *)arg, sizeof(ul_ArgArray) ) )
		return -EFAULT;	
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			ret = i_APCI1710_Write16BitCounterValue (pdev, ul_ArgArray[0], ul_ArgArray[1], ul_ArgArray[2] );
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}                            														
																										
	return ret;
}

//------------------------------------------------------------------------------
/** Write the selected 32-Bit counter.
 * 
 * Write a 32-Bit value (ui_WriteValue) in to the selected
 * 32-Bit counter (b_SelectedCounter) from selected module (b_ModulNbr).
 *
 * @param [in] argv                  : A pointer to an array of 2 unsigned long 
 * 
 * arg[0] [in] (b_ModulNbr)        : Module number to configure (0 to 3).
 * arg[2] [in] (pui_CounterValue) : 32-Bit counter value
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter". 
 * @retval -EFAULT : Failed to access user data ( bad pointer )
 */
int do_CMD_APCI1710_Write32BitCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int ret = 0;
	unsigned long ul_ArgArray[2];
    			        
	if ( copy_from_user( ul_ArgArray, (unsigned long __user *)arg, sizeof(ul_ArgArray) ) )
		return -EFAULT;	
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			ret = i_APCI1710_Write32BitCounterValue (pdev, ul_ArgArray[0], ul_ArgArray[1] );
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}                            														

	return ret;
}

//------------------------------------------------------------------------------
/** Returns the counter progress latched status after a index interrupt occur.
 * 
 * @param [in] argv                  : A pointer to an unsigned long 
 * 
 * arg[0] [in] b_ModulNbr    : Module number (0 to 3). 
 * arg[0] [out] pul_UDStatus   : 0 : Counter progress in the selected mode down
								1 : Counter progress in the selected mode up
								2 : No index interrupt occur
 *  
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range  
 * @retval 3: module b_ModulNbr is not a counter module 
 * @retval 4: Counter not initialised see function "i_APCI1710_InitCounter"
 * @retval 5: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutine"
 */
int do_CMD_APCI1710_GetInterruptUDLatchedStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	uint8_t ul_Arg;
	uint8_t ul_Resp;

	if ( copy_from_user( &ul_Arg, (uint8_t __user *)arg, sizeof(ul_Arg) ) )
		return -EFAULT;	

	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			ret = i_APCI1710_GetInterruptUDLatchedStatus (pdev, ul_Arg, &ul_Resp );
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	if ( copy_to_user( (uint8_t __user *)arg , &ul_Resp, sizeof(ul_Resp) ) )
		return -EFAULT;

	return 0;
}

//------------------------------------------------------------------------------
/** Initialise the index corresponding to the selected
module (b_ModulNbr). If a INDEX flag occur, you have
the possibility to clear the 32-Bit counter or to latch
the current 32-Bit value in to the first latch
register. The b_IndexOperation parameter give the
possibility to choice the INDEX action.
If you have enabled the automatic mode, each INDEX
action is cleared automatically, else you must read
the index status ("CMD_APCI1710_GetIndexStatus")
after each INDEX action.

 * @param [in] argv : A pointer to an array of 5 unsigned char
 * 
 * arg[0] [in] b_ModulNbr    : Module number (0 to 3).
 * arg[0] [in] b_ReferenceAction : 	Determine if the reference must set or no for the acceptance from index  
 * 									APCI1710_ENABLE : Reference must be set for accepted the index
 * 									APCI1710_DISABLE : Reference have no importance
 * arg[0] [in] b_IndexOperation  : (APCI1710_LATCH_COUNTER,APCI1710_CLEAR_COUNTER)  
 * 									APCI1710_LATCH_COUNTER : After a index signal, the counter value (32-Bit) is latched in to the first latch register
 *									APCI1710_CLEAR_COUNTER : After a index signal, the counter value is cleared (32-Bit)
 * 
 * arg[0] [in] b_AutoMode : Enable or disable the automatic index reset : (APCI1710_ENABLE, APCI1710_DISABLE)
 * 
 * arg[0] [in] b_InterruptEnable : Enable or disable the interrupt : (APCI1710_ENABLE, APCI1710_DISABLE)
 * 
 * @retval 0: No error 
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see function "i_APCI1710_InitCounter"
 * @retval 5  The reference action parameter is wrong
 * @retval 6: The index operating mode parameter is wrong
 * @retval 7: The auto mode parameter is wrong
 * @retval 8: Interrupt parameter is wrong
 * @retval 9: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutineX"
 * 
 * @note This command applies to counter modules only
 */
int do_CMD_APCI1710_InitIndex (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	uint8_t b_ArgArray[5];
	
	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;	
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{ 
			ret = i_APCI1710_InitIndex (pdev, b_ArgArray[0], b_ArgArray[1], b_ArgArray[2], b_ArgArray[3], b_ArgArray[4] );
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	
	return ret;
}

//------------------------------------------------------------------------------
/** Enable the index corresponding to the selected module (b_ModulNbr).
 * 
 * @param [in] b_ModulNbr    : Module number (0 to 3).
 * 
 * @retval 0: No error 
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range 
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see command "CMD_APCI1710_InitCounter"
 * @retval 5: Index not initialised - see function "CMD_APCI1710_InitIndex"
 * 
 * @note This command applies to counter modules only
*/
int do_CMD_APCI1710_EnableIndex (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{ 
			ret = i_APCI1710_EnableIndex (pdev, (uint8_t) arg );
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	
	return ret;

}

//------------------------------------------------------------------------------
/** Disable the index corresponding to the selected module (b_ModulNbr).
 * 
 * @param [in] b_ModulNbr    : Module number (0 to 3).
 * 
 * @retval 0: No error 
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see command "CMD_APCI1710_InitCounter"
 * @retval 5:  Index not initialised - see function "CMD_APCI1710_InitIndex"
 * 
 * @note This command applies to counter modules only 
 */
int do_CMD_APCI1710_DisableIndex (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{ 
			ret = i_APCI1710_DisableIndex (pdev, (uint8_t) arg);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	
	return ret;
}

//------------------------------------------------------------------------------
/** Returns the INDEX status.
 * 
 * @param [in] argv : A pointer to one unsigned char
 * 
 * arg[0] [in] b_ModulNbr : Module number (0 to 3).
 * arg[0] [out] pb_IndexStatus : (0,1) 
 * 							0 : No INDEX occur
 * 							1 : An INDEX event occured
 * 
 * @retval 0: No error 
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module 
 * @retval 4: Counter not initialised see command "CMD_APCI1710_InitCounter"
 * @retval 5: Index not initialised - see function "CMD_APCI1710_InitIndex"
 * 
 * @note This command applies to counter modules only
*/
int do_CMD_APCI1710_GetIndexStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	uint8_t b_ModulNbr;
	uint8_t b_IndexStatus;
	
	if ( copy_from_user( &b_ModulNbr, (uint8_t __user *)arg, sizeof(b_ModulNbr) ) )
		return -EFAULT;	
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{ 
			ret = i_APCI1710_GetIndexStatus (pdev, b_ModulNbr, &b_IndexStatus);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	
	if (ret)
		return ret;

	if ( copy_to_user( (uint8_t __user *)arg , &b_IndexStatus, sizeof(b_IndexStatus) ) )
		return -EFAULT;

	return 0;
}
//------------------------------------------------------------------------------
/** Determine the hardware source for the index and the reference logic.
 * 
 * By default the index logic is connected to the difference input C and the reference
 * logic is connected to the 24V input E
 * 
 * @param [in] argv : A pointer to an array of 2 unsigned char
 * 
 * arg[0] [in] b_ModulNbr : Module number (0 to 3).
 * arg[1] [in] b_SourceSelection : (APCI1710_SOURCE_0,APCI1710_SOURCE_1) 
 * 									APCI1710_SOURCE_0 : The index logic is connected to the difference input C 
 * 														and the reference logic is connected to the 24V input E.
 * 														This is the default configuration.
 * 									APCI1710_SOURCE_1 : The reference logic is connected to the difference
 * 														input C and the index logic is connected to the 24V input E
 * 
 * @retval 0: No error 
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: The parameter b_SourceSelection is incorrect  
 * @retval 5: The board does not support this operation ( firmware revision < 1.5 )
 * 
 * @note This command applies to counter modules only
 */
int do_CMD_APCI1710_SetIndexAndReferenceSource (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	uint8_t b_ArgArray[2];
	
	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;	
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{ 
			ret = i_APCI1710_SetIndexAndReferenceSource (pdev, b_ArgArray[0], b_ArgArray[1] );
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	
	return ret;
}
//------------------------------------------------------------------------------
/** Initialise the reference corresponding to the selected module (b_ModulNbr).
 * 
 * @param [in] argv : A pointer to an array of 2 unsigned char
 * 
 * arg[0] [in] b_ModulNbr : Module number (0 to 3).
 * arg[1] [in] b_ReferenceLevel : (APCI1710_LOW,APCI1710_HIGH)
 * 									APCI1710_LOW : Reference occur if "0"
 * 									APCI1710_HIGH : Reference occur if "1"
 * 
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see command "CMD_APCI1710_InitCounter"
 * @retval 5: The parameter b_ReferenceLevel is incorrect 
 *
 * @note This command applies to counter modules only
 */
int do_CMD_APCI1710_InitReference (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	uint8_t b_ArgArray[2];
	
	if ( copy_from_user( b_ArgArray, (uint8_t __user *)arg, sizeof(b_ArgArray) ) )
		return -EFAULT;	
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{ 
			ret = i_APCI1710_InitReference (pdev, b_ArgArray[0], b_ArgArray[1] );
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	
	return ret;
}
//------------------------------------------------------------------------------
/** Return the reference status.
 * 
 * @param [in] argv : A pointer to an one unsigned char
 * 
 * arg[0] [in]  b_ModulNbr : Module number (0 to 3).
 * arg[0] [out] pb_ReferenceStatus : (0,1)
 * 										0: No REFERENCE occurs
 * 										1: A REFERENCE occurs
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see function "CMD_APCI1710_InitCounter"
 * @retval 5: Reference not initialised see command "CMD_APCI1710_InitReference"
 * 
 * @note This command applies to counter modules only
 */
int do_CMD_APCI1710_GetReferenceStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	uint8_t b_ModulNbr;
	uint8_t b_ReferenceStatus;
	
	if ( copy_from_user( &b_ModulNbr, (uint8_t __user *)arg, sizeof(b_ModulNbr) ) )
		return -EFAULT;	
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{ 
			ret = i_APCI1710_GetReferenceStatus (pdev, b_ModulNbr, &b_ReferenceStatus);
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	
	if (ret)
		return ret;

	if ( copy_to_user( (uint8_t __user *)arg , &b_ReferenceStatus, sizeof(b_ReferenceStatus) ) )
		return -EFAULT;

	return 0;
}

/** Return the current initialisation status of an incremental counter module.
 * 
 * @param [out] argv : A pointer to an array of 7 unsigned integers.
 * 
 * arg[0] [int] i_ModulNbr : Module number (0 to 3)
 * arg[1] [out] i_IndexInit : index function was initialised
 * arg[2] [out] i_CounterInit : counter function was initialised
 * arg[3] [out] i_ReferenceInit : reference function was initialised
 * arg[4] [out] i_CompareLogicInit : compare logic function was initialised
 * arg[5] [out] i_FrequencyMeasurementInit : Frequency Measurement function was initialised
 * arg[6] [out] i_FrequencyMeasurementEnable : Frequency Measurement function was enabled
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * 
 * @note This command applies to counter modules only
 */
 int do_CMD_APCI1710_GetCounterInitialisationStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
 {
 	int ret = 0;
	unsigned int ui_ArgArray[7];
	
	if ( copy_from_user( ui_ArgArray, (unsigned long __user *)arg, sizeof(ui_ArgArray) ) )
		return -EFAULT;	
	
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{ 
			ret = i_APCI1710_GetCounterInitialisationStatus(pdev, ui_ArgArray[0], &(ui_ArgArray[1]) );
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}
	
	if (ret)
		return ret;

	if ( copy_to_user( (unsigned long __user *)arg , ui_ArgArray, sizeof(ui_ArgArray) ) )
		return -EFAULT;

	return 0;
 }
