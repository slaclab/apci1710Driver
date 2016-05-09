/** @file etm-kapi.c

   Contains ETM kernel functions.

   @par CREATION
   @author Krauth Julien
   @date   23.08.11

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

EXPORT_SYMBOL(i_APCI1710_InitETM);
EXPORT_SYMBOL(i_APCI1710_EnableETM);
EXPORT_SYMBOL(i_APCI1710_DisableETM);
EXPORT_SYMBOL(i_APCI1710_GetETMProgressStatus);
EXPORT_SYMBOL(i_APCI1710_ReadETMValue);
EXPORT_SYMBOL(i_APCI1710_ReadETMTotalTime);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/* Initialize ETM functionality.
 *
 * @param [in] pdev                  : The device to use.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_ClockSelection      : Selection from PCI bus clock
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
 * @param [in] ul_Timing              : Base timing value.
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: Module selection wrong
 * @retval 3: The module is not a ETM module
 * @retval 4: The selected input clock is wrong
 * @retval 5: Timing Unit selection is wrong
 * @retval 6: Base timing selection is wrong
 * @retval 7: You can not used the 40MHz clock selection with this board
 */
int i_APCI1710_InitETM (struct pci_dev *pdev,
						uint8_t  b_ModulNbr,
						uint8_t  b_ClockSelection,
						uint8_t  b_TimingUnit,
						uint32_t ul_Timing)
{
	int i_ReturnValue = 0;
	uint32_t ul_TimerValue = 0;

	if (!pdev) return 1;

	/* Test the module number */
	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	{
		/* Test if ETM */
		if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_ETM)
		{
			/* Test the PCI bus clock */
			if ((b_ClockSelection == APCI1710_30MHZ) || (b_ClockSelection == APCI1710_33MHZ) || (b_ClockSelection == APCI1710_40MHZ))
			{
				/* Test the timing unity */
				if (b_TimingUnit <= 2)
				{
					/* Test the base timing selection */
					if (((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 0) && (ul_Timing >= 33) && (ul_Timing <= 559240500UL)) ||
					((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 1) && (ul_Timing >= 1)  && (ul_Timing <= 559240UL))    ||
					((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 2) && (ul_Timing >= 1)  && (ul_Timing <= 559UL))       ||
					((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 0) && (ul_Timing >= 30) && (ul_Timing <= 508400454UL)) ||
					((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 1) && (ul_Timing >= 1)  && (ul_Timing <= 508400UL))    ||
					((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 2) && (ul_Timing >= 1)  && (ul_Timing <= 508UL))       ||
					((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 0) && (ul_Timing >= 25) && (ul_Timing <= 419430375UL)) ||
					((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 1) && (ul_Timing >= 1)  && (ul_Timing <= 419430UL))    ||
					((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 2) && (ul_Timing >= 1)  && (ul_Timing <= 419UL)))
					{
						/* Calculate the division fator */
						switch (b_TimingUnit)
						{
							/* ns */
							case 0:
								ul_TimerValue = (ul_Timing * b_ClockSelection) / 1000;
							break;

							/* us */
							case 1:
							ul_TimerValue = (ul_Timing * b_ClockSelection);
							break;

							/* ms */
							case 2:
							ul_TimerValue = (ul_Timing * 1000 * b_ClockSelection);
							break;
						}

						/* Save the PCI input clock */
						APCI1710_PRIVDATA(pdev)->
						s_ModuleInfo [(int)b_ModulNbr].
						s_ETMModuleInfo.
						b_ClockSelection = b_ClockSelection;

						/* Save the timing unity */
						APCI1710_PRIVDATA(pdev)->
						s_ModuleInfo [(int)b_ModulNbr].
						s_ETMModuleInfo.
						b_TimingUnit = b_TimingUnit;

						/* Save the base timing */
						APCI1710_PRIVDATA(pdev)->
						s_ModuleInfo [(int)b_ModulNbr].
						s_ETMModuleInfo.
						ul_Timing = ul_Timing;

						OUTPDW (GET_BAR2(pdev),
						4 + MODULE_OFFSET(b_ModulNbr),
						0x10);

						/* Write the division factor */
						OUTPDW (GET_BAR2(pdev),
						MODULE_OFFSET(b_ModulNbr),
						ul_TimerValue);

						/* Test if 40MHz used */
						if (b_ClockSelection == APCI1710_40MHZ)
						{
							/* Set the 40 MHz clock */
							OUTPDW (GET_BAR2(pdev),
							4 + MODULE_OFFSET(b_ModulNbr),
							1);
						}
						else
						{
							/* Reset the 40 MHz clock */
							OUTPDW (GET_BAR2(pdev),
							4 + MODULE_OFFSET(b_ModulNbr),
							0);
						}

						/* ETM initialised */
						APCI1710_PRIVDATA(pdev)->
						s_ModuleInfo [(int)b_ModulNbr].
						s_ETMModuleInfo.
						b_ETMInit = APCI1710_ENABLE;
					}
					else
					{
						/* Base timing selection is wrong */
						i_ReturnValue = 6;
					}
				}
				else
				{
					/* Timing Unit selection is wrong */
					i_ReturnValue = 5;
				}
			}
			else
			{
				/* The selected input clock is wrong */
				i_ReturnValue = 4;
			}
		}
		else
		{
			/* The module is not a ETM module */
			i_ReturnValue = 3;
		}
	}
	else
	{
		/* Module selection wrong */
		i_ReturnValue = 2;
	}

