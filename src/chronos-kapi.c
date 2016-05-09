/** @file kapi_inc_cpt.c

   Contains chronos kernel functions.

   @par CREATION
   @author Krauth Julien
   @date   15.03.2010

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

EXPORT_SYMBOL(i_APCI1710_InitChrono);
EXPORT_SYMBOL(i_APCI1710_EnableChrono);
EXPORT_SYMBOL(i_APCI1710_DisableChrono);
EXPORT_SYMBOL(i_APCI1710_GetChronoProgressStatus);
EXPORT_SYMBOL(i_APCI1710_ReadChronoValue);
EXPORT_SYMBOL(i_APCI1710_SetChronoChlOn);
EXPORT_SYMBOL(i_APCI1710_SetChronoChlOff);
EXPORT_SYMBOL(i_APCI1710_ReadChronoChlValue);
EXPORT_SYMBOL(i_APCI1710_ReadChronoPortValue);

EXPORT_NO_SYMBOLS;

//------------------------------------------------------------------------------

/** Initialize chronos functionality.
 *
 * Configure the chronometer operating mode (b_ChronoMode)
 * from selected module (b_ModulNbr).
 * The ul_TimingInterval and ul_TimingUnit determine the
 * timing base for the measurement.
 * The pul_RealTimingInterval return the real timing
 * value. You must calling this function be for you call
 * any other function witch access of the chronometer.
 *
 * Witch this functionality from the APCI-1710 you have
 * the possibility to measure the timing witch two event.
 *
 * The mode 0 and 1 is appropriate for period measurement.
 * The mode 2 and 3 is appropriate for frequent measurement.
 * The mode 4 to 7 is appropriate for measuring the timing
 * between  two event.
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [in] b_ModulNbr             : Module number to configure (0 to 3).
 * @param [in] b_ChronoMode           : Chronometer action mode (0 to 7).
 * @param [in] b_PCIInputClock        : Selection from PCI bus clock
 *                                      - APCI1710_30MHZ :
 *                                        The PC have a PCI bus
 *                                        clock from 30 MHz
 *                                        Not available with the APCIe-1711
 *                                      - APCI1710_33MHZ :
 *                                        The PC have a PCI bus
 *                                        clock from 33 MHz
 *                                        Not available with the APCIe-1711
 *                                      - APCI1710_40MHZ
 *                                        The APCI-1710 have a
 *                                        integrated 40Mhz quartz.
 * @param [in] b_TimingUnit           : Base timing unity (0 to 4)
 *                                        0 : ns
 *                                        1 : us
 *                                        2 : ms
 *                                        3 : s
 *                                        4 : mn
 * @param [in] ul_TimingInterval      : Base timing value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer mode selection is wrong.
 * @retval 5: The selected PCI input clock is wrong.
 * @retval 6: Timing unity selection is wrong.
 * @retval 7: Base timing selection is wrong.
 * @retval 8: You can not used the 40MHz clock selection wich this board.
 * @retval 9: You can not used the 40MHz clock selection wich this CHRONOS version.
 */
