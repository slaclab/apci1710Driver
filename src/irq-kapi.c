/** @file kapi_irq.c

   Contains interrupt kernel functions.

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
    77836 Rheinmuenster
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

EXPORT_SYMBOL(i_APCI1710_SetBoardIntRoutine);
EXPORT_SYMBOL(i_APCI1710_TestInterrupt);

EXPORT_NO_SYMBOLS;

//------------------------------------------------------------------------------

str_InterruptFunctionality s_InterruptFunctionality [] =
								 {{APCI1710_INCREMENTAL_COUNTER, (v_APCI1710_InterruptFunctionality) v_APCI1710_IncrementalCounter_InterruptFunction},
								 {APCI1710_PULSE_ENCODER       , (v_APCI1710_InterruptFunctionality) v_APCI1710_ImpulsCounter_InterruptFunction},
								 {APCI1710_CHRONOMETER         , (v_APCI1710_InterruptFunctionality) v_APCI1710_Chronos_InterruptFunction},
#ifdef WITH_IDV_OPTION
								 {APCI1710_IDV                 , (v_APCI1710_InterruptFunctionality) v_APCI1710_IDV_InterruptFunction},
#endif
								 {APCI1710_ETM                 , (v_APCI1710_InterruptFunctionality) v_APCI1710_ETM_InterruptFunction}};
/* Not implemented */
//								 {APCI1710_SSI_COUNTER        , (v_APCI1710_InterruptFunctionality) NULL},
//								 {APCI1710_TTL_IO             , (v_APCI1710_InterruptFunctionality) NULL},
//								 {APCI1710_DIGITAL_IO         , (v_APCI1710_InterruptFunctionality) NULL},
//								 {APCI1710_82X54_TIMER        , (v_APCI1710_InterruptFunctionality) v_APCI1710_82X54_InterruptFunction},
//								 {APCI1710_TOR_COUNTER        , (v_APCI1710_InterruptFunctionality) v_APCI1710_TOR_InterruptFunction},
//								 {APCI1710_PWM                , (v_APCI1710_InterruptFunctionality) v_APCI1710_PWM_InterruptFunction},
//								 {APCI1710_CDA                , (v_APCI1710_InterruptFunctionality) v_APCI1710_CDA_InterruptFunction},
//								 {APCI1710_SSI_MONITOR        , (v_APCI1710_InterruptFunctionality) v_APCI1710_SSIMon_InterruptFunction}};

//------------------------------------------------------------------------------

/* removes the user handler for a given module */
int i_APCI1710_ClearUserHandler(struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	/* Clear the module interrupt function address */
	APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality [b_ModulNbr].v_InterruptFunction = NULL;
	return 0;
}
//------------------------------------------------------------------------------
/** Disable and reset the interrupt routine.
 *
 * @param [in] pdev              : The device to initialize.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 */

