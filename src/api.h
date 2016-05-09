 /* @file api.h
  *
  * @brief Mainly this header should contains definition of ioctl handler.
  *
  * @author Krauth Julien
  *
  */

/** @par LICENCE
 *
 *  @verbatim
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

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the
 Free Software Foundation; either version 2.1 of the License,
 or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA


 You also shoud find the complete LGPL in the LGPL.txt file accompanying
 this source code.

 @endverbatim
**/

#ifndef __apci1710_API_H_
#define __apci1710_API_H_

//------------------------------------------------------------------------------

/* deprecated */
int do_CMD_APCI1710_CheckAndGetPCISlotNumber(struct pci_dev * pdev,unsigned int cmd, unsigned long arg);
int do_CMD_APCI1710_GetHardwareInformation(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

int do_CMD_APCI1710_SetBoardInformation(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_InitCounter (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_ClearCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Clear all counter value.
 *
 * @param [in] pdev          : The device to initialize.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: No counter module found.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_ClearAllCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

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
 * @retval 3: The module is not a counter module or not initialised.
 * @retval 4: The selected PCI input clock is wrong.
 * @retval 5: The selected filter value is wrong.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetInputFilter (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

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

int do_CMD_APCI1710_EnableLatchInterrupt (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


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

int do_CMD_APCI1710_DisableLatchInterrupt (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

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
int do_CMD_APCI1710_InitCompareLogic (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

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
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_EnableCompareLogic (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

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
int do_CMD_APCI1710_DisableCompareLogic (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

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
int do_CMD_APCI1710_Read16BitCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

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
int do_CMD_APCI1710_Read32BitCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

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
int do_CMD_APCI1710_SetDigitalChlOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

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
int do_CMD_APCI1710_SetDigitalChlOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Enable and set the interrupt routine.
 *
 * @param [in] pdev              : The device to initialize.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The interrupt callback is NULL.
 */
int do_CMD_APCI1710_SetBoardIntRoutine (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Disable and reset the interrupt routine.
 *
 * @param [in] pdev              : The device to initialize.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 */
int do_CMD_APCI1710_ResetBoardIntRoutine (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_TestInterrupt (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Configure the digital I/O operating mode.
 *
 * Configure the digital I/O operating mode from selected
 * module  (b_ModulNbr). You must calling this function be
 * for you call any other function witch access of digital I/O.
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_ChannelAMode)  : Channel A mode selection <br>
 *                                        0 : Channel used for digital input <br>
 *                                        1 : Channel used for digital output
 * @param [in] arg[2] (b_ChannelBMode)  : Channel B mode selection <br>
 *                                        0 : Channel used for digital input
 *                                        1 : Channel used for digital output
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Bi-directional channel A configuration error.
 * @retval 5: Bi-directional channel B configuration error.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_InitDigitalIO (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Read the status from selected digital I/O digital input (b_InputChannel).
 *
 * @param [in] pdev                    : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)     : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_InputChannel) : Selection from digital input (0 to 6) <br>
 *                                       0 : Channel C <br>
 *                                       1 : Channel D <br>
 *                                       2 : Channel E <br>
 *                                       3 : Channel F <br>
 *                                       4 : Channel G <br>
 *                                       5 : Channel A <br>
 *                                       6 : Channel B
 *
 * @param [out] arg[0] (pb_ChannelStatus) : Digital input channel status <br>
 *                                          0 : Channle is not active <br>
 *                                          1 : Channle is active
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: The selected digital I/O digital input is wrong.
 * @retval 5: Digital I/O not initialised.
 * @retval 6: The digital channel A is used for output.
 * @retval 7: The digital channel B is used for output.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_ReadDigitalIOChlValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Read the status from digital input port.
 *
 * Read the status from digital input port from selected
 * digital I/O module (b_ModulNbr).
 *
 * @param [in] pdev                : The device to initialize.
 * @param [in] arg (b_ModulNbr)    : Module number to configure (0 to 3).
 *
 * @param [out] arg (pb_PortValue) : Digital I/O inputs port status.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Digital I/O not initialised.
 */
int do_CMD_APCI1710_ReadDigitalIOPortValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Activates the digital output memory.
 *
 * After having called up this function, the output you have previously
 * activated with the function "i_APCI1710_SetDigitalIOOutputXOn" are not reset.
 * You can reset them with the function "i_APCI1710_SetDigitalIOOutputXOff".
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg (b_ModulNbr)      : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Digital I/O not initialised.
 */
int do_CMD_APCI1710_SetDigitalIOMemoryOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Deactivates the digital output memory.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg (b_ModulNbr)      : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Digital I/O not initialised.
 */
int do_CMD_APCI1710_SetDigitalIOMemoryOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Sets the output.
 *
 * Sets the output witch has been passed with the
 * parameter b_Channel. Setting an output means setting
 * an ouput high.
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_OutputChannel) : Selection from digital output <br>
 *                                        channel (0 to 2) <br>
 *                                        0 : Channel H <br>
 *                                        1 : Channel A <br>
 *                                        2 : Channel B
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".
 * @retval 6: The digital channel A is used for input.
 * @retval 7: The digital channel B is used for input.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetDigitalIOChlOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Resets the output.
 *
 * Resets the output witch has been passed with the
 * parameter b_Channel. Resetting an output means setting
 * an ouput low.
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_OutputChannel) : Selection from digital output channel (0 to 2) <br>
 *                                        0 : Channel H <br>
 *                                        1 : Channel A <br>
 *                                        2 : Channel B
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".
 * @retval 6: The digital channel A is used for input.
 * @retval 7: The digital channel B is used for input.
 * @retval 8: Digital Output Memory OFF Use previously the function "i_APCI1710_SetDigitalIOMemoryOn".
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetDigitalIOChlOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Sets one or several outputs from port.
 *
 * Setting an output means setting an output high.
 * If you have switched OFF the digital output memory
 * (OFF), all the other output are set to "0".
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)   : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PortValue)  : Output Value ( 0 To 7 ).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Output value wrong.
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".
 * @retval 6: The digital channel A is used for input.
 * @retval 7: The digital channel B is used for input.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetDigitalIOPortOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Resets one or several output from port.
 *
 * Resetting means setting low.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)   : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PortValue)  : Output Value ( 0 To 7 ).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Output value wrong.
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".
 * @retval 6: The digital channel A is used for input.
 * @retval 7: The digital channel B is used for input.
 * @retval 8: Digital Output Memory OFF. Use previously the function "i_APCI1710_SetDigitalIOMemoryOn".
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetDigitalIOPortOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Configure the TTL I/O operating mode on the selected module.
 *  You must call this function before you call any other TTL function.
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PortAMode)     : Port A mode selection <br>
 *                                        0 : digital input <br>
 *                                        1 : digital output
 * @param [in] arg[2] (b_PortBMode)     : Port B mode selection <br>
 *                                        0 : digital input <br>
 *                                        1 : digital output
 * @param [in] arg[3] (b_PortCMode)     : Port C mode selection <br>
 *                                        0 : digital input <br>
 *                                        1 : digital output
 * @param [in] arg[4] (b_PortDMode)     : Port D mode selection <br>
 *                                        0 : digital input <br>
 *                                        1 : digital output
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 * @retval 4: Function not available for this version.
 * @retval 5: Port A mode selection is wrong.
 * @retval 6: Port B mode selection is wrong.
 * @retval 7: Port C mode selection is wrong.
 * @retval 8: Port D mode selection is wrong.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_InitTTLIODirection (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Set the output witch has been passed with the
 *  parameter b_Channel. Setting an output means setting
 *  an ouput high.
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_OutputChannel) : Selection from digital output channel (0 or 1):<br>
 *                                        0       : PD0<br>
 *                                        1       : PD1<br>
 *                                        2 to 9  : PA<br>
 *                                        10 to 17: PB<br>
 *                                        8 to 25 : PC
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: TTL I/O not initialised see function i_APCI1710_InitTTLIO.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetTTLIOChlOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Reset the output witch has been passed with the
 *  parameter b_Channel. Resetting an output means setting
 *  an ouput low.
 *
 * @param [in] pdev                     : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_OutputChannel) : Selection from digital output channel (0 or 1):<br>
 *                                        0       : PD0<br>
 *                                        1       : PD1<br>
 *                                        2 to 9  : PA<br>
 *                                        10 to 17: PB<br>
 *                                        8 to 25 : PC
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: TTL I/O not initialised see function i_APCI1710_InitTTLIO.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetTTLIOChlOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Execute a command.
*
* @param pdev Pointer to a device.
* @param cmd Command to execute
* @param arg Argument of the command.
*/
int apci1710_do_ioctl(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

/** Return the module functionalities.
 *
 * @param [in] pdev          : The device to initialize.
 * @param [in] b_ModulNbr    : Module number to configure (0 to 3).
 *
 * @param [out] pui_Functionality : Module configuration.
 *
 * @retval 0: No error.
 * @retval -EFAULT : Failed to copy data in user space (incorrect pointer).
 */
int do_CMD_APCI1710_GetModulesId (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_Write16BitCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_Write32BitCounterValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------
/** Returns the counter progress latched status after a index interrupt occur.
 *
 * @param [in] argv                  : A pointer to an array of 2 unsigned long
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
int do_CMD_APCI1710_GetInterruptUDLatchedStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/* see apci1710.h/inc_cpt.c for documentation */
int do_CMD_APCI1710_InitIndex (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/* see apci1710.h/inc_cpt.c for documentation */
int do_CMD_APCI1710_EnableIndex (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/* see apci1710.h/inc_cpt.c for documentation */
int do_CMD_APCI1710_DisableIndex (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/* see apci1710.h/inc_cpt.c for documentation */
int do_CMD_APCI1710_GetIndexStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/* see apci1710.h/inc_cpt.c for documentation */
int do_CMD_APCI1710_SetIndexAndReferenceSource (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/* see apci1710.h/inc_cpt.c for documentation */
int do_CMD_APCI1710_InitReference (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/* see apci1710.h/inc_cpt.c for documentation */
int do_CMD_APCI1710_GetReferenceStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/* see apci1710.h/inc_cpt.c for documentation */
int do_CMD_APCI1710_GetCounterInitialisationStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int i_APCI1710_GetCounterInitialisationStatus(struct pci_dev * pdev, unsigned int ui_ModulNbr, unsigned int ui_ArgArray[6] );


/** Initialize the impuls counter.
 *
 * Configure the pulse encoder operating mode selected via b_ModulNbr and b_PulseEncoderNbr.
 * The pulse encoder after each pulse decrement the counter value from 1.
 * You must calling this function be for you call any
 * other function witch access of pulse encoders.
 *
 *
 * @param [in] pdev                           : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)            : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr)     : Pulse encoder selection (0 to 3)
 * @param [in] arg[2] (b_InputLevelSelection) : Input level selection (0 or 1) <br>
 *                                                0 : Set pulse encoder count the the low level pulse. <br>
 *                                                1 : Set pulse encoder count the the high level pulse.
 * @param [in] arg[3] (b_TriggerOutputAction) : Digital TRIGGER output action <br>
 *                                                0 : No action <br>
 *                                                1 : Set the trigger output to "1" <br>
 *                                                    (high) after the passage from 1 to 0 <br>
 *                                                    from pulse encoder. <br>
 *                                                2 : Set the trigger output to "0" <br>
 *                                                    (low) after the passage from 1 to 0 <br>
 *                                                    from pulse encoder
 * @param [in] arg[4] (ul_StartValue)        : Pulse encoder start value (1 to 4294967295)
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module is not a pulse encoder module.
 * @retval 3: Pulse encoder selection is wrong.
 * @retval 4: Input level selection is wrong.
 * @retval 5: Digital TRIGGER output action selection is wrong.
 * @retval 6: Pulse encoder start value is wrong.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_InitPulseEncoder (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


/** Enable the pulse encoder
 *
 * Enable the selected pulse encoder (b_PulseEncoderNbr)
 * from selected module (b_ModulNbr). Each input pulse
 * decrement the pulse encoder counter value from 1.
 * If you enabled the interrupt (b_InterruptHandling), a
 * interrupt is generated when the pulse encoder has run down.
 *
 * @param [in] pdev                         : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr           : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr)   : Pulse encoder selection (0 to 3).
 * @param [in] arg[2] (b_CycleSelection)    : APCI1710_CONTINUOUS: <br>
 *                                              Each time the counting value is set <br>
 *                                              on "0", the pulse encoder load the <br>
 *                                              start value after the next pulse. <br>
 *                                            APCI1710_SINGLE:
 *                                              If the counter is set on "0", the pulse <br>
 *                                              encoder is stopped.
 * @param [in] arg[3] (b_InterruptHandling) : Interrupts can be generated, when the pulse <br>
 *                                            encoder has run down. <br>
 *                                            With this parameter the user decides if <br>
 *                                            interrupts are used or not. <br>
 *                                            APCI1710_ENABLE : Interrupts are enabled <br>
 *                                            APCI1710_DISABLE: Interrupts are disabled
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval 5: Cycle selection mode is wrong.
 * @retval 6: Interrupt handling mode is wrong.
 * @retval 7: Interrupt routine not installed. See function "i_APCI1710_SetBoardIntRoutine"
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_EnablePulseEncoder (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


/** Disable the pulse encoder.
 *
 * Disable the selected pulse encoder (b_PulseEncoderNbr) from selected module (b_ModulNbr).
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr         : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr) : Pulse encoder selection (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_DisablePulseEncoder (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


/** Reads the pulse encoder status
 *
 * Reads the status from selected pulse encoder (b_PulseEncoderNbr)
 * from selected module (b_ModulNbr).
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr         : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr) : Pulse encoder selection (0 to 3).
 *
 * @param [out] arg[0] (pb_Status)        : Pulse encoder status. <br>
 *                                            0 : No overflow occur <br>
 *                                            1 : Overflow occur
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_ReadPulseEncoderStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


/** Reads the pulse encoder value.
 *
 * Reads the value from selected pulse encoder (b_PulseEncoderNbr) from selected module (b_ModulNbr).
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr         : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr) : Pulse encoder selection (0 to 3).
 *
 * @param [out] arg[0] (pul_ReadValue)    : Pulse encoder value
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_ReadPulseEncoderValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


/** Writes a 32-bit value into the pulse encoder.
 *
 * Writes a 32-bit value (ul_WriteValue) into the selected pulse encoder (b_PulseEncoderNbr)
 * from selected module (b_ModulNbr). This operation set the new start pulse
 * encoder value.                                         |
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr         : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_PulseEncoderNbr) : Pulse encoder selection (0 to 3).
 * @param [in] arg[2] (ul_WriteValue)     : 32-bit value to be written.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Pulse encoder selection is wrong
 * @retval 4: Pulse encoder not initialised. See function "i_APCI1710_InitPulseEncoder"
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_WritePulseEncoderValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


/** Set the digital output H on.
 *
 * Set the digital outp H on (High) from selected module (b_ModulNbr)
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)        : Module number (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Functionality not available
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_PulseEncoderSetDigitalOutputOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


/** Set the digital output H off.
 *
 * Set the digital outp H off (Low) from selected module (b_ModulNbr)
 *
 * @param [in] pdev                       : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)        : Module number (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection is wrong
 * @retval 3: Functionality not available
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_PulseEncoderSetDigitalOutputOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


/* remove user handler associated with given module */
int i_APCI1710_ClearUserHandler(struct pci_dev * pdev, uint8_t b_ModulNbr);

//------------------------------------------------------------------------------
/** Uses printk() to output the detected module functionality.
 *
 * @param [in] pdev          : The device to initialize.
 *
 * @param [out] pui_Functionality : Module configuration.
 *
 * @retval 0: No error.
 * @retval 1: parameter pdev is NULL.
 */
int  i_APCI1710_DumpModulesFunctionalities (struct pci_dev *pdev);

//------------------------------------------------------------------------------

/** Initialize chronos functionality.
 *
 * Configure the chronometer operating mode (b_ChronoMode)
 * from selected module (b_ModuleNbr).
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
 * @param [in] struct pci_dev *pdev                                 : The device to use.
 * @param [in] uint32_t arg[0] (b_ModuleNbr)       : Module number to configure (0 to 3).
 * @param [in] uint32_t arg[1] (b_ChronoMode)      : Chronometer action mode (0 to 7).
 * @param [in] uint32_t arg[2] (b_PCIInputClock)   : Selection from PCI bus clock
 *                                                   - APCI1710_30MHZ :
 *                                                     The PC have a PCI bus
 *                                                     clock from 30 MHz
 *                                                     Not available with the APCIe-1711
 *                                                   - APCI1710_33MHZ :
 *                                                     The PC have a PCI bus
 *                                                     clock from 33 MHz
 *                                                     Not available with the APCIe-1711
 *                                                   - APCI1710_40MHZ
 *                                                     The APCI-1710 have a
 *                                                     integrated 40Mhz quartz.
 * @param [in] uint32_t arg[3] (b_TimingUnit)      : Base timing unity (0 to 4)
 *                                                   0 : ns
 *                                                   1 : �s
 *                                                   2 : ms
 *                                                   3 : s
 *                                                   4 : mn
 * @param [in] uint32_t arg[4] (ul_TimingInterval  : Base timing value.
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
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_InitChrono (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Enable the chronometer.
 *
 * Enable the chronometer from selected module
 * (b_ModuleNbr). You must calling the
 * "i_APCI1710_InitChrono" function be for you call this
 * function.
 * If you enable the chronometer interrupt, the
 * chronometer generate a interrupt after the stop signal.
 * See function "i_APCI1710_SetBoardIntRoutineX" and the
 * Interrupt mask description chapter from this manual.
 * The b_CycleMode parameter determine if you will
 * measured a single or more cycle.
 *
 * @param [in] struct pci_dev *pdev                                : The device to use.
 * @param [in] uint8_t arg[0] (b_ModuleNbr)       : Module number to configure (0 to 3).
 * @param [in] uint8_t arg[1] (b_CycleMode)       : Selected the chronometer acquisition mode.
 * @param [in] uint8_t arg[2] (b_InterruptEnable) : Enable or disable the chronometer interrupt.
 *                                                  - APCI1710_ENABLE: Enable the chronometer interrupt.
 *                                                  - APCI1710_DISABLE:Disable the chronometer interrupt.
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval 5: Chronometer acquisition mode cycle is wrong.
 * @retval 6: Interrupt parameter is wrong.
 * @retval 7: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutineX".
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_EnableChrono (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Disable the chronometer.
 *
 * Disable the chronometer from selected module
 * (b_ModuleNbr). If you disable the chronometer after a
 * start signal occur and you restart the chronometer
 * witch the " i_APCI1710_EnableChrono" function, if no
 * stop signal occur this start signal is ignored.
 *
 * @param [in] struct pci_dev *pdev                             : The device to use.
 * @param [in] uint8_t arg (b_ModuleNbr)       : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 */
int do_CMD_APCI1710_DisableChrono (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Get the chronometer status.
 *
 * Return the chronometer status (pb_ChronoStatus) from
 * selected chronometer module (b_ModuleNbr).
 *
 * @param [in] struct pci_dev *pdev                        : The device to use.
 * @param [in] uint8_t* arg (b_ModuleNbr) : Module number to configure (0 to 3).
 *
 * @param [out] uint8_t* arg (b_ChronoStatus) : Return the chronometer status.
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
 * @retval -EFAULT : Fail to return user data.
 */
int do_CMD_APCI1710_GetChronoProgressStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Read the chronometer value.
 *
 * Return the chronometer status (pb_ChronoStatus) and the
 * timing value (pul_ChronoValue) after a stop signal
 * occur from selected chronometer module (b_ModuleNbr).
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
 * @param [in] struct pci_dev *pdev                           : The device to use.
 * @param [in] uint32_t arg[0] (b_ModuleNbr) : Module number to configure (0 to 3).
 * @param [in] uint32_t arg[1] (ul_TimeOut)  : 0 not used, >0 number of loop to to before timeout.
 *
 * @param [out] uint32_t arg[0] (b_ChronoStatus)  : Return the chronometer status.
 *                                                  0 : Measurement not started.
 *                                                      No start signal occur.
 *                                                  1 : Measurement started.
 *                                                      A start signal occur.
 *                                                  2 : Measurement stopped.
 *                                                      A stop signal occur.
 *                                                      The measurement is terminate.
 *                                                  3: A overflow occur. You must change the base
 *                                                     timing witch the function "i_APCI1710_InitChrono"
 *
 * @param [out] uint32_t arg[1] (ul_ChronoValue) : Chronometer timing value.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval 5: Timeout parameter is wrong (0 to 65535).
 * @retval 6: Interrupt routine installed.
 *            You can not read directly the chronometer measured timing.
 * @retval -EFAULT : Fail to retrieve / return user data.
 */
int do_CMD_APCI1710_ReadChronoValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Set the selected chronometer channel on.
 *
 * Sets the output witch has been passed with the
 * parameter b_Channel. Setting an output means setting an
 * output high.
 *
 * @param [in] struct pci_dev *pdev                              : The device to use.
 * @param [in] uint8_t arg[0] (b_ModuleNbr)     : Module number to configure (0 to 3).
 * @param [in] uint8_t arg[1] (b_OutputChannel) : Selection from digital output
 *                                                 channel (0 to 2)
 *                                                 0 : Channel H
 *                                                 1 : Channel A
 *                                                 2 : Channel B
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: Timeout parameter is wrong (0 to 65535).
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetChronoChlOn (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Set the selected chronometer channel off.
 *
 * Reset the output witch has been passed with the
 * parameter b_Channel. Setting an output means setting an
 * output high.
 *
 * @param [in] struct pci_dev *pdev                              : The device to initialize.
 * @param [in] uint8_t arg[0] (b_ModuleNbr)     : Module number to configure (0 to 3).
 * @param [in] uint8_t arg[1] (b_OutputChannel) : Selection from digital output
 *                                                channel (0 to 2)
 *                                                0 : Channel H
 *                                                1 : Channel A
 *                                                2 : Channel B
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetChronoChlOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Read the selected chronometer channel value.
 *
 * Return the status from selected digital input
 * (b_InputChannel) from selected chronometer
 * module (b_ModuleNbr).
 *
 * @param [in] struct pci_dev *pdev                             : The device to use.
 * @param [in] uint8_t arg[0] (b_ModuleNbr)    : Module number to configure (0 to 3).
 * @param [in] uint8_t arg[1] (b_InputChannel) : Selection from digital output
 *                                               channel (0 to 2)
 *                                               0 : Channel E
 *                                               1 : Channel F
 *                                               2 : Channel G
 *
 * @param [out] uint8_t arg[0] (b_ChannelStatus) : Digital input channel status.
 *                                                 0 : Channel is not active
 *                                                 1 : Channel is active
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: The selected digital input is wrong.
 * @retval 5: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval -EFAULT : Fail to retrieve / return user data.
 */
int do_CMD_APCI1710_ReadChronoChlValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Read the selected chronometer port value.
 *
 * Return the status from digital inputs port from
 * selected  (b_ModuleNbr) chronometer module.
 *
 * @param [in] struct pci_dev *pdev                         : The device to use.
 * @param [in] uint8_t* arg (b_ModuleNbr)  : Module number to configure (0 to 3).
 *
 * @param [out] uint8_t* arg (b_PortValue) : Digital inputs port status.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a chronos module.
 * @retval 4: Chronometer not initialised see function "i_APCI1710_InitChrono".
 * @retval -EFAULT : Fail to return user data.
 */
int do_CMD_APCI1710_ReadChronoPortValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Initialize the master and the slave(s) for single cycle read / write.
 * @param[in] deviceData				Pointer to the device
 * @param[in] moduleIndex				Index of the slave (0->3)
 * @param[in] sensorDataFreqDivisor		Sensor data frequency (0 -> 31, 16 not permitted)
 * @param[in] registerDataFreqDivisor	Register data frequency : depend from the sensor data frequency (0 -> 7)
 * @param[in] channel0BISSSSIMode		Select the mode of the channel 0 (0: BISS, 1: SSI)
 * @param[in] channel0BissMode			define the BISS mode for the channel 0 (if used) (0 : B mode, 1 : C mode)
 * @param[in] channel1BISSSSIMode		Select the mode of the channel 1 (0: BISS, 1: SSI)
 * @param[in] channel1BissMode			define the BISS mode for the channel 1 (if used) (0 : B mode, 1 : C mode)
 * @param[in] nbrOfSlave				number of slaves (sensors used) (1 -> 6)
 * @param[in] slaveInfos[].channel		channel selection (0 to 1)
 * @param[in] slaveInfos[].dataLength	Data length (0 to 64)
 * @param[in] slaveInfos[].option		reserved set it to 0
 * @param[in] slaveInfos[].CRCPolynom	Polynom check of the single cycle data (0 to 255)
 * @param[in] slaveInfos[].CRCInvert	define if the CRC is inverted or not (0: not inverted, 1 : inverted)
 * @retval 0: success
 * @retval 1 : Invalid moduleIndex
 * @retval 2 : Invalid sensorDataFreqDivisor
 * @retval 3 : Invalid registerDataFreqDivisor
 * @retval 4 : Invalid channel0BISSSSIMode
 * @retval 5 : Invalid channel0BissMode
 * @retval 6 : Invalid channel1BISSSSIMode
 * @retval 7 : Invalid channel1BissMode
 * @retval 8 : Invalid nbrOfSlave
 * @retval 9 : The first slave (sensor) must use channel 0
 * @retval 10 : Slaves (sensors) that use channel 1 must be declared at the end
 * @retval 11 : Invalid channel
 * @retval 12 : Invalid dataLength
 * @retval 13 : Invalid CRCPolynom
 * @retval 14 : Invalid CRCInvert
 * @retval 15 : the component is not programmed as Biss
 */
int do_CMD_APCI1710_BissMasterInitSingleCycle(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Do a single cycle read of the data of a slave
 * @param[in] deviceData		Pointer to the device
 * @param[in] moduleIndex		Index of the slave (0->3)
 * @param[in] slaveIndex		index of the slave(sensor) (depend of the index by the initialisation)
 * @param[out] dataLow			low part (D0 to D31) of the data
 * @param[out] dataHigh			low part (D63 to D32) of the data
 * @retval 0: success
 * @retval 1 : Invalid moduleIndex
 * @retval 2 : Invalid slaveIndex
 * @retval 3 : the component is not programmed as Biss
 * @retval 4 : Cycle acquisition not started
 * @retval 5 : Error while reading the data
 */
int do_CMD_APCI1710_BissMasterSingleCycleDataRead(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Do a single cycle read on the register of a slave
 * @param[in] deviceData		Pointer to the device
 * @param[in] moduleIndex		Index of the slave (0->3)
 * @param[in] slaveIndex		index of the slave(sensor) (depend of the index by the initialisation)
 * @param[in] address			register address (0->127)
 * @param[in] size				number of byte to read (1->64)
 * @param[out] data				return the read data
 * @retval 0: success
 * @retval 1 : Invalid moduleIndex
 * @retval 2 : Invalid slaveIndex
 * @retval 3 : Invalid address
 * @retval 4 : Invalid size
 * @retval 5 : the component is not programmed as Biss
 * @retval 6 : Cycle acquisition not started
 * @retval 7 : Slave (sensor) is not configured as Biss but as SSI
 * @retval 8 : Error while reading the data
 * */
int do_CMD_APCI1710_BissMasterSingleCycleRegisterRead(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Do a single cycle write on the register of a slave
 * @param[in] deviceData		Pointer to the device
 * @param[in] moduleIndex		Index of the slave (0->3)
 * @param[in] slaveIndex		index of the slave(sensor) (depend of the index by the initialisation)
 * @param[in] address			register address (0->127)
 * @param[in] size				number of bytes to write (1->64)
 * @param[in] data : 			Data to write
 * @retval 0: success
 * @retval 1 : Invalid moduleIndex
 * @retval 2 : Invalid slaveIndex
 * @retval 3 : Invalid address
 * @retval 4 : Invalid size
 * @retval 5 : the component is not programmed as Biss
 * @retval 6 : Cycle acquisition not started
 * @retval 7 : Slave (sensor) is not configured as Biss but as SSI
 * @retval 8 : Error while writing the data
 * */
int do_CMD_APCI1710_BissMasterSingleCycleRegisterWrite(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** release the master, the slave(s) for single cycle read / write.
 * @param[in] deviceData				Pointer to the device
 * @param[in] moduleIndex				Index of the slave (0->3)
 * @retval 0: success
 * @retval 1 : moduleIndex is incorrect
 * @retval 2 : the component is not programmed as Biss
 * */
int do_CMD_APCI1710_BissMasterReleaseSingleCycle(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------
/** Initialise the EnDat sensor.
 * @param[in] pdev : Pointer to the device
 * @param[in] moduleIndex : index of the slave (0->3)
 * @param[in] channel : index of the channel (0->3)
 * @param[in] freqValue : Counter value for the frequency transmission divider - in kHz (250,500,800,1000,2500,4000,5000,6666)
 * @retval 1 Invalid moduleIndex
 * @retval 2 Invalid channel
 * @retval 3 Invalid freqValue
 * @retval 4 The component selected with moduleIndex is not programmed as EnDat
 * @retval 5 Error while resetting the sensor
 * @retval 6 Error while selecting the memory area 0xB9
 * @retval 7 Error while asking parameter at address 0x0
 * @retval 8 Error while asking parameter at address 0x1
 * @retval 9 Error while setting parameter at address 0x0 to 0
 * @retval 10 Error while setting parameter at address 0x1 to 0
 * @retval 11 Error while selecting the memory area 0xA1
 * @retval 12 Error while asking parameter at address 0xD
 * @retval 13 Error while selecting the memory space 0xA5
 * @retval 14 Error while asking parameter at address 0x5
 * @retval 15 Invalid freqValue
 * @retval 20 Transmission error
 */
int do_CMD_APCI1711_EndatInitialiseSensor(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Enable to execute the action "Sensor receive reset" (see page 19/131 of EnDat specification)
 * The EnDat mode is 0x2A
 * This function has the same effect as an hardware reboot
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 the component is not programmed as EnDat
 * @retval 4 timeout
 * @retval 20 transmission error
 */
int do_CMD_APCI1711_EndatSensorReceiveReset(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Reset the error bits
 * It can be used before each command in order to get (after the call of the command) the status of the system
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 the component is not programmed as EnDat
 */
int do_CMD_APCI1711_EndatResetErrorBits(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Get the error sources
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @param[out] errorSrc1 : Invalid mode command
 * @param[out] errorSrc2 : Invalid MRS-Code
 * @param[out] errorSrc3 : Transmission is not completed
 * @param[out] errorSrc4 : Communication command is not supported
 * @param[out] errorSrc7 : MRS-Code is not allowed
 * @param[out] errorSrc8 : Invalid address is selected or sensor's EEPROM is written while being busy
 * @param[out] errorSrc9 : Try to write a protected memory place
 * @param[out] errorSrc10 : Write-protect configuration is tried to be reset (if a memory place is write-protected, it cannot be reset)
 * @param[out] errorSrc11 : Block address is not available
 * @param[out] errorSrc12 : Invalid address for the communication command
 * @param[out] errorSrc13 : Invalid additional data (or additional data not supported by the sensor)
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 the component is not programmed as EnDat
 */
int do_CMD_APCI1711_EndatGetErrorSources(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Enable to execute the action "Select memory area" (see page 19/131 of EnDat specification)
 * The EnDat mode is 0xE
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel :  Index of the EnDat channel (0,1)
 * @param [in] mrsCode : The MRS-code corresponding to the memory area you want to select (see page 31/131 and 51/131 of EnDat specification)
 * \li 0xB9:    Operating status (address area: 0x0 - 0x3)
 * \li 0xA1:    Parameters of the encoder manufacturer - first part (address area: 0x4 - 0xF)
 * \li 0xA3:    Parameters of the encoder manufacturer - second part (address area: 0x0 - 0xF)
 * \li 0xA5:    Parameters of the encoder manufacturer - third part (address area: 0x0 - 0xF)
 * \li 0xA7:    Operating parameters (address area: 0x0 - 0xF)
 * \li 0xA9:    Parameters of the OEM - first part (address area: depending on the sensor)
 * \li 0xAB:    Parameters of the OEM - second part (address area: depending on the sensor)
 * \li 0xAD:    Parameters of the OEM - third part (address area: depending on the sensor)
 * \li 0xAF:    Parameters of the OEM - fourth part (address area: depending on the sensor)
 * \li 0xB1:    Compensation values of the encoder manufacturer - first part (address area: depending on the sensor)
 * \li 0xB3:    Compensation values of the encoder manufacturer - second part (address area: depending on the sensor)
 * \li 0xB5:    Compensation values of the encoder manufacturer - third part (address area: depending on the sensor)
 * \li 0xB7:    Compensation values of the encoder manufacturer - fourth part (address area: depending on the sensor)
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 mrsCode is incorrect
 * @retval 4 the component is not programmed as EnDat
 * @retval 5 the sensor is not initialised (initialise it and recall this function)
 * @retval 6 timeout
 * @retval 20 transmission error
 */
int do_CMD_APCI1711_EndatSelectMemoryArea(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Enable to execute the action "Encoder Send Position value and receive selection of memory area" (see page 19/131 of EnDat specification)
 * The EnDat mode is 0x09
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @param [in] mrsCode : The MRS-code corresponding to the memory area you want to select (see page 31/131 and 51/131 of EnDat specification)
 * \li 0xB9:    Operating status (address area: 0x0 - 0x3)
 * \li 0xA1:    Parameters of the encoder manufacturer - first part (address area: 0x4 - 0xF)
 * \li 0xA3:    Parameters of the encoder manufacturer - second part (address area: 0x0 - 0xF)
 * \li 0xA5:    Parameters of the encoder manufacturer - third part (address area: 0x0 - 0xF)
 * \li 0xA7:    Operating parameters (address area: 0x0 - 0xF)
 * \li 0xA9:    Parameters of the OEM - first part (address area: depending on the sensor)
 * \li 0xAB:    Parameters of the OEM - second part (address area: depending on the sensor)
 * \li 0xAD:    Parameters of the OEM - third part (address area: depending on the sensor)
 * \li 0xAF:    Parameters of the OEM - fourth part (address area: depending on the sensor)
 * \li 0xB1:    Compensation values of the encoder manufacturer - first part (address area: depending on the sensor)
 * \li 0xB3:    Compensation values of the encoder manufacturer - second part (address area: depending on the sensor)
 * \li 0xB5:    Compensation values of the encoder manufacturer - third part (address area: depending on the sensor)
 * \li 0xB7:    Compensation values of the encoder manufacturer - fourth part (address area: depending on the sensor)
 * \li 0xBD:    Parameters of the encoder manufacturer for EnDat 2.2
 * \li 0xBF:    Parameters of the section 2 memory area
 * \li 0xBB:    Operating parameters 2
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 mrsCode is incorrect
 * @retval 4 the component is not programmed as EnDat
 * @retval 5 the sensor is not initialised (initialise it and recall this function)
 * @retval 6 timeout
 * @retval 20 transmission error
 */
int do_CMD_APCI1711_EndatSensorSendPositionAndRecvMemArea(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Enable to execute the action "Sensor send parameter" (see page 19/131 of EnDat specification)
 * The EnDat mode is 0x23
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @param [in] mrsCode : The MRS-code corresponding to the memory area of the parameter you want to read (see page 51/131 of EnDat specification)
 * \li 0xB9:    Operating status (address area: 0x0 - 0x3)
 * \li 0xA1:    Parameters of the encoder manufacturer - first part (address area: 0x4 - 0xF)
 * \li 0xA3:    Parameters of the encoder manufacturer - second part (address area: 0x0 - 0xF)
 * \li 0xA5:    Parameters of the encoder manufacturer - third part (address area: 0x0 - 0xF)
 * \li 0xA7:    Operating parameters (address area: 0x0 - 0xF)
 * \li 0xA9:    Parameters of the OEM - first part (address area: depending on the sensor)
 * \li 0xAB:    Parameters of the OEM - second part (address area: depending on the sensor)
 * \li 0xAD:    Parameters of the OEM - third part (address area: depending on the sensor)
 * \li 0xAF:    Parameters of the OEM - fourth part (address area: depending on the sensor)
 * \li 0xB1:    Compensation values of the encoder manufacturer - first part (address area: depending on the sensor)
 * \li 0xB3:    Compensation values of the encoder manufacturer - second part (address area: depending on the sensor)
 * \li 0xB5:    Compensation values of the encoder manufacturer - third part (address area: depending on the sensor)
 * \li 0xB7:    Compensation values of the encoder manufacturer - fourth part (address area: depending on the sensor)
 * \li 0xBB:    Operating parameters 2
 * \li 0xBD:    Parameters of the encoder manufacturer for EnDat 2.2
 * \li 0xBF:    Parameters of the section 2 memory area
 * @param [in] address : The address of the parameter you want to read (0->0xFF)
 * @param [out] param : Value of the parameter
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 mrsCode is incorrect
 * @retval 4 address is incorrect
 * @retval 5 the component is not programmed as EnDat
 * @retval 6 the sensor is not initialised (initialise it and recall this function)
 * @retval 7 the sensor should allow endat22 for 0xBD, 0xBF, 0xBB memory space.
 * @retval 8 timeout
 * @retval 20 transmission error
 */
int do_CMD_APCI1711_EndatSensorSendParameter(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Enable to execute the action "Sensor receive parameter" (see page 19/131 of EnDat specification)
 * The EnDat mode is 0x1C
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex :Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @param [in] mrsCode : The MRS-code corresponding to the parameter you want to set (see page 51/131 of EnDat specification)
 * \li 0xB9:    Operating status (address area: 0x0 - 0x3)
 * \li 0xA1:    Parameters of the encoder manufacturer - first part (address area: 0x4 - 0xF)
 * \li 0xA3:    Parameters of the encoder manufacturer - second part (address area: 0x0 - 0xF)
 * \li 0xA5:    Parameters of the encoder manufacturer - third part (address area: 0x0 - 0xF)
 * \li 0xA7:    Operating parameters (address area: 0x0 - 0xF)
 * \li 0xA9:    Parameters of the OEM - first part (address area: depending on the sensor)
 * \li 0xAB:    Parameters of the OEM - second part (address area: depending on the sensor)
 * \li 0xAD:    Parameters of the OEM - third part (address area: depending on the sensor)
 * \li 0xAF:    Parameters of the OEM - fourth part (address area: depending on the sensor)
 * \li 0xB1:    Compensation values of the encoder manufacturer - first part (address area: depending on the sensor)
 * \li 0xB3:    Compensation values of the encoder manufacturer - second part (address area: depending on the sensor)
 * \li 0xB5:    Compensation values of the encoder manufacturer - third part (address area: depending on the sensor)
 * \li 0xB7:    Compensation values of the encoder manufacturer - fourth part (address area: depending on the sensor)
 * @param [in] : address               The address of the parameter you want to set (0->0xFF)
 * @param [in] : param                 New value of the parameter
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 mrsCode is incorrect
 * @retval 4 address is incorrect
 * @retval 5 the component is not programmed as EnDat
 * @retval 6 the sensor is not initialised (initialise it and recall this function)
 * @retval 7 timeout
 * @retval 20 transmission error
 */
int do_CMD_APCI1711_EndatSensorReceiveParameter(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Enable to execute the action "Sensor receive parameter" (see page 19/131 of EnDat specification)
 * The EnDat mode is 0x1C
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex :Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @param [in] mrsCode : The MRS-code corresponding to the parameter you want to set (see page 51/131 of EnDat specification)
 * \li 0xB9:    Operating status (address area: 0x0 - 0x3)
 * \li 0xA1:    Parameters of the encoder manufacturer - first part (address area: 0x4 - 0xF)
 * \li 0xA3:    Parameters of the encoder manufacturer - second part (address area: 0x0 - 0xF)
 * \li 0xA5:    Parameters of the encoder manufacturer - third part (address area: 0x0 - 0xF)
 * \li 0xA7:    Operating parameters (address area: 0x0 - 0xF)
 * \li 0xA9:    Parameters of the OEM - first part (address area: depending on the sensor)
 * \li 0xAB:    Parameters of the OEM - second part (address area: depending on the sensor)
 * \li 0xAD:    Parameters of the OEM - third part (address area: depending on the sensor)
 * \li 0xAF:    Parameters of the OEM - fourth part (address area: depending on the sensor)
 * \li 0xB1:    Compensation values of the encoder manufacturer - first part (address area: depending on the sensor)
 * \li 0xB3:    Compensation values of the encoder manufacturer - second part (address area: depending on the sensor)
 * \li 0xB5:    Compensation values of the encoder manufacturer - third part (address area: depending on the sensor)
 * \li 0xB7:    Compensation values of the encoder manufacturer - fourth part (address area: depending on the sensor)
 * @param [in] : address               The address of the parameter you want to set (0->0xFF)
 * @param [in] : param                 New value of the parameter
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 mrsCode is incorrect
 * @retval 4 address is incorrect
 * @retval 5 the component is not programmed as EnDat
 * @retval 6 the sensor is not initialised (initialise it and recall this function)
 * @retval 7 timeout
 * @retval 20 transmission error
 */
//int do_CMD_APCI1711_EndatSensorReceiveParameter(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Enable to execute the action "Sensor send position value" (see page 19/131 of EnDat specification)
 * The EnDat mode is 0x7
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @param [out] positionLow : Low bits of the position
 * @param [out] positionHigh : High bits of the position
 * @param [out] positionSz : Size of the position in bits
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 the component is not programmed as EnDat
 * @retval 4 the sensor is not initialised (initialise it and recall this function)
 * @retval 5 timeout
 * @retval 20 transmission error
 */
int do_CMD_APCI1711_EndatSensorSendPositionValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------
/**
 * Enable to select the additional data that will be send when using EnDat 2.2 commands that send additional data.
 * Some values are not available on all sensors. If you select a MRS-Code that is not available, you will get the error 13.
 * @param[in] pdev : Pointer to the device
 * @param[in] moduleIndex :	index of the slave (0->3)
 * @param[in] channel :	index of the channel (0->1)
 * @param[in] addInfoCount : The number of selected add info (0->2)
 * @param[in] mrsCodeAI1 : The MRS-Code for the first additional data
 * \li 0x40: Send additional info 1 without data contents
 * \li 0x41: Send diagnostic values
 * \li 0x42: Send position value 2 word 1 LSB
 * \li 0x43: Send position value 2 word 2
 * \li 0x44: Send position value 2 word 3 MSB
 * \li 0x45: Acknowledge memory content LSB
 * \li 0x46: Acknowledge memory content MSB
 * \li 0x47: Acknowledge MRS code
 * \li 0x48: Acknowledge test command
 * \li 0x49: Send test values word 1 LSB
 * \li 0x4A: Send test values word 2
 * \li 0x4B: Send test values word 3 MSB
 * \li 0x4C: Send temperature 1
 * \li 0x4D: Send temperature 2
 * \li 0x4E: Additional sensors
 * @param [in] : mrsCodeAI2    The MRS-Code for the second additional data
 * \li 0x50: Send additional datum 2 without data contents
 * \li 0x51: Send commutation
 * \li 0x52: Send acceleration
 * \li 0x53: Send commutation & acceleration
 * \li 0x54: Send limit position signals
 * \li 0x55: Send limit position signals & acceleration
 * \li 0x56: Asynchronous position value word 1 LSB
 * \li 0x57: Asynchronous position value word 2
 * \li 0x58: Asynchronous position value word 3 MSB
 * \li 0x59: Operating status error sources
 * \li 0x5A: Currently not assigned
 * \li 0x5B: Timestamp
 * @param [in] : mrsCodeAI2    Index of the EnDat channel (0,1)
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 addInfoCount is incorrect
 * @retval 4 mrsCodeAI1 is incorrect
 * @retval 5 mrsCodeAI2 is incorrect
 * @retval 6 the component is not programmed as EnDat
 * @retval 7 The sensor is not compatible with EnDat 2.2 commands
 * @retval 8 the sensor is not initialised (initialise it and recall this function)
 * @retval 9 Error while deactivating the second additional data
 * @retval 10 Error while deactivating the first additional data
 * @retval 11 Error while activating the first additional data
 * @retval 12 Error while activating the second additional data
 * @retval 20 transmission error
 */
int do_CMD_APCI1711_EndatSelectAdditionalData(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
//------------------------------------------------------------------------------

/** Prompts the sensor to send its position value with additional data by sending it the mode command 0x38 (see EnDat specification). Before calling this function, you have to select the additional data that you want by calling the function i_PCIe1711_EndatSelectAdditionalData
 * @param[in] pdev : Pointer to the device
 * @param[in] moduleIndex :	index of the slave (0->3)
 * @param[in] channel :	index of the channel (0->1)
 * @param [out] positionLow :Low bits of the position
 * @param [out] positionHigh : High bits of the position
 * @param [out] positionSz : Size of the position in bits
 * @param [out] addInfo1 : Value of the additional data 1
 * @param [out] addInfo2 : Value of the additional data 2
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 the component is not programmed as EnDat
 * @retval 4 The sensor is not compatible with EnDat 2.2 commands
 * @retval 5 the sensor is not initialised (initialise it and recall this function)
 * @retval 6 timeout
 * @retval 20 transmission error
 */
int do_CMD_APCI1711_EndatSensorSendPositionValueWithAdditionalData(struct pci_dev *pdev, unsigned int cmd, unsigned long arg); 
//------------------------------------------------------------------------------
//SSI---------------------------------------------------------------------------

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
int do_CMD_APCI1710_InitSSI(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_InitSSIRawData(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_Read1SSIValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_Read1SSIRawDataValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_ReadAllSSIValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_ReadAllSSIRawDataValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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

int do_CMD_APCI1710_StartSSIAcquisition(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


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
int do_CMD_APCI1710_GetSSIAcquisitionStatus(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_GetSSIValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_GetSSIRawDataValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_ReadSSI1DigitalInput(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_ReadSSIAllDigitalInput(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

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
int do_CMD_APCI1710_SetSSIDigitalOutputOn(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


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
int do_CMD_APCI1710_SetSSIDigitalOutputOff(struct pci_dev *pdev, unsigned int cmd, unsigned long arg);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Get the board type
 *
 * @param [out] arg : Board type:	APCI1710_BOARD_TYPE
 *								   	PCI3009_BOARD_TYPE
 *									APCIE1711_BOARD_TYPE
 * @retval 0: No error.
 * @retval -EFAULT : Failed to copy data in user space (incorrect pointer).
 */
int do_CMD_APCI1710_GetBoardType (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Write data on the board.
 *
 * This function can be changed, it is recommended NOT TO USE IT!
 *
 * @param [in] arg :	arg[0] : Type of access
 * 						arg[1] : How many bytes (1, 2, 4, 8)
 * 						arg[2] : Offset
 * 						arg[3] : Value
 *
 * @retval 0: No error.
 * @retval -EFAULT : Failed to copy data in user space (incorrect pointer).
 * @retval -EADDRNOTAVAIL : Type of access not available.
 */
int do_CMD_APCI1710_WRITE (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

/** Read data from the board.
 *
 * This function can be changed, it is recommended NOT TO USE IT!
 *
 * @param [in] arg :	arg[0] : Type of access
 * 						arg[1] : How many bytes (1, 2, 4, 8)
 * 						arg[2] : Offset
 *
 * @param [out] arg :
 * 						arg[0] : Value
 *
 * @retval 0: No error.
 * @retval -EFAULT : Failed to copy data in user space (incorrect pointer).
 * @retval -EADDRNOTAVAIL : Type of access not available.
 */
int do_CMD_APCI1710_READ (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//------------------------------------------------------------------------------

#ifdef WITH_BALISE_OPTION	

/** Switch the balise off/on.
 *
 * @param [in] pdev                     : The device to use.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_BaliseOff)     : Balise status to set On / Off (On: 0 / Off: 1)
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a balise module.
 * @retval 4: The selected switch parameter is wrong.
 * @retval -EFAULT : Fail to retrieve user data. 
 */
int do_CMD_APCI1710_SetBaliseOff (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/** Set new balise telegram.
 *
 * @param [in] pdev                     : The device to use.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (telegram_len)	: Length of the telegram (1 to 2048).
 * @param [in] arg[2] (pc_Telegram)		: Telegram (1 to 256 character).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module is wrong.
 * @retval 3: The module is not a balise module.
 * @retval 4: The parameter for telegram length is wrong.
 * @retval 5: The pointer address is not correct.
 * @retval 6: The telegram size is to small.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetBaliseTelegram (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

#endif

//------------------------------------------------------------------------------
#ifdef WITH_IDV_OPTION

/** Initialize the IDV.
 *
 * Configure the IDV operating mode from selected module (b_ModulNbr).
 * You must call this function before you call any other function
 * witch access of IDV.
 *
 * @param [in] pdev                           : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)            : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_Stueck)              : Stueck: (1 to 255).
 * @param [in] arg[2] (b_OutputH)             : Output-H:  0 = Off, 1 = On.
 * @param [in] arg[3] (b_Irq)                 : Interrupt: 0 = Off, 1 = On.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module is not a IDV module.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_InitIdv (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Returns the IDV progress latched status after a index interrupt occur.
 *
 * @param [in] pdev                         : The device to use.
 * @param [in] arg[0] (b_ModulNbr)          : Module number to configure (0 to 3).
 *
 * @param [out] arg[1] (pul_Status)         : 0 : IRQ Lese-Ende, 0 = Nein, 1 = Ja
                                              1 : Stueck, 0 = Aus, 1 = Ein
                                              2 : Eingang E
                                              3 : Eingang F
                                              4 : Eingang G
 * @param [out] arg[2] (pul_MessUnterkante) : IDV-Messunterkante
 * @param [out] arg[3] (pul_MessObjekt)     : IDV-Messobjekt
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: parameter b_ModulNbr is not in the correct range.
 * @retval 3: IDV not initialised see function "i_APCI1710_InitIdv".
 * @retval 4: Interrupt function not initialised. See function "i_APCI1710_SetBoardIntRoutine".
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_GetInterruptIdvStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

//----------------------------------------------------------------------------

/** Sets or resets the digital output H.
 *
 * Sets or resets the digital output H.
 * Setting an output means setting an ouput high.
 * Resetting an output means setting an ouput low.
 *
 * @param [in] pdev                 : The device to use.
 * @param [in] arg[0] (b_ModulNbr)  : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_OnOffFlag) : Set or reset output? 0 = Reset, 1 = Set.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The selected module number is wrong.
 * @retval 3: IDV not initialised see function "i_APCI1710_InitIdv".
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_SetIdvDigitalH (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

#endif

#ifdef WITH_EL_TIMERS_OPTION

/** Initialize the delay and pulse width timers.
 *
 * @param [in] pdev                             : The device to initialize.
 * @param [in] arg[0] (b_ModulNbr)              : Module number to configure (0 to 3).
 * @param [in] arg[1] (dw_DelayTime)            : Delay in hundreds of nanoseconds before generating the pulse (1 step = 100 ns).
 * @param [in] arg[2] (dw_PulseWidth)           : Pulse width in hundreds of nanoseconds (1 step = 100 ns).
 * @param [in] arg[3] (b_OutputLevel)           : 0 : Low active / 1: High active.
 * @param [in] arg[4] (b_HardwareTriggerLevel)  : 0 : Low active / 1: High active.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: Module selection wrong.
 * @retval 3: The module is not a EL TIMER module.
 * @retval 4: Selection from output clock level is wrong.
 * @retval 5: Selection from hardware gate level is wrong.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_ELInitDelayAndPulseWidth (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/** Enable the timers.
 *
 * Once enable, the APCI-1710 is waiting for a trigger signal
 * to start.
 *
 * @param [in] pdev                  : The device to manage.
 * @param [in] arg[0] (b_ModulNbr)   : Module number to configure (0 to 3).
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: Module selection wrong.
 * @retval 3: The module is not a EL TIMER module.
 * @retval 4: Timer not initialised.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_ELEnableTimers (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/** Disable the timer
 *
 * Once disable, the timers are stopped.
 *
 * @param [in] pdev                  : The device to manage.
 * @param [in] arg[0] (b_ModulNbr)   : Module number to configure (0 to 3).
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: Module selection wrong.
 * @retval 3: The module is not a EL TIMER module.
 * @retval 4: Timer not initialised.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_ELDisableTimers (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/** Change the timer value.
 *
 * @param [in] pdev                     : The device to manage.
 * @param [in] arg[0] (b_ModulNbr)      : Module number to configure (0 to 3).
 * @param [in] arg[1] (b_TimerNbr)      : Timer to set (0: Delay timer / 1: Pulse width timer).
 * @param [in] arg[2] (dw_DelayTime)    : Delay in hundreds of nanoseconds before generating the pulse (1 step = 100 ns).
 *
 * @retval 0: No error
 * @retval 1: The handle parameter of the board is wrong
 * @retval 2: Module selection wrong.
 * @retval 3: Timer selection wrong.
 * @retval 4: The module is not a TIMER module.
 * @retval 5: Timer not initialised.
 * @retval -EFAULT : Fail to retrieve user data.
 */
int do_CMD_APCI1710_ELSetNewTimerValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // __apci1710_API_H_