return (i_ReturnValue);
}

//------------------------------------------------------------------------------

/* Enable ETM functionality.
 *
 * Enable the selected ETM (b_ETM) of the selected
 * module (b_ModulNbr). First call the function
 * "i_APCI1710_InitETM" before you call this function.
 * If you enable the ETM interrupt, the ETM generates a
 * interrupt after each trigger signal.
 * See function "i_APCI1710_SetBoardIntRoutineX" and the
 * chapter interrupt mask description in this manual.
 *
 * @param [in] pdev                  : The device to use.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_ETM		         : Selected ETM (0 or 1).
 * @param [in] b_EdgeLevel           : Selection from edge to
 *                                     measure the time
 *                                     0 : Measure the low level time
 *                                     1 : Measure the high level time
 * @param [in] b_TriggerLevel        : Selection from triggerlevel
 *                                     0 : The ETM trigger which a low level
 *                                     1 : The ETM trigger which a high level
 * @param [in] b_CycleMode	         : Selection from mode
 *                                     0 : Single mode
 *                                     1 : Continuous mode.
 *                                         Each trigger stop the measurement a start a
 *                                         new measurement cycle
 * @param [in] b_FirstTriggerMode    : First trigger mode
 *                                     0 : The first edge time measurement start
 *                                         after the calling from "i_APCI1710_EnableETM"
 *                                     1 : The first edge time measurement start after the next
 *                                         trigger signal
 * @param [in] b_InterruptEnable      : Enable or disable the ETM interrupt.
 *                                      APCI1710_ENABLE: Enable the ETM interrupt
 *                                      An interrupt occurs after trigger
 *                                      APCI1710_DISABLE: Disable the ETM interrupt
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: Module selection wrong
 * @retval 3: The module is not a ETM module
 * @retval 4: ETM selection is wrong
 * @retval 5: ETM not initialised, see function "i_APCI1710_InitETM"
 * @retval 6: Edge level selection is wrong
 * @retval 7: Trigger level selection is wrong
 * @retval 8: Mode selection is wrong
 * @retval 9: First trigger mode selection is wrong
 * @retval 10: Interrupt parameter is wrong
 * @retval 11: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutineX"
 */