/* moved in file reset_board-kapi.c */
//int   i_APCI1710_ResetBoardIntRoutine     (struct pci_dev * pdev)
//	{
//	uint8_t   b_ModuleCpt       = 0;
//
//		if (!pdev) return 1;
//
//		/* Disable all APCI-1710 interrupt */
//		for (b_ModuleCpt = 0; b_ModuleCpt < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule; b_ModuleCpt ++)
//			{
//			/* Test if incremental counter */
//			if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModuleCpt) == APCI1710_INCREMENTAL_COUNTER)
//				{
//			  	/* Disable the frequency measurement */
//			  	OUTPDW (GET_BAR2(pdev),
//						20 + MODULE_OFFSET(b_ModuleCpt),
//						APCI1710_PRIVDATA(pdev)->
//						s_ModuleInfo [b_ModuleCpt].
//						s_SiemensCounterInfo.
//						s_ModeRegister.
//						dw_ModeRegister1_2_3_4 & (((APCI1710_DISABLE_FREQUENCY & APCI1710_DISABLE_FREQUENCY_INT) << 12) | 0xFF));
//
//				/* Disable interrupt */
//				APCI1710_PRIVDATA(pdev)->
//				s_ModuleInfo [b_ModuleCpt].
//				s_SiemensCounterInfo.
//				s_ModeRegister.
//				s_ByteModeRegister.
//				b_ModeRegister3 = APCI1710_PRIVDATA(pdev)->
//									s_ModuleInfo [b_ModuleCpt].
//									s_SiemensCounterInfo.
//									s_ModeRegister.
//									s_ByteModeRegister.
//									b_ModeRegister3 & APCI1710_DISABLE_FREQUENCY & APCI1710_DISABLE_FREQUENCY_INT;
//
//				/* Disable the extern latch interrupt */
//				OUTPDW (GET_BAR2(pdev),
//						20 + MODULE_OFFSET(b_ModuleCpt),
//						APCI1710_PRIVDATA(pdev)->
//						s_ModuleInfo [b_ModuleCpt].
//						s_SiemensCounterInfo.
//						s_ModeRegister.
//						dw_ModeRegister1_2_3_4 & ((APCI1710_DISABLE_LATCH_INT << 8) | 0xFF));
//
//				mdelay (1);
//
//				/* Disable interrupt */
//				APCI1710_PRIVDATA(pdev)->
//				s_ModuleInfo [b_ModuleCpt].
//				s_SiemensCounterInfo.
//				s_ModeRegister.
//				s_ByteModeRegister.
//				b_ModeRegister2 = APCI1710_PRIVDATA(pdev)->
//									s_ModuleInfo [b_ModuleCpt].
//									s_SiemensCounterInfo.
//									s_ModeRegister.
//									s_ByteModeRegister.
//									b_ModeRegister2 & APCI1710_DISABLE_LATCH_INT;
//				}
//   			} // for (b_FctCpt = 0; b_FctCpt < 2; b_FctCpt ++)
//
//			/* Functionality interrupt function installation */
//			for (b_ModuleCpt = 0; b_ModuleCpt < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule; b_ModuleCpt ++)
//				{
//				/* Test if interrupt function installed */
//				if (APCI1710_PRIVDATA(pdev)->
//					s_InterruptFunctionality [b_ModuleCpt].
//					v_InterruptFunction != NULL)
//					{
//					/* Clear the module interrupt function address */
//					i_APCI1710_ClearUserHandler(struct pci_dev * b_ModulNbr)
//					APCI1710_PRIVDATA(pdev)->
//					s_InterruptFunctionality [b_ModuleCpt].
//					v_InterruptFunction = NULL;
//					} // if (ps_APCI1710Variable->s_Board [b_BoardHandle].s_InterruptFunctionality [b_ModuleCpt].v_InterruptFunction != NULL)
//				} // for (b_ModuleCpt = 0; b_ModuleCpt < 4; b_ModuleCpt ++)
//
//
//
//				APCI1710_PRIVDATA(pdev)->s_UserInterruptCallback.v_UserInterruptFunction = NULL;
//
//				APCI1710_PRIVDATA(pdev)->s_InterruptInfos.b_InterruptInitialized = 0;
//
//	   return 0;
//	   }

//------------------------------------------------------------------------------

/** Enable and set the interrupt routine.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] InterruptCallback : The user interrupt callback.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The interrupt callback is NULL.
 */