int i_APCI1710_InitChrono (struct pci_dev *pdev,
                           uint8_t b_ModulNbr,
                           uint8_t b_ChronoMode,
                           uint8_t b_PCIInputClock,
                           uint8_t b_TimingUnit,
                           uint32_t ul_TimingInterval)
{
	int    i_ReturnValue = 0;
	uint32_t ul_TimerValue = 0;
	uint32_t dw_ModeArray [8] = {0x01, 0x05, 0x00, 0x04, 0x02, 0x0E, 0x0A, 0x06};

	if (!pdev) return 1;

	/*******************/
	/* Test if chronos */
	/*******************/

	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_CHRONOMETER)
	{
		/**************************/
		/* Test the module number */
		/**************************/

		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
			/*****************************/
			/* Test the chronometer mode */
			/*****************************/

				if (b_ChronoMode <= 7)
				{
					/**************************/
					/* Test the PCI bus clock */
					/**************************/

					if (((b_PCIInputClock == APCI1710_30MHZ) && (pdev->device == apci1710_BOARD_DEVICE_ID)) ||
						((b_PCIInputClock == APCI1710_33MHZ) && (pdev->device == apci1710_BOARD_DEVICE_ID)) ||
						(b_PCIInputClock == APCI1710_40MHZ))
					{
						/*************************/
						/* Test the timing unity */
						/*************************/
						if (b_TimingUnit <= 4)
						{
							/**********************************/
							/* Test the base timing selection */
							/**********************************/

							if (((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 0) && (ul_TimingInterval >= 66) && (ul_TimingInterval <= 0xFFFFFFFFUL)) ||
							((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 1) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 143165576UL))  ||
							((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 2) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 143165UL))     ||
							((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 3) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 143UL))        ||
							((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 4) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 2UL))          ||
							((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 0) && (ul_TimingInterval >= 60) && (ul_TimingInterval <= 0xFFFFFFFFUL)) ||
							((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 1) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 130150240UL))  ||
							((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 2) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 130150UL))     ||
							((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 3) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 130UL))        ||
							((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 4) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 2UL))          ||
							((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 0) && (ul_TimingInterval >= 50) && (ul_TimingInterval <= 0xFFFFFFFFUL)) ||
							((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 1) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 107374182UL))  ||
							((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 2) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 107374UL))     ||
							((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 3) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 107UL))        ||
							((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 4) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 1UL)))
							{

								switch (b_TimingUnit)
								{
									/* ns */
									case 0:
									ul_TimerValue = (ul_TimingInterval * b_PCIInputClock) / 1000;
									break;

									/* us */
									case 1:
									ul_TimerValue = (ul_TimingInterval * b_PCIInputClock);
									break;

									/* ms */
									case 2:
									ul_TimerValue = (ul_TimingInterval * 1000 * b_PCIInputClock);
									break;

									/* s */
									case 3:
									ul_TimerValue = (ul_TimingInterval * 1000000 * b_PCIInputClock);
									break;

									/* mn */
									case 4:
									ul_TimerValue = (ul_TimingInterval * 60 * 1000000 * b_PCIInputClock);
									break;
								}

								if (ul_TimerValue > 1)
									ul_TimerValue -= 2;
								else
									ul_TimerValue = 0;

								if (b_PCIInputClock != APCI1710_40MHZ)
								{
									ul_TimerValue = (ul_TimerValue * 99392) / 100000;
								}

								/****************************/
								/* Save the PCI input clock */
								/****************************/

								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [(int)b_ModulNbr].
								s_ChronoModuleInfo.
								b_PCIInputClock = b_PCIInputClock;

								/*************************/
								/* Save the timing unity */
								/*************************/

								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [(int)b_ModulNbr].
								s_ChronoModuleInfo.
								b_TimingUnit = b_TimingUnit;

								/****************************/
								/* Set the chronometer mode */
								/****************************/

								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [(int)b_ModulNbr].
								s_ChronoModuleInfo.
								dw_ConfigReg = dw_ModeArray [b_ChronoMode];

								/***********************/
								/* Test if 40 MHz used */
								/***********************/

								if (b_PCIInputClock == APCI1710_40MHZ)
								{
									APCI1710_PRIVDATA(pdev)->
									s_ModuleInfo [(int)b_ModulNbr].
									s_ChronoModuleInfo.
									dw_ConfigReg =  APCI1710_PRIVDATA(pdev)->
													s_ModuleInfo [(int)b_ModulNbr].
													s_ChronoModuleInfo.
													dw_ConfigReg | 0x80;
								}

								OUTPDW (GET_BAR2(pdev),
								16 + MODULE_OFFSET(b_ModulNbr),
								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [(int)b_ModulNbr].
								s_ChronoModuleInfo.
								dw_ConfigReg);

								/***********************/
								/* Write timer 0 value */
								/***********************/

								OUTPDW (GET_BAR2(pdev),
								MODULE_OFFSET(b_ModulNbr),
								ul_TimerValue);

								/*********************/
								/* Chronometer init. */
								/*********************/

								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [(int)b_ModulNbr].
								s_ChronoModuleInfo.
								b_ChronoInit = 1;
							}
							else
							{
								/**********************************/
								/* Base timing selection is wrong */
								/**********************************/

								i_ReturnValue = 7;
							}
						}
						else
						{
							/***********************************/
							/* Timing unity selection is wrong */
							/***********************************/

							i_ReturnValue = 6;
						}
					}
					else
					{
						/*****************************************/
						/* The selected PCI input clock is wrong */
						/*****************************************/

						i_ReturnValue = 5;
					}
				}
				else
				{
					/***************************************/
					/* Chronometer mode selection is wrong */
					/***************************************/

					i_ReturnValue = 4;
				}
			}
			else
			{
				/***********************/
				/* Module number error */
				/***********************/

				i_ReturnValue = 2;
			}
		}
		else
		{
			/******************************************/
			/* The module is not a Chronometer module */
			/******************************************/

			i_ReturnValue = 3;
		}

	return (i_ReturnValue);
}

