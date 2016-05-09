/** @file ssi-kapi.c
 
   Contains SSI kernel functions.
 
   @par CREATION  
   @author Krauth Julien
   @date   30.05.08
   
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

EXPORT_SYMBOL(i_APCI1710_InitSSI);
EXPORT_SYMBOL(i_APCI1710_InitSSIRawData);
EXPORT_SYMBOL(i_APCI1710_Read1SSIValue);
EXPORT_SYMBOL(i_APCI1710_Read1SSIRawDataValue);
EXPORT_SYMBOL(i_APCI1710_ReadAllSSIValue);
EXPORT_SYMBOL(i_APCI1710_ReadAllSSIRawDataValue);
EXPORT_SYMBOL(i_APCI1710_StartSSIAcquisition);
EXPORT_SYMBOL(i_APCI1710_GetSSIAcquisitionStatus);
EXPORT_SYMBOL(i_APCI1710_GetSSIValue);
EXPORT_SYMBOL(i_APCI1710_GetSSIRawDataValue);
EXPORT_SYMBOL(i_APCI1710_ReadSSI1DigitalInput);
EXPORT_SYMBOL(i_APCI1710_ReadSSIAllDigitalInput);
EXPORT_SYMBOL(i_APCI1710_SetSSIDigitalOutputOn);
EXPORT_SYMBOL(i_APCI1710_SetSSIDigitalOutputOff);


EXPORT_NO_SYMBOLS;


//------------------------------------------------------------------------------

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
int   i_APCI1710_InitSSI (struct pci_dev *pdev,
                          uint8_t b_ModulNbr,
                          uint8_t b_SSIProfile,
                          uint8_t b_PositionTurnLength,
                          uint8_t b_TurnCptLength,
                          uint8_t b_PCIInputClock,
                          uint32_t ul_SSIOutputClock,
                          uint8_t b_SSICountingMode)
	{
	int   i_ReturnValue = 0;
	unsigned int ui_TimerValue = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
	      {
	      /*******************************/
	      /* Test the SSI profile length */
	      /*******************************/

	      // CG 22/03/00 b_SSIProfile >= 2 anstatt b_SSIProfile > 2
	      if (b_SSIProfile >= 2 && b_SSIProfile < 33)
		 {
		 /*************************************/
		 /* Test the SSI position data length */
		 /*************************************/

		 if (b_PositionTurnLength > 0 && b_PositionTurnLength < 32)
		    {
		    /*****************************************/
		    /* Test the SSI turn counter data length */
		    /*****************************************/

		    // Begin 16.09.03 SW : 2243-0703 -> 2244-0903 : 0 for single turn

		    // if (b_TurnCptLength >= 0 && b_TurnCptLength < 32)
			if (b_TurnCptLength < 32)

		    // End 16.09.03 SW : 2243-0703 -> 2244-0903 : 0 for single turn
		       {
		       /***************************/
		       /* Test the profile length */
		       /***************************/

		       if ((b_TurnCptLength + b_PositionTurnLength) <= b_SSIProfile)
			  {
			  /****************************/
			  /* Test the PCI input clock */
			  /****************************/

			  if (b_PCIInputClock == APCI1710_30MHZ ||
			      b_PCIInputClock == APCI1710_33MHZ)
			     {
			     /*************************/
			     /* Test the output clock */
			     /*************************/

			     if ((b_PCIInputClock == APCI1710_30MHZ &&
				      (ul_SSIOutputClock > 228 && ul_SSIOutputClock <= 5000000UL)) ||
				      (b_PCIInputClock == APCI1710_33MHZ &&
				      (ul_SSIOutputClock > 251 && ul_SSIOutputClock <= 5000000UL)))
				{
				if (b_SSICountingMode == APCI1710_BINARY_MODE ||
				    b_SSICountingMode == APCI1710_GRAY_MODE)
				   {
				   /**********************/
				   /* Save configuration */
				   /**********************/

				   APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModulNbr].
				   s_SSICounterInfo.
				   b_SSIProfile = b_SSIProfile;

				   APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModulNbr].
				   s_SSICounterInfo.
				   b_PositionTurnLength = b_PositionTurnLength;

				   APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModulNbr].
				   s_SSICounterInfo.
				   b_TurnCptLength = b_TurnCptLength;

				   APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModulNbr].
				   s_SSICounterInfo.
				   b_SSICountingMode = b_SSICountingMode;

				   /*********************************/
				   /* Initialise the profile length */
				   /*********************************/

				   // Begin 16.09.03 SW : 2243-0703 -> 2244-0903 : Only binary for multi turn

				   if ((b_SSICountingMode == APCI1710_BINARY_MODE) && (b_TurnCptLength != 0))
				      {
				   // End 16.09.03 SW : 2243-0703 -> 2244-0903 : Only binary for multi turn
						OUTPDW (GET_BAR2(pdev),
								  4 + MODULE_OFFSET(b_ModulNbr),
								  b_SSIProfile + 1);
				      }
				   else
				      {
						OUTPDW (GET_BAR2(pdev),
								  4 + MODULE_OFFSET(b_ModulNbr),
								  b_SSIProfile);
				      }

				   /******************************/
				   /* Calculate the output clock */
				   /******************************/

				   ui_TimerValue = (uint16_t) (((uint32_t) (b_PCIInputClock) * 500000UL) / ul_SSIOutputClock);

				   /************************/
				   /* Initialise the timer */
				   /************************/

						OUTPDW (GET_BAR2(pdev),
								  MODULE_OFFSET(b_ModulNbr),
								  ui_TimerValue);

				   if ((b_SSICountingMode == APCI1710_BINARY_MODE) &&
				       (b_TurnCptLength   != 0))
				      {
				      /********************************/
				      /* Initialise the counting mode */
				      /********************************/

						OUTPDW (GET_BAR2(pdev),
								  12 + MODULE_OFFSET(b_ModulNbr),
								  7);
				      }
				   else
				      {
				      /********************************/
				      /* Initialise the counting mode */
				      /********************************/

						OUTPDW (GET_BAR2(pdev),
								  12 + MODULE_OFFSET(b_ModulNbr),
								  0);
				      }

				   APCI1710_PRIVDATA(pdev)->
				   s_ModuleInfo [(int)b_ModulNbr].
				   s_SSICounterInfo.
				   b_SSIInit = 1;
				   }
				else
				   {
				   /*****************************************************/
				   /* The selected SSI counting mode parameter is wrong */
				   /*****************************************************/

				   i_ReturnValue = 9;
				   }
				}
			     else
				{
				/******************************************/
				/* The selected SSI output clock is wrong */
				/******************************************/

				i_ReturnValue = 8;
				}
			     }
			  else
			     {
			     /*****************************************/
			     /* The selected PCI input clock is wrong */
			     /*****************************************/

			     i_ReturnValue = 7;
			     }
			  }
		       else
			  {
			  /********************************************/
			  /* The selected SSI profile length is wrong */
			  /********************************************/

			  i_ReturnValue = 4;
			  }
		       }
		    else
		       {
		       /******************************************************/
		       /* The selected SSI turn counter data length is wrong */
		       /******************************************************/

		       i_ReturnValue = 6;
		       }
		    }
		 else
		    {
		    /**************************************************/
		    /* The selected SSI position data length is wrong */
		    /**************************************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /********************************************/
		 /* The selected SSI profile length is wrong */
		 /********************************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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
int   i_APCI1710_InitSSIRawData (struct pci_dev *pdev,
                                 uint8_t b_ModulNbr,
                                 uint8_t b_SSIProfile,
                                 uint8_t b_PCIInputClock,
                                 uint32_t ul_SSIOutputClock)
	{
	int   i_ReturnValue = 0;
	unsigned int ui_TimerValue = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
			// Test if SSI counter version is greater than 1.0 (ASCII 0x3130) to support profile length greater than 32 bits
			if (((APCI1710_PRIVDATA(pdev)->
				s_BoardInfos.
				dw_MolduleConfiguration [b_ModulNbr] & 0x0000FFFFUL) > 0x00003130UL) || (b_SSIProfile < 33))
				{
				// Test the SSI profile length 
				if (b_SSIProfile >= 2 && b_SSIProfile < 49)
					{
					// Test the PCI input clock 
					if (b_PCIInputClock == APCI1710_30MHZ ||
						b_PCIInputClock == APCI1710_33MHZ)
						{
						// Test the output clock 
						if ((b_PCIInputClock == APCI1710_30MHZ &&
							(ul_SSIOutputClock > 228 && ul_SSIOutputClock <= 5000000UL)) ||
							(b_PCIInputClock == APCI1710_33MHZ &&
							(ul_SSIOutputClock > 251 && ul_SSIOutputClock <= 5000000UL)))
							{
							// Save configuration 
							APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_SSICounterInfo.
							b_SSIProfile = b_SSIProfile;

							APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_SSICounterInfo.
							b_PositionTurnLength = b_SSIProfile;

							APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_SSICounterInfo.
							b_TurnCptLength = 0;

							APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_SSICounterInfo.
							b_SSICountingMode = APCI1710_GRAY_MODE;

							OUTPDW (GET_BAR2(pdev),
								  4 + MODULE_OFFSET(b_ModulNbr),
								  b_SSIProfile);

							// Calculate the output clock 
							ui_TimerValue = (uint16_t) (((uint32_t) (b_PCIInputClock) * 500000UL) / ul_SSIOutputClock);

							// Initialise the timer 
							OUTPDW (GET_BAR2(pdev),
								  MODULE_OFFSET(b_ModulNbr),
								  ui_TimerValue);

							// Initialise the counting mode 
							OUTPDW (GET_BAR2(pdev),
								  12 + MODULE_OFFSET(b_ModulNbr),
								  0);

							APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_SSICounterInfo.
							b_SSIInit = 1;
							}
						else
							{
							// The selected SSI output clock is wrong 
							i_ReturnValue = 7;
							}
						}
					else
						{
						// The selected PCI input clock is wrong 
						i_ReturnValue = 6;
						}
					}
				else
					{
					// The selected SSI profile length is wrong 
					i_ReturnValue = 5;
					}
				}
			else
				{
				// The module does not support this profile length 
				i_ReturnValue = 4;
				}
			}
		else
			{
			// The module is not a SSI module 
			i_ReturnValue = 3;
			}
		}
	else
		{
		// Module number error 
		i_ReturnValue = 2;
		}

	return (i_ReturnValue);
	}
	
//------------------------------------------------------------------------------	
	
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
int i_APCI1710_Read1SSIValue (struct pci_dev *pdev,
										uint8_t b_ModulNbr,
										uint8_t b_SelectedSSI,
										uint32_t *pul_Position,
										uint32_t *pul_TurnCpt)
	{
	int i_ReturnValue = 0;
	unsigned char b_Cpt = 0;
	unsigned char b_Length = 0;
	unsigned char b_Schift = 0;
	unsigned char b_GrayCpt = 0;
	int i_Sign = 0;
	uint32_t dw_And = 0;
	uint32_t dw_StatusReg = 0;
	uint32_t dw_CounterValue = 0;
	long  l_Binary = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
	      /***************************/
	      /* Test if SSI initialised */
	      /***************************/

	      if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SSICounterInfo.
				b_SSIInit == 1)

		 {
		 /****************************************/
		 /* Test the selected SSI counter number */
		 /****************************************/

		 if (b_SelectedSSI < 3)
		    {
			// Test if more than 32 bits profile length initialisied
			 if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
					s_SSICounterInfo.
					b_SSIProfile < 33)
				{

		    /************************/
		    /* Start the conversion */
		    /************************/

			 OUTPDW (GET_BAR2(pdev),
					8 + MODULE_OFFSET(b_ModulNbr),
					0);

		    do
		       {
		       /*******************/
		       /* Read the status */
		       /*******************/
				 INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);
		       }
		    while ((dw_StatusReg & 0x1) != 0);

		    /******************************/
		    /* Read the SSI counter value */
		    /******************************/

			 INPDW (GET_BAR2(pdev), 4 + (b_SelectedSSI * 4) + MODULE_OFFSET(b_ModulNbr), &dw_CounterValue);

		    // Begin 16.09.03 SW : 2243-0703 -> 2244-0903 : Only for multi turn

		    if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
			s_SSICounterInfo.
			b_TurnCptLength != 0)
		       {
		       if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
			   s_SSICounterInfo.
			   b_SSIProfile != (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
					       s_SSICounterInfo.
					       b_PositionTurnLength + 
					       APCI1710_PRIVDATA(pdev)->
				          s_ModuleInfo [(int)b_ModulNbr].
					       s_SSICounterInfo.
					       b_TurnCptLength))
		          {
		          b_Length = APCI1710_PRIVDATA(pdev)->
				     s_ModuleInfo [(int)b_ModulNbr].
				     s_SSICounterInfo.
				     b_SSIProfile / 2;

		          if ((b_Length * 2) != APCI1710_PRIVDATA(pdev)->
				                          s_ModuleInfo [(int)b_ModulNbr].
					                       s_SSICounterInfo.
					                       b_SSIProfile)
			     {
			     b_Length ++;
			     }

		          b_Schift = b_Length - APCI1710_PRIVDATA(pdev)->
				                          s_ModuleInfo [(int)b_ModulNbr].
					                       s_SSICounterInfo.
					                       b_PositionTurnLength;

			  }
		       else
		          {
			  b_Length = APCI1710_PRIVDATA(pdev)->
				          s_ModuleInfo [(int)b_ModulNbr].
				          s_SSICounterInfo.
				          b_PositionTurnLength;

			  b_Schift = 0;
			  }


	               *pul_Position = dw_CounterValue >> b_Schift;

		       dw_And = 1;

		       for (b_Cpt = 0; b_Cpt < APCI1710_PRIVDATA(pdev)->
				                         s_ModuleInfo [(int)b_ModulNbr].
					                      s_SSICounterInfo.
					                      b_PositionTurnLength; b_Cpt ++)
			  {
			  dw_And = dw_And * 2;
			  }

		       *pul_Position = *pul_Position & ((dw_And) - 1);

		       *pul_TurnCpt = dw_CounterValue >> b_Length;

		       dw_And = 1;

		       for (b_Cpt = 0; b_Cpt < APCI1710_PRIVDATA(pdev)->
				                         s_ModuleInfo [(int)b_ModulNbr].
					                      s_SSICounterInfo.
					                      b_TurnCptLength; b_Cpt ++)
			  {
			  dw_And = dw_And * 2;
			  }

		       *pul_TurnCpt = *pul_TurnCpt & ((dw_And) - 1);
		       }
		    else
		       {
		       *pul_TurnCpt = 0;

		       dw_And = 1;
		       for (b_Cpt = 0; b_Cpt < APCI1710_PRIVDATA(pdev)->
				                         s_ModuleInfo [(int)b_ModulNbr].
					                      s_SSICounterInfo.
					                      b_PositionTurnLength; b_Cpt ++)
			  {
			  dw_And = dw_And * 2;
			  }

		       *pul_Position = dw_CounterValue & ((dw_And) - 1);

		       /***********************/
		       /* Test if binary mode */
		       /***********************/

		       if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
			   s_SSICounterInfo.
			   b_SSICountingMode == APCI1710_BINARY_MODE)
			  {
			  /************************************/
			  /* Convert the Gray value to Binary */
			  /************************************/

			  l_Binary = 0;
			  i_Sign   = 1;

			  for (b_GrayCpt = 32; b_GrayCpt > 0; b_GrayCpt --)
			     {
			     dw_And = 1;
			     for (b_Cpt = 0; b_Cpt < b_GrayCpt; b_Cpt ++)
				{
				dw_And = dw_And * 2;
				}

			     if (((*pul_Position >> (b_GrayCpt - 1)) & 1) == 1)
				{
				l_Binary = l_Binary + ((dw_And - 1) * i_Sign);
				if (i_Sign == 1)
				   {
				   i_Sign = -1;
				   }
				else
				   {
				   i_Sign = 1;
				   }
				}
			     }

			  *pul_Position = (uint32_t) l_Binary;

			  }
		       }
		    // End 16.09.03 SW : 2243-0703 -> 2244-0903 : Only for multi turn
				}
			 else
				{
				// This function does not support more than 32 bits profile length
				i_ReturnValue = 6;
				}
		    }
		 else
		    {
		    /*****************************/
		    /* The selected SSI is wrong */
		    /*****************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /***********************/
		 /* SSI not initialised */
		 /***********************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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
int i_APCI1710_Read1SSIRawDataValue (struct pci_dev *pdev,
                                     uint8_t b_ModulNbr,
                                     uint8_t b_SelectedSSI,
                                     uint32_t * pul_ValueArray,
                                     uint8_t b_ValueArraySize)
	{
	int i_ReturnValue = 0;
	uint32_t dw_StatusReg = 0;
	uint32_t dw_CounterValue = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
			// Test if SSI initialised 
			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SSICounterInfo.
				b_SSIInit == 1)
				{
				// Test the selected SSI counter number 
				if (b_SelectedSSI < 3)
					{

					// Start the conversion 
			 		OUTPDW (GET_BAR2(pdev),
						8 + MODULE_OFFSET(b_ModulNbr),
						0);

					do
						{
						// Read the status 
						INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);
						}
					while ((dw_StatusReg & 0x1) != 0);

					if (b_ValueArraySize >= 1)
						{
						// Read the SSI counter value 
						INPDW (GET_BAR2(pdev), 4 + (b_SelectedSSI * 4) + MODULE_OFFSET(b_ModulNbr), &dw_CounterValue);

						if (APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_SSICounterInfo.
							b_SSIProfile >= 32)
							{
							pul_ValueArray[0] = dw_CounterValue;
							}
						else
							{
							pul_ValueArray[0] = dw_CounterValue & (~(0x1 << APCI1710_PRIVDATA(pdev)->
																							s_ModuleInfo [(int)b_ModulNbr].
																							s_SSICounterInfo.
																							b_SSIProfile));
							}

						// Test if SSI counter version is greater than 1.0 (ASCII 0x3130) to support profile length greater than 32 bits
						if ((b_ValueArraySize >= 2) && ((APCI1710_PRIVDATA(pdev)->
														s_BoardInfos.
														dw_MolduleConfiguration [b_ModulNbr] & 0x0000FFFFUL) > 0x00003130UL) 
													&& (APCI1710_PRIVDATA(pdev)->
														s_ModuleInfo [(int)b_ModulNbr].
														s_SSICounterInfo.
														b_SSIProfile > 32))
							{
							// Read the SSI counter value 
							INPDW (GET_BAR2(pdev), 16 + (b_SelectedSSI * 4) + MODULE_OFFSET(b_ModulNbr), &dw_CounterValue);

							pul_ValueArray[1] = dw_CounterValue & (~(0x1 << (APCI1710_PRIVDATA(pdev)->
																							s_ModuleInfo [(int)b_ModulNbr].
																							s_SSICounterInfo.
																							b_SSIProfile - 32)));
							}
						}
					else
						{
						// The b_ValueArraySize parameter is wrong
						i_ReturnValue = 6;
						}

					}
				else
					{
					// The selected SSI is wrong 
					i_ReturnValue = 5;
					}
				}
			else
				{
				// SSI not initialised 
				i_ReturnValue = 4;
				}
			}
		else
			{
			// The module is not a SSI module 
			i_ReturnValue = 3;
			}
		}
	else
		{
		// Module number error 
		i_ReturnValue = 2;
		}

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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
int i_APCI1710_ReadAllSSIValue (struct pci_dev *pdev,
                                uint8_t b_ModulNbr,
                                uint32_t * pul_Position,
                                uint32_t *pul_TurnCpt)
	{
	int i_ReturnValue = 0;
	unsigned char b_Cpt = 0;
	unsigned char b_Length = 0;
	unsigned char b_Schift = 0;
	unsigned char b_SSICpt = 0;
	unsigned char b_GrayCpt = 0;
	int i_Sign = 0;
	long l_Binary = 0;
	uint32_t dw_And = 0;
	uint32_t dw_And1 = 0;
	uint32_t dw_And2 = 0;
	uint32_t dw_StatusReg = 0;
	uint32_t dw_CounterValue = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
			// Test if SSI initialised 
			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SSICounterInfo.
				b_SSIInit == 1)
				{
			 // Test if more than 32 bits profile length initialisied
			 if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
					s_SSICounterInfo.
					b_SSIProfile < 33)
				{

		 dw_And1 = 1;

		 for (b_Cpt = 0; b_Cpt < APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
					                s_SSICounterInfo.
					                b_PositionTurnLength; b_Cpt ++)
		    {
		    dw_And1 = dw_And1 * 2;
		    }

		 dw_And2 = 1;

		 for (b_Cpt = 0; b_Cpt < APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
					                s_SSICounterInfo.
					                b_TurnCptLength; b_Cpt ++)
		    {
		    dw_And2 = dw_And2 * 2;
		    }

		 /************************/
		 /* Start the conversion */
		 /************************/

		OUTPDW (GET_BAR2(pdev),
			8 + MODULE_OFFSET(b_ModulNbr),
			0);

		 do
		    {
		    /*******************/
		    /* Read the status */
		    /*******************/
			 INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);
		    }
		 while ((dw_StatusReg & 0x1) != 0);

		 for (b_SSICpt = 0; b_SSICpt < 3; b_SSICpt ++)
		    {
		    /******************************/
		    /* Read the SSI counter value */
		    /******************************/

			 INPDW (GET_BAR2(pdev), 4 + (b_SSICpt * 4) + MODULE_OFFSET(b_ModulNbr), &dw_CounterValue);

		    if (APCI1710_PRIVDATA(pdev)->
				  s_ModuleInfo [(int)b_ModulNbr].
				  s_SSICounterInfo.
				  b_TurnCptLength != 0)
		       {
		       if (APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
			   s_SSICounterInfo.
			   b_SSIProfile != (APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
					       s_SSICounterInfo.
					       b_PositionTurnLength + 
					       APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
					       s_SSICounterInfo.
					       b_TurnCptLength))
		          {
		          b_Length = APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
				     s_SSICounterInfo.
				     b_SSIProfile / 2;

		          if ((b_Length * 2) != APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
					        s_SSICounterInfo.
					        b_SSIProfile)
			     {
			     b_Length ++;
			     }

		          b_Schift = b_Length - APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
					        s_SSICounterInfo.
					        b_PositionTurnLength;

			  }
		       else
		          {
			  b_Length = APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
				     s_SSICounterInfo.
				     b_PositionTurnLength;

			  b_Schift = 0;
			  }

		       pul_Position [b_SSICpt] = dw_CounterValue >> b_Schift;
		       pul_Position [b_SSICpt] = pul_Position [b_SSICpt] & ((dw_And1) - 1);

		       pul_TurnCpt [b_SSICpt] = dw_CounterValue >> b_Length;
		       pul_TurnCpt [b_SSICpt] = pul_TurnCpt [b_SSICpt] & ((dw_And2) - 1);
		       }
		    else
		       {
		       // Begin 16.09.03 SW : 2243-0703 -> 2244-0903 : Only for multi turn
		       pul_TurnCpt [b_SSICpt] = 0;

		       dw_And = 1;
		       for (b_Cpt = 0; b_Cpt < APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
					       s_SSICounterInfo.
					       b_PositionTurnLength; b_Cpt ++)
			  {
			  dw_And = dw_And * 2;
			  }

		       pul_Position [b_SSICpt] = dw_CounterValue & ((dw_And) - 1);

		       /***********************/
		       /* Test if binary mode */
		       /***********************/

		       if (APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
			   s_SSICounterInfo.
			   b_SSICountingMode == APCI1710_BINARY_MODE)
			  {
			  /************************************/
			  /* Convert the Gray value to Binary */
			  /************************************/

			  l_Binary = 0;
			  i_Sign   = 1;

			  for (b_GrayCpt = 32; b_GrayCpt > 0; b_GrayCpt --)
			     {
			     dw_And = 1;
			     for (b_Cpt = 0; b_Cpt < b_GrayCpt; b_Cpt ++)
				{
				dw_And = dw_And * 2;
				}

			     if (((pul_Position [b_SSICpt] >> (b_GrayCpt - 1)) & 1) == 1)
				{
				l_Binary = l_Binary + ((dw_And - 1) * i_Sign);
				if (i_Sign == 1)
				   {
				   i_Sign = -1;
				   }
				else
				   {
				   i_Sign = 1;
				   }
				}
			     }
			  pul_Position [b_SSICpt] = (uint32_t) l_Binary;
			  }
		       // Begin 16.09.03 SW : 2243-0703 -> 2244-0903 : Only for multi turn
		       }
		    }
				}
			 else
				{
				// This function does not support more than 32 bits profile length
				i_ReturnValue = 5;
				}
		 }
	      else
		 {
		 /***********************/
		 /* SSI not initialised */
		 /***********************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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
int i_APCI1710_ReadAllSSIRawDataValue (struct pci_dev *pdev,
                                       uint8_t b_ModulNbr,
                                       uint32_t * pul_ValueArray,
                                       uint8_t b_ValueArraySize)
	{
	int i_ReturnValue = 0;
	uint32_t dw_StatusReg = 0;
	uint32_t dw_CounterValue[6];
	unsigned char b_SSICpt = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
			// Test if SSI initialised 
			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SSICounterInfo.
				b_SSIInit == 1)
				{
			// Start the conversion 
				OUTPDW (GET_BAR2(pdev),
					8 + MODULE_OFFSET(b_ModulNbr),
					0);

		 do
		    {
		    // Read the status 
		    INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);
		    }
		 while ((dw_StatusReg & 0x1) != 0);

		 for (b_SSICpt = 0; b_SSICpt < 3; b_SSICpt ++)
		    {
			if (b_ValueArraySize >= 3)
			   {
			   // Read the SSI counter value 
				INPDW (GET_BAR2(pdev), 4 + (b_SSICpt * 4) + MODULE_OFFSET(b_ModulNbr), &dw_CounterValue[b_SSICpt]);

				// Test if SSI counter version is greater than 1.0 (ASCII 0x3130) to support profile length greater than 32 bits
				if ((b_ValueArraySize >= 6) && ((APCI1710_PRIVDATA(pdev)->
												  s_BoardInfos.
												 dw_MolduleConfiguration [b_ModulNbr] & 0x0000FFFFUL) > 0x00003130UL) 
											&& (APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
												s_SSICounterInfo.
												b_SSIProfile > 32))
					{
					// Read the SSI counter value 
					INPDW (GET_BAR2(pdev), 16 + (b_SSICpt * 4) + MODULE_OFFSET(b_ModulNbr), &dw_CounterValue[b_SSICpt + 3]);
					}
				}
			else
				{
				break;
				}
		    }
			// Test if SSI counter version is greater than 1.0 (ASCII 0x3130) to support profile length greater than 32 bits
			if ((b_ValueArraySize >= 6) && ((APCI1710_PRIVDATA(pdev)->
											  s_BoardInfos.
											 dw_MolduleConfiguration [b_ModulNbr] & 0x0000FFFFUL) > 0x00003130UL) 
										&& (APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
											s_SSICounterInfo.
											b_SSIProfile > 32))
				{
			    pul_ValueArray[0] = dw_CounterValue[0];
				pul_ValueArray[1] = dw_CounterValue[3] & (~(0x1 << (APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
																	s_SSICounterInfo.
																	b_SSIProfile - 32)));
				
			    pul_ValueArray[2] = dw_CounterValue[1];
				pul_ValueArray[3] = dw_CounterValue[4] & (~(0x1 << (APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
																	s_SSICounterInfo.
																	b_SSIProfile - 32)));
				pul_ValueArray[4] = dw_CounterValue[2];
				pul_ValueArray[5] = dw_CounterValue[5] & (~(0x1 << (APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
																	s_SSICounterInfo.
																	b_SSIProfile - 32)));
				}
			else
				{
				if (b_ValueArraySize >= 3)
					{
					if (APCI1710_PRIVDATA(pdev)->
						s_ModuleInfo [(int)b_ModulNbr].
						s_SSICounterInfo.
						b_SSIProfile == 32)
						{
						pul_ValueArray[0] = dw_CounterValue[0];
						pul_ValueArray[1] = dw_CounterValue[1];
						pul_ValueArray[2] = dw_CounterValue[2];
						}
					else
						{
						if (APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_SSICounterInfo.
							b_SSIProfile < 32)
							{
							pul_ValueArray[0] = dw_CounterValue[0] & (~(0x1 << APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
																				s_SSICounterInfo.
																				b_SSIProfile));
							pul_ValueArray[1] = dw_CounterValue[1] & (~(0x1 << APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
																				s_SSICounterInfo.
																				b_SSIProfile));
							pul_ValueArray[2] = dw_CounterValue[2] & (~(0x1 << APCI1710_PRIVDATA(pdev)->
				                   s_ModuleInfo [(int)b_ModulNbr].
																				s_SSICounterInfo.
																				b_SSIProfile));
							}
						}
					}
				else
					{
					// The b_ValueArraySize parameter is wrong  
					i_ReturnValue = 5;
					}
				}
		 }
	      else
		 {
		 // SSI not initialised 
		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      // The module is not a SSI module 
	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   // Module number error 
	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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

int i_APCI1710_StartSSIAcquisition (struct pci_dev *pdev,
                                    uint8_t b_ModulNbr)
	{
	int i_ReturnValue = 0;
	uint32_t dw_StatusReg;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
			// Test if SSI initialised 
			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SSICounterInfo.
				b_SSIInit == 1)
				{
				INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);


		 /**********************************/
		 /* Test if conversion not started */
		 /**********************************/
		 
		 if ((dw_StatusReg & 0x1) == 0)
		    {
		    /************************/
		    /* Start the conversion */
		    /************************/

			OUTPDW (GET_BAR2(pdev),
						8 + MODULE_OFFSET(b_ModulNbr),
						0);
		    }
		 else
		    {
		    /***************************/
		    /* Acquisition in progress */
		    /***************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /***********************/
		 /* SSI not initialised */
		 /***********************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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

int i_APCI1710_GetSSIAcquisitionStatus (struct pci_dev *pdev,
                                        uint8_t b_ModulNbr,
                                        uint8_t * pb_Status)
{
	int i_ReturnValue = 0;
	uint32_t dw_StatusReg = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
			// Test if SSI initialised 
			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SSICounterInfo.
				b_SSIInit == 1)
				{
				INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);

		 /**********************/
		 /* Get the SSI status */
		 /**********************/
		 
		 *pb_Status = (unsigned char) (dw_StatusReg & 0x1);
		 }
	      else
		 {
		 /***********************/
		 /* SSI not initialised */
		 /***********************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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

int i_APCI1710_GetSSIValue (struct pci_dev *pdev,
                            uint8_t b_ModulNbr,
                            uint8_t b_SelectedSSI,
                            uint32_t * pul_Position,
                            uint32_t * pul_TurnCpt)
	{
	int i_ReturnValue = 0;
	unsigned char b_Cpt = 0;
	unsigned char b_Length = 0;
	unsigned char b_Schift = 0;
	unsigned char b_GrayCpt = 0;
	int i_Sign = 0;
	uint32_t dw_And = 0;
	uint32_t dw_StatusReg = 0;
	uint32_t dw_CounterValue = 0;
	long l_Binary = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
			// Test if SSI initialised 
			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SSICounterInfo.
				b_SSIInit == 1)
				{
		 /****************************************/
		 /* Test the selected SSI counter number */
		 /****************************************/

		 if (b_SelectedSSI < 3)
		    {
		    /*******************/
		    /* Read the status */
		    /*******************/

		    INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);

		    if ((dw_StatusReg & 1) == 0)
		       {
			 // Test if more than 32 bits profile length initialisied
			 if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
					s_SSICounterInfo.
					b_SSIProfile < 33)
				{
		       /******************************/
		       /* Read the SSI counter value */
		       /******************************/
				 INPDW (GET_BAR2(pdev), 4 + (b_SelectedSSI * 4) + MODULE_OFFSET(b_ModulNbr), &dw_CounterValue);

		       if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
			   s_SSICounterInfo.
			   b_TurnCptLength != 0)
		          {
		          b_Length = APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				     s_SSICounterInfo.
				     b_SSIProfile / 2;

		          if ((b_Length * 2) != APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
					        s_SSICounterInfo.
					        b_SSIProfile)
			     {
			     b_Length ++;
			     }

		          b_Schift = b_Length - APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
					        s_SSICounterInfo.
					        b_PositionTurnLength;


		          *pul_Position = dw_CounterValue >> b_Schift;

		          dw_And = 1;

		          for (b_Cpt = 0; b_Cpt < APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
					          s_SSICounterInfo.
					          b_PositionTurnLength; b_Cpt ++)
			     {
			     dw_And = dw_And * 2;
			     }

		          *pul_Position = *pul_Position & ((dw_And) - 1);

		          *pul_TurnCpt = dw_CounterValue >> b_Length;

		          dw_And = 1;

		          for (b_Cpt = 0; b_Cpt < APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
					          s_SSICounterInfo.
					          b_TurnCptLength; b_Cpt ++)
			     {
			     dw_And = dw_And * 2;
			     }

		          *pul_TurnCpt = *pul_TurnCpt & ((dw_And) - 1);
		          }
		       else
		          {
		          *pul_TurnCpt = 0;

		          dw_And = 1;
		          for (b_Cpt = 0; b_Cpt < APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
					          s_SSICounterInfo.
					          b_PositionTurnLength; b_Cpt ++)
			     {
			     dw_And = dw_And * 2;
			     }

		          *pul_Position = dw_CounterValue & ((dw_And) - 1);

		          /***********************/
		          /* Test if binary mode */
		          /***********************/

		          if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
			      s_SSICounterInfo.
			      b_SSICountingMode == APCI1710_BINARY_MODE)
			     {
			     /************************************/
			     /* Convert the Gray value to Binary */
			     /************************************/

			     l_Binary = 0;
			     i_Sign   = 1;

			     for (b_GrayCpt = 32; b_GrayCpt > 0; b_GrayCpt --)
			        {
			        dw_And = 1;
			        for (b_Cpt = 0; b_Cpt < b_GrayCpt; b_Cpt ++)
				   {
				   dw_And = dw_And * 2;
				   }

			        if (((*pul_Position >> (b_GrayCpt - 1)) & 1) == 1)
				   {
				   l_Binary = l_Binary + ((dw_And - 1) * i_Sign);
				   if (i_Sign == 1)
				      {
				      i_Sign = -1;
				      }
				   else
				      {
				      i_Sign = 1;
				      }
				   }
			        }
			     *pul_Position = (uint32_t) l_Binary;
			     }
		          }
				}
			 else
				{
				// This function does not support more than 32 bits profile length
				i_ReturnValue = 7;
				}
		       }
		    else
		       {
		       /***************************/
		       /* Acquisition in progress */
		       /***************************/

		       i_ReturnValue = 6;
  		       }
		    }
		 else
		    {
		    /*****************************/
		    /* The selected SSI is wrong */
		    /*****************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /***********************/
		 /* SSI not initialised */
		 /***********************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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

int i_APCI1710_GetSSIRawDataValue (struct pci_dev *pdev,
                                   uint8_t b_ModulNbr,
                                   uint8_t b_SelectedSSI,
                                   uint32_t * pul_ValueArray,
                                   uint8_t b_ValueArraySize)
	{
	int i_ReturnValue = 0;
	uint32_t dw_StatusReg = 0;
	uint32_t dw_CounterValue = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
			// Test if SSI initialised 
			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_SSICounterInfo.
				b_SSIInit == 1)
				{
				// Test the selected SSI counter number 
				if (b_SelectedSSI < 3)
					{
					// Read the status 
					INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);

				    if ((dw_StatusReg & 1) == 0)
						{
						if (b_ValueArraySize >= 1)
							{
							// Read the SSI counter value 
							INPDW (GET_BAR2(pdev), 4 + (b_SelectedSSI * 4) + MODULE_OFFSET(b_ModulNbr), &dw_CounterValue);

							pul_ValueArray[0] = dw_CounterValue;

							// Test if SSI counter version is greater than 1.0 (ASCII 0x3130) to support profile length greater than 32 bits
							if ((b_ValueArraySize >= 2) && ((APCI1710_PRIVDATA(pdev)->
															s_BoardInfos.
															dw_MolduleConfiguration [b_ModulNbr] & 0x0000FFFFUL) > 0x00003130UL))
								{
								// Read the SSI counter value 
								INPDW (GET_BAR2(pdev), 16 + (b_SelectedSSI * 4) + MODULE_OFFSET(b_ModulNbr), &dw_CounterValue);
	
								pul_ValueArray[1] = dw_CounterValue;
								}
							}
						else
							{
							// The b_ValueArraySize parameter is wrong             
							i_ReturnValue = 7;
  							}
						}
					else
						{
						// Acquisition in progress 
						i_ReturnValue = 6;
  						}
					}
				else
					{
					// The selected SSI is wrong 
					i_ReturnValue = 5;
					}
				}
			else
				{
				// SSI not initialised 
				i_ReturnValue = 4;
				}
			}
		else
			{
			// The module is not a SSI module 
			i_ReturnValue = 3;
			}
		}
	else
		{
		// Module number error 
		i_ReturnValue = 2;
		}

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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

int i_APCI1710_ReadSSI1DigitalInput  (struct pci_dev *pdev,
                                      uint8_t b_ModulNbr,
                                      uint8_t b_InputChannel,
                                      uint8_t * pb_ChannelStatus)
	{
	int i_ReturnValue = 0;
	uint32_t dw_StatusReg = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
	      /******************************************/
	      /* Test the digital imnput channel number */
	      /******************************************/

	      if (b_InputChannel <= 2)
		 {
		 /**************************/
		 /* Read all digital input */
		 /**************************/

		 INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);

		 *pb_ChannelStatus = (unsigned char) (((~dw_StatusReg) >> (4 + b_InputChannel)) & 1);
		 }
	      else
		 {
		 /********************************/
		 /* Selected digital input error */
		 /********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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

int i_APCI1710_ReadSSIAllDigitalInput (struct pci_dev *pdev,
                                      uint8_t b_ModulNbr,
                                      uint8_t * pb_InputStatus)
	{
	int i_ReturnValue = 0;
	uint32_t dw_StatusReg = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
	      /**************************/
	      /* Read all digital input */
	      /**************************/

			INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr), &dw_StatusReg);

	      *pb_InputStatus = (unsigned char) (((~dw_StatusReg) >> 4) & 7);
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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

int i_APCI1710_SetSSIDigitalOutputOn  (struct pci_dev *pdev,
                                      uint8_t b_ModulNbr)
	{
	int i_ReturnValue = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
	      /*****************************/
	      /* Set the digital output ON */
	      /*****************************/

			OUTPDW (GET_BAR2(pdev),
						16 + MODULE_OFFSET(b_ModulNbr),
						1);
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

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

int i_APCI1710_SetSSIDigitalOutputOff  (struct pci_dev *pdev,
                                      uint8_t b_ModulNbr)
	{
	int i_ReturnValue = 0;

	if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /***********************/
	   /* Test if SSI counter */
	   /***********************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_SSI_COUNTER)
			{
	      /*****************************/
	      /* Set the digital output ON */
	      /*****************************/

			OUTPDW (GET_BAR2(pdev),
						16 + MODULE_OFFSET(b_ModulNbr),
						0);
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------