int   i_APCI1710_SetBoardIntRoutine  (struct pci_dev * pdev, void (*InterruptCallback) (struct pci_dev * pdev))
	{
	int  i_ReturnValue = 0;
	uint8_t b_ModuleCpt = 0;
	uint8_t b_SerachModuleCpt = 0;
	uint8_t b_FunctionalityCpt = 0;

		if (!pdev)
			return 1;

		/* If the global interrupt is not set */
		if (APCI1710_PRIVDATA(pdev)->s_UserInterruptCallback.v_UserInterruptFunction == NULL)
			{
			/* Set the user callback function */
	        APCI1710_PRIVDATA(pdev)->s_UserInterruptCallback.v_UserInterruptFunction = InterruptCallback;
			}

		/* For all modules */
		for (b_ModuleCpt = 0; b_ModuleCpt < NUMBER_OF_MODULE(pdev); b_ModuleCpt ++)
			{
			/* For all functionality */
			for (b_FunctionalityCpt = 0; b_FunctionalityCpt < (sizeof (s_InterruptFunctionality) / sizeof (str_InterruptFunctionality)); b_FunctionalityCpt ++)
			{
				/* If the module functionality can managed interrupt and the interrupt is set. */
				if ( ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModuleCpt) ==s_InterruptFunctionality[b_FunctionalityCpt].dw_Functionality) &&
					 (s_InterruptFunctionality[b_FunctionalityCpt].v_InterruptFunction != NULL) )
				{
					break;
				}
			}

			/* Test if interrupt used for this functionality */
			if (b_FunctionalityCpt < (sizeof (s_InterruptFunctionality) / sizeof (str_InterruptFunctionality)))
				{
				/* Search if any other module has installed the same interrupt functionality */
				for (b_SerachModuleCpt = 0; b_SerachModuleCpt < NUMBER_OF_MODULE(pdev); b_SerachModuleCpt ++)
				{
					if ( (APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality [b_SerachModuleCpt].v_InterruptFunction != NULL) &&
						 ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModuleCpt) == APCI1710_MODULE_FUNCTIONALITY(pdev,b_SerachModuleCpt) ) )
						/*((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModuleCpt] & 0xFFFF0000UL) ==
						(APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_SerachModuleCpt] & 0xFFFF0000UL)))
						*/
					{
						/* Save the interrupt function */
						APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality [b_ModuleCpt].v_InterruptFunction =
						APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality [b_SerachModuleCpt].v_InterruptFunction;
						break;
					}
				} // for (b_SerachModuleCpt = 0; b_SerachModuleCpt < 4; b_SerachModuleCpt ++)

				/* Test if interrupt function found for this functionality */
				if (b_SerachModuleCpt != NUMBER_OF_MODULE(pdev))
					{
					continue;
					} // if (b_SerachModuleCpt != 4)

				/* Test if not interrupt function found for this functionality */
				/* Save the functionality interrupt function */
				if (APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality [b_ModuleCpt].v_InterruptFunction == NULL)
					APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality [b_ModuleCpt].v_InterruptFunction = s_InterruptFunctionality[b_FunctionalityCpt].v_InterruptFunction;

				} // if (b_FunctionalityCpt < (sizeof (s_InterruptFunctionality)) / sizeof (str_InterruptFunctionality)))
			else
				{
				/* No interrupt function used */
				i_APCI1710_ClearUserHandler(pdev, b_ModuleCpt);
				}
			} // for (b_ModuleCpt = 0; b_ModuleCpt < 4; b_ModuleCpt ++)

			APCI1710_PRIVDATA(pdev)->s_InterruptInfos.b_InterruptInitialized = 1;

		return (i_ReturnValue);
	}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Incremental counter interrupt function management.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ul_InterruptMask  : Interrupt mask.
 *
 * @param [out] pb_InterruptFlag : 0 -> No interrupt generated.
 *                                 1 -> Interrupt generated.
 */
