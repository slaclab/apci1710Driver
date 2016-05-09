/** @file kapi_utils.c
 
   Contains utilities kernel functions.
 
   @par CREATION  
   @author Krauth Julien
   @date   23.05.06
   
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

EXPORT_SYMBOL(i_APCI1710_GetModuleId);
EXPORT_SYMBOL(i_APCI1710_GetFunctionality);
EXPORT_SYMBOL(i_APCI1710_GetBoardType);

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

//------------------------------------------------------------------------------

/** Read modules configuration.
 * 
 * @param [in] pdev          : The device to initialize.
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 */
int   i_APCI1710_ReadModulesConfiguration (struct pci_dev *pdev)
{
                    
    if (!pdev) 
    	return 1;

    INPDW (GET_BAR2(pdev), 60, &APCI1710_PRIVDATA(pdev)->
                                s_BoardInfos.
                                dw_MolduleConfiguration [0]);
                                                               
    INPDW (GET_BAR2(pdev), 124, &APCI1710_PRIVDATA(pdev)->
                                s_BoardInfos.
                                dw_MolduleConfiguration [1]);
                                
    INPDW (GET_BAR2(pdev), 188, &APCI1710_PRIVDATA(pdev)->
                                s_BoardInfos.
                                dw_MolduleConfiguration [2]);
                                
    INPDW (GET_BAR2(pdev), 252, &APCI1710_PRIVDATA(pdev)->
                                s_BoardInfos.
                                dw_MolduleConfiguration [3]);                                                                                                                           
    
    return 0;
}

//------------------------------------------------------------------------------

/** Return the module functionality.
 *
 * @param [in] pdev          : The device to initialize.
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 *
 * @param [out] pui_Functionality : Module configuration.
 *
 * @retval 0: No error.
 * @retval 1: pdev is NULL.
 * @retval 2: b_ModuleNbr is incorrect.  
 */
int   i_APCI1710_GetFunctionality (struct pci_dev *pdev,
                                   uint8_t  b_ModuleNbr,
                                   uint32_t * pui_Functionality)
{
    if (!pdev) 
    	return 1;
    
	/* Test the module number */
	if ( NOT_A_MODULE(pdev,b_ModuleNbr) )
		return 2;
    
	*pui_Functionality = APCI1710_MODULE_FUNCTIONALITY(pdev, b_ModuleNbr);

	return 0;
}
//------------------------------------------------------------------------------
/** returns the content of the module configuration register (ID).
 * 
 * This information helps to identify the type of module that is installed on the board.
 * 
 * @param [in] argv                  : A pointer to an array of 4 unsigned long
 * 
 * @retval 0: No error. 
 * @retval 1: pdev is NULL.
 * @retval 2: b_ModuleNbr is incorrect  
 * 
 * The 2 first bytes identify the functionality ( see enumeration in apci1710.h )
 * the two last byte is the firmware version 
 *  
 */
int   i_APCI1710_GetModuleId (struct pci_dev *pdev,uint8_t  b_ModuleNbr,uint32_t * pui_Id)
{
    if (!pdev) 
    	return 1;
    
	/* Test the module number */
	if ( NOT_A_MODULE(pdev,b_ModuleNbr) )
		return 2;

	*pui_Id = APCI1710_MODULE_ID(pdev, b_ModuleNbr);

	return 0;
}
//------------------------------------------------------------------------------
static const char * functionality_id_to_str(unsigned long Functionality)
{
	switch(Functionality)
	{
		case APCI1710_INCREMENTAL_COUNTER:
			return "INCREMENTAL_COUNTER";
		case APCI1710_SSI_COUNTER:
			return "SSI_COUNTER";
		case APCI1710_TTL_IO:
			return "TTL_IO";
		case APCI1710_DIGITAL_IO:
			return "DIGITAL_IO";
		case APCI1710_82X54_TIMER:
			return "82X54_TIMER";
		case APCI1710_CHRONOMETER:
			return "CHRONOMETER";
		case APCI1710_PULSE_ENCODER:
			return "PULSE_ENCODER";
		case APCI1710_TOR_COUNTER:
			return "TOR_COUNTER";
		case APCI1710_PWM:
			return "PWM";
		case APCI1710_ETM:
			return "ETM";
		case APCI1710_CDA:
			return "CDA";
		case APCI1710_BALISE:
			return "BALISE";
		case APCI1710_IDV:
			return "IDV";
		case APCI1710_EL_TIMERS:
			return "EL_TIMERS";
		case PCIE1711_ENDAT:
			return "PCIE1711_ENDAT";
		default:
			return "UNKNOWN";
	}
}
//------------------------------------------------------------------------------
static __inline__ int PRINTK_MODULE_INFO(struct pci_dev *pdev,uint8_t b_ModuleNbr)
{
	printk(KERN_INFO "module %d: ID is 0x%.8X (functionality: %s, version: 0x%.4X)\n",
			b_ModuleNbr, 
			APCI1710_MODULE_ID(pdev,b_ModuleNbr),
			functionality_id_to_str(APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModuleNbr)),
			APCI1710_MODULE_VERSION(pdev,b_ModuleNbr)
			);
	return 0;	
} 

//------------------------------------------------------------------------------
int  i_APCI1710_DumpModulesFunctionalities (struct pci_dev *pdev)
{
    if (!pdev) 
    	return 1;

	PRINTK_MODULE_INFO(pdev,0);
	PRINTK_MODULE_INFO(pdev,1);
	PRINTK_MODULE_INFO(pdev,2);
	PRINTK_MODULE_INFO(pdev,3);    
    return 0;
}

//------------------------------------------------------------------------------

/** Get the board type.
 *
 * @param [in] pdev			: The device to use.
 *
 * @param [out] boardType	: Board hardware type:
 * 							  - APCI1710_10K10_BOARD_TYPE
 * 							  - APCI1710_10K20_BOARD_TYPE
 * 							  - APCIE1711_BOARD_TYPE
 * 							  - UNKNOWN_BOARD_TYPE
 *
 * @retval 0: No error.
 * @retval 1: pdev error.
 */
int i_APCI1710_GetBoardType (struct pci_dev *pdev, uint32_t *boardType)
{
	uint8_t data = 0;
	uint32_t address = 0;

	if (!pdev)
    	return 1;

	*boardType = UNKNOWN_BOARD_TYPE;

	switch (pdev->device)
	{
		case apcie1711_BOARD_DEVICE_ID:
			*boardType = APCIE1711_BOARD_TYPE;
		break;

		case apci1710_BOARD_DEVICE_ID:
		{
			for (address=0; address < (14751UL * 2); address++)
			{
				OUTPDW (GET_BAR1(pdev), 4, address + (14751UL * 4));
				INP (GET_BAR1(pdev), 3, &data);

				if (data != 0xFF)
				{
					*boardType = APCI1710_10K20_BOARD_TYPE;
					return 0;
				}
			}

			*boardType = APCI1710_10K10_BOARD_TYPE;
		}
		break;
	}

	return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

