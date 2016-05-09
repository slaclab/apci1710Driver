/** @file kapi_inc_cpt.c
 
   Contains incremental counter kernel functions.
 
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
    77836 Rheinmnster
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

EXPORT_SYMBOL(i_APCI1710_InitCounter);
EXPORT_SYMBOL(i_APCI1710_ClearCounterValue);
EXPORT_SYMBOL(i_APCI1710_ClearAllCounterValue);
EXPORT_SYMBOL(i_APCI1710_LatchCounter);
EXPORT_SYMBOL(i_APCI1710_ReadLatchRegisterStatus);
EXPORT_SYMBOL(i_APCI1710_ReadLatchRegisterValue);
EXPORT_SYMBOL(i_APCI1710_EnableLatchInterrupt);
EXPORT_SYMBOL(i_APCI1710_DisableLatchInterrupt);
EXPORT_SYMBOL(i_APCI1710_Read16BitCounterValue);
EXPORT_SYMBOL(i_APCI1710_Read32BitCounterValue);
EXPORT_SYMBOL(i_APCI1710_Write16BitCounterValue);
EXPORT_SYMBOL(i_APCI1710_Write32BitCounterValue);
EXPORT_SYMBOL(i_APCI1710_InitCompareLogic);
EXPORT_SYMBOL(i_APCI1710_EnableCompareLogic);
EXPORT_SYMBOL(i_APCI1710_DisableCompareLogic);
EXPORT_SYMBOL(i_APCI1710_SetCompareValue);
EXPORT_SYMBOL(i_APCI1710_SetComparePort);
EXPORT_SYMBOL(i_APCI1710_InitCompareWatchdog);
EXPORT_SYMBOL(i_APCI1710_GetCompareWatchdogStatus);
EXPORT_SYMBOL(i_APCI1710_ClearCompareFIFO);
EXPORT_SYMBOL(i_APCI1710_ChangeCounterDirection);
EXPORT_SYMBOL(i_APCI1710_SetDigitalChlOn);
EXPORT_SYMBOL(i_APCI1710_SetDigitalChlOff);
#ifndef WITH_IDV_OPTION
EXPORT_SYMBOL(i_APCI1710_GetInterruptUDLatchedStatus);
#endif
EXPORT_SYMBOL(i_APCI1710_InitIndex);
EXPORT_SYMBOL(i_APCI1710_EnableIndex);
EXPORT_SYMBOL(i_APCI1710_DisableIndex);
EXPORT_SYMBOL(i_APCI1710_GetIndexStatus);
EXPORT_SYMBOL(i_APCI1710_SetIndexAndReferenceSource);
EXPORT_SYMBOL(i_APCI1710_InitReference);
EXPORT_SYMBOL(i_APCI1710_GetReferenceStatus);

EXPORT_SYMBOL(i_APCI1710_SetInputFilter);

EXPORT_NO_SYMBOLS;

/* returns 1 if the counter isn't initialised */
static __inline__ int COUNTER_NOT_INITIALISED(struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	return ( APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_CounterInit != 1 );
}

/* returns 1 if the reference has not been initialised yet */
static __inline__ int REFERENCE_NOT_INITIALISED(struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	return ( (APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_ReferenceInit) != 1 );
}
/* returns 1 if the index has not been initialised yet */
static __inline__ int INDEX_NOT_INITIALISED(struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	return ( APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_IndexInit != 1 );
}
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
   
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_CounterRange        : Selection form counter range.
 * @param [in] b_FirstCounterModus   : First counter operating mode.
 * @param [in] b_FirstCounterOption  : First counter option.
 * @param [in] b_SecondCounterModus  : Second counter operating mode.
 * @param [in] b_SecondCounterOption : Second counter option.
 *
 * @retval 0: No error.                                            
 * @retval 1: The handle parameter of the board is wrong.          
 * @retval 2: The module is not a counter module.                  
 * @retval 3: The selected counter range is wrong.                
 * @retval 4: The selected first counter operating mode is wrong. 
 * @retval 5: The selected first counter operating option is wrong.
 * @retval 6: The selected second counter operating mode is wrong.
 * @retval 7: The selected second counter operating option is wrong.                                              
 */