//------------------------------------------------------------------------------

/** Enable the chronometer.
 *
 * Enable the chronometer from selected module
 * (b_ModulNbr). You must calling the
 * "i_APCI1710_InitChrono" function be for you call this
 * function.
 * If you enable the chronometer interrupt, the
 * chronometer generate a interrupt after the stop signal.
 * See function "i_APCI1710_SetBoardIntRoutineX" and the
 * Interrupt mask description chapter from this manual.
 * The b_CycleMode parameter determine if you will
 * measured a single or more cycle.
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [in] b_ModulNbr             : Module number to configure (0 to 3).
 * @param [in] b_CycleMode            : Selected the chronometer acquisition mode.
 * @param [in] b_InterruptEnable      : Enable or disable the chronometer interrupt.
 *                                      APCI1710_ENABLE: Enable the chronometer interrupt.
 *                                      APCI1710_DISABLE:Disable the chronometer interrupt.
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval 5: Chronometer acquisition mode cycle is wrong.
 * @retval 6: Interrupt parameter is wrong.
 * @retval 7: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutineX".
 */
int i_APCI1710_EnableChrono (struct pci_dev *pdev,
							 uint8_t b_ModulNbr,
							 uint8_t b_CycleMode,
							 uint8_t b_InterruptEnable)
{
	int i_ReturnValue = 0;

	if (!pdev) return 1;

	/*******************/
	/* Test if chronos */
	/*******************/
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_CHRONOMETER)
	{
		/**************************/
		/* Test the module number */
		/**************************/
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
			/***********************************/
			/* Test if chronometer initialised */
			/***********************************/

			if (APCI1710_PRIVDATA(pdev)->
			s_ModuleInfo [(int)b_ModulNbr].
			s_ChronoModuleInfo.
			b_ChronoInit == 1)
			{
				/*********************************/
				/* Test the cycle mode parameter */
				/*********************************/

				if ((b_CycleMode == APCI1710_SINGLE) || (b_CycleMode == APCI1710_CONTINUOUS))
				{
					/***************************/
					/* Test the interrupt flag */
					/***************************/

					if ((b_InterruptEnable == APCI1710_ENABLE) || (b_InterruptEnable == APCI1710_DISABLE))
					{
						/**************************/
						/* Test if interrupt used */
						/**************************/

						if (b_InterruptEnable == APCI1710_ENABLE)
						{
							/******************************************/
							/* Test if interrupt function initialised */
							/******************************************/

							if ((APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality[b_ModulNbr].v_InterruptFunction == NULL))
							{
								/**************************************/
								/* Interrupt function not initialised */
								/**************************************/

								i_ReturnValue = 7;
							}
						}

						/***********************/
						/* Test if error occur */
						/***********************/

						if (!i_ReturnValue)
						{
							/***************************/
							/* Save the interrupt flag */
							/***************************/

							APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_ChronoModuleInfo.
							b_InterruptMask = b_InterruptEnable;

							/***********************/
							/* Save the cycle mode */
							/***********************/

							APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_ChronoModuleInfo.
							b_CycleMode = b_CycleMode;

							APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_ChronoModuleInfo.
							dw_ConfigReg =  (APCI1710_PRIVDATA(pdev)->
											s_ModuleInfo [(int)b_ModulNbr].
											s_ChronoModuleInfo.
											dw_ConfigReg & 0x8F)           |
											((1 & b_InterruptEnable) << 5) |
											((1 & b_CycleMode) << 6)       |
											0x10;

							/*****************************/
							/* Test if interrupt enabled */
							/*****************************/

							if (b_InterruptEnable == APCI1710_ENABLE)
							{
								/****************************/
								/* Clear the interrupt flag */
								/****************************/

								OUTPDW (GET_BAR2(pdev),
								32 + MODULE_OFFSET(b_ModulNbr),
								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [(int)b_ModulNbr].
								s_ChronoModuleInfo.
								dw_ConfigReg);
							}

							/***********************************/
							/* Enable or disable the interrupt */
							/* Enable the chronometer          */
							/***********************************/

							OUTPDW (GET_BAR2(pdev),
							16 + MODULE_OFFSET(b_ModulNbr),
							APCI1710_PRIVDATA(pdev)->
							s_ModuleInfo [(int)b_ModulNbr].
							s_ChronoModuleInfo.
							dw_ConfigReg);

							/*************************/
							/* Clear status register */
							/*************************/

							OUTPDW (GET_BAR2(pdev),
							36 + MODULE_OFFSET(b_ModulNbr),
							0);
						}
					}
					else
					{
						/********************************/
						/* Interrupt parameter is wrong */
						/********************************/

						i_ReturnValue = 6;
					}
				}
				else
				{
					/***********************************************/
					/* Chronometer acquisition mode cycle is wrong */
					/***********************************************/

					i_ReturnValue = 5;
				}
			}
			else
			{
				/*******************************/
				/* Chronometer not initialised */
				/*******************************/

				i_ReturnValue = 4;
			}
		}
		else
		{
			/***********************/
			/* Module number error */
			/***********************/

			i_ReturnValue = 2;
		}
	}
	else
	{
		/******************************************/
		/* The module is not a Chronometer module */
		/******************************************/

		i_ReturnValue = 3;
	}

	return (i_ReturnValue);
}

