/** @file imp_cpt-kapi.c
 
   Impuls counter kernel functions.
 
   @par CREATION  
   @author S. Weber
   @date   02.10.08
   
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

EXPORT_SYMBOL(i_APCI1710_InitPulseEncoder);
EXPORT_SYMBOL(i_APCI1710_EnablePulseEncoder);
EXPORT_SYMBOL(i_APCI1710_DisablePulseEncoder);
EXPORT_SYMBOL(i_APCI1710_ReadPulseEncoderStatus);
EXPORT_SYMBOL(i_APCI1710_ReadPulseEncoderValue);
EXPORT_SYMBOL(i_APCI1710_WritePulseEncoderValue);

EXPORT_NO_SYMBOLS;






/* returns 1 if the counter isn't initialised */
static __inline__ int IMP_COUNTER_NOT_INITIALISED(struct pci_dev * pdev, uint8_t b_ModulNbr, uint8_t b_PulseEncoderNbr)
{
	return ( APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].s_PulseEncoderModuleInfo.s_PulseEncoderInfo [(int)b_PulseEncoderNbr].b_PulseEncoderInit != 1 );
}

//------------------------------------------------------------------------------
       	
/** Initialize the impuls counter.
 * 
 * Configure the pulse encoder operating mode selected via b_ModulNbr and b_PulseEncoderNbr. 
 * The pulse encoder after each pulse decrement the counter value from 1.
 * You must calling this function be for you call any
 * other function witch access of pulse encoders.
 *  
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_PulseEncoderNbr     : Pulse encoder selection (0 to 3)
 * @param [in] b_InputLevelSelection : Input level selection (0 or 1) <br>
 *                                        0 : Set pulse encoder count the the low level pulse. <br>
 *                                        1 : Set pulse encoder count the the high level pulse.
 * @param [in] b_TriggerOutputAction : Digital TRIGGER output action <br>
 *                                        0 : No action <br>
 *                                        1 : Set the trigger output to "1" <br>
 *                                            (high) after the passage from 1 to 0 <br>
 *                                            from pulse encoder. <br>
 *                                        2 : Set the trigger output to "0" <br>
 *                                            (low) after the passage from 1 to 0 <br>
 *                                            from pulse encoder 
 * @param [in] ul_StartValue        : Pulse encoder start value (1 to 4294967295)
 *
 * @retval 0: No error.                                            
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module is not a pulse encoder module.
 * @retval 3: Pulse encoder selection is wrong.
 * @retval 4: Input level selection is wrong.
 * @retval 5: Digital TRIGGER output action selection is wrong.
 * @retval 6: Pulse encoder start value is wrong.
 */