int   i_APCI1710_InitCounter (struct pci_dev *pdev,
                              uint8_t b_ModulNbr,
                              uint8_t b_CounterRange,
                              uint8_t b_FirstCounterModus,
                              uint8_t b_FirstCounterOption,
                              uint8_t b_SecondCounterModus,
                              uint8_t b_SecondCounterOption)
	{
    int i_ReturnValue = 0;

//	unsigned long l_b_ModulNbr = b_ModulNbr;
//    unsigned long l_b_CounterRange = b_CounterRange;
//    unsigned long l_b_FirstCounterModus = b_FirstCounterModus;
//    unsigned long l_b_FirstCounterOption = b_FirstCounterOption;
//    unsigned long l_b_SecondCounterModus = b_SecondCounterModus;
//    unsigned long l_b_SecondCounterOption = b_SecondCounterOption;

    if (!pdev) return 1;

	/*******************************/
	/* Test if incremental counter */
	/*******************************/
	
	if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_INCREMENTAL_COUNTER)
	   {
	   /**************************/
	   /* Test the counter range */
	   /**************************/

	   if (b_CounterRange == APCI1710_16BIT_COUNTER || b_CounterRange == APCI1710_32BIT_COUNTER)
	      {
	      /********************************/
	      /* Test the first counter modus */
	      /********************************/

	      if (b_FirstCounterModus == APCI1710_QUADRUPLE_MODE ||
		  b_FirstCounterModus == APCI1710_DOUBLE_MODE    ||
		  b_FirstCounterModus == APCI1710_SIMPLE_MODE    ||
		  b_FirstCounterModus == APCI1710_DIRECT_MODE)
		 {
		 /*********************************/
		 /* Test the first counter option */
		 /*********************************/

		 if ((b_FirstCounterModus   == APCI1710_DIRECT_MODE   &&
		      (b_FirstCounterOption == APCI1710_INCREMENT     ||
		       b_FirstCounterOption == APCI1710_DECREMENT))   ||
		     (b_FirstCounterModus   != APCI1710_DIRECT_MODE   &&
		      (b_FirstCounterOption == APCI1710_HYSTERESIS_ON ||
		       b_FirstCounterOption == APCI1710_HYSTERESIS_OFF)))
		    {
		    /**************************/
		    /* Test if 16-bit counter */
		    /**************************/

		    if (b_CounterRange == APCI1710_16BIT_COUNTER)
		       {
		       /*********************************/
		       /* Test the second counter modus */
		       /*********************************/

		       if ((b_FirstCounterModus != APCI1710_DIRECT_MODE      &&
			    (b_SecondCounterModus == APCI1710_QUADRUPLE_MODE ||
			     b_SecondCounterModus == APCI1710_DOUBLE_MODE    ||
			     b_SecondCounterModus == APCI1710_SIMPLE_MODE))  ||
			   (b_FirstCounterModus == APCI1710_DIRECT_MODE &&
			    b_SecondCounterModus == APCI1710_DIRECT_MODE))
			  {
			  /**********************************/
			  /* Test the second counter option */
			  /**********************************/

			  if ((b_SecondCounterModus   == APCI1710_DIRECT_MODE   &&
			       (b_SecondCounterOption == APCI1710_INCREMENT     ||
				b_SecondCounterOption == APCI1710_DECREMENT))   ||
			      (b_SecondCounterModus   != APCI1710_DIRECT_MODE   &&
			       (b_SecondCounterOption == APCI1710_HYSTERESIS_ON ||
				b_SecondCounterOption == APCI1710_HYSTERESIS_OFF)))
			     {
			     i_ReturnValue = 0;
			     }
			  else
			     {
			     /*********************************************************/
			     /* The selected second counter operating option is wrong */
			     /*********************************************************/

			     i_ReturnValue = 7;
			     }
			  }
		       else
			  {
			  /*******************************************************/
			  /* The selected second counter operating mode is wrong */
			  /*******************************************************/

			  i_ReturnValue = 6;
			  }
		       }
		    }
		 else
		    {
		    /********************************************************/
		    /* The selected first counter operating option is wrong */
		    /********************************************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /******************************************************/
		 /* The selected first counter operating mode is wrong */
		 /******************************************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /***************************************/
	      /* The selected counter range is wrong */
	      /***************************************/

	      i_ReturnValue = 3;
	      }

	   /*************************/
	   /* Test if a error occur */
	   /*************************/

	   if (i_ReturnValue == 0)
	      {
	      /**************************/
	      /* Test if 16-Bit counter */
	      /**************************/

	      if (b_CounterRange == APCI1710_32BIT_COUNTER)
		 {
		 APCI1710_PRIVDATA(pdev)->
		 s_ModuleInfo [(int)b_ModulNbr].
		 s_SiemensCounterInfo.
		 s_ModeRegister.
		 s_ByteModeRegister.
		 b_ModeRegister1 = b_CounterRange      |
				   b_FirstCounterModus |
				   b_FirstCounterOption;
		 }
	      else
		 {
		 APCI1710_PRIVDATA(pdev)->
		 s_ModuleInfo [(int)b_ModulNbr].
		 s_SiemensCounterInfo.
		 s_ModeRegister.
		 s_ByteModeRegister.
		 b_ModeRegister1 = b_CounterRange                 |
				   (b_FirstCounterModus   & 0x5)  |
				   (b_FirstCounterOption  & 0x20) |
				   (b_SecondCounterModus  & 0xA)  |
				   (b_SecondCounterOption & 0x40);

		 /***********************/
		 /* Test if direct mode */
		 /***********************/

		 if (b_FirstCounterModus == APCI1710_DIRECT_MODE)
		    {
		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [(int)b_ModulNbr].
		    s_SiemensCounterInfo.
		    s_ModeRegister.
		    s_ByteModeRegister.
		    b_ModeRegister1 = APCI1710_PRIVDATA(pdev)->
				      s_ModuleInfo [(int)b_ModulNbr].
				      s_SiemensCounterInfo.
				      s_ModeRegister.
				      s_ByteModeRegister.
				      b_ModeRegister1 | APCI1710_DIRECT_MODE;
		    }
		 }

	      /***************************/
	      /* Write the configuration */
	      /***************************/

	      OUTPDW (GET_BAR2(pdev),
		      20 + MODULE_OFFSET(b_ModulNbr),
		      APCI1710_PRIVDATA(pdev)->
		      s_ModuleInfo [(int)b_ModulNbr].
		      s_SiemensCounterInfo.
		      s_ModeRegister.
		      dw_ModeRegister1_2_3_4);


	      APCI1710_PRIVDATA(pdev)->
	      s_ModuleInfo [(int)b_ModulNbr].
	      s_SiemensCounterInfo.
	      s_InitFlag.
	      b_CounterInit = 1;
	      }
	   }
	else
	   {
	   /**************************************/
	   /* The module is not a counter module */
	   /**************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}       

//------------------------------------------------------------------------------

/** Clear the counter value.
 * 
 * Clear the counter value from selected module (b_ModulNbr)
 * 
 * @param [in] pdev          : The device to initialize.
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module number parameter is wrong.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 */
int   i_APCI1710_ClearCounterValue (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /*********************/
	      /* Clear the counter */
	      /*********************/

	      OUTPDW (GET_BAR2(pdev), 16 + MODULE_OFFSET(b_ModulNbr), 1);
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
	
//------------------------------------------------------------------------------	
	
/** Clear all counter value.
 * 
 * @param [in] pdev          : The device to initialize.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 */
int   i_APCI1710_ClearAllCounterValue (struct pci_dev *pdev)
	{
	char   b_ModulCpt    = 0;
	int   i_ReturnValue = 0;

    if (!pdev) return 1;

	/********************************/
	/* Test if counter module found */
	/********************************/

	if ( (APCI1710_MODULE_FUNCTIONALITY(pdev,0) == APCI1710_INCREMENTAL_COUNTER) ||
	    (APCI1710_MODULE_FUNCTIONALITY(pdev,1) == APCI1710_INCREMENTAL_COUNTER) ||
	    (APCI1710_MODULE_FUNCTIONALITY(pdev,2) == APCI1710_INCREMENTAL_COUNTER) ||
	    (APCI1710_MODULE_FUNCTIONALITY(pdev,3) == APCI1710_INCREMENTAL_COUNTER) )
	   {
	   for (b_ModulCpt = 0; b_ModulCpt < NUMBER_OF_MODULE(pdev) ; b_ModulCpt ++)
	      {
	      /*******************************/
	      /* Test if incremental counter */
	      /*******************************/

	      if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulCpt) == APCI1710_INCREMENTAL_COUNTER)
		 {
		 /*********************/
		 /* Clear the counter */
		 /*********************/

		 OUTPDW (GET_BAR2(pdev), 16 + MODULE_OFFSET(b_ModulCpt), 1);
		 }
	      }
	   }
	else
	   {
	   /***************************/
	   /* No counter module found */
	   /***************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Set / reset a filter on the selected module.
 *
 * @param [in] pdev             : The device to initialize.
 * @param [in] b_ModulNbr       : Module number to configure (0 to 3).
 * @param [in] b_ClockSelection : Selection from PCI bus clock
 *                                - APCI1710_30MHZ:
 *                                  The PC has a PCI bus clock from 30 MHz
 *                                  Not available with the APCIe-1711
 *                                - APCI1710_33MHZ:
 *                                  The PC has a PCI bus clock from 33 MHz
 *                                  Not available with the APCIe-1711
 *                                - APCI1710_40MHZ
 *                                  The APCI-1710 has a integrated 40Mhz quartz.
 * @param [in] b_Filter         : Filter selection
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
 */
int i_APCI1710_SetInputFilter (struct pci_dev *pdev,
                               uint8_t b_ModulNbr,
                               uint8_t b_PCIInputClock,
                               uint8_t b_Filter)
{
	uint32_t dw_Status = 0;

    if (!pdev) return 1;

	/* Test the module number */
	if (b_ModulNbr >= NUMBER_OF_MODULE(pdev))
		return 2;

	/* Test if counter initialized */
	if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_CounterInit != 1)
		return 3;

	/* check firmware revision */
	if (APCI1710_MODULE_VERSION(pdev,b_ModulNbr) < 0x3135)
		return 3;

	/* Test the PCI bus clock */
	if ((b_PCIInputClock != APCI1710_30MHZ) && (b_PCIInputClock != APCI1710_33MHZ) && (b_PCIInputClock != APCI1710_40MHZ))
		return 4;

	/* Test the filter value */
	if (b_Filter >= 16)
		return 5;

	/* Test if 40MHz used */
	if (b_PCIInputClock == APCI1710_40MHZ)
	{
		/* Test if 40MHz quartz on board */
		 INPDW (GET_BAR2(pdev), 36 + MODULE_OFFSET(b_ModulNbr), &dw_Status);

		/* Test the quartz flag (DQ0) */
		if ((dw_Status & 1) != 1)
		{
			/* 40MHz quartz not on board */
			return 6;
		}
	}

	/* Test if 40MHz used */
	if (b_PCIInputClock == APCI1710_40MHZ)
	{
		/* Enable the 40MHz quarz (DQ31) */
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 |= APCI1710_ENABLE_40MHZ_FILTER;
	}
	else
	{
		/* Disable the 40MHz quarz (DQ31) */
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 &= APCI1710_DISABLE_40MHZ_FILTER;
	}

	/* Set the filter value */
	APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister3 =
	(APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister3 & 0x1F) | ((b_Filter & 0x7) << 5);

	APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 =
	(APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 & 0xFE) | ((b_Filter & 0x8) >> 3);

	/* Write the configuration */
	OUTPDW (GET_BAR2(pdev), 20 + MODULE_OFFSET(b_ModulNbr),APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.dw_ModeRegister1_2_3_4);

	return 0;
}

//------------------------------------------------------------------------------

/** Latch the value from selected module.
 * 
 * Latch the value from selected module (b_ModulNbr) 
 * in to the selected latch register (b_LatchReg).
 *
 * @param [in] pdev          : The device to initialize.
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 * @param [in] b_LatchReg    : Selected latch register <br>
 *                             0 : for the first latch register. <br>
 *                             1 : for the second latch register.
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: The selected latch register parameter is wrong.
 */
int   i_APCI1710_LatchCounter (struct pci_dev *pdev,
                               uint8_t b_ModulNbr,
                               uint8_t b_LatchReg)
	{
	int i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /*************************************/
	      /* Test the latch register parameter */
	      /*************************************/

	      if (b_LatchReg < 2)
		 {
		 /*********************/
		 /* Tatch the counter */
		 /*********************/

		 OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), 1 << (b_LatchReg * 4));
		 }
	      else
		 {
		 /**************************************************/
		 /* The selected latch register parameter is wrong */
		 /**************************************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------
	
/** Read the latch register status from selected module.
 * 
 * Read the latch register status from selected module
 * (b_ModulNbr) and selected latch register (b_LatchReg).
 *
 * @param [in] pdev          : The device to initialize.
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 * @param [in] b_LatchReg    : Selected latch register <br>
 *                             0 : for the first latch register. <br>
 *                             1 : for the second latch register.
 * 
 * @param [out] pb_LatchStatus : Latch register status. <br>
 *                               0 : No latch occur.
 *                               1 : A software latch occur.
 *                               2 : A hardware latch occur.
 *                               3 : A software and hardware latch occur.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: The selected latch register parameter is wrong.
 */
int   i_APCI1710_ReadLatchRegisterStatus (struct pci_dev *pdev,
                                          uint8_t b_ModulNbr,
                                          uint8_t b_LatchReg,
                                          uint8_t * pb_LatchStatus)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_LatchReg;
	
    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /*************************************/
	      /* Test the latch register parameter */
	      /*************************************/

	      if (b_LatchReg < 2)
		 {
		 INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_LatchReg);

		 *pb_LatchStatus = (char) ((dw_LatchReg >> (b_LatchReg * 4)) & 0x3);
		 }
	      else
		 {
		 /**************************************************/
		 /* The selected latch register parameter is wrong */
		 /**************************************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }


	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Read the latch register value. 
 * 
 * Read the latch register value from selected module
 * (b_ModulNbr) and selected latch register (b_LatchReg).
 * 
 * @param [in] pdev          : The device to initialize.
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 * @param [in] b_LatchReg    : Selected latch register <br>
 *                             0 : for the first latch register. <br>
 *                             1 : for the second latch register.
 * 
 * @param [out] pul_LatchValue : Latch register value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: The selected latch register parameter is wrong.
 */
int   i_APCI1710_ReadLatchRegisterValue (struct pci_dev *pdev,
                                         uint8_t     b_ModulNbr,
                                         uint8_t     b_LatchReg,
                                         uint32_t * pul_LatchValue)
	{
	int i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /*************************************/
	      /* Test the latch register parameter */
	      /*************************************/

	      if (b_LatchReg < 2)
		 {
		 INPDW (GET_BAR2(pdev), ((b_LatchReg + 1) * 4) + MODULE_OFFSET(b_ModulNbr), pul_LatchValue);
		 }
	      else
		 {
		 /**************************************************/
		 /* The selected latch register parameter is wrong */
		 /**************************************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
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
int   i_APCI1710_EnableLatchInterrupt (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int    i_ReturnValue   = 0;
	uint32_t dw_Configuration = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /********************************/
	      /* Test if interrupt initialsed */
	      /********************************/

	     if ((APCI1710_PRIVDATA(pdev)->s_InterruptInfos.b_InterruptInitialized) != 0)
		 {
		 /**************************************************************/
		 /* Begin SW 2240-0602 -> 2241-0802 Interrupt latch management */
                 /**************************************************************/

		 /************************************************/
		 /* Set the interrupt management to the new mode */
                 /************************************************/

		 INPDW (GET_BAR2(pdev),
			40 + MODULE_OFFSET(b_ModulNbr),
			&dw_Configuration);

		 dw_Configuration = dw_Configuration | 1;

		 OUTPDW (GET_BAR2(pdev),
			 40 + MODULE_OFFSET(b_ModulNbr),
			 dw_Configuration);

		 /************************************************************/
		 /* End SW 2240-0602 -> 2241-0802 Interrupt latch management */
		 /************************************************************/

		 /********************/
		 /* Enable interrupt */
		 /********************/

		 APCI1710_PRIVDATA(pdev)->
		 s_ModuleInfo [(int)b_ModulNbr].
		 s_SiemensCounterInfo.
		 s_ModeRegister.
		 s_ByteModeRegister.
		 b_ModeRegister2 = APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModulNbr].
				   s_SiemensCounterInfo.
				   s_ModeRegister.
				   s_ByteModeRegister.
				   b_ModeRegister2 | APCI1710_ENABLE_LATCH_INT;

		 /***************************/
		 /* Write the configuration */
		 /***************************/

		 OUTPDW (GET_BAR2(pdev),
			 20 + MODULE_OFFSET(b_ModulNbr),
			 APCI1710_PRIVDATA(pdev)->
			 s_ModuleInfo [(int)b_ModulNbr].
			 s_SiemensCounterInfo.
			 s_ModeRegister.
			 dw_ModeRegister1_2_3_4);
		 }
	      else
		 {
		 /***********************************/
		 /* Interrupt routine not installed */
		 /***********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }


	return (i_ReturnValue);
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
int   i_APCI1710_DisableLatchInterrupt (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /********************************/
	      /* Test if interrupt initialsed */
	      /********************************/

	      if ((APCI1710_PRIVDATA(pdev)->s_InterruptInfos.b_InterruptInitialized) != 0)
		 {
		 /***************************/
		 /* Write the configuration */
		 /***************************/

		 OUTPDW (GET_BAR2(pdev),
			 20 + MODULE_OFFSET(b_ModulNbr),
			 APCI1710_PRIVDATA(pdev)->
			 s_ModuleInfo [(int)b_ModulNbr].
			 s_SiemensCounterInfo.
			 s_ModeRegister.
			 dw_ModeRegister1_2_3_4 & ((APCI1710_DISABLE_LATCH_INT << 8) | 0xFF));

		    mdelay (1);

		 /*********************/
		 /* Disable interrupt */
		 /*********************/

		 APCI1710_PRIVDATA(pdev)->
		 s_ModuleInfo [(int)b_ModulNbr].
		 s_SiemensCounterInfo.
		 s_ModeRegister.
		 s_ByteModeRegister.
		 b_ModeRegister2 = APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModulNbr].
				   s_SiemensCounterInfo.
				   s_ModeRegister.
				   s_ByteModeRegister.
				   b_ModeRegister2 & APCI1710_DISABLE_LATCH_INT;

		 }
	      else
		 {
		 /***********************************/
		 /* Interrupt routine not installed */
		 /***********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------	
		
/** Latch the selected 16-Bit counter. 
 * 
 * Latch the selected 16-Bit counter (b_SelectedCounter)
 * from selected module (b_ModulNbr) in to the first
 * latch register and return the latched value.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_SelectedCounter : Selected 16-Bit counter (0 or 1).
 * 
 * @param [out] pui_CounterValue : 16-Bit counter value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: The selected 16-Bit counter parameter is wrong.
 */
int   i_APCI1710_Read16BitCounterValue (struct pci_dev *pdev,
                                        uint8_t    b_ModulNbr,
                                        uint8_t    b_SelectedCounter,
                                        uint32_t * pui_CounterValue)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_LathchValue = 0;
	
    if (!pdev) return 1;	

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /******************************/
	      /* Test the counter selection */
	      /******************************/

	      if (b_SelectedCounter < 2)
		 {
		 /*********************/
		 /* Latch the counter */
		 /*********************/

		 OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), 1);

		 /************************/
		 /* Read the latch value */
		 /************************/

		 INPDW (GET_BAR2(pdev), 4 + MODULE_OFFSET(b_ModulNbr), &dw_LathchValue);

		 *pui_CounterValue = (unsigned int) ((dw_LathchValue >> (16 * b_SelectedCounter)) & 0xFFFFU);
		 }
	      else
		 {
		 /**************************************************/
		 /* The selected 16-Bit counter parameter is wrong */
		 /**************************************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
		
//------------------------------------------------------------------------------
		
/** Latch the 32-Bit counter. 
 * 
 * Latch the 32-Bit counter from selected module
 * (b_ModulNbr) in to the first latch register and return the latched value.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @param [out] pui_CounterValue : 32-Bit counter value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 */
 int   i_APCI1710_Read32BitCounterValue (struct pci_dev *pdev,
                                        uint8_t      b_ModulNbr,
                                        uint32_t * pul_CounterValue)
	{
	int    i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /*********************/
	      /* Tatch the counter */
	      /*********************/

	      OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), 1);

	      /************************/
	      /* Read the latch value */
	      /************************/

	      INPDW (GET_BAR2(pdev), 4 + MODULE_OFFSET(b_ModulNbr), pul_CounterValue);
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
	
//------------------------------------------------------------------------------
	
/** Write a 16-Bit value. 
 * 
 * Write a 16-Bit value (ui_WriteValue) in to the selected
 * 16-Bit counter (b_SelectedCounter) from selected module (b_ModulNbr).
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_SelectedCounter : Selected 16-Bit counter (0 or 1).
 * @param [in] ui_WriteValue     : 16-Bit write value : (0x0 - 0xFFFF)
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: The b_SelectedCounter counter parameter is wrong.
 * @retval 5: The given ui_WriteValue value is not in the correct range. 
 */
int   i_APCI1710_Write16BitCounterValue (struct pci_dev *pdev,
                                         uint8_t  b_ModulNbr,
                                         uint8_t  b_SelectedCounter,
                                         uint32_t ui_WriteValue)
	{
	int i_ReturnValue = 0;

    if (!pdev) return 1;

	/* check ui_WriteValue */
	switch(ui_WriteValue)
	{
		case 0 ... 0xFFFF:
			break;
		default:
			return -5;
	}
	
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /******************************/
	      /* Test the counter selection */
	      /******************************/

	      if (b_SelectedCounter < 2)
		 {
		 /*******************/
		 /* Write the value */
		 /*******************/

		 OUTPDW (GET_BAR2(pdev), 8 + (b_SelectedCounter * 4) + MODULE_OFFSET(b_ModulNbr), (uint32_t) ((uint32_t) (ui_WriteValue) << (16 * b_SelectedCounter)));
		 }
	      else
		 {
		 /**************************************************/
		 /* The selected 16-Bit counter parameter is wrong */
		 /**************************************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
		
//------------------------------------------------------------------------------
			
/** Write a 32-Bit value. 
 * 
 * Write a 32-Bit value (ui_WriteValue) in to the selected module (b_ModulNbr).
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ul_WriteValue     : 32-Bit write value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 */
int   i_APCI1710_Write32BitCounterValue (struct pci_dev *pdev,
                                         uint8_t   b_ModulNbr,
                                         uint32_t ul_WriteValue)
	{
	int i_ReturnValue = 0;

    if (!pdev) 
    	return 1;
  
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /*******************/
	      /* Write the value */
	      /*******************/

	      OUTPDW (GET_BAR2(pdev), 4 + MODULE_OFFSET(b_ModulNbr), ul_WriteValue);
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Set the 32-Bit compare value. 
 * 
 * Set the 32-Bit compare value. At that moment that the
 * incremental counter arrive to the compare value       
 * (ui_CompareValue) a interrupt is generated.           
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ui_CompareValue   : 32-Bit compare value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 */
int   i_APCI1710_InitCompareLogic (struct pci_dev *pdev,
                                   uint8_t   b_ModulNbr,
                                   uint32_t ui_CompareValue)
	{
	int    i_ReturnValue = 0;
	
    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      OUTPDW (GET_BAR2(pdev), 28 + MODULE_OFFSET(b_ModulNbr), ui_CompareValue);

	      APCI1710_PRIVDATA(pdev)->
	      s_ModuleInfo [(int)b_ModulNbr].
	      s_SiemensCounterInfo.
	      s_InitFlag.
	      b_CompareLogicInit = 1;
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Enable the 32-Bit compare logic. 
 * 
 * Enable the 32-Bit compare logic. At that moment that
 * the incremental counter arrive to the compare value a
 * interrupt is generated.
 * 
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: Compare logic not initialised. See function "i_APCI1710_InitCompareLogic".
 * @retval 5: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutine".
 */
int   i_APCI1710_EnableCompareLogic (struct pci_dev *pdev, uint8_t   b_ModulNbr)
	{
	int    i_ReturnValue = 0;

    if (!pdev) return 1;
    
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /*************************************/
	      /* Test if compare logic initialised */
	      /*************************************/

	      if (APCI1710_PRIVDATA(pdev)->
		  s_ModuleInfo [(int)b_ModulNbr].
		  s_SiemensCounterInfo.
		  s_InitFlag.
		  b_CompareLogicInit == 1)
		 {
		 /******************************************/
		 /* Test if interrupt function initialised */
		 /******************************************/

		 if ((APCI1710_PRIVDATA(pdev)->s_InterruptInfos.b_InterruptInitialized) != 0)
		    {
		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [(int)b_ModulNbr].
		    s_SiemensCounterInfo.
		    s_ModeRegister.
		    s_ByteModeRegister.
		    b_ModeRegister3 = APCI1710_PRIVDATA(pdev)->
				      s_ModuleInfo [(int)b_ModulNbr].
				      s_SiemensCounterInfo.
				      s_ModeRegister.
				      s_ByteModeRegister.
				      b_ModeRegister3 | APCI1710_ENABLE_COMPARE_INT;

		    /***************************/
		    /* Write the configuration */
		    /***************************/

		    OUTPDW (GET_BAR2(pdev),
			    20 + MODULE_OFFSET(b_ModulNbr),
			    APCI1710_PRIVDATA(pdev)->
			    s_ModuleInfo [(int)b_ModulNbr].
			    s_SiemensCounterInfo.
			    s_ModeRegister.
			    dw_ModeRegister1_2_3_4);
		    }
		 else
		    {
		    /**************************************/
		    /* Interrupt function not initialised */
		    /**************************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /*********************************/
		 /* Compare logic not initialised */
		 /*********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
	
//------------------------------------------------------------------------------
	
/** Disable the 32-Bit compare logic.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found. 
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 * @retval 4: Compare logic not initialised. See function "i_APCI1710_InitCompareLogic".
 */
int   i_APCI1710_DisableCompareLogic  (struct pci_dev *pdev,
                                       uint8_t   b_ModulNbr)
	{
	int    i_ReturnValue = 0;

    if (!pdev) return 1;
   
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      /*************************************/
	      /* Test if compare logic initialised */
	      /*************************************/

	      if (APCI1710_PRIVDATA(pdev)->
		  s_ModuleInfo [(int)b_ModulNbr].
		  s_SiemensCounterInfo.
		  s_InitFlag.
		  b_CompareLogicInit == 1)
		 {
		 APCI1710_PRIVDATA(pdev)->
		 s_ModuleInfo [(int)b_ModulNbr].
		 s_SiemensCounterInfo.
		 s_ModeRegister.
		 s_ByteModeRegister.
		 b_ModeRegister3 = APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModulNbr].
				   s_SiemensCounterInfo.
				   s_ModeRegister.
				   s_ByteModeRegister.
				   b_ModeRegister3 & APCI1710_DISABLE_COMPARE_INT;

		 /***************************/
		 /* Write the configuration */
		 /***************************/

		 OUTPDW (GET_BAR2(pdev),
			 20 + MODULE_OFFSET(b_ModulNbr),
			 APCI1710_PRIVDATA(pdev)->
			 s_ModuleInfo [(int)b_ModulNbr].
			 s_SiemensCounterInfo.
			 s_ModeRegister.
			 dw_ModeRegister1_2_3_4);
		 }
	      else
		 {
		 /*********************************/
		 /* Compare logic not initialised */
		 /*********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
		
//------------------------------------------------------------------------------
		
/** Set the 32-Bit compare value.
 * 
 * Set the 32-Bit compare value. At that moment that the
 * incremental counter arrive to the compare value
 * (ui_CompareValue) a interrupt is generated and the TTL
 * output mask (ui_OutputMask is setting).
 * 
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ui_CompareValue   : 32-Bit compare value.
 * @param [in] ui_OutputMask     : TTL output mask.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: The selected functionality is not available.
 * @retval 4: FIFO full.
 */
int   i_APCI1710_SetCompareValue (struct pci_dev *pdev,
                                  uint8_t   b_ModulNbr,
                                  uint32_t  ui_CompareValue,
                                  uint32_t  ui_OutputMask)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_Status      = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /*******************/
	   /* Test if INC_CPT */
	   /*******************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr)== APCI1710_INCREMENTAL_COUNTER)
	      {
	      /********************/
	      /* Test the version */
	      /********************/

	      if ( APCI1710_MODULE_VERSION(pdev,b_ModulNbr) >= 0x3430)
		 {
		 /***********************/
		 /* Get the FIFO status */
		 /***********************/

		 INPDW (GET_BAR2(pdev),
			36 + MODULE_OFFSET(b_ModulNbr), &dw_Status);

		 /*************************/
		 /* Test if FIFO not full */
		 /*************************/

		 if (((dw_Status >> 16) & 1) == 0)
		    {
		    /***************************/
		    /* Write the compare value */
		    /***************************/

		    OUTPDW (GET_BAR2(pdev),
			    28 + MODULE_OFFSET(b_ModulNbr),
			    ui_CompareValue);

		    /*************************/
		    /* Write the output mask */
		    /*************************/

		    OUTPDW (GET_BAR2(pdev),
			    28 + MODULE_OFFSET(b_ModulNbr),
			    ui_OutputMask);

		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [(int)b_ModulNbr].
		    s_SiemensCounterInfo.
		    s_InitFlag.
		    b_CompareLogicInit = 1;
		    }
		 else
		    {
		    /*************/
		    /* FIFO full */
		    /*************/

		    i_ReturnValue = 4;
		    }
		 }
	      else
		 {
		 /**************************************/
		 /* The module is not a INC_CPT module */
		 /**************************************/

		 i_ReturnValue = 3;
		 }
	      }
	   else
	      {
	      /**************************************/
	      /* The module is not a INC_CPT module */
	      /**************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Compare logic TTL port selection.
 * 
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_PortSelection   : 0 : Port 0 selected. <br>
 *                                 1 : Port 1 selected.
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: The selected functionality is not available.
 * @retval 4: Port selection error.
 */
int   i_APCI1710_SetComparePort	(struct pci_dev *pdev,
                                 uint8_t   b_ModulNbr,
                                 uint8_t   b_PortSelection)
	{
	int    i_ReturnValue = 0;
	char   b_ModuleCpt   = 0;

    if (!pdev) return 1;
    
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************/
	   /* Test if INC_CPT */
	   /*******************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_INCREMENTAL_COUNTER)
	      {
	      /********************/
	      /* Test the version */
	      /********************/

	      if ( APCI1710_MODULE_VERSION(pdev,b_ModulNbr) >= 0x3430)
		 {
		 /***************************/
		 /* Test the port selection */
		 /***************************/

		 if ((b_PortSelection == 0) || (b_PortSelection == 1))
		    {
		    for (b_ModuleCpt = 0; b_ModuleCpt < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule; b_ModuleCpt ++)
		       {
		       /*******************/
		       /* Test if INC_CPT */
		       /*******************/

		       if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_INCREMENTAL_COUNTER)
			  {
			  /********************/
			  /* Test the version */
			  /********************/

			  if ( APCI1710_MODULE_VERSION(pdev,b_ModulNbr) >= 0x3430)
			     {
			     /*****************************/
			     /* Test if port already used */
			     /*****************************/

			     if (((APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModuleCpt].
				   s_SiemensCounterInfo.
				   s_InitCompare.
				   s_BitRegister.
				   b_Port0Selection == 1) &&
				   (b_PortSelection == 0)) ||
				 ((APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModuleCpt].
				   s_SiemensCounterInfo.
				   s_InitCompare.
				   s_BitRegister.
				   b_Port1Selection == 1) &&
				   (b_PortSelection == 1)))
				{
				if (b_PortSelection == 0)
				   {
				   APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModuleCpt].
				   s_SiemensCounterInfo.
				   s_InitCompare.
				   s_BitRegister.
				   b_Port0Selection = 0;
				   }
				else
				   {
				   APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModuleCpt].
				   s_SiemensCounterInfo.
				   s_InitCompare.
				   s_BitRegister.
				   b_Port1Selection = 0;
				   }

				/***************************************/
				/* Reset the selected compare TTL port */
				/***************************************/

				OUTPDW (GET_BAR2(pdev),
					52 + MODULE_OFFSET(b_ModuleCpt),
					APCI1710_PRIVDATA(pdev)->
					s_ModuleInfo [(int)b_ModuleCpt].
					s_SiemensCounterInfo.
					s_InitCompare.
					dw_DWordRegister);
				}
			     }
			  }
		       }

		    if (b_PortSelection == 0)
		       {
		       APCI1710_PRIVDATA(pdev)->
		       s_ModuleInfo [(int)b_ModulNbr].
		       s_SiemensCounterInfo.
		       s_InitCompare.
		       s_BitRegister.
		       b_Port1Selection = 0;

		       APCI1710_PRIVDATA(pdev)->
		       s_ModuleInfo [(int)b_ModulNbr].
		       s_SiemensCounterInfo.
		       s_InitCompare.
		       s_BitRegister.
		       b_Port0Selection = 1;
		       }
		    else
		       {
		       APCI1710_PRIVDATA(pdev)->
		       s_ModuleInfo [(int)b_ModulNbr].
		       s_SiemensCounterInfo.
		       s_InitCompare.
		       s_BitRegister.
		       b_Port1Selection = 1;

		       APCI1710_PRIVDATA(pdev)->
		       s_ModuleInfo [(int)b_ModulNbr].
		       s_SiemensCounterInfo.
		       s_InitCompare.
		       s_BitRegister.
		       b_Port0Selection = 0;
		       }

		    /*************************************/
		    /* Set the selected compare TTL port */
		    /*************************************/

		    OUTPDW (GET_BAR2(pdev),
			    52 + MODULE_OFFSET(b_ModulNbr),
			    APCI1710_PRIVDATA(pdev)->
			    s_ModuleInfo [(int)b_ModulNbr].
			    s_SiemensCounterInfo.
			    s_InitCompare.
			    dw_DWordRegister);
		    }
		 else
		    {
		    /************************/
		    /* Port selection error */
		    /************************/

		    i_ReturnValue = 4;
		    }
		 }
	      else
		 {
		 /**************************************/
		 /* The module is not a INC_CPT module */
		 /**************************************/

		 i_ReturnValue = 3;
		 }
	      }
	   else
	      {
	      /**************************************/
	      /* The module is not a INC_CPT module */
	      /**************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Compare logic watchdog initialisation and activation.
 * 
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ui_WatchdogTime   : Watchdog time (1 to 65535ms).
 * @param [in] ui_OutputMask     : APCI1710_ENABLE : Enable the watchdog. <br>
 *                                 APCI1710_DISABLE: Disable the watchdog.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: The selected functionality is not available.
 * @retval 4: Selected watchdog time error.
 * @retval 5: Enable/Disable flag selection error.
 */
int   i_APCI1710_InitCompareWatchdog (struct pci_dev *pdev,
                                      uint8_t   b_ModulNbr,
                                      uint32_t  ui_WatchdogTime,
                                      uint8_t   b_EnableDisable)
	{
	int    i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************/
	   /* Test if INC_CPT */
	   /*******************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_INCREMENTAL_COUNTER)
	      {
	      /********************/
	      /* Test the version */
	      /********************/

	      if ( APCI1710_MODULE_VERSION(pdev,b_ModulNbr) >= 0x3430)
		 {
		 /********************************/
		 /* Test the enable disable flag */
		 /********************************/

		 if ((b_EnableDisable == APCI1710_ENABLE) ||
		     (b_EnableDisable == APCI1710_DISABLE))
		    {
		    /**************************/
		    /* Test the watchdog time */
		    /**************************/

		    if ((b_EnableDisable == APCI1710_DISABLE) ||
			((b_EnableDisable == APCI1710_ENABLE) &&
			 ((ui_WatchdogTime >= 1) && (ui_WatchdogTime <= 65535))))
		       {
		       /******************/
		       /* Test if enable */
		       /******************/

		       if (b_EnableDisable == APCI1710_ENABLE)
			  {
			  /*************************/
			  /* Set the watchdog time */
			  /*************************/

			  OUTPDW (GET_BAR2(pdev),
				  48 + MODULE_OFFSET(b_ModulNbr),
				  ui_WatchdogTime);
			  }

		       APCI1710_PRIVDATA(pdev)->
		       s_ModuleInfo [(int)b_ModulNbr].
		       s_SiemensCounterInfo.
		       s_InitCompare.
		       s_BitRegister.
		       b_EnableTimer = b_EnableDisable;

		       /*******************************/
		       /* Enable/Disable the watchdog */
		       /*******************************/

		       OUTPDW (GET_BAR2(pdev),
			       52 + MODULE_OFFSET(b_ModulNbr),
			       APCI1710_PRIVDATA(pdev)->
			       s_ModuleInfo [(int)b_ModulNbr].
			       s_SiemensCounterInfo.
			       s_InitCompare.
			       dw_DWordRegister);
		       }
		    else
		       {
		       /********************************/
		       /* Selected watchdog time error */
		       /********************************/

		       i_ReturnValue = 4;
		       }
		    }
		 else
		    {
		    /*****************************/
		    /* Enable disable flag error */
		    /*****************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /**************************************/
		 /* The module is not a INC_CPT module */
		 /**************************************/

		 i_ReturnValue = 3;
		 }
	      }
	   else
	      {
	      /**************************************/
	      /* The module is not a INC_CPT module */
	      /**************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
		
//------------------------------------------------------------------------------
					
/** Get the compare watchdog status.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * 
 * @param pb_Status : 0: No watchdog overflow occur. <br>
 *                    1: Watchdog overflow occur.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: The selected functionality is not available.
 * @retval 4: Watchdog not active.
 */
int   i_APCI1710_GetCompareWatchdogStatus (struct pci_dev *pdev,
                                           uint8_t   b_ModulNbr,
                                           uint8_t * pb_Status)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_Status      = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************/
	   /* Test if INC_CPT */
	   /*******************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_INCREMENTAL_COUNTER)
	      {
	      /********************/
	      /* Test the version */
	      /********************/

	      if ( APCI1710_MODULE_VERSION(pdev,b_ModulNbr) >= 0x3430)
		 {
		 /***************************/
		 /* Test if watchdog active */
		 /***************************/

		 if (APCI1710_PRIVDATA(pdev)->
		     s_ModuleInfo [(int)b_ModulNbr].
		     s_SiemensCounterInfo.
		     s_InitCompare.
		     s_BitRegister.
		     b_EnableTimer)
		    {
		    /***************************/
		    /* Get the Watchdog status */
		    /***************************/

		    INPDW (GET_BAR2(pdev),
			   56 + MODULE_OFFSET(b_ModulNbr), &dw_Status);

		    /********************/
		    /* Test if overflow */
                    /********************/

		    if (dw_Status & 1)
		       {
		       *pb_Status = 1;
		       }
		    else
		       {
		       *pb_Status = 0;
		       }
		    }
		 else
		    {
		    /*************************/
		    /* Watchdog not activate */
		    /*************************/

		    i_ReturnValue = 4;
		    }
		 }
	      else
		 {
		 /**************************************/
		 /* The module is not a INC_CPT module */
		 /**************************************/

		 i_ReturnValue = 3;
		 }
	      }
	   else
	      {
	      /**************************************/
	      /* The module is not a INC_CPT module */
	      /**************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
	
//------------------------------------------------------------------------------
	
/** Clear the compare FIFO.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval 3: The selected functionality is not available.
 */
int   i_APCI1710_ClearCompareFIFO (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int    i_ReturnValue = 0;

    if (!pdev) return 1;
    
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************/
	   /* Test if INC_CPT */
	   /*******************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_INCREMENTAL_COUNTER)
	      {
	      /********************/
	      /* Test the version */
	      /********************/

	      if ( APCI1710_MODULE_VERSION(pdev,b_ModulNbr) >= 0x3430)
		 {
		 /******************/
		 /* Clear the FIFO */
		 /******************/

		 OUTPDW (GET_BAR2(pdev),
			 16 + MODULE_OFFSET(b_ModulNbr),
			 0x4);
		 }
	      else
		 {
		 /**************************************/
		 /* The module is not a INC_CPT module */
		 /**************************************/

		 i_ReturnValue = 3;
		 }
	      }
	   else
	      {
	      /**************************************/
	      /* The module is not a INC_CPT module */
	      /**************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
		
//------------------------------------------------------------------------------
					
/** Change the counter direction.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_Direction       : 0 : not invert.
 *                                 1 : invert.
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module number is wrong.
 * @retval 3: The selected functionality is not available.
 * @retval 4: The selected direction is wrong.
 */
int   i_APCI1710_ChangeCounterDirection (struct pci_dev *pdev,
                                         uint8_t   b_ModulNbr,
                                         uint8_t   b_Direction)
	{
	int    i_ReturnValue  = 0;
	uint32_t  ul_OutputValue = 0;

    if (!pdev) return 1;
    
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************/
	   /* Test if INC_CPT */
	   /*******************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_INCREMENTAL_COUNTER)
	      {
	      /********************/
	      /* Test the version */
	      /********************/

	      if ( APCI1710_MODULE_VERSION(pdev,b_ModulNbr) >= 0x3430)
		 {
		 /**********************/
		 /* Test the direction */
		 /**********************/

		 if ((b_Direction == 0) || (b_Direction == 1))
		    {
		    /*******************/
		    /* write direction */
		    /*******************/

                    ul_OutputValue = (unsigned long)b_Direction;

		    OUTPDW (GET_BAR2(pdev),
			    56 + MODULE_OFFSET(b_ModulNbr),
			    ul_OutputValue);
	            }
	         else
		    {
		    /***********************************/
		    /* The selected direction is wrong */
		    /***********************************/

		    i_ReturnValue = 4;
		    }
		 }
	      else
		 {
		 /**************************************/
		 /* The module is not a INC_CPT module */
		 /**************************************/

		 i_ReturnValue = 3;
		 }
	      }
	   else
	      {
	      /**************************************/
	      /* The module is not a INC_CPT module */
	      /**************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
	
//------------------------------------------------------------------------------
	
/** Sets the digital output H. 
 * 
 * Sets the digital output H. Setting an output means setting an ouput high.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module number is wrong.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 */
int	i_APCI1710_SetDigitalChlOn (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int    i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      APCI1710_PRIVDATA(pdev)->
	      s_ModuleInfo [(int)b_ModulNbr].
	      s_SiemensCounterInfo.
	      s_ModeRegister.
	      s_ByteModeRegister.
	      b_ModeRegister3 = APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SiemensCounterInfo.
				s_ModeRegister.
				s_ByteModeRegister.
				b_ModeRegister3 | 0x10;

	      /*********************/
	      /* Set the output On */
	      /*********************/

	      OUTPDW (GET_BAR2(pdev),
		      20 + MODULE_OFFSET(b_ModulNbr),
		      APCI1710_PRIVDATA(pdev)->
		      s_ModuleInfo [(int)b_ModulNbr].
		      s_SiemensCounterInfo.
		      s_ModeRegister.
		      dw_ModeRegister1_2_3_4);
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Resets the digital output H. 
 * 
 * Resets the digital output H. Resetting an output means setting an ouput low.
 * 
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module number is wrong.
 * @retval 3: Counter not initialised see function "i_APCI1710_InitCounter".
 */
int	i_APCI1710_SetDigitalChlOff (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int    i_ReturnValue = 0;

    if (!pdev) return 1;
    
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if counter initialised */
	   /*******************************/

	   if (APCI1710_PRIVDATA(pdev)->
	       s_ModuleInfo [(int)b_ModulNbr].
	       s_SiemensCounterInfo.
	       s_InitFlag.
	       b_CounterInit == 1)
	      {
	      APCI1710_PRIVDATA(pdev)->
	      s_ModuleInfo [(int)b_ModulNbr].
	      s_SiemensCounterInfo.
	      s_ModeRegister.
	      s_ByteModeRegister.
	      b_ModeRegister3 = APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SiemensCounterInfo.
				s_ModeRegister.
				s_ByteModeRegister.
				b_ModeRegister3 & 0xEF;

	      /**********************/
	      /* Set the output Off */
	      /**********************/

	      OUTPDW (GET_BAR2(pdev),
		      20 + MODULE_OFFSET(b_ModulNbr),
		      APCI1710_PRIVDATA(pdev)->
		      s_ModuleInfo [(int)b_ModulNbr].
		      s_SiemensCounterInfo.
		      s_ModeRegister.
		      dw_ModeRegister1_2_3_4);
	      }
	   else
	      {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*************************************************/
	   /* The selected module number parameter is wrong */
	   /*************************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
	
//------------------------------------------------------------------------------
#ifndef WITH_IDV_OPTION
/** Returns the counter progress latched status after a index interrupt occur.
 * 
 * @param [in] pdev          : The device
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 * 
 * @param [out] pul_UDStatus  : 0 : Counter progress in the selected mode down
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
int i_APCI1710_GetInterruptUDLatchedStatus  (struct pci_dev * pdev,
                                                 uint8_t b_ModulNbr,
                                                 uint8_t * pul_UDStatus)
{
	if (!pdev)
		return 1;
	
	if ( NOT_A_MODULE(pdev, b_ModulNbr) )
		return 2;	
	
	if ( NOT_A_COUNTER(pdev, b_ModulNbr) )
		return 3;
	
	if ( COUNTER_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 4;
	
	if ( INTERRUPT_FUNCTION_NOT_INITIALISED(pdev) )
		return 5;
	
	/* has index interrupt occured ? */	
	if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_IndexInterruptOccur == 1)
    {
    	/* clear flag */
    	APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_IndexInterruptOccur = 0;
		{
			uint32_t val;
		    INPDW  ( GET_BAR2(pdev), 12 + MODULE_OFFSET(b_ModulNbr), &val);
			*pul_UDStatus = ((val >> 1) & 0x1);
		}
	}		
	else
	{	   
		/* no index interrupt */
		*pul_UDStatus=2;
	}
	
	return 0;
}

#endif

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

 * 
 * @param [in] b_ModulNbr    : Module number (0 to 3).
 * @param [in] b_ReferenceAction : 	Determine if the reference must set or no for the acceptance from index  
 * 									APCI1710_ENABLE : Reference must be set for accepted the index
 * 									APCI1710_DISABLE : Reference have no importance
 * @param [in] b_IndexOperation  : (APCI1710_HIGH_EDGE_LATCH_COUNTER, APCI1710_LOW_EDGE_LATCH_COUNTER, APCI1710_HIGH_EDGE_CLEAR_COUNTER, APCI1710_LOW_EDGE_CLEAR_COUNTER, APCI1710_HIGH_EDGE_LATCH_AND_CLEAR_COUNTER, APCI1710_LOW_EDGE_LATCH_AND_CLEAR_COUNTER)
 * 
 * @param [in] b_AutoMode : Enable or disable the automatic index reset : (APCI1710_ENABLE, APCI1710_DISABLE)
 * 
 * @param [in] b_InterruptEnable : Enable or disable the interrupt : (APCI1710_ENABLE, APCI1710_DISABLE)
 * 
 * @retval 0: No error 
 * @retval 1: parameter pdev is NULL.
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see function "i_APCI1710_InitCounter"
 * @retval 5  The reference action parameter is wrong
 * @retval 6: The index operating mode parameter is wrong
 * @retval 7: The auto mode parameter is wrong
 * @retval 8: Interrupt parameter is wrong
 * @retval 9: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutineX"
 * 
 * @note This function applies to counter modules only
 */
int i_APCI1710_InitIndex (struct pci_dev * pdev, uint8_t b_ModulNbr, uint8_t b_ReferenceAction, uint8_t b_IndexOperation, uint8_t b_AutoMode, uint8_t b_InterruptEnable )
{
	if (!pdev)
		return 1;
	
	if ( NOT_A_MODULE(pdev, b_ModulNbr) )
		return 2;
	
	if ( NOT_A_COUNTER(pdev, b_ModulNbr) )
		return 3;
	
	if ( COUNTER_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 4;	

	/* check b_ReferenceAction */
	switch(b_ReferenceAction)
	{
		case APCI1710_ENABLE:
		case APCI1710_DISABLE:
			break;
		default:
			return 5;
	}
	
	/* check b_IndexOperation */
	switch(b_IndexOperation)
	{
		case APCI1710_HIGH_EDGE_LATCH_COUNTER:
		case APCI1710_LOW_EDGE_LATCH_COUNTER:
		case APCI1710_HIGH_EDGE_CLEAR_COUNTER:
		case APCI1710_LOW_EDGE_CLEAR_COUNTER:
		case APCI1710_HIGH_EDGE_LATCH_AND_CLEAR_COUNTER:
		case APCI1710_LOW_EDGE_LATCH_AND_CLEAR_COUNTER:
			break;
		default:
			return 6;
	}

	/* check b_AutoMode */
	switch(b_AutoMode)
	{
		case APCI1710_ENABLE:
		case APCI1710_DISABLE:
			break;
		default:
			return 7;
	}
	
	/* check b_InterruptEnable */
	switch(b_InterruptEnable)
	{
		case APCI1710_ENABLE:
		case APCI1710_DISABLE:
			break;
		default:
			return 8;		
	}

	/* if b_InterruptEnable is true then interrupt must be initialised */
	if ( (b_InterruptEnable == APCI1710_ENABLE) &&  INTERRUPT_FUNCTION_NOT_INITIALISED(pdev) )
		return 9;

	switch(b_ReferenceAction)
	{
		case APCI1710_ENABLE:
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 |= APCI1710_ENABLE_INDEX_ACTION;
			break;
		default:
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 &= APCI1710_DISABLE_INDEX_ACTION;
	}
	
	/* Test if low level latch or/and clear */
	switch(b_IndexOperation)
	{
		case APCI1710_LOW_EDGE_LATCH_COUNTER:
		case APCI1710_LOW_EDGE_CLEAR_COUNTER:
		case APCI1710_LOW_EDGE_LATCH_AND_CLEAR_COUNTER:
			/* Set the index level to low (DQ26) */
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 |= APCI1710_SET_LOW_INDEX_LEVEL;
			break;
		default:
			/* Set the index level to high (DQ26) */
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 &= APCI1710_SET_HIGH_INDEX_LEVEL;
	}
	
	/* Test if latch and clear counter */
	switch(b_IndexOperation)
	{
		case APCI1710_HIGH_EDGE_LATCH_AND_CLEAR_COUNTER:
		case APCI1710_LOW_EDGE_LATCH_AND_CLEAR_COUNTER:
			/* Set the latch and clear flag (DQ27) */
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 |= APCI1710_ENABLE_LATCH_AND_CLEAR;
			break;
		default:
			/* Clear the latch and clear flag (DQ27) */
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 &= APCI1710_DISABLE_LATCH_AND_CLEAR;
			
			/* Test if latch counter */
			switch(b_IndexOperation)
			{
				case APCI1710_HIGH_EDGE_LATCH_COUNTER:
				case APCI1710_LOW_EDGE_LATCH_COUNTER:
					/* Enable the latch from counter */
					APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 |= APCI1710_INDEX_LATCH_COUNTER;
					break;
		    	default:
		    		/* Enable the clear from counter */
					APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 &= (~APCI1710_INDEX_LATCH_COUNTER);		    		
		    		break;
			}
			break;
	}

	switch(b_AutoMode)
	{
		case APCI1710_DISABLE:
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 |= APCI1710_INDEX_AUTO_MODE;
			break;
		default:
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 &= (~APCI1710_INDEX_AUTO_MODE);
			break;
	}

	switch(b_InterruptEnable)
	{
		case APCI1710_ENABLE:
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister3 |= APCI1710_ENABLE_INDEX_INT;
			break;
		default:
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister3 &= APCI1710_DISABLE_INDEX_INT;
			break;
	}
	
	APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_IndexInit = 1;

	return 0;
}

//------------------------------------------------------------------------------
/* Enable the index corresponding to the selected module (b_ModulNbr).
 * 
 * @param [in] b_ModulNbr    : Module number (0 to 3).
 * 
 * @retval 0: No error 
 * @retval 1: parameter pdev is NULL.
 * @retval 2: parameter b_ModulNbr is not in the correct range 
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see command "CMD_APCI1710_InitCounter"
 * @retval 5: Index not initialised - see function "CMD_APCI1710_InitIndex"
 * 
 * @note This function applies to counter modules only
 */
int i_APCI1710_EnableIndex (struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	if (!pdev)
		return 1;
	
	if ( NOT_A_MODULE(pdev, b_ModulNbr) )
		return 2;
	
	if ( NOT_A_COUNTER(pdev, b_ModulNbr) )
		return 3;
	
	if ( COUNTER_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 4;

	if ( INDEX_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 5;

	APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 |= APCI1710_ENABLE_INDEX; 

	{
		uint32_t ul_InterruptLatchReg;
		/* why this access ? */
		INPDW ( GET_BAR2(pdev), 24 + MODULE_OFFSET(b_ModulNbr), &ul_InterruptLatchReg);
		OUTPDW (GET_BAR2(pdev), 20 + MODULE_OFFSET(b_ModulNbr),APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.dw_ModeRegister1_2_3_4);
	}
	return 0;
}


//------------------------------------------------------------------------------
/* Disable the index corresponding to the selected module (b_ModulNbr).
 * 
 * @param [in] b_ModulNbr    : Module number (0 to 3).
 * 
 * @retval 0: No error 
 * @retval 1: parameter pdev is NULL.
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see command "CMD_APCI1710_InitCounter"
 * @retval 5:  Index not initialised - see function "CMD_APCI1710_InitIndex"
 * 
 * @note This function applies to counter modules only 
 */
int i_APCI1710_DisableIndex (struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	if (!pdev)
		return 1;
	
	if ( NOT_A_MODULE(pdev, b_ModulNbr) )
		return 2;
	
	if ( NOT_A_COUNTER(pdev, b_ModulNbr) )
		return 3;
	
	if ( COUNTER_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 4;

	if ( INDEX_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 5;

	APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 &= APCI1710_DISABLE_INDEX; 

	OUTPDW ( GET_BAR2(pdev), 20 + MODULE_OFFSET(b_ModulNbr), APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.dw_ModeRegister1_2_3_4);
	
	return 0;
}


//------------------------------------------------------------------------------
/* Returns the INDEX status.
 * 
 * @param [in] b_ModulNbr : Module number (0 to 3).
 * @param [out] pb_IndexStatus : (0,1) 
 * 							0 : No INDEX occur
 * 							1 : An INDEX event occured
 * 
 * @retval 0: No error 
 * @retval 1: parameter pdev is NULL.
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module 
 * @retval 4: Counter not initialised see command "CMD_APCI1710_InitCounter"
 * @retval 5: Index not initialised - see function "CMD_APCI1710_InitIndex"
 * 
 * @note This function applies to counter modules only
*/
int i_APCI1710_GetIndexStatus (struct pci_dev * pdev, uint8_t b_ModulNbr, uint8_t * pb_IndexStatus)
{
	if (!pdev)
		return 1;
	
	if ( NOT_A_MODULE(pdev, b_ModulNbr) )
		return 2;
	
	if ( NOT_A_COUNTER(pdev, b_ModulNbr) )
		return 3;
	
	if ( COUNTER_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 4;

	if ( INDEX_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 5;
	
	{
		uint32_t StatusReg;
		INPDW  (GET_BAR2(pdev), 12 + MODULE_OFFSET(b_ModulNbr), &StatusReg);
		 *pb_IndexStatus = (uint8_t) (StatusReg & 0x1);	
	}
	
	return 0;
}


//------------------------------------------------------------------------------
/* Determine the hardware source for the index and the reference logic.
 * 
 * By default the index logic is connected to the difference input C and the reference
 * logic is connected to the 24V input E
 *  
 * @param [in] b_ModulNbr : Module number (0 to 3).
 * @param [in] b_SourceSelection : (APCI1710_SOURCE_0,APCI1710_SOURCE_1) 
 * 									APCI1710_SOURCE_0 : The index logic is connected to the difference input C 
 * 														and the reference logic is connected to the 24V input E.
 * 														This is the default configuration.
 * 									APCI1710_SOURCE_1 : The reference logic is connected to the difference
 * 														input C and the index logic is connected to the 24V input E
 * 
 * @retval 0: No error 
 * @retval 1: parameter pdev is NULL.
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: The parameter b_SourceSelection is incorrect
 * @retval 5: The board does not support this operation ( firmware revision < 1.5 )
 * 
 * @note This function applies to counter modules only
 * @note This function may be applied on unitialised counter/index
 */
int i_APCI1710_SetIndexAndReferenceSource (struct pci_dev * pdev, uint8_t b_ModulNbr, uint8_t b_SourceSelection )
{
	if (!pdev)
		return 1;
	
	if ( NOT_A_MODULE(pdev, b_ModulNbr) )
		return 2;
	
	if ( NOT_A_COUNTER(pdev, b_ModulNbr) )
		return 3;
	
	/* check firmware revision */
	if ( APCI1710_MODULE_VERSION(pdev,b_ModulNbr) < 0x3135 )
		return 5;
	
	switch(b_SourceSelection)
	{
		case APCI1710_SOURCE_0:
		    /* Set the default configuration (DQ25) */
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 &= APCI1710_DEFAULT_INDEX_RFERENCE;  
			break;		
		case APCI1710_SOURCE_1:
		    /* Invert index and reference source (DQ25) */
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister4 |= APCI1710_INVERT_INDEX_RFERENCE;  
			break;
		default: /* incorrect b_SourceSelection */
			return 4;
	}

	return 0;
}


//------------------------------------------------------------------------------
/* Initialise the reference corresponding to the selected module (b_ModulNbr).
 *  
 * @param [in] b_ModulNbr : Module number (0 to 3).
 * @param [in] b_ReferenceLevel : (APCI1710_LOW,APCI1710_HIGH)
 * 									APCI1710_LOW : Reference occur if "0"
 * 									APCI1710_HIGH : Reference occur if "1"
 * 
 * @retval 0: No error
 * @retval 1: parameter pdev is NULL.
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see command "CMD_APCI1710_InitCounter"
 * @retval 5: The parameter b_ReferenceLevel is incorrect 
 *
 * @note This function applies to counter modules only
 */
int i_APCI1710_InitReference (struct pci_dev * pdev, uint8_t b_ModulNbr, uint8_t b_ReferenceLevel )
{
	if (!pdev)
		return 1;
	
	if ( NOT_A_MODULE(pdev, b_ModulNbr) )
		return 2;
	
	if ( NOT_A_COUNTER(pdev, b_ModulNbr) )
		return 3;
	
	if ( COUNTER_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 4;
	
	switch(b_ReferenceLevel)
	{
		case APCI1710_LOW:
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 &= APCI1710_REFERENCE_LOW;
			break;
		case APCI1710_HIGH: 
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.s_ByteModeRegister.b_ModeRegister2 |= APCI1710_REFERENCE_HIGH;
			break;
		default: /* b_ReferenceLevel is incorrect  */
			return 5;
	}
	
	OUTPDW( GET_BAR2(pdev), 20 + MODULE_OFFSET(b_ModulNbr),
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[b_ModulNbr].s_SiemensCounterInfo.s_ModeRegister.dw_ModeRegister1_2_3_4);

	/* set flag to indicate reference was initialised */ 
	APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_ReferenceInit = 1;
			
	return 0;
}


//------------------------------------------------------------------------------
/* Return the reference status.
 * 
 * @param [in]  b_ModulNbr : Module number (0 to 3).
 * @param [out] pb_ReferenceStatus : (0,1)
 * 										0: No REFERENCE occurs
 * 										1: A REFERENCE occurs
 *
 * @retval 0: No error
 * @retval 1: parameter pdev is NULL.
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * @retval 4: Counter not initialised see function "CMD_APCI1710_InitCounter"
 * @retval 5: Reference not initialised see command "CMD_APCI1710_InitReference"
 * 
 * @note This function applies to counter modules only
 */
int i_APCI1710_GetReferenceStatus (struct pci_dev * pdev, uint8_t b_ModulNbr, uint8_t * pb_ReferenceStatus)
{
	if (!pdev)
		return 1;
	
	if ( NOT_A_MODULE(pdev, b_ModulNbr) )
		return 2;
	
	if ( NOT_A_COUNTER(pdev, b_ModulNbr) )
		return 3;
	
	if ( COUNTER_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 4;
	
	if ( REFERENCE_NOT_INITIALISED(pdev, b_ModulNbr) )
		return 5;
	
	switch(APCI1710_MODULE_VERSION(pdev,b_ModulNbr) )
	{
		case 0x3236:
			{
				/* if PLD Version >= 2.6 (0x3236), adresse + 36 is used instead of adresse + 24. 
	 			* interrupts are then not ack'ed. */
				uint32_t StatusReg;
				INPDW  ( GET_BAR2(pdev), 36 + MODULE_OFFSET(b_ModulNbr), &StatusReg);
				*pb_ReferenceStatus = (uint8_t) ((((~StatusReg) >> 2) & 1));
			}
			break;
		default:
			{
				uint32_t StatusReg;
				INPDW  ( GET_BAR2(pdev), 24 + MODULE_OFFSET(b_ModulNbr), &StatusReg);
				*pb_ReferenceStatus = (uint8_t) ((~StatusReg) & 1);
			}
			break;
	}

	return 0;
}
//------------------------------------------------------------------------------
/* Return the current initialisation status of an incremental counter module.
 * 
 * @param [in] ui_ModulNbr :  Module number (0 to 3).
 * @param [out] ui_ArgArray :  An array of unsigned int  
 * 
 * ui_ArgArray[0] [out] i_IndexInit : index function was initialised
 * ui_ArgArray[1] [out] i_CounterInit : counter function was initialised
 * ui_ArgArray[2] [out] i_ReferenceInit : reference function was initialised
 * ui_ArgArray[3] [out] i_CompareLogicInit : compare logic function was initialised
 * ui_ArgArray[4] [out] i_FrequencyMeasurementInit : Frequency Measurement function was initialised
 * ui_ArgArray[5] [out] i_FrequencyMeasurementEnable : Frequency Measurement function was enabled
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: parameter b_ModulNbr is not in the correct range
 * @retval 3: The given b_ModulNbr is not a counter module
 * 
 * @note This command applies to counter modules only
 */
int i_APCI1710_GetCounterInitialisationStatus(struct pci_dev * pdev, unsigned int ui_ModulNbr, unsigned int ui_ArgArray[6] )
{
	if (!pdev)
		return 1;
	
	if ( NOT_A_MODULE(pdev, ui_ModulNbr) )
		return 2;
	
	if ( NOT_A_COUNTER(pdev, ui_ModulNbr) )
		return 3;

	ui_ArgArray[0] = APCI1710_PRIVDATA(pdev)->s_ModuleInfo[ui_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_IndexInit;
	ui_ArgArray[1] = APCI1710_PRIVDATA(pdev)->s_ModuleInfo[ui_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_CounterInit;
	ui_ArgArray[2] = APCI1710_PRIVDATA(pdev)->s_ModuleInfo[ui_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_ReferenceInit;
	ui_ArgArray[3] = APCI1710_PRIVDATA(pdev)->s_ModuleInfo[ui_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_CompareLogicInit;
	ui_ArgArray[4] = APCI1710_PRIVDATA(pdev)->s_ModuleInfo[ui_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_FrequencyMeasurementInit;
	ui_ArgArray[5] = APCI1710_PRIVDATA(pdev)->s_ModuleInfo[ui_ModulNbr].s_SiemensCounterInfo.s_InitFlag.b_FrequencyMeasurementEnable;	 
	return 0;
}