//------------------------------------------------------------------------------

/** Disable the chronometer.
 *
 * Disable the chronometer from selected module
 * (b_ModulNbr). If you disable the chronometer after a
 * start signal occur and you restart the chronometer
 * witch the " i_APCI1710_EnableChrono" function, if no
 * stop signal occur this start signal is ignored.
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [in] b_ModulNbr             : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 */
int i_APCI1710_DisableChrono (struct pci_dev *pdev,
							  uint8_t b_ModulNbr)
{
	int i_ReturnValue = 0;

	if (!pdev) return 1;

	/*******************/
	/* Test if chronos */
	/*******************/
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_CHRONOMETER)
	{
		/**************************/
		/* Test the module number */
		/**************************/
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
			/***********************************/
			/* Test if chronometer initialised */
			/***********************************/

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				b_ChronoInit == 1)
			{
				APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				b_InterruptMask = 0;

				//ES 19.12.02:
				//changing dw_ConfigReg & 0x2F to dw_ConfigReg & 0xAF
				//cause the "DisableChrono" resets the 40Mhz selection
				APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				dw_ConfigReg = 	APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [(int)b_ModulNbr].
								s_ChronoModuleInfo.
								dw_ConfigReg & 0xAF;

				/***************************/
				/* Disable the interrupt   */
				/* Disable the chronometer */
				/***************************/

				OUTPDW (GET_BAR2(pdev),
				16 + MODULE_OFFSET(b_ModulNbr),
				APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				dw_ConfigReg);

				/***************************/
				/* Test if continuous mode */
				/***************************/

				if (APCI1710_PRIVDATA(pdev)->
					s_ModuleInfo [(int)b_ModulNbr].
					s_ChronoModuleInfo.
					b_CycleMode == APCI1710_CONTINUOUS)
				{
					/*************************/
					/* Clear status register */
					/*************************/

					OUTPDW (GET_BAR2(pdev),
					36 + MODULE_OFFSET(b_ModulNbr),
					0);
				}
			}
			else
			{
				/*******************************/
				/* Chronometer not initialised */
				/*******************************/

				i_ReturnValue = 4;
			}
		}
		else
		{
			/***********************/
			/* Module number error */
			/***********************/

			i_ReturnValue = 2;
		}
	}
	else
	{
		/******************************************/
		/* The module is not a Chronometer module */
		/******************************************/

		i_ReturnValue = 3;
	}

	return (i_ReturnValue);
}

//------------------------------------------------------------------------------

