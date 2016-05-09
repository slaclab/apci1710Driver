/**@file reset_board-kapi.c
 * 
 * where general board reset routines hides
 * 
 * @author Sylvain Nahas
 * @par LICENCE
 *  
 * 
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
#include "irq-private-kapi.h"

EXPORT_SYMBOL(i_APCI1710_ResetBoardIntRoutine);

EXPORT_NO_SYMBOLS;


/* Disable all counter-module related interrupt sources */
static void i_APCI1710_ResetCounterModuleIntRoutine (struct pci_dev * pdev, uint8_t b_ModuleCpt)
{	
	i_APCI1710_DisableLatchInterrupt(pdev,b_ModuleCpt);
	i_APCI1710_DisableCompareLogic(pdev,b_ModuleCpt);
	i_APCI1710_DisableIndex(pdev,b_ModuleCpt);
	//TODO Disable Frequency (when functionality is added)
}


/** Disable and reset the interrupt routine.
 * 
 * @param [in] pdev              : The device to initialize.
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Interrupt was not previously initialised 
 */
 
/* moved in file reset_board-kapi.c */
int i_APCI1710_ResetBoardIntRoutine (struct pci_dev * pdev)
{
	uint8_t b_ModulNbr = 0;

	if ( !pdev)
		return 1;

	if ( INTERRUPT_FUNCTION_NOT_INITIALISED(pdev) )
		return 2;
	
    /* Disable all APCI-1710 interrupt */
	for (b_ModulNbr = 0; b_ModulNbr < NUMBER_OF_MODULE(pdev); b_ModulNbr++ )
   	{
   		if ( IS_A_COUNTER(pdev, b_ModulNbr) )
			i_APCI1710_ResetCounterModuleIntRoutine(pdev,b_ModulNbr);
		
#ifdef WITH_IDV_OPTION
   		i_APCI1710_DisableIdvInterrupt(pdev, b_ModulNbr);
#endif
		/* Test if interrupt function installed */
		if ( MODULE_HAS_USER_HANDLER(pdev,b_ModulNbr) )
		{
			/* Clear the module interrupt function address */
			i_APCI1710_ClearUserHandler(pdev, b_ModulNbr);
		}
	}

	/* end of hardware deactivation */

	/* now, remove global user handler */

	APCI1710_PRIVDATA(pdev)->s_UserInterruptCallback.v_UserInterruptFunction = NULL;
	APCI1710_PRIVDATA(pdev)->s_InterruptInfos.b_InterruptInitialized = 0;
	return 0;
}