int   i_APCI1710_InitPulseEncoder     (struct pci_dev *pdev, 
                                       uint8_t        b_ModulNbr,
                                       uint8_t        b_PulseEncoderNbr,
                                       uint8_t        b_InputLevelSelection,
                                       uint8_t        b_TriggerOutputAction,
                                       uint32_t      ul_StartValue)
	{
	int i_ReturnValue = 0;
	uint32_t dw_IntRegister;


	/***************************/
	/* Test if board handle OK */
	/***************************/

	if (!pdev) 
	   {
	   return 1;
	   }

	/***********************************/
	/* Test the selected module number */
	/***********************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*************************/
	   /* Test if pulse encoder */
	   /*************************/

	   if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_PULSE_ENCODER)
	      {
	      /******************************************/
	      /* Test the selected pulse encoder number */
	      /******************************************/

	      if (b_PulseEncoderNbr <= 3)
		 {
		 /************************/
		 /* Test the input level */
		 /************************/

		 if ((b_InputLevelSelection == 0) || (b_InputLevelSelection == 1))
		    {
		    /*******************************************/
		    /* Test the ouput TRIGGER action selection */
		    /*******************************************/

		    if ((b_TriggerOutputAction <= 2) || (b_PulseEncoderNbr > 0))
		       {
		       if (ul_StartValue > 1)
			  {
			  INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + 20, &dw_IntRegister);

			  /***********************/
			  /* Set the start value */
			  /***********************/

			  OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + (b_PulseEncoderNbr * 4), ul_StartValue);

			  /***********************/
			  /* Set the input level */
			  /***********************/

			  APCI1710_PRIVDATA(pdev)->
	                  s_ModuleInfo [(int)b_ModulNbr].
			  s_PulseEncoderModuleInfo.
			  dw_SetRegister = (APCI1710_PRIVDATA(pdev)->
	                                    s_ModuleInfo [(int)b_ModulNbr].
					    s_PulseEncoderModuleInfo.
					    dw_SetRegister & (0xFFFFFFFFUL - (1UL << (8 + b_PulseEncoderNbr)))) |
					   ((1UL & (~b_InputLevelSelection)) << (8 + b_PulseEncoderNbr));

			  /*******************************/
			  /* Test if output trigger used */
			  /*******************************/

			  if ((b_TriggerOutputAction > 0) && (b_PulseEncoderNbr == 0))
			     {
			     /****************************/
			     /* Enable the output action */
			     /****************************/

			     APCI1710_PRIVDATA(pdev)->
	                     s_ModuleInfo [(int)b_ModulNbr].
			     s_PulseEncoderModuleInfo.
			     dw_SetRegister = APCI1710_PRIVDATA(pdev)->
	                                      s_ModuleInfo [(int)b_ModulNbr].
					      s_PulseEncoderModuleInfo.
					      dw_SetRegister |
					      (1UL << (4 + b_PulseEncoderNbr));

			     /*********************************/
			     /* Set the output TRIGGER action */
			     /*********************************/

			     APCI1710_PRIVDATA(pdev)->
	                     s_ModuleInfo [(int)b_ModulNbr].
			     s_PulseEncoderModuleInfo.
			     dw_SetRegister = (APCI1710_PRIVDATA(pdev)->
	                                       s_ModuleInfo [(int)b_ModulNbr].
					       s_PulseEncoderModuleInfo.
					       dw_SetRegister & (0xFFFFFFFFUL - (1UL << (12 + b_PulseEncoderNbr)))) |
					      ((1UL & (b_TriggerOutputAction - 1)) << (12 + b_PulseEncoderNbr));
			     }
			  else
			     {
			     /*****************************/
			     /* Disable the output action */
			     /*****************************/

			     APCI1710_PRIVDATA(pdev)->
	                     s_ModuleInfo [(int)b_ModulNbr].
			     s_PulseEncoderModuleInfo.
			     dw_SetRegister = APCI1710_PRIVDATA(pdev)->
	                                      s_ModuleInfo [(int)b_ModulNbr].
					      s_PulseEncoderModuleInfo.
					      dw_SetRegister & (0xFFFFFFFFUL - (1UL << (4 + b_PulseEncoderNbr)));
			     }

			  /*************************/
			  /* Set the configuration */
			  /*************************/

			  OUTPDW (GET_BAR2(pdev), 
			          MODULE_OFFSET(b_ModulNbr) + 20,
			          APCI1710_PRIVDATA(pdev)->
	                          s_ModuleInfo [(int)b_ModulNbr].
				  s_PulseEncoderModuleInfo.
				  dw_SetRegister);

			  APCI1710_PRIVDATA(pdev)->
	                  s_ModuleInfo [(int)b_ModulNbr].
			  s_PulseEncoderModuleInfo.
			  s_PulseEncoderInfo [b_PulseEncoderNbr].
			  b_PulseEncoderInit = 1;
			  }
		       else
			  {
			  /**************************************/
			  /* Pulse encoder start value is wrong */
			  /**************************************/

			  i_ReturnValue = 6;
			  }
		       }
		    else
		       {
		       /****************************************************/
		       /* Digital TRIGGER output action selection is wrong */
		       /****************************************************/

		       i_ReturnValue = 5;
		       }
		    }
		 else
		    {
		    /**********************************/
		    /* Input level selection is wrong */
		    /**********************************/

		    i_ReturnValue = 4;
		    }
		 }
	      else
		 {
		 /************************************/
		 /* Pulse encoder selection is wrong */
		 /************************************/

		 i_ReturnValue = 3;
		 }
	      }
	   else
	      {
	      /********************************************/
	      /* The module is not a pulse encoder module */
	      /********************************************/

	      i_ReturnValue = 2;
	      }
	   }
	else
	   {
	   /********************************************/
	   /* The module is not a pulse encoder module */
	   /********************************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
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
 * @param [in] pdev                : The device to initialize.
 * @param [in] b_ModulNbr          : Module number to configure (0 to 3).
 * @param [in] b_PulseEncoderNbr   : Pulse encoder selection (0 to 3).
 * @param [in] b_CycleSelection    : APCI1710_CONTINUOUS: <br>
 *                                     Each time the counting value is set <br>
 *                                     on "0", the pulse encoder load the <br>
 *                                     start value after the next pulse. <br>
 *                                   APCI1710_SINGLE:
 *                                     If the counter is set on "0", the pulse <br>
 *                                     encoder is stopped.
 * @param [in] b_InterruptHandling : Interrupts can be generated, when the pulse <br>
 *                                   encoder has run down. <br>
 *                                   With this parameter the user decides if <br>
 *                                   interrupts are used or not. <br>
 *                                     APCI1710_ENABLE : Interrupts are enabled <br>
 *                                     APCI1710_DISABLE: Interrupts are disabled
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval 5: Cycle selection mode is wrong.
 * @retval 6: Interrupt handling mode is wrong.
 * @retval 7: Interrupt routine not installed. See function "i_APCI1710_SetBoardIntRoutine"
 */
int   i_APCI1710_EnablePulseEncoder   (struct pci_dev *pdev, 
                                       uint8_t        b_ModulNbr,
                                       uint8_t        b_PulseEncoderNbr,
                                       uint8_t        b_CycleSelection,
                                       uint8_t        b_InterruptHandling)
	{
	int i_ReturnValue = 0;


	/***************************/
	/* Test if board handle OK */
	/***************************/

	if (!pdev) 
	   {
	   return 1;
	   }

	/***********************************/
	/* Test the selected module number */
	/***********************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /******************************************/
	   /* Test the selected pulse encoder number */
	   /******************************************/

	   if (b_PulseEncoderNbr <= 3)
	      {
	      /*************************************/
	      /* Test if pulse encoder initialised */
	      /*************************************/

	      if (!IMP_COUNTER_NOT_INITIALISED (pdev, b_ModulNbr, b_PulseEncoderNbr))
		 {
		 /****************************/
		 /* Test the cycle selection */
		 /****************************/

		 if (b_CycleSelection == APCI1710_CONTINUOUS || b_CycleSelection == APCI1710_SINGLE)
		    {
		    /*******************************/
		    /* Test the interrupt handling */
		    /*******************************/

		    if (b_InterruptHandling == APCI1710_ENABLE || b_InterruptHandling == APCI1710_DISABLE)
		       {
		       /******************************/
		       /* Test if interrupt not used */
		       /******************************/

		       if (b_InterruptHandling == APCI1710_DISABLE)
			  {
			  /*************************/
			  /* Disable the interrupt */
			  /*************************/

			  APCI1710_PRIVDATA(pdev)->
	                  s_ModuleInfo [(int)b_ModulNbr].
			  s_PulseEncoderModuleInfo.
			  dw_SetRegister = APCI1710_PRIVDATA(pdev)->
	                                   s_ModuleInfo [(int)b_ModulNbr].
					   s_PulseEncoderModuleInfo.
					   dw_SetRegister & (0xFFFFFFFFUL - (1UL << b_PulseEncoderNbr));
			  }
		       else
			  {
			  /********************************/
			  /* Test if interrupt initialsed */
			  /********************************/

	                  if ((APCI1710_PRIVDATA(pdev)->s_InterruptInfos.b_InterruptInitialized) != 0)
			     {
			     /************************/
			     /* Enable the interrupt */
			     /************************/

			     APCI1710_PRIVDATA(pdev)->
	                     s_ModuleInfo [(int)b_ModulNbr].
			     s_PulseEncoderModuleInfo.
			     dw_SetRegister = APCI1710_PRIVDATA(pdev)->
	                                      s_ModuleInfo [(int)b_ModulNbr].
					      s_PulseEncoderModuleInfo.
					      dw_SetRegister | (1UL << b_PulseEncoderNbr);
			     }
			  else
			     {
			     /***********************************/
			     /* Interrupt routine not installed */
			     /***********************************/

			     i_ReturnValue = -7;
			     }
			  }

		       if (!i_ReturnValue)
			  {
			  /***********************************/
			  /* Enable or disable the interrupt */
			  /***********************************/

			  OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + 20,
				  APCI1710_PRIVDATA(pdev)->
	                          s_ModuleInfo [(int)b_ModulNbr].
				  s_PulseEncoderModuleInfo.
				  dw_SetRegister);

			  /****************************/
			  /* Enable the pulse encoder */
			  /****************************/

			  APCI1710_PRIVDATA(pdev)->
	                  s_ModuleInfo [(int)b_ModulNbr].
			  s_PulseEncoderModuleInfo.
			  dw_ControlRegister = APCI1710_PRIVDATA(pdev)->
	                                       s_ModuleInfo [(int)b_ModulNbr].
					       s_PulseEncoderModuleInfo.
					       dw_ControlRegister | (1UL << b_PulseEncoderNbr);

			  /**********************/
			  /* Set the cycle mode */
			  /**********************/

			  APCI1710_PRIVDATA(pdev)->
	                  s_ModuleInfo [(int)b_ModulNbr].
			  s_PulseEncoderModuleInfo.
			  dw_ControlRegister = (APCI1710_PRIVDATA(pdev)->
	                                        s_ModuleInfo [(int)b_ModulNbr].
						s_PulseEncoderModuleInfo.
						dw_ControlRegister & (0xFFFFFFFFUL - (1 << (b_PulseEncoderNbr + 4)))) |
						((b_CycleSelection & 1UL) << (4 + b_PulseEncoderNbr));

			  /****************************/
			  /* Enable the pulse encoder */
			  /****************************/

			  OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + 16,
				  APCI1710_PRIVDATA(pdev)->
	                          s_ModuleInfo [(int)b_ModulNbr].
				  s_PulseEncoderModuleInfo.
				  dw_ControlRegister);
			  }
		       }
		    else
		       {
		       /************************************/
		       /* Interrupt handling mode is wrong */
		       /************************************/

		       i_ReturnValue = 6;
		       }
		    }
		 else
		    {
		    /*********************************/
		    /* Cycle selection mode is wrong */
		    /*********************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /*********************************/
		 /* Pulse encoder not initialised */
		 /*********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /************************************/
	      /* Pulse encoder selection is wrong */
	      /************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*****************************/
	   /* Module selection is wrong */
	   /*****************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
	
//------------------------------------------------------------------------------	
	
/** Disable the pulse encoder.
 * 
 * Disable the selected pulse encoder (b_PulseEncoderNbr) from selected module (b_ModulNbr).
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_PulseEncoderNbr : Pulse encoder selection (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 */
int   i_APCI1710_DisablePulseEncoder  (struct pci_dev *pdev, 
                                       uint8_t        b_ModulNbr,
                                       uint8_t        b_PulseEncoderNbr)
	{
	int i_ReturnValue = 0;


	/***************************/
	/* Test if board handle OK */
	/***************************/

	if (!pdev) 
	   {
	   return 1;
	   }

	/***********************************/
	/* Test the selected module number */
	/***********************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /******************************************/
	   /* Test the selected pulse encoder number */
	   /******************************************/

	   if (b_PulseEncoderNbr <= 3)
	      {
	      /*************************************/
	      /* Test if pulse encoder initialised */
	      /*************************************/

	      if (!IMP_COUNTER_NOT_INITIALISED (pdev, b_ModulNbr, b_PulseEncoderNbr))
		 {
		 APCI1710_PRIVDATA(pdev)->
	         s_ModuleInfo [(int)b_ModulNbr].
		 s_PulseEncoderModuleInfo.
		 dw_ControlRegister = APCI1710_PRIVDATA(pdev)->
	                              s_ModuleInfo [(int)b_ModulNbr].
				      s_PulseEncoderModuleInfo.
				      dw_ControlRegister & (0xFFFFFFFFUL - (1UL << b_PulseEncoderNbr));

		 /*****************************/
		 /* Disable the pulse encoder */
		 /*****************************/

		 OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + 16,
			 APCI1710_PRIVDATA(pdev)->
	                 s_ModuleInfo [(int)b_ModulNbr].
			 s_PulseEncoderModuleInfo.
			 dw_ControlRegister);
		 }
	      else
		 {
		 /*********************************/
		 /* Pulse encoder not initialised */
		 /*********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /************************************/
	      /* Pulse encoder selection is wrong */
	      /************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*****************************/
	   /* Module selection is wrong */
	   /*****************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Reads the pulse encoder status
 * 
 * Reads the status from selected pulse encoder (b_PulseEncoderNbr) 
 * from selected module (b_ModulNbr).
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_PulseEncoderNbr : Pulse encoder selection (0 to 3).
 *
 * @param [out] pb_Status         : Pulse encoder status. <br>
 *                                    0 : No overflow occur <br>
 *                                    1 : Overflow occur
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 */
int  i_APCI1710_ReadPulseEncoderStatus (struct pci_dev *pdev, 
                                        uint8_t        b_ModulNbr,
                                        uint8_t        b_PulseEncoderNbr,
                                        uint8_t        *pb_Status)
	{
	int            i_ReturnValue = 0;
	uint32_t dw_StatusRegister;


	/***************************/
	/* Test if board handle OK */
	/***************************/

	if (!pdev) 
	   {
	   return 1;
	   }

	/***********************************/
	/* Test the selected module number */
	/***********************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /******************************************/
	   /* Test the selected pulse encoder number */
	   /******************************************/

	   if (b_PulseEncoderNbr <= 3)
	      {
	      /*************************************/
	      /* Test if pulse encoder initialised */
	      /*************************************/

	      if (!IMP_COUNTER_NOT_INITIALISED (pdev, b_ModulNbr, b_PulseEncoderNbr))
		 {
		 /****************************/
		 /* Read the status register */
		 /****************************/

		 INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + 16, &dw_StatusRegister);

		 APCI1710_PRIVDATA(pdev)->
	         s_ModuleInfo [(int)b_ModulNbr].
		 s_PulseEncoderModuleInfo.
		 dw_StatusRegister = APCI1710_PRIVDATA(pdev)->
	                             s_ModuleInfo [(int)b_ModulNbr].
				     s_PulseEncoderModuleInfo.
				     dw_StatusRegister | dw_StatusRegister;

		 *pb_Status = (uint8_t) ((APCI1710_PRIVDATA(pdev)->s_ModuleInfo [(int)b_ModulNbr].s_PulseEncoderModuleInfo.dw_StatusRegister >> (1 + (int)b_PulseEncoderNbr)) & 1);

		 APCI1710_PRIVDATA(pdev)->
	         s_ModuleInfo [(int)b_ModulNbr].
		 s_PulseEncoderModuleInfo.
		 dw_StatusRegister = APCI1710_PRIVDATA(pdev)->
	                             s_ModuleInfo [(int)b_ModulNbr].
				     s_PulseEncoderModuleInfo.
				     dw_StatusRegister & (0xFFFFFFFFUL - (1 << (1 + b_PulseEncoderNbr)));
		 }
	      else
		 {
		 /*********************************/
		 /* Pulse encoder not initialised */
		 /*********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /************************************/
	      /* Pulse encoder selection is wrong */
	      /************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*****************************/
	   /* Module selection is wrong */
	   /*****************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------
	
/** Reads the pulse encoder value.
 * 
 * Reads the value from selected pulse encoder (b_PulseEncoderNbr) from selected module (b_ModulNbr).
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_PulseEncoderNbr : Pulse encoder selection (0 to 3).
 * 
 * @param [out] pul_ReadValue    : Pulse encoder value
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 */
int i_APCI1710_ReadPulseEncoderValue   (struct pci_dev *pdev, 
                                        uint8_t        b_ModulNbr,
                                        uint8_t        b_PulseEncoderNbr,
                                        uint32_t    *pul_ReadValue)
	{
	int i_ReturnValue = 0;


	/***************************/
	/* Test if board handle OK */
	/***************************/

	if (!pdev) 
	   {
	   return 1;
	   }

	/***********************************/
	/* Test the selected module number */
	/***********************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /******************************************/
	   /* Test the selected pulse encoder number */
	   /******************************************/

	   if (b_PulseEncoderNbr <= 3)
	      {
	      /*************************************/
	      /* Test if pulse encoder initialised */
	      /*************************************/

	      if (!IMP_COUNTER_NOT_INITIALISED (pdev, b_ModulNbr, b_PulseEncoderNbr))
		 {
		 /******************/
		 /* Read the value */
		 /******************/

		 INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + (4 * b_PulseEncoderNbr), pul_ReadValue);
		 }
	      else
		 {
		 /*********************************/
		 /* Pulse encoder not initialised */
		 /*********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /************************************/
	      /* Pulse encoder selection is wrong */
	      /************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*****************************/
	   /* Module selection is wrong */
	   /*****************************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Writes a 32-bit value into the pulse encoder.
 * 
 * Writes a 32-bit value (ul_WriteValue) into the selected pulse encoder (b_PulseEncoderNbr) 
 * from selected module (b_ModulNbr). This operation set the new start pulse
 * encoder value.                                         |
 * 
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] b_PulseEncoderNbr : Pulse encoder selection (0 to 3).
 * @param [in] ul_WriteValue     : 32-bit value to be written
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 */
int   i_APCI1710_WritePulseEncoderValue (struct pci_dev *pdev, 
                                         uint8_t        b_ModulNbr,
                                         uint8_t        b_PulseEncoderNbr,
                                         uint32_t      ul_WriteValue)
	{
	int i_ReturnValue = 0;


	/***************************/
	/* Test if board handle OK */
	/***************************/

	if (!pdev) 
	   {
	   return 1;
	   }

	/***********************************/
	/* Test the selected module number */
	/***********************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /******************************************/
	   /* Test the selected pulse encoder number */
	   /******************************************/

	   if (b_PulseEncoderNbr <= 3)
	      {
	      /*************************************/
	      /* Test if pulse encoder initialised */
	      /*************************************/

	      if (!IMP_COUNTER_NOT_INITIALISED (pdev, b_ModulNbr, b_PulseEncoderNbr))
		 {
		 /*******************/
		 /* Write the value */
		 /*******************/

		 OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + (4 * b_PulseEncoderNbr), ul_WriteValue);
		 }
	      else
		 {
		 /*********************************/
		 /* Pulse encoder not initialised */
		 /*********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /************************************/
	      /* Pulse encoder selection is wrong */
	      /************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*****************************/
	   /* Module selection is wrong */
	   /*****************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}


/** Set the digital output H on.
 * 
 * Set the digital outp H on (High) from selected module (b_ModulNbr)
 * 
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Functionality not available
 */
int   i_APCI1710_PulseEncoderSetDigitalOutputOn (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int i_ReturnValue = 0;


	/***************************/
	/* Test if board handle OK */
	/***************************/

	if (!pdev) 
	   {
	   return 1;
	   }

	/***********************************/
	/* Test the selected module number */
	/***********************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*************************/
	   /* Test if pulse encoder */
	   /*************************/

	   if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_PULSE_ENCODER)
	      {
	      /**************************/
	      /* Test the version >= 30 */
	      /**************************/

	      if ((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [(int)b_ModulNbr] & 0xFFFF) >= 0x3330)
	         {
	         APCI1710_PRIVDATA(pdev)->
	         s_ModuleInfo [(int)b_ModulNbr].
	         s_PulseEncoderModuleInfo.
	         dw_ControlRegister = APCI1710_PRIVDATA(pdev)->
	                              s_ModuleInfo [(int)b_ModulNbr].
				      s_PulseEncoderModuleInfo.
				      dw_ControlRegister | 0x100;

	         /*******************************/
	         /* Set the digital output H on */
	         /*******************************/

	         OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + 16,
	                 APCI1710_PRIVDATA(pdev)->
	                 s_ModuleInfo [(int)b_ModulNbr].
	                 s_PulseEncoderModuleInfo.
	                 dw_ControlRegister);
	         }
	      else
		 {
		 /*******************************/
		 /* Functionality not available */
		 /*******************************/

		 i_ReturnValue = 3;
		 }
	      }
	   else
	      {
	      /*******************************/
	      /* Functionality not available */
	      /*******************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*****************************/
	   /* Module selection is wrong */
	   /*****************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

/** Set the digital output H off.
 * 
 * Set the digital outp H off (Low) from selected module (b_ModulNbr)
 * 
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Functionality not available
 */
int   i_APCI1710_PulseEncoderSetDigitalOutputOff (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int i_ReturnValue = 0;


	/***************************/
	/* Test if board handle OK */
	/***************************/

	if (!pdev) 
	   {
	   return 1;
	   }

	/***********************************/
	/* Test the selected module number */
	/***********************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*************************/
	   /* Test if pulse encoder */
	   /*************************/

	   if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_PULSE_ENCODER)
	      {
	      /**************************/
	      /* Test the version >= 30 */
	      /**************************/

	      if ((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [(int)b_ModulNbr] & 0xFFFF) >= 0x3330)
	         {
		 APCI1710_PRIVDATA(pdev)->
	         s_ModuleInfo [(int)b_ModulNbr].
		 s_PulseEncoderModuleInfo.
		 dw_ControlRegister = APCI1710_PRIVDATA(pdev)->
	                              s_ModuleInfo [(int)b_ModulNbr].
				      s_PulseEncoderModuleInfo.
				      dw_ControlRegister & 0xFFFFFEFFUL;

	         /*******************************/
	         /* Set the digital output H on */
	         /*******************************/

	         OUTPDW (GET_BAR2(pdev), MODULE_OFFSET(b_ModulNbr) + 16,
	                 APCI1710_PRIVDATA(pdev)->
	                 s_ModuleInfo [(int)b_ModulNbr].
	                 s_PulseEncoderModuleInfo.
	                 dw_ControlRegister);
	         }
	      else
		 {
		 /*******************************/
		 /* Functionality not available */
		 /*******************************/

		 i_ReturnValue = 3;
		 }
	      }
	   else
	      {
	      /*******************************/
	      /* Functionality not available */
	      /*******************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /*****************************/
	   /* Module selection is wrong */
	   /*****************************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}