/** Get the chronometer status.
 *
 * Return the chronometer status (pb_ChronoStatus) from
 * selected chronometer module (b_ModulNbr).
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [in] b_ModulNbr             : Module number to configure (0 to 3).
 *
 * @param [out] pb_ChronoStatus : Return the chronometer status.
 *                                0 : Measurement not started.
 *                                    No start signal occur.
 *                                1 : Measurement started.
 *                                    A start signal occur.
 *                                2 : Measurement stopped.
 *                                    A stop signal occur.
 *                                    The measurement is terminate.
 *                                3: A overflow occur. You must change the base
 *                                   timing witch the function "i_APCI1710_InitChrono"
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 */
int i_APCI1710_GetChronoProgressStatus (struct pci_dev *pdev,
										uint8_t b_ModulNbr,
										uint8_t  *pb_ChronoStatus)
{
	int i_ReturnValue = 0;
	uint32_t dw_Status = 0;

	if (!pdev) return 1;

	/*******************/
	/* Test if chronos */
	/*******************/
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_CHRONOMETER)
	{
		/**************************/
		/* Test the module number */
		/**************************/
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
			/***********************************/
			/* Test if chronometer initialised */
			/***********************************/

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				b_ChronoInit == 1)
			{

				INPDW (GET_BAR2(pdev), 8 + MODULE_OFFSET(b_ModulNbr), &dw_Status);

				/********************/
				/* Test if overflow */
				/********************/

				if ((dw_Status & 8) == 8)
				{
					/******************/
					/* Overflow occur */
					/******************/

					*pb_ChronoStatus = 3;
				}
				else
				{
					/*******************************/
					/* Test if measurement stopped */
					/*******************************/

					if ((dw_Status & 2) == 2)
					{
						/***********************/
						/* A stop signal occur */
						/***********************/

						*pb_ChronoStatus = 2;
					}
					else
					{
						/*******************************/
						/* Test if measurement started */
						/*******************************/

						if ((dw_Status & 1) == 1)
						{
							/************************/
							/* A start signal occur */
							/************************/

							*pb_ChronoStatus = 1;
						}
						else
						{
							/***************************/
							/* Measurement not started */
							/***************************/

							*pb_ChronoStatus = 0;
						}
					}
				}
			}
			else
			{
				/*******************************/
				/* Chronometer not initialised */
				/*******************************/

				i_ReturnValue = 4;
			}
		}
		else
		{
			/***********************/
			/* Module number error */
			/***********************/

			i_ReturnValue = 2;
		}
	}
	else
	{
		/******************************************/
		/* The module is not a Chronometer module */
		/******************************************/

		i_ReturnValue = 3;
	}

	return (i_ReturnValue);
}

//------------------------------------------------------------------------------

/** Read the chronometer value.
 *
 * Return the chronometer status (pb_ChronoStatus) and the
 * timing value (pul_ChronoValue) after a stop signal
 * occur from selected chronometer module (b_ModulNbr).
 * This function are only avaible if you have disabled
 * the interrupt functionality. See function
 * "i_APCI1710_EnableChrono" and the Interrupt mask
 * description chapter.
 * You can test the chronometer status witch the
 * "i_APCI1710_GetChronoProgressStatus" function.
 *
 * The returned value from pul_ChronoValue parameter is
 * not real measured timing.
 * You must used the "i_APCI1710_ConvertChronoValue"
 * function or make this operation for calculate the
 * timing:
 *
 * Timing = pul_ChronoValue * pul_RealTimingInterval.
 *
 * pul_RealTimingInterval is the returned parameter from
 * "i_APCI1710_InitChrono" function and the time unity is
 * the b_TimingUnit from "i_APCI1710_InitChrono" function
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [in] b_ModulNbr             : Module number to configure (0 to 3).
 * @param [in] ul_TimeOut
 *
 * @param [out] pb_ChronoStatus : Return the chronometer status.
 *                                0 : Measurement not started.
 *                                    No start signal occur.
 *                                1 : Measurement started.
 *                                    A start signal occur.
 *                                2 : Measurement stopped.
 *                                    A stop signal occur.
 *                                    The measurement is terminate.
 *                                3: A overflow occur. You must change the base
 *                                   timing witch the function "i_APCI1710_InitChrono"
 *
 * @param [out] pul_ChronoValue : Chronometer timing value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval 5: Timeout parameter is wrong (0 to 65535).
 * @retval 6: Interrupt routine installed.
 *            You can not read directly the chronometer measured timing.
 */