void	v_APCI1710_IncrementalCounter_InterruptFunction (struct pci_dev *pdev,
                                                         uint8_t b_Module,
                                                         uint8_t * pb_InterruptFlag)
{
	/*
	 * ALL PART THAT ARE NOT USEFULL ARE COMMENTED
	 * They can be implemented later that's why they are keept.
	 */
	uint32_t ul_StatusRegister = 0;
	uint32_t ul_InterruptLatchReg = 0;
	uint32_t ul_LatchRegisterValue = 0;
	uint32_t ui_16BitValue = 0;
		/* Test if latch interrupt is enabled. */
	if (APCI1710_PRIVDATA(pdev)->
		s_ModuleInfo[b_Module].
		s_SiemensCounterInfo.
		s_ModeRegister.
		s_ByteModeRegister.
		b_ModeRegister2 & APCI1710_ENABLE_LATCH_INT)
	{
		INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_Module), &ul_InterruptLatchReg);

		/* Test if interrupt */
		if (ul_InterruptLatchReg & 0x66)
		{
			*pb_InterruptFlag = 1;

			/* Clear the interrupt */
			OUTPDW (GET_BAR2(pdev), 44 + MODULE_OFFSET(b_Module), ul_InterruptLatchReg & 0x66);
		}

		/* Test if interrupt */
		if ((ul_InterruptLatchReg & 0x66) && (APCI1710_PRIVDATA(pdev)->
												s_ModuleInfo[b_Module].
												s_SiemensCounterInfo.
												s_ModeRegister.
												s_ByteModeRegister.
												b_ModeRegister2 & 0x80))
		{
			/* Test if strobe latch I interrupt */
			if (ul_InterruptLatchReg & 6)
			{
				INPDW (GET_BAR2(pdev), 4 + MODULE_OFFSET(b_Module), &ul_LatchRegisterValue);

				/* Set the interrupt flag */
				*pb_InterruptFlag = 1;

				/* Test if high level */
				if (ul_InterruptLatchReg & 2)
				{
					/* User interrupt management */
					v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x1UL, &ul_LatchRegisterValue);
				}

				/* Test if low level */
				if (ul_InterruptLatchReg & 4)
				{
					/* User interrupt management */
					v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x10001UL, &ul_LatchRegisterValue);
				}
			}

			/* Test if strobe latch II interrupt */
			if (ul_InterruptLatchReg & 0x60)
			{
				INPDW (GET_BAR2(pdev), 8 + MODULE_OFFSET(b_Module), &ul_LatchRegisterValue);

				/* Set the interrupt flag */
				*pb_InterruptFlag = 1;

				/* Test if high level */
				if (ul_InterruptLatchReg & 0x20)
				{
					/* User interrupt management */
					v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x2UL, &ul_LatchRegisterValue);
				}

				/* Test if high level */
				if (ul_InterruptLatchReg & 0x40)
				{
					/* User interrupt management */
					v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x10002UL, &ul_LatchRegisterValue);
				}
			}
		}
	}


	/* Read the register status for Index, Frequency and compare. */
	if (APCI1710_PRIVDATA(pdev)->
		s_ModuleInfo[b_Module].
		s_SiemensCounterInfo.
		s_ModeRegister.
		s_ByteModeRegister.
		b_ModeRegister3 & (APCI1710_ENABLE_INDEX_INT | APCI1710_ENABLE_FREQUENCY_INT | APCI1710_ENABLE_COMPARE_INT))
	{
		INPDW (GET_BAR2(pdev), 24 + MODULE_OFFSET(b_Module), &ul_InterruptLatchReg);
	}

	if (APCI1710_PRIVDATA(pdev)->
		s_ModuleInfo[b_Module].
		s_SiemensCounterInfo.
		s_ModeRegister.
		s_ByteModeRegister.
		b_ModeRegister3 & APCI1710_ENABLE_INDEX_INT)
	{
		/* Test if index interrupt */
		if (ul_InterruptLatchReg & 0x48)
		{
			*pb_InterruptFlag = 1;
			APCI1710_PRIVDATA(pdev)->
			s_ModuleInfo[b_Module].
			s_SiemensCounterInfo.
			s_InitFlag.
			b_IndexInterruptOccur = 1;

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo[b_Module].
				s_SiemensCounterInfo.
				s_ModeRegister.
				s_ByteModeRegister.
				b_ModeRegister2 & APCI1710_INDEX_AUTO_MODE)
			{
				OUTPDW (GET_BAR2(pdev),
				20 + MODULE_OFFSET(b_Module),
				APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo[b_Module].
				s_SiemensCounterInfo.
				s_ModeRegister.
				dw_ModeRegister1_2_3_4);
			}

			/* Test if interrupt enabled */
			if ((APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo[b_Module].
				s_SiemensCounterInfo.
				s_ModeRegister.
				s_ByteModeRegister.
				b_ModeRegister3 & APCI1710_ENABLE_INDEX_INT) == APCI1710_ENABLE_INDEX_INT)
			{
				/* Set the interrupt flag */
				*pb_InterruptFlag = 1;

				/* Test if high level */
				if (ul_InterruptLatchReg & 0x8)
				{
					/* User interrupt management */
					v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x4UL, &ul_LatchRegisterValue);
				}

				/* Test if low level */
				if (ul_InterruptLatchReg & 0x40)
				{
					/* User interrupt management */
					v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x10004UL, &ul_LatchRegisterValue);
				}
			}
		}
	}

	if (APCI1710_PRIVDATA(pdev)->
		s_ModuleInfo[b_Module].
		s_SiemensCounterInfo.
		s_ModeRegister.
		s_ByteModeRegister.
		b_ModeRegister3 & APCI1710_ENABLE_COMPARE_INT)
	{
		/* Test if compare interrupt */
		if (ul_InterruptLatchReg & 0x10)
		{
			*pb_InterruptFlag = 1;

			/* Test if interrupt enabled */
			if ((APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo[b_Module].
				s_SiemensCounterInfo.
				s_ModeRegister.
				s_ByteModeRegister.
				b_ModeRegister3 & APCI1710_ENABLE_COMPARE_INT) == APCI1710_ENABLE_COMPARE_INT)
			{
				/* Set the interrupt flag */
				*pb_InterruptFlag = 1;
				ul_LatchRegisterValue = 0;

				/* User interrupt management */
				v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x8UL, &ul_LatchRegisterValue);
			}
		}
	}

	if (APCI1710_PRIVDATA(pdev)->
			s_ModuleInfo[b_Module].
			s_SiemensCounterInfo.
			s_ModeRegister.
			s_ByteModeRegister.
			b_ModeRegister3 & APCI1710_ENABLE_FREQUENCY_INT)
	{
		/* Test if frequency measurement interrupt */
		if (ul_InterruptLatchReg & 0x20)
		{
			*pb_InterruptFlag = 1;

			/* Read the status */
			INPDW (GET_BAR2(pdev), 32 + MODULE_OFFSET(b_Module), &ul_StatusRegister);

			/* Read the value */
			INPDW (GET_BAR2(pdev), 28 + MODULE_OFFSET(b_Module), &ul_LatchRegisterValue);

			if (((ul_StatusRegister >> 1) & 3) == 0)
			{
				/* Test the counter mode */
				if ((APCI1710_PRIVDATA(pdev)->
					s_ModuleInfo [b_Module].
					s_SiemensCounterInfo.
					s_ModeRegister.
					s_ByteModeRegister.
					b_ModeRegister1 & APCI1710_16BIT_COUNTER) == APCI1710_16BIT_COUNTER)
				{
					/* Test if 16-bit counter 1 pulse occur */
					if ((ul_LatchRegisterValue & 0xFFFFU) != 0)
					{
						ui_16BitValue         = (uint32_t) ul_LatchRegisterValue & 0xFFFFU;
						ul_LatchRegisterValue = (ul_LatchRegisterValue & 0xFFFF0000UL) | (0xFFFFU - ui_16BitValue);
					}

					/* Test if 16-bit counter 2 pulse occur */
					if ((ul_LatchRegisterValue & 0xFFFF0000UL) != 0)
					{
						ui_16BitValue         = (uint32_t) ((ul_LatchRegisterValue >> 16) & 0xFFFFU);
						ul_LatchRegisterValue = (ul_LatchRegisterValue & 0xFFFFUL) | ((0xFFFFU - ui_16BitValue) << 16);
					}
				}
				else
				{
					if (ul_LatchRegisterValue != 0)
					{
						ul_LatchRegisterValue = 0xFFFFFFFFUL - ul_LatchRegisterValue;
					}
				}
			}

			if (((ul_StatusRegister >> 1) & 3) == 1)
			{
					/* Test if 16-bit counter 2 pulse occur */

				if ((ul_LatchRegisterValue & 0xFFFF0000UL) != 0)
				{
					ui_16BitValue         = (uint32_t) ((ul_LatchRegisterValue >> 16) & 0xFFFFU);
					ul_LatchRegisterValue = (ul_LatchRegisterValue & 0xFFFFUL) | ((0xFFFFU - ui_16BitValue) << 16);
				}
			}

			if (((ul_StatusRegister >> 1) & 3) == 2)
			{
				/* Test if 16-bit counter 1 pulse occur */
				if ((ul_LatchRegisterValue & 0xFFFFU) != 0)
				{
					ui_16BitValue         = (uint32_t) ul_LatchRegisterValue & 0xFFFFU;
					ul_LatchRegisterValue = (ul_LatchRegisterValue & 0xFFFF0000UL) | (0xFFFFU - ui_16BitValue);
				}
			}

			/* Set the interrupt flag */
			*pb_InterruptFlag = 1;

			/* User interrupt management */
			v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x10000UL, &ul_LatchRegisterValue);
		}
	}
}