int	i_APCI1710_EnableETM	(struct pci_dev *pdev,
		uint8_t		 b_ModulNbr,
		uint8_t		 b_ETM,
		uint8_t		 b_EdgeLevel,
		uint8_t		 b_TriggerLevel,
		uint8_t		 b_CycleMode,
		uint8_t		 b_FirstTriggerMode,
		uint8_t		 b_InterruptEnable)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_Status         = 0;
	uint32_t dw_Initialisation = 0;

	if (!pdev) return 1;

	/* Test if ETM */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_ETM)
	{
		/* Test the module number */
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
	      /* Test the ETM selection */
	      if (b_ETM <= 1)
		 {
		 /* Get ETM initialisation */
		 INPDW (GET_BAR2(pdev),
			4 + MODULE_OFFSET(b_ModulNbr),
			&dw_Status);

		 /* Test if module initialised */
		 if ((dw_Status & 2) == 2)
		    {
		    /* Test the edge level selection */
		    if ((b_EdgeLevel == 0) || (b_EdgeLevel == 1))
		       {
		       /* Test the trigger level selection */
		       if ((b_TriggerLevel == 0) || (b_TriggerLevel == 1))
			  {
			  /* Test the mode */
			  if ((b_CycleMode == 0) || (b_CycleMode == 1))
			     {
			     /* Test te first trigger mode */
			     if ((b_FirstTriggerMode == 0) || (b_FirstTriggerMode == 1))
				{
				/* Test the interrupt mode */
				if ((b_InterruptEnable == APCI1710_ENABLE) ||
				    (b_InterruptEnable == APCI1710_DISABLE))
				   {
				   /* Test if interrupt routine installed */
				   if ((b_InterruptEnable == APCI1710_DISABLE) ||
				       ((b_InterruptEnable == APCI1710_ENABLE) && (APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality[b_ModulNbr].v_InterruptFunction != NULL)))
				      {
				      /* Clear the last ETM Initalisation */
				      OUTPDW (GET_BAR2(pdev),
					      8 + MODULE_OFFSET(b_ModulNbr) + (b_ETM * 16), 0);

				      /* Set the new initialisation */
				      dw_Initialisation = (b_EdgeLevel        << 0) |
							  (b_TriggerLevel     << 1) |
							  (b_CycleMode        << 2) |
							  (b_FirstTriggerMode << 3) |
							  (b_InterruptEnable  << 4) |
							  (1                  << 5);

				      /* Save the interrupt mode */
				      APCI1710_PRIVDATA(pdev)->
				      								s_ModuleInfo [(int)b_ModulNbr].
				      s_ETMModuleInfo.
				      s_ETMInfo [b_ETM].
				      b_ETMInterrupt = b_InterruptEnable;

				      APCI1710_PRIVDATA(pdev)->
				      								s_ModuleInfo [(int)b_ModulNbr].
				      s_ETMModuleInfo.
				      s_ETMInfo [b_ETM].
				      b_ETMEnable = APCI1710_ENABLE;


				      /* Start the ETM */
				      OUTPDW (GET_BAR2(pdev),
					      8 + MODULE_OFFSET(b_ModulNbr) + (b_ETM * 16), dw_Initialisation);
				      }
				   else
				      {
				      /* Interrupt function not initialised */
				      i_ReturnValue = 11;
				      }
				   } // if ((b_InterruptEnable == APCI1710_ENABLE) || (b_InterruptEnable == APCI1710_DISABLE))
				else
				   {
				   /* Interrupt parameter is wrong */
				   i_ReturnValue = 10;
				   } // if ((b_InterruptEnable == APCI1710_ENABLE) || (b_InterruptEnable == APCI1710_DISABLE))
				}
			     else
				{
				/* First trigger mode selection is wrong */
				i_ReturnValue = 9;
				}
			     } // if ((b_FirstTriggerMode == 0) || ((b_FirstTriggerMode == 1))
			  else
			     {
			     /* Mode selection is wrong */
			     i_ReturnValue = 8;
			     } // if ((b_FirstTriggerMode == 0) || ((b_FirstTriggerMode == 1))

			  } // if ((b_TriggerLevel == 0) || (b_TriggerLevel == 1))
		       else
			  {
			  /* Trigger level selection is wrong */
			  i_ReturnValue = 7;
			  } // if ((b_TriggerLevel == 0) || (b_TriggerLevel == 1))
		       } // if ((b_EdgeLevel == 0) || (b_EdgeLevel == 1))
		    else
		       {
		       /* Edge level selection is wrong */
		       i_ReturnValue = 6;
		       } // if ((b_EdgeLevel == 0) || (b_EdgeLevel == 1))
		    }
		 else
		    {
		    /* ETM not initialised */
		    i_ReturnValue = 5;
		    } // if ((dw_Status & 2) == 2)
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      else
		 {
		 /* ETM selection is wrong */
		 i_ReturnValue = 4;
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      } // if (... == APCI1710_ETM)
	   else
	      {
	      /* The module is not a ETM module */
	      i_ReturnValue = 3;
	      } // if (... == APCI1710_ETM)
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)
	else
	   {
	   /* Module selection wrong */
	   i_ReturnValue = 2;
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/* Disable ETM functionality.
 *
 * @param [in] pdev                  : The device to use.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_ETM		         : Selected ETM (0 or 1).
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: Module selection wrong
 * @retval 3: The module is not a ETM module
 * @retval 4: ETM selection is wrong
 * @retval 5: ETM not initialised, see function "i_APCI1710_InitETM"
 */
int i_APCI1710_DisableETM	(struct pci_dev *pdev,
		uint8_t  b_ModulNbr,
		uint8_t  b_ETM)
	{
	int i_ReturnValue = 0;
	uint32_t dw_Status         = 0;
	uint32_t dw_Initialisation = 0;

	if (!pdev) return 1;

	/* Test if ETM */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_ETM)
	{
		/* Test the module number */
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
	      /* Test the ETM selection */
	      if (b_ETM <= 1)
		 {
		 /* Get ETM initialisation */
		 INPDW (GET_BAR2(pdev),
			4 + MODULE_OFFSET(b_ModulNbr),
			&dw_Status);

		 /* Test if module initialised */
		 if ((dw_Status & 2) == 2)
		    {
		    /* Get the ETM Initialisation */
		    INPDW (GET_BAR2(pdev),
			   8 + MODULE_OFFSET(b_ModulNbr) + (b_ETM * 16),
			   &dw_Initialisation);

		    /* Disable the ETM */
		    OUTPDW (GET_BAR2(pdev),
			    8 + MODULE_OFFSET(b_ModulNbr) + (b_ETM * 16),
			    dw_Initialisation & 0xFFFFFFDFUL);

		    APCI1710_PRIVDATA(pdev)->
		    				s_ModuleInfo [(int)b_ModulNbr].
		    s_ETMModuleInfo.
		    s_ETMInfo [b_ETM].
		    b_ETMEnable = APCI1710_DISABLE;
		    } // if ((dw_Status & 2) == 2)
		 else
		    {
		    /* ETM not initialised */
		    i_ReturnValue = 5;
		    } // if ((dw_Status & 2) == 2)
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      else
		 {
		 /* ETM selection is wrong */
		 i_ReturnValue = 4;
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      } // if (... == APCI1710_ETM)
	   else
	      {
	      /* The module is not a ETM module */
	      i_ReturnValue = 3;
	      } // if (... == APCI1710_ETM)
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)
	else
	   {
	   /* Module selection wrong */
	   i_ReturnValue = 2;
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/* Get ETM status.
 *
 * @param [in] pdev                  : The device to use.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_ETM		         : Selected ETM (0 or 1).
 *
 * @param [out] pb_ETMStatus          : Returns the ETM status.
 *                                     0: Measurement not started. No start trigger signal occurs.                    |
 *                                     1: Measurement started. A start trigger signal occurs.
 *                                     2: Measurement stopped. A stop trigger signal occurs. The measurement is ended.
 *                                     3: An overflow occurs. You must change the base time with the function "i_APCI1710_InitETM"
 *                                     4: A timeout occurred.
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: Module selection wrong
 * @retval 3: The module is not a ETM module
 * @retval 4: ETM selection is wrong
 * @retval 5: ETM not initialised, see function "i_APCI1710_InitETM"
 */
int	i_APCI1710_GetETMProgressStatus	(struct pci_dev *pdev,
		uint8_t    b_ModulNbr,
		uint8_t	  b_ETM,
		uint8_t	 *pb_ETMStatus)
	{
	int  i_ReturnValue 	= 0;
	uint32_t dw_Status         = 0;
	uint32_t dw_ProgressStatus = 0;

	if (!pdev) return 1;

	/* Test if ETM */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_ETM)
	{
		/* Test the module number */
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
	      /* Test the ETM selection */
	      if (b_ETM <= 1)
		 {
		 /* Get ETM initialisation */
		 INPDW (GET_BAR2(pdev),
			4 + MODULE_OFFSET(b_ModulNbr),
			&dw_Status);

		 /* Test if module initialised */
		 if ((dw_Status & 2) == 2)
		    {
		    /* Get the ETM Progress status */
		    INPDW (GET_BAR2(pdev),
			   20 + MODULE_OFFSET(b_ModulNbr) + (b_ETM * 16),
			   &dw_ProgressStatus);

		    /* Test if start */
		    if (dw_ProgressStatus & 1)
		       {
		       /* Test if overflow */
		       if (dw_ProgressStatus & 4)
			  {
			  /* ETM overflow */
			  *pb_ETMStatus = 3;
			  } // if (dw_ProgressStatus & 4)
		       else
			  {
			  /* Test if stop */
			  if (dw_ProgressStatus & 2)
			     {
			     /* ETM stoped */
			     *pb_ETMStatus = 2;
			     } // if (dw_ProgressStatus & 2)
			  else
			     {
			     /* ETM started */
			     *pb_ETMStatus = 1;
			     } // if (dw_ProgressStatus & 2)
			  } // if (dw_ProgressStatus & 4)
		       } // if (dw_ProgressStatus & 1)
		    else
		       {
		       /* ETM not started */
		       *pb_ETMStatus = 0;
		       } // if (dw_ProgressStatus & 1)
		    } // if ((dw_Status & 2) == 2)
		 else
		    {
		    /* ETM not initialised */
		    i_ReturnValue = 5;
		    } // if ((dw_Status & 2) == 2)
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      else
		 {
		 /* ETM selection is wrong */
		 i_ReturnValue = 4;
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      } // if (... == APCI1710_ETM)
	   else
	      {
	      /* The module is not a ETM module */
	      i_ReturnValue = 3;
	      } // if (... == APCI1710_ETM)
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)
	else
	   {
	   /* Module selection wrong */
	   i_ReturnValue = 2;
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/* Read ETM value.
 *
 * Returns the ETM status (pb_ETMStatus) and the time value
 * (pul_ETMValue) after a stop signal occurred on the
 * selected ETM module (b_ModulNbr). This function is only
 * available if you have disabled the interrupt function.
 * See function " i_APCI1710_EnableETM" and table 3-1.
 * You can test the ETM status with "i_APCI1710_GetETMProgressStatus".
 *
 * The returned value of pul_ETMValue is not the actual
 * measured time value. You must use the "i_APCI1710_ConvertETMValue" function
 * or make this operation for calculate the time value:
 * Time value  = pul_ETMValue x pul_RealTiming.
 * pul_RealTiming is the returned parameter of
 * "i_APCI1710_InitETM" and the time unit is the
 * b_TimingUnit of the "i_APCI1710_InitETM" function.
 *
 * @param [in] pdev                  : The device to use.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_ETM		         : Selected ETM (0 or 1).
 * @param [in] ui_TimeOut            : Timeout selection (0 to 65535)
 *                                     If you used this function in to the interrupt
 *                                     function, this parameter a not used
 *                                     0: Timeout not used.
 *                                        The function returns the ETM status and if
 *                                        a stop signal occurs the measured time value.
 *                                     1 to 65535: Determines the timeout in ms.
 *                                                 The function return after a timeout or a stop signal occur.
 * @param [out] pb_ETMStatus          : Returns the ETM status.
 *                                     0: Measurement not started. No start trigger signal occurs.                    |
 *                                     1: Measurement started. A start trigger signal occurs.
 *                                     2: Measurement stopped. A stop trigger signal occurs. The measurement is ended.
 *                                     3: An overflow occurs. You must change the base time with the function "i_APCI1710_InitETM"
 *                                     4: A timeout occurred.
 * @param [out] pul_ETMValue         : ETM time value.
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: Module selection wrong
 * @retval 3: The module is not a ETM module
 * @retval 4: ETM selection is wrong
 * @retval 5: ETM not initialised, see function "i_APCI1710_InitETM"
 * @retval 6: Timeout parameter is wrong
 * @retval 7: Interrupt routine installed. You cannot directly read the ETM measured time.
 */
int	i_APCI1710_ReadETMValue	(struct pci_dev *pdev,
							uint8_t	   b_ModulNbr,
							uint8_t	   b_ETM,
							uint32_t   ui_TimeOut,
							uint8_t	  *pb_ETMStatus,
							uint32_t *pul_ETMValue)
	{
	int  i_ReturnValue 	= 0;
	uint32_t dw_Status         = 0;
	uint32_t dw_TimeOut 	= 0;
	uint32_t dw_ProgressStatus = 0;

	if (!pdev) return 1;

	/* Test if ETM */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_ETM)
	{
		/* Test the module number */
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
	      /* Test the ETM selection */
	      if (b_ETM <= 1)
		 {
		 /* Get ETM initialisation */
		 INPDW (GET_BAR2(pdev),
			4 + MODULE_OFFSET(b_ModulNbr),
			&dw_Status);

		 /* Test if module initialised */
		 if ((dw_Status & 2) == 2)
		    {
		    /* Test the timout parameter */
		    if ((ui_TimeOut >= 0) && (ui_TimeOut <= 65535UL))
		       {
		       /* Test if no interrupt used */
		       if (APCI1710_PRIVDATA(pdev)->
						s_ModuleInfo [(int)b_ModulNbr].
			   s_ETMModuleInfo.
			   s_ETMInfo [b_ETM].
			   b_ETMInterrupt == 0)
			  {
			  for (;;)
			     {
			     /* Get the ETM Progress status */
			     INPDW (GET_BAR2(pdev),
				    20 + MODULE_OFFSET(b_ModulNbr) + (b_ETM * 16),
				    &dw_ProgressStatus);

			     /* Test if start */
			     if (dw_ProgressStatus & 1)
				{
				/* Test if overflow */
				if (dw_ProgressStatus & 4)
				   {
				   /* ETM overflow */
				   *pb_ETMStatus = 3;
				   } // if (dw_ProgressStatus & 4)
				else
				   {
				   /* Test if stop */
				   if (dw_ProgressStatus & 2)
				      {
				      /* ETM stoped */
				      *pb_ETMStatus = 2;
				      } // if (dw_ProgressStatus & 2)
				   else
				      {
				      /* ETM started */
				      *pb_ETMStatus = 1;
				      } // if (dw_ProgressStatus & 2)
				   } // if (dw_ProgressStatus & 4)
				} // if (dw_ProgressStatus & 1)
			     else
				{
				/* ETM not started */
				*pb_ETMStatus = 0;
				} // if (dw_ProgressStatus & 1)

			     if ((dw_TimeOut == ui_TimeOut) ||
				 (dw_ProgressStatus & 5))
				{
				/* Timeout occur */
				break;
				}
			     else
				{
				/* Increment the timeout */
				dw_TimeOut = dw_TimeOut + 1;
				   mdelay(1);
				}
			     } // for (;;)

			  /* Test if stopped or overflow */
			  if (dw_ProgressStatus & 5)
			     {
			     /* Read the ETM Value */
			     INPDW (GET_BAR2(pdev),
				    12 + MODULE_OFFSET(b_ModulNbr) + (b_ETM * 16),
				    pul_ETMValue);
			     } // if (dw_ProgressStatus & 5)
			  else
			     {
			     if (ui_TimeOut > 0)
				{
				*pb_ETMStatus = 4;
				} // if (ui_TimeOut > 0)
			     } // if (dw_ProgressStatus & 5);
			  } // if (ps_APCI1710Variable->...b_ETMInterrupt == 0)
		       else
			  {
			  /* Interrupt routine installed */
			  i_ReturnValue = 7;
			  } // if (ps_APCI1710Variable->...b_ETMInterrupt == 0)
		       } // if ((ui_TimeOut >= 0) && (ui_TimeOut <= 65535UL))
		    else
		       {
		       /* Timeout parameter is wrong */
		       i_ReturnValue = 6;
		       } // // if ((ui_TimeOut >= 0) && (ui_TimeOut <= 65535UL))
		    } // if ((dw_Status & 2) == 2)
		 else
		    {
		    /* ETM not initialised */
		    i_ReturnValue = 5;
		    } // if ((dw_Status & 2) == 2)
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      else
		 {
		 /* ETM selection is wrong */
		 i_ReturnValue = 4;
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      } // if (... == APCI1710_ETM)
	   else
	      {
	      /* The module is not a ETM module */
	      i_ReturnValue = 3;
	      } // if (... == APCI1710_ETM)
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)
	else
	   {
	   /* Module selection wrong */
	   i_ReturnValue = 2;
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/* Read total ETM value.
 *
 * Returns the ETM status (pb_ETMStatus) and the time value
 * (pul_ETMValue) after a stop signal occurred on the
 * selected ETM module (b_ModulNbr). This function is only
 * available if you have disabled the interrupt function.
 * See function " i_APCI1710_EnableETM" and table 3-1.
 * You can test the ETM status with "i_APCI1710_GetETMProgressStatus".
 *
 * The returned value of pul_ETMValue is not the actual
 * measured time value. You must use the "i_APCI1710_ConvertETMValue" function
 * or make this operation for calculate the time value:
 * Time value  = pul_ETMValue x pul_RealTiming.
 * pul_RealTiming is the returned parameter of
 * "i_APCI1710_InitETM" and the time unit is the
 * b_TimingUnit of the "i_APCI1710_InitETM" function.
 *
 * @param [in] pdev                  : The device to use.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_ETM		         : Selected ETM (0 or 1).
 * @param [in] ui_TimeOut            : Timeout selection (0 to 65535)
 *                                     If you used this function in to the interrupt
 *                                     function, this parameter a not used
 *                                     0: Timeout not used.
 *                                        The function returns the ETM status and if
 *                                        a stop signal occurs the measured time value.
 *                                     1 to 65535: Determines the timeout in ms.
 *                                                 The function return after a timeout or a stop signal occur.
 * @param [out] pb_ETMStatus          : Returns the ETM status.
 *                                     0: Measurement not started. No start trigger signal occurs.                    |
 *                                     1: Measurement started. A start trigger signal occurs.
 *                                     2: Measurement stopped. A stop trigger signal occurs. The measurement is ended.
 *                                     3: An overflow occurs. You must change the base time with the function "i_APCI1710_InitETM"
 *                                     4: A timeout occurred.
 * @param [out] pul_ETMValue         : ETM time value.
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: Module selection wrong
 * @retval 3: The module is not a ETM module
 * @retval 4: ETM selection is wrong
 * @retval 5: ETM not initialised, see function "i_APCI1710_InitETM"
 * @retval 6: Timeout parameter is wrong
 */

int	i_APCI1710_ReadETMTotalTime	(struct pci_dev *pdev,
								uint8_t	   b_ModulNbr,
								uint8_t	   b_ETM,
								uint32_t   ui_TimeOut,
								uint8_t	  *pb_ETMStatus,
								uint32_t *pul_ETMValue)
	{
	int  i_ReturnValue 	= 0;
	uint32_t dw_Status         = 0;
	uint32_t dw_TimeOut 	= 0;
	uint32_t dw_ProgressStatus = 0;

	if (!pdev) return 1;

	/* Test if ETM */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_ETM)
	{
		/* Test the module number */
		if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
		{
	      /* Test the ETM selection */
	      if (b_ETM <= 1)
		 {
		 /* Get ETM initialisation */
		 INPDW (GET_BAR2(pdev),
			4 + MODULE_OFFSET(b_ModulNbr),
			&dw_Status);

		 /* Test if module initialised */
		 if ((dw_Status & 2) == 2)
		    {
		    /* Test the timout parameter */
		    if ((ui_TimeOut >= 0) && (ui_TimeOut <= 65535UL))
		       {
		       for (;;)
			  {
			  /* Get the ETM Progress status */
			  INPDW (GET_BAR2(pdev),
				 20 + MODULE_OFFSET(b_ModulNbr) + (b_ETM * 16),
				 &dw_ProgressStatus);

			  /* Test if start */
			  if (dw_ProgressStatus & 1)
			     {
			     /* Test if overflow */
			     if (dw_ProgressStatus & 4)
				{
				/* ETM overflow */
				*pb_ETMStatus = 3;
				} // if (dw_ProgressStatus & 4)
			     else
				{
				/* Test if stop */
				if (dw_ProgressStatus & 2)
				   {
				   /* ETM stoped */
				   *pb_ETMStatus = 2;
				   } // if (dw_ProgressStatus & 2)
				else
				   {
				   /* ETM started */
				   *pb_ETMStatus = 1;
				   } // if (dw_ProgressStatus & 2)
				} // if (dw_ProgressStatus & 4)
			     } // if (dw_ProgressStatus & 1)
			  else
			     {
			     /* ETM not started */
			     *pb_ETMStatus = 0;
			     } // if (dw_ProgressStatus & 1)

			  if ((dw_TimeOut == ui_TimeOut) ||
			      (dw_ProgressStatus & 5))
			     {
			     /* Timeout occur */
			     break;
			     }
			  else
			     {
			     /* Increment the timeout */
			     dw_TimeOut = dw_TimeOut + 1;
				mdelay (1);

			     }
			  } // for (;;)

		       /* Test if stopped or overflow */
		       if (dw_ProgressStatus & 5)
			  {
			  /* Read the ETM Value */
			  INPDW (GET_BAR2(pdev),
				 16 + MODULE_OFFSET(b_ModulNbr) + (b_ETM * 16),
				 pul_ETMValue);
			  } // if (dw_ProgressStatus & 5)
		       else
			  {
			  if (ui_TimeOut > 0)
			     {
			     *pb_ETMStatus = 4;
			     } // if (ui_TimeOut > 0)
			  } // if (dw_ProgressStatus & 5);
		       } // if ((ui_TimeOut >= 0) && (ui_TimeOut <= 65535UL))
		    else
		       {
		       /* Timeout parameter is wrong */
		       i_ReturnValue = 6;
		       } // // if ((ui_TimeOut >= 0) && (ui_TimeOut <= 65535UL))
		    } // if ((dw_Status & 2) == 2)
		 else
		    {
		    /* ETM not initialised */
		    i_ReturnValue = 5;
		    } // if ((dw_Status & 2) == 2)
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      else
		 {
		 /* ETM selection is wrong */
		 i_ReturnValue = 4;
		 } // if (b_ETM >= 0 && b_ETM <= 1)
	      } // if (... == APCI1710_ETM)
	   else
	      {
	      /* The module is not a ETM module */
	      i_ReturnValue = 3;
	      } // if (... == APCI1710_ETM)
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)
	else
	   {
	   /* Module selection wrong */
	   i_ReturnValue = 2;
	   } // if (b_ModulNbr < ps_APCI1710Variable->s_Board [b_BoardHandle].s_BoardInfos.b_NumberOfModule)

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