int i_APCI1710_ReadChronoValue (struct pci_dev *pdev,
								uint8_t b_ModulNbr,
								uint32_t ul_TimeOut,
								uint8_t *pb_ChronoStatus,
								uint32_t *pul_ChronoValue)
{
	int i_ReturnValue = 0;
	uint32_t dw_Status = 0;
	uint32_t dw_TimeOut = 0;

	if (!pdev) return 1;

	/*******************/
	/* Test if chronos */
	/*******************/
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_CHRONOMETER)
	{
		/**************************/
		/* Test the module number */
		/**************************/
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
			/***********************************/
			/* Test if chronometer initialised */
			/***********************************/

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				b_ChronoInit == 1)
			{
				/*****************************/
				/* Test the timout parameter */
				/*****************************/

				if ((ul_TimeOut >= 0) && (ul_TimeOut <= 65535UL))
				{
					/***************************************/
					/* Test if interrupt routine installed */
					/***************************************/

					if ((APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality[b_ModulNbr].v_InterruptFunction == NULL))
					{
						for (;;)
						{
							/*******************/
							/* Read the status */
							/*******************/
							INPDW (GET_BAR2(pdev), 8 + MODULE_OFFSET(b_ModulNbr), &dw_Status);

							/********************/
							/* Test if overflow */
							/********************/

							if ((dw_Status & 8) == 8)
							{
								/******************/
								/* Overflow occur */
								/******************/

								*pb_ChronoStatus = 3;

								/***************************/
								/* Test if continuous mode */
								/***************************/

								if (APCI1710_PRIVDATA(pdev)->
										s_ModuleInfo [(int)b_ModulNbr].
										s_ChronoModuleInfo.
										b_CycleMode == APCI1710_CONTINUOUS)
								{
									/*************************/
									/* Clear status register */
									/*************************/

									OUTPDW (GET_BAR2(pdev),
									36 + MODULE_OFFSET(b_ModulNbr),
									0);
								}

								break;
							}
							else
							{
								/*******************************/
								/* Test if measurement stopped */
								/*******************************/

								if ((dw_Status & 2) == 2)
								{
									/***********************/
									/* A stop signal occur */
									/***********************/

									*pb_ChronoStatus = 2;

									/***************************/
									/* Test if continnous mode */
									/***************************/

									if (APCI1710_PRIVDATA(pdev)->
										s_ModuleInfo [(int)b_ModulNbr].
										s_ChronoModuleInfo.
										b_CycleMode == APCI1710_CONTINUOUS)
									{
										/*************************/
										/* Clear status register */
										/*************************/

										OUTPDW (GET_BAR2(pdev),
										36 + MODULE_OFFSET(b_ModulNbr),
										0);
									}
									break;
								}
								else
								{
									/*******************************/
									/* Test if measurement started */
									/*******************************/

									if ((dw_Status & 1) == 1)
									{
										/************************/
										/* A start signal occur */
										/************************/

										*pb_ChronoStatus = 1;
									}
									else
									{
										/***************************/
										/* Measurement not started */
										/***************************/

										*pb_ChronoStatus = 0;
									}
								}
							}

							if (dw_TimeOut == ul_TimeOut)
							{
								/*****************/
								/* Timeout occur */
								/*****************/

								break;
							}
							else
							{
								/*************************/
								/* Increment the timeout */
								/*************************/

								dw_TimeOut = dw_TimeOut + 1;
							}
						}

						/*****************************/
						/* Test if stop signal occur */
						/*****************************/

						if (*pb_ChronoStatus == 2)
						{
							/**********************************/
							/* Read the measured timing value */
							/**********************************/

							INPDW (GET_BAR2(pdev), 4 + MODULE_OFFSET(b_ModulNbr), pul_ChronoValue);

							if (*pul_ChronoValue != 0)
							{
								*pul_ChronoValue = *pul_ChronoValue - 1;
							}
						}
						else
						{
							/*************************/
							/* Test if timeout occur */
							/*************************/

							if ((*pb_ChronoStatus != 3) && (dw_TimeOut == ul_TimeOut) && (ul_TimeOut != 0))
							{
								/*****************/
								/* Timeout occur */
								/*****************/

								*pb_ChronoStatus = 4;
							}
						}
					}
					else
					{
						/*******************************/
						/* Interrupt routine installed */
						/*******************************/

						i_ReturnValue = 6;
					}
				}
				else
				{
					/******************************/
					/* Timeout parameter is wrong */
					/******************************/

					i_ReturnValue = 5;
				}
			}
			else
			{
				/*******************************/
				/* Chronometer not initialised */
				/*******************************/

				i_ReturnValue = 4;
			}
		}
		else
		{
			/***********************/
			/* Module number error */
			/***********************/

			i_ReturnValue = 2;
		}
	}
	else
	{
		/******************************************/
		/* The module is not a Chronometer module */
		/******************************************/

		i_ReturnValue = 3;
	}

	return (i_ReturnValue);
}