//------------------------------------------------------------------------------

/** Impuls counter interrupt function management.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ul_InterruptMask  : Interrupt mask.
 *
 * @param [out] pb_InterruptFlag : 0 -> No interrupt generated.
 *                                 1 -> Interrupt generated.
 */
void	v_APCI1710_ImpulsCounter_InterruptFunction (struct pci_dev *pdev,
                                                    uint8_t b_Module,
                                                    uint8_t * pb_InterruptFlag)
	{
	uint8_t   b_PulseIncoderCpt;
	uint32_t ul_StatusRegister;
	uint32_t ul_Dummy = 0;

	*pb_InterruptFlag = 0;

	if (((APCI1710_PRIVDATA(pdev)->
	      s_ModuleInfo[b_Module].
	      s_PulseEncoderModuleInfo.
	      s_PulseEncoderInfo [0].
	      b_PulseEncoderInit == 1) ||
	     (APCI1710_PRIVDATA(pdev)->
	      s_ModuleInfo[b_Module].
	      s_PulseEncoderModuleInfo.
	      s_PulseEncoderInfo [1].
	      b_PulseEncoderInit == 1) ||
	     (APCI1710_PRIVDATA(pdev)->
	      s_ModuleInfo[b_Module].
	      s_PulseEncoderModuleInfo.
	      s_PulseEncoderInfo [2].
	      b_PulseEncoderInit == 1) ||
	     (APCI1710_PRIVDATA(pdev)->
	      s_ModuleInfo[b_Module].
	      s_PulseEncoderModuleInfo.
	      s_PulseEncoderInfo [3].
	      b_PulseEncoderInit == 1)) &&
	    ((APCI1710_PRIVDATA(pdev)->
	      s_ModuleInfo[b_Module].
	      s_PulseEncoderModuleInfo.
	      dw_SetRegister & 0xF) != 0))
	   {
	   /****************************/
	   /* Read the status register */
	   /****************************/

	   INPDW (GET_BAR2(pdev), MODULE_OFFSET(b_Module) + 20, &ul_StatusRegister);

	   if (ul_StatusRegister & 0xF)
	      {
	      for (b_PulseIncoderCpt = 0; b_PulseIncoderCpt < 4; b_PulseIncoderCpt ++)
	         {
	         /*************************************/
	         /* Test if pulse encoder initialised */
	         /*************************************/

	         if ((APCI1710_PRIVDATA(pdev)->
	              s_ModuleInfo[b_Module].
		      s_PulseEncoderModuleInfo.
		      s_PulseEncoderInfo [b_PulseIncoderCpt].
		      b_PulseEncoderInit == 1) &&
		     (((APCI1710_PRIVDATA(pdev)->
	                s_ModuleInfo[b_Module].
		        s_PulseEncoderModuleInfo.
		        dw_SetRegister >> b_PulseIncoderCpt) & 1) == 1) &&
		     (((ul_StatusRegister >> (b_PulseIncoderCpt)) & 1) == 1))
		    {
		    /**************************/
		    /* Set the interrupt flag */
		    /**************************/

		    *pb_InterruptFlag = 1;

		    /*****************************/
		    /* User interrupt management */
		    /*****************************/

		    v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x100UL << b_PulseIncoderCpt, &ul_Dummy);
		    }
	         }
	      }
	   }
	}

