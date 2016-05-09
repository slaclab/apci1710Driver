/** @file Endat_1711-kapi.c

 Contains Endat Function ioctl functions.

 @par CREATION
 @author Laaziz ElBakali
 @date   10.12.2014

 @par VERSION
 @verbatim
 $LastChangedRevision:$
 $LastChangedDate:$
 @endverbatim

 @par LICENCE
 @verbatim
 Copyright (C) 2013  ADDI-DATA GmbH for the source code of this module.

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

EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user
#endif

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
int do_CMD_APCI1711_EndatInitialiseSensor(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[3];

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
	return -EFAULT;

	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
	return i_APCI1711_EndatInitialiseSensor(pdev,
	                                        (uint8_t)argArray[0], // moduleIndex
	                                        (uint8_t)argArray[1],// channel
	                                        (uint32_t)argArray[2]);// freqValue
	return -ENOSYS;
}

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
int do_CMD_APCI1711_EndatSensorReceiveReset(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t argArray[2] = {0};

	if (copy_from_user(argArray, (uint8_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device != apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;

	return i_APCI1711_EndatSensorReceiveReset(pdev,
	                                          argArray[0],   	// moduleIndex
											  argArray[1]); 	// channel

}

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
int do_CMD_APCI1711_EndatResetErrorBits(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t argArray[2]  = {0};

	if (copy_from_user(argArray, (uint8_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device != apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;

	return i_APCI1711_EndatResetErrorBits(pdev,
	                                      argArray[0], // moduleIndex
	                                      argArray[1]); // channel

}

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
int do_CMD_APCI1711_EndatGetErrorSources(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t argArray[13] = {0};
	int ret = 0;

	if (copy_from_user(argArray, (uint8_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device != apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;

	ret = i_APCI1711_EndatGetErrorSources(pdev,
	                                      argArray[0], // moduleIndex
	                                      argArray[1], // channel
	                                      &argArray[2], //errorSrc1
	                                      &argArray[3], //errorSrc2
	                                      &argArray[4], //errorSrc3
	                                      &argArray[5], //errorSrc4
	                                      &argArray[6], //errorSrc7
	                                      &argArray[7], //errorSrc8
	                                      &argArray[8], //errorSrc9
	                                      &argArray[9], //errorSrc10
	                                      &argArray[10], //errorSrc11
	                                      &argArray[11], //errorSrc12
	                                      &argArray[12]); //errorSrc12

	if (ret != 0)
		return ret;

	if (copy_to_user((uint8_t __user *)arg , argArray, sizeof(argArray)))
		return -EFAULT;

	return 0;
}

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
int do_CMD_APCI1711_EndatSelectMemoryArea(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[3] = {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device != apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;

	return  i_APCI1711_EndatSelectMemoryArea(pdev,
	                                          (uint8_t) argArray[0], // moduleIndex
	                                          (uint8_t) argArray[1],// channel
	                                          argArray[2]);// mrsCode
}

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
int do_CMD_APCI1711_EndatSensorSendPositionAndRecvMemArea(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[3] = {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device != apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;

	return i_APCI1711_EndatSensorSendPositionAndRecvMemArea(pdev,
	                                                        (uint8_t) argArray[0], // moduleIndex
	                                                        (uint8_t) argArray[1], // channel
	                                                        argArray[2]); // mrsCode

}

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
int do_CMD_APCI1711_EndatSensorSendParameter(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int returnValue = 0;

	uint32_t argArray[5] = {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;


	returnValue =  i_APCI1711_EndatSensorSendParameter(pdev,
												(uint32_t)argArray[0], // moduleIndex
												(uint32_t)argArray[1], // channel
												argArray[2], // mrsCode
												argArray[3], // address
												&argArray[4]); // param

	if (returnValue != 0)
		return returnValue;

	if (copy_to_user((uint32_t __user *)arg , argArray, sizeof(argArray)))
		return -EFAULT;

	return 0;

}

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
int do_CMD_APCI1711_EndatSensorReceiveParameter(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[5] = {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device != apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;

	return  i_APCI1711_EndatSensorReceiveParameter(pdev,
	                                          (uint8_t) argArray[0], // moduleIndex
	                                          (uint8_t) argArray[1],// channel
	                                          argArray[2],// mrsCode
											  argArray[3],// address
											  argArray[4]);// param
}

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
int do_CMD_APCI1711_EndatSensorSendPositionValue(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int returnValue = 0;
	uint32_t argArray[5] = {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;

	returnValue =  i_APCI1711_EndatSensorSendPositionValue(pdev,
	                                                       (uint8_t)argArray[0],   // moduleIndex
	                                                       (uint8_t)argArray[1],   // channel
		                                                   &argArray[2],  // positionLow
		                                                   &argArray[3],  // positionHigh
		                                                   &argArray[4]); // positionSz

	if (returnValue != 0)
		return returnValue;

	if (copy_to_user((uint32_t __user *)arg , argArray, sizeof(argArray)))
		return -EFAULT;

	return 0;

}

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
int do_CMD_APCI1711_EndatSelectAdditionalData(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{

	uint32_t argArray[5] = {0};
	int returnValue = 0;

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;

	returnValue = i_APCI1711_EndatSelectAdditionalData(pdev,
														(uint8_t) argArray[0], // moduleIndex
														(uint8_t) argArray[1], // channel
														(uint8_t) argArray[2], // addDataCount
														(uint32_t)argArray[3], // mrsCodeAI1
														(uint32_t)argArray[4]); // mrsCodeAI2

	if (returnValue != 0)
		return returnValue;

	return 0;

}

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
int do_CMD_APCI1711_EndatSensorSendPositionValueWithAdditionalData(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int returnValue = 0;
	uint32_t argArray[7]= {0};

	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;

	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
		return -ENOSYS;

		returnValue = i_APCI1711_EndatSensorSendPositionValueWithAdditionalData(pdev,
																			(uint8_t) argArray[0], // moduleIndex
																			(uint8_t) argArray[1], // channel
																			&argArray[2], // positionLow
																			&argArray[3], // positionHigh
																			&argArray[4], // positionSz
																			&argArray[5], // addInfo1
																			&argArray[6]); // addInfo2

		if (returnValue != 0)
			return returnValue;

		if (copy_to_user((uint32_t __user *)arg , argArray, sizeof(argArray)))
			return -EFAULT;

		return 0;

}