//------------------------------------------------------------------------------

/** Set the selected chronometer channel on.
 *
 * Sets the output witch has been passed with the
 * parameter b_Channel. Setting an output means setting an
 * output high.
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [in] b_ModulNbr             : Module number to configure (0 to 3).
 * @param [in] b_OutputChannel        : Selection from digital output
 *                                      channel (0 to 2)
 *                                      0 : Channel H
 *                                      1 : Channel A
 *                                      2 : Channel B
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: Timeout parameter is wrong (0 to 65535).
 */
int i_APCI1710_SetChronoChlOn (struct pci_dev *pdev,
								uint8_t b_ModulNbr,
								uint8_t b_OutputChannel)
{
	int i_ReturnValue = 0;

	if (!pdev) return 1;

	/*******************/
	/* Test if chronos */
	/*******************/
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_CHRONOMETER)
	{
		/**************************/
		/* Test the module number */
		/**************************/
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
			/***********************************/
			/* Test if chronometer initialised */
			/***********************************/

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				b_ChronoInit == 1)
			{
				/***********************************/
				/* Test the digital output channel */
				/***********************************/

				if (b_OutputChannel <= 2)
				{
					OUTPDW (GET_BAR2(pdev),
					20 + (b_OutputChannel * 4) + MODULE_OFFSET(b_ModulNbr),
					1);

				}
				else
				{
					/****************************************/
					/* The selected digital output is wrong */
					/****************************************/

					i_ReturnValue = 4;
				}
			}
			else
			{
				/*******************************/
				/* Chronometer not initialised */
				/*******************************/

				i_ReturnValue = 5;
			}
		}
		else
		{
			/***********************/
			/* Module number error */
			/***********************/

			i_ReturnValue = 2;
		}
	}
	else
	{
		/******************************************/
		/* The module is not a Chronometer module */
		/******************************************/

		i_ReturnValue = 3;
	}

	return (i_ReturnValue);
}

//------------------------------------------------------------------------------

/** Set the selected chronometer channel off.
 *
 * Reset the output witch has been passed with the
 * parameter b_Channel. Setting an output means setting an
 * output high.
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [in] b_ModulNbr             : Module number to configure (0 to 3).
 * @param [in] b_OutputChannel        : Selection from digital output
 *                                      channel (0 to 2)
 *                                      0 : Channel H
 *                                      1 : Channel A
 *                                      2 : Channel B
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: Chronometer not initialised see function "i_APCI1710_InitChrono".
 */
int i_APCI1710_SetChronoChlOff (struct pci_dev *pdev,
								uint8_t b_ModulNbr,
								uint8_t b_OutputChannel)
{
	int i_ReturnValue = 0;

	if (!pdev) return 1;

	/*******************/
	/* Test if chronos */
	/*******************/
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_CHRONOMETER)
	{
		/**************************/
		/* Test the module number */
		/**************************/
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
			/***********************************/
			/* Test if chronometer initialised */
			/***********************************/

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				b_ChronoInit == 1)
			{
				/***********************************/
				/* Test the digital output channel */
				/***********************************/

				if (b_OutputChannel <= 2)
				{
					OUTPDW (GET_BAR2(pdev),
					20 + (b_OutputChannel * 4) + MODULE_OFFSET(b_ModulNbr),
					0);

				}
				else
				{
					/****************************************/
					/* The selected digital output is wrong */
					/****************************************/

					i_ReturnValue = 4;
				}
			}
			else
			{
				/*******************************/
				/* Chronometer not initialised */
				/*******************************/

				i_ReturnValue = 5;
			}
		}
		else
		{
			/***********************/
			/* Module number error */
			/***********************/

			i_ReturnValue = 2;
		}
	}
	else
	{
		/******************************************/
		/* The module is not a Chronometer module */
		/******************************************/

		i_ReturnValue = 3;
	}

	return (i_ReturnValue);
}

