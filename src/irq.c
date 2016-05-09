/** @file irq.c
 
   Contains interrupt ioctl functions.
 
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
#include "irq-private-kapi.h"

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user 
#endif

//---------------------------------------------------------------------------- 

/** Enable and set the interrupt routine.
 * 
 * @param [in] pdev              : The device to initialize.
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The interrupt callback is NULL.
 */
int do_CMD_APCI1710_SetBoardIntRoutine (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_SetBoardIntRoutine (pdev, NULL);			
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}														     								
		      
//---------------------------------------------------------------------------- 

/** Disable and reset the interrupt routine.
 * 
 * @param [in] pdev              : The device to initialize.
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 */
int do_CMD_APCI1710_ResetBoardIntRoutine (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_ResetBoardIntRoutine (pdev);			
		}
		APCI1710_UNLOCK(pdev,irqstate);
	}									
	return (i_ErrorCode);
}														     								
		      
//---------------------------------------------------------------------------- 

/** Return interrupt information.
 * 
 * @param [in] pdev                            : The device to initialize.
 * 
 * @param [out] arg[0] (pb_ModuleMask)         : Mask of the events <br>
 *                                               which have generated the interrupt. 
 * @param [out] arg[1] (pul_InterruptMask)     :
 * @param [out] arg[2] (pul_CounterLatchValue) : The latched values of <br>
 *                                               the counter are returned.
 *
 * @retval 1  : No interrupt.
 * @retval >0 : IRQ number
 * @retval -EFAULT : Fail to retrieve user data.  
 */
int do_CMD_APCI1710_TestInterrupt (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	int i_ErrorCode = 0;
	uint8_t b_ArgTmp = 0;
	uint32_t ul_ArgArray[3];
	{
		unsigned long irqstate;
		APCI1710_LOCK(pdev,&irqstate);
		{
			i_ErrorCode = i_APCI1710_TestInterrupt (pdev, &b_ArgTmp, &ul_ArgArray[1], &ul_ArgArray[2]);
		}
		APCI1710_UNLOCK(pdev,irqstate);		
	}
	ul_ArgArray[0] = b_ArgTmp;
		
	if (i_ErrorCode <= 0)
		return (i_ErrorCode);		
		
	if ( copy_to_user( (uint32_t __user *)arg , ul_ArgArray, sizeof(ul_ArgArray) ) )
		return -EFAULT;			
												
	return (i_ErrorCode);
}														     								
		      
//---------------------------------------------------------------------------- 