/** Chronos interrupt function management.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ul_InterruptMask  : Interrupt mask.
 *
 * @param [out] pb_InterruptFlag : 0 -> No interrupt generated.
 *                                 1 -> Interrupt generated.
 */
void	v_APCI1710_Chronos_InterruptFunction (struct pci_dev *pdev,
                                                    uint8_t b_Module,
                                                    uint8_t * pb_InterruptFlag)

{
	uint32_t ul_InterruptLatchReg = 0;
	uint32_t ul_LatchRegisterValue = 0;

	// Begin CG 2249-0505 -> 2250-0705 : Only Interrupt if interrupt was enabled !
	if (APCI1710_PRIVDATA(pdev)->
		s_ModuleInfo [(int)b_Module].
		s_ChronoModuleInfo.
		b_InterruptMask != 0)
	{
	// End CG 2249-0505 -> 2250-0705 : Only Interrupt if interrupt was enabled !

		/*****************************/
		/* Read the interrupt status */
		/*****************************/

		INPDW (GET_BAR2(pdev), 12 + MODULE_OFFSET(b_Module), &ul_InterruptLatchReg);

		/***************************/
		/* Test if interrupt occur */
		/***************************/

		if ((ul_InterruptLatchReg & 0x8) == 0x8)
		{
			/****************************/
			/* Clear the interrupt flag */
			/****************************/

			OUTPDW (GET_BAR2(pdev),
			32 + MODULE_OFFSET(b_Module),
			0);

			/***************************/
			/* Test if continuous mode */
			/***************************/

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_Module].
				s_ChronoModuleInfo.
				b_CycleMode == APCI1710_ENABLE)
			{
			/********************/
			/* Clear the status */
			/********************/

			OUTPDW (GET_BAR2(pdev),
			33 + MODULE_OFFSET(b_Module),
			0);
			}

			/*************************/
			/* Read the timing value */
			/*************************/

			INPDW (GET_BAR2(pdev), 4 + MODULE_OFFSET(b_Module), &ul_LatchRegisterValue);

			/*****************************/
			/* Test if interrupt enabled */
			/*****************************/

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_Module].
				s_ChronoModuleInfo.
				b_InterruptMask)
			{
				/**************************/
				/* Set the interrupt flag */
				/**************************/

				*pb_InterruptFlag = 1;

				/*****************************/
				/* User interrupt management */
				/*****************************/

				v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x80UL, &ul_LatchRegisterValue);
			}
		}
		// Begin CG 2249-0505 -> 2250-0705 : Only Interrupt if interrupt was enabled !
	}
	// End CG 2249-0505 -> 2250-0705 : Only Interrupt if interrupt was enabled !
}