//------------------------------------------------------------------------------

/** Read the selected chronometer channel value.
 *
 * Return the status from selected digital input
 * (b_InputChannel) from selected chronometer
 * module (b_ModulNbr).
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [in] b_ModulNbr             : Module number to configure (0 to 3).
 * @param [in] b_InputChannel         : Selection from digital output
 *                                      channel (0 to 2)
 *                                      0 : Channel E
 *                                      1 : Channel F
 *                                      2 : Channel G
 *
 * @param [out] pb_ChannelStatus      : Digital input channel status.
 *                                      0 : Channel is not active
 *                                      1 : Channel is active
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: The selected digital input is wrong.
 * @retval 5: Chronometer not initialised see function "i_APCI1710_InitChrono".
 */
int i_APCI1710_ReadChronoChlValue (struct pci_dev *pdev,
								   uint8_t b_ModulNbr,
								   uint8_t b_InputChannel,
								   uint8_t *pb_ChannelStatus)
{
	int i_ReturnValue = 0;
	uint32_t dw_Status = 0;

	if (!pdev) return 1;

	/*******************/
	/* Test if chronos */
	/*******************/
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_CHRONOMETER)
	{
		/**************************/
		/* Test the module number */
		/**************************/
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
			/***********************************/
			/* Test if chronometer initialised */
			/***********************************/

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				b_ChronoInit == 1)
			{
				/**********************************/
				/* Test the digital input channel */
				/**********************************/

				if (b_InputChannel <= 2)
				{
					INPDW (GET_BAR2(pdev), 12 + MODULE_OFFSET(b_ModulNbr), &dw_Status);

					*pb_ChannelStatus = (uint8_t) (((dw_Status >> b_InputChannel) & 1) ^ 1);
				}
				else
				{
					/***************************************/
					/* The selected digital input is wrong */
					/***************************************/

					i_ReturnValue = 4;
				}
			}
			else
			{
				/*******************************/
				/* Chronometer not initialised */
				/*******************************/

				i_ReturnValue = 5;
			}
		}
		else
		{
			/***********************/
			/* Module number error */
			/***********************/

			i_ReturnValue = 2;
		}
	}
	else
	{
		/******************************************/
		/* The module is not a Chronometer module */
		/******************************************/

		i_ReturnValue = 3;
	}

	return (i_ReturnValue);
}

//------------------------------------------------------------------------------

/** Read the selected chronometer port value.
 *
 * Return the status from digital inputs port from
 * selected  (b_ModulNbr) chronometer module.
 *
 * @param [in] pdev                   : The device to initialize.
 * @param [in] b_ModulNbr             : Module number to configure (0 to 3).
 *
 * @param [out] pb_PortValue      : Digital inputs port status.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 */
int i_APCI1710_ReadChronoPortValue (struct pci_dev *pdev,
								   uint8_t b_ModulNbr,
								   uint8_t *pb_PortValue)
{
	int i_ReturnValue = 0;
	uint32_t dw_Status = 0;

	if (!pdev) return 1;

	/*******************/
	/* Test if chronos */
	/*******************/
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_CHRONOMETER)
	{
		/**************************/
		/* Test the module number */
		/**************************/
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
			/***********************************/
			/* Test if chronometer initialised */
			/***********************************/

			if (APCI1710_PRIVDATA(pdev)->
				s_ModuleInfo [(int)b_ModulNbr].
				s_ChronoModuleInfo.
				b_ChronoInit == 1)
			{
				INPDW (GET_BAR2(pdev), 12 + MODULE_OFFSET(b_ModulNbr), &dw_Status);

				*pb_PortValue = (uint8_t) ((dw_Status & 0x7) ^ 7);
			}
			else
			{
				/*******************************/
				/* Chronometer not initialised */
				/*******************************/

				i_ReturnValue = 4;
			}
		}
		else
		{
			/***********************/
			/* Module number error */
			/***********************/

			i_ReturnValue = 2;
		}
	}
	else
	{
		/******************************************/
		/* The module is not a Chronometer module */
		/******************************************/

		i_ReturnValue = 3;
	}

	return (i_ReturnValue);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