//------------------------------------------------------------------------------

void v_APCI1710_ETM_InterruptFunction	(struct pci_dev *pdev,
						 uint8_t b_Module,
						 uint8_t * pb_InterruptFlag)
	{
	uint8_t   b_ETMCpt = 0;
	uint32_t ul_StatusRegister = 0;
	uint32_t ul_Value[2] = {0, 0};

	/*************************************/
	/* Test if ETM interrupt initialised */
	/*************************************/

	if ((APCI1710_PRIVDATA(pdev)->
			s_ModuleInfo [(int)b_Module].
	     s_ETMModuleInfo.
	     s_ETMInfo [0].
	     b_ETMInterrupt == APCI1710_ENABLE) ||
	    (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_Module].
	     s_ETMModuleInfo.
	     s_ETMInfo [1].
	     b_ETMInterrupt == APCI1710_ENABLE))
	   {
	   /*******************/
	   /* Read the status */
	   /*******************/

	   INPDW (GET_BAR2(pdev),
		  4 + MODULE_OFFSET(b_Module),
		  &ul_StatusRegister);

	   ul_StatusRegister = (ul_StatusRegister >> 2) & 3;

	   for (b_ETMCpt = 0; b_ETMCpt < 2; b_ETMCpt ++)
	      {
	      /***************************/
	      /* Test if interrupt occur */
	      /***************************/

	      if (((ul_StatusRegister >> b_ETMCpt) & 1) == 1)
		 {
		 /**********************/
		 /* Read the ETM value */
		 /**********************/

		 INPDW (GET_BAR2(pdev),
			12 + (b_ETMCpt * 16) + MODULE_OFFSET(b_Module),
			&ul_Value[0]);

		 // Read the ETM total time
		 INPDW (GET_BAR2(pdev),
			16  + (b_ETMCpt * 16) + MODULE_OFFSET(b_Module),
			&ul_Value[1]);

		 /**************************/
		 /* Set the interrupt flag */
		 /**************************/

		 *pb_InterruptFlag = 1;

		 /*****************************/
		 /* User interrupt management */
		 /*****************************/

		 v_APCI1710_UserInterruptManagement (pdev, b_Module, 0x20000UL << b_ETMCpt, ul_Value);

		 } // if (((ul_StatusRegister >> b_EtmCpt) & 1) == 1)
	      } // for (b_ETMCpt = 0; b_ETMCpt < 2; b_ETMCpt ++)
	   }
	}

//------------------------------------------------------------------------------

/** Return interrupt information.
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [out] pb_ModuleMask         : Mask of the events <br>
 *                                      which have generated the interrupt.
 * @param [out] pul_InterruptMask     :
 * @param [out] pul_Value             : Depends on the functionality that generates the interrupt.
 *
 * @retval 1  : No interrupt.
 * @retval >0 : IRQ number
 */
int   i_APCI1710_TestInterrupt (struct pci_dev *pdev,
								uint8_t *   pb_ModuleMask,
								uint32_t * pul_InterruptMask,
								uint32_t * pul_Value)
	{

		if (APCI1710_PRIVDATA(pdev)->s_InterruptParameters.ul_InterruptOccur <= 0)
			return 1;

  		*pul_InterruptMask = APCI1710_PRIVDATA(pdev)->
							s_InterruptParameters.
							s_FIFOInterruptParameters [APCI1710_PRIVDATA(pdev)->s_InterruptParameters.ui_Read].ul_OldInterruptMask;

		*pb_ModuleMask = APCI1710_PRIVDATA(pdev)->
							s_InterruptParameters.
							s_FIFOInterruptParameters [APCI1710_PRIVDATA(pdev)->s_InterruptParameters.ui_Read].b_OldModuleMask;

		// Get the counter or ETM value
		pul_Value[0] = APCI1710_PRIVDATA(pdev)->
						s_InterruptParameters.
						s_FIFOInterruptParameters [APCI1710_PRIVDATA(pdev)->s_InterruptParameters.ui_Read].ul_OldValue[0];

		// If ETM interrupt informations
		if ((0x60000UL & *pul_InterruptMask) != 0)
		{
			// Get the ETM total time value
			pul_Value[1] = APCI1710_PRIVDATA(pdev)->
							s_InterruptParameters.
							s_FIFOInterruptParameters [APCI1710_PRIVDATA(pdev)->s_InterruptParameters.ui_Read].ul_OldValue[1];
		}

		APCI1710_PRIVDATA(pdev)->s_InterruptParameters.ul_InterruptOccur --;

		/* Increment the read FIFO */
		APCI1710_PRIVDATA(pdev)->s_InterruptParameters.ui_Read = (APCI1710_PRIVDATA(pdev)->s_InterruptParameters.ui_Read + 1) % APCI1710_SAVE_INTERRUPT;


		return (pdev->irq);
	}

//------------------------------------------------------------------------------
