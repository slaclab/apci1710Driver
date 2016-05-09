/** @file Endat_1711-kapi.c

 Contains Endat kernel functions.

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

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
static __inline__ unsigned long msecs_to_jiffies(unsigned long msecs)
{
	return ((HZ * msecs + 999) / 1000);
}
#endif

EXPORT_SYMBOL( i_APCI1711_EndatInitialiseSensor);
EXPORT_SYMBOL( i_APCI1711_EndatSensorReceiveReset);
EXPORT_SYMBOL( i_APCI1711_EndatResetErrorBits);
EXPORT_SYMBOL( i_APCI1711_EndatGetErrorSources);
EXPORT_SYMBOL( i_APCI1711_EndatSelectMemoryArea);
EXPORT_SYMBOL( i_APCI1711_EndatSensorSendPositionAndRecvMemArea);
EXPORT_SYMBOL( i_APCI1711_EndatSensorSendParameter);
EXPORT_SYMBOL( i_APCI1711_EndatSensorReceiveParameter);
EXPORT_SYMBOL( i_APCI1711_EndatSensorSendPositionValue);
EXPORT_SYMBOL( i_APCI1711_EndatSelectAdditionalData);
EXPORT_SYMBOL( i_APCI1711_EndatSensorSendPositionValueWithAdditionalData);

EXPORT_NO_SYMBOLS;

#define WINDOWS_TO_LINUX_OFFSET		4

/** Wait the end of the transmission.
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @retval 0: Transmission conclude
 * @retval 1: Timeout
 */
static __inline__ int WaitEndOfTransmission(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint32_t ms)
{
	unsigned long timeout = jiffies + msecs_to_jiffies(ms);

	for (;;)
	{
		if (((readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 1) * WINDOWS_TO_LINUX_OFFSET) >> 22) & 1) != 1)
			return 0;

		if (time_after_eq(jiffies, timeout))
			break;

	}

	return 1; /* timeout */
}

/**
 * Send a command
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @param [in] modeCommand : EnDat mode (ex: 0x07, 0x0E, ...) See EnDat specification page 19/131
 * @param [in] mrsCode : EnDat mrs code (ex: 0xA1, 0xA3, ...) See EnDat specification page 31/131, 51/131, ...
 * @param [in] address : Address (usefull when getting/writting parameter) See EnDat specification page 51/131
 * @param [in] cmd : Command to send
 * @retval 0 success
 * @retval 1 timeout while sending
 */
unsigned long Primary_EndatSendCommand(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint32_t modeCommand, uint32_t mrsCode, uint32_t address, uint32_t cmd)
{
	uint32_t aiCount = 0;

	/*
	 * get the number of add info.
	 * will be used by the state machine of the PLD
	 */
	aiCount = (readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET)) & 3;

	/* write the mrs code and the address */
	writel(((mrsCode << 24) + (address << 16)), APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) (64 * moduleIndex + (32 * channel) + 4) * WINDOWS_TO_LINUX_OFFSET);

	/* write the mode command */
	writel((modeCommand << 2) + aiCount, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) (64 * moduleIndex + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET);

	/* write the command that will be send to the sensor */
	writel(cmd, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) (64 * moduleIndex + (32 * channel) + 2) * WINDOWS_TO_LINUX_OFFSET);

	/* start the transmission */
	writel(1, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) (64 * moduleIndex + (32 * channel) + 11) * WINDOWS_TO_LINUX_OFFSET);

	/* wait the end of the transmission */
	if (WaitEndOfTransmission(pdev, moduleIndex, channel, 1000) != 0)
	{
		/* timeout */
		writel(0x2, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 11) * WINDOWS_TO_LINUX_OFFSET);

		/* delay of 30 ms - as described in the specification */
		mdelay(30);

		writel(0x0, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 11) * WINDOWS_TO_LINUX_OFFSET);
		return 1;
	}

	/* delay - as asked by the device manufacturor for compatibility with old devices */
	mdelay(1);

	return 0;
}

/**
 * Send a command
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @param [in] modeCommand : EnDat mode (ex: 0x07, 0x0E, ...) See EnDat specification page 19/131
 * @param [in] mrsCode : EnDat mrs code (ex: 0xA1, 0xA3, ...) See EnDat specification page 31/131, 51/131, ...
 * @param [in] addres : Address (usefull when getting/writting parameter) See EnDat specification page 51/131
 * @param [in] cmd  : Command to send
 * @param [in] extraCmd : Command sent directly after the first one
 * @retval 0 success
 * @retval 1 timeout while sending
 */
unsigned long Primary_EndatSendCommandWithExtraCmd(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel,uint32_t modeCommand,uint32_t mrsCode,
                                                   uint32_t address, uint32_t cmd,uint32_t extraCmd)
{
	uint32_t aiCount = 0;

	/*
	 * get the number of add info.
	 * will be used by the state machine of the PLD
	 */
	aiCount = (readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) ((64 * moduleIndex) + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET)) & 3;

	/* write the mrs code and the address */
	writel(((mrsCode << 24) + (address << 16)), APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) ((64 * moduleIndex + (32 * channel) + 4) * WINDOWS_TO_LINUX_OFFSET));

	/* write the mode command */
	writel((modeCommand << 2) + aiCount, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) ((64 * moduleIndex + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET));

	/* write the command that will be send to the sensor */
	writel(cmd, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) ((64 * moduleIndex + (32 * channel) + 2) * WINDOWS_TO_LINUX_OFFSET));

	/* start the transmission */
	writel(1, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) ((64 * moduleIndex + (32 * channel) + 11) * WINDOWS_TO_LINUX_OFFSET));

	/* write the extra cmd */
	writel(extraCmd, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) ((64 * moduleIndex + (32 * channel) + 2) * WINDOWS_TO_LINUX_OFFSET));

	/* wait the end of the transmission */
	if (WaitEndOfTransmission(pdev, moduleIndex, channel, 1000) != 0)
	{
		/* timeout */
		writel(0x2, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 11) * WINDOWS_TO_LINUX_OFFSET);

		/* delay of 30 ms - as described in the specification */
		mdelay(30);

		writel(0x0, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 11) * WINDOWS_TO_LINUX_OFFSET);
		return 1;
	}

	/* delay - as asked by the device manufacturor for compatibility with old devices */
	mdelay(1);

	return 0;
}

/**
 * This function checks if the sensor is initialised
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @retval 0 The sensor is NOT initialised
 * @retval 1 The sensor is initialised
 */
static int IsSensorInitialised(struct pci_dev *pdev, unsigned char moduleIndex, unsigned char channel)
{
	return APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel];
}

/**
 * This function checks if the sensor is compatible with EnDat 2.2 mode
 * @param [in] pdev : Pointer to the device
 * @param [in] moduleIndex : Index of the slave (0->3)
 * @param [in] channel : Index of the EnDat channel (0,1)
 * @retval 0 Sensor is not compatible with EnDat 2.2
 * @retval 1 Sensor is compatible with Endat 2.2
 */
static int AllowEnDat22Command(struct pci_dev *pdev, unsigned char moduleIndex, unsigned char channel)
{

	unsigned long registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 1) * WINDOWS_TO_LINUX_OFFSET);

	if (((registerContent >> 23) & 1) == 1)
	{
		/* allowed */
		return 1;
	}
	else
	{
		/* not allowed */
		return 0;
	}
}


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
int i_APCI1711_EndatInitialiseSensor(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint32_t freqValue)
{
	uint32_t param = 0;
	uint32_t registerValue = 0;
	uint8_t freqTstValue = 124;
	uint32_t error = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/*Check the Channel*/
	if (channel > 1)
		return 2;

	/*Check the frekency*/
	if ((freqValue != 250) && (freqValue != 500) && (freqValue != 800) && (freqValue != 1000) && (freqValue != 1250) && (freqValue != 2000) && (freqValue != 2500) && (freqValue != 4000)
	        && (freqValue != 5000) && (freqValue != 6666))
		return 3;

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 4;

	/*reset the sensor*/
	if (i_APCI1711_EndatSensorReceiveReset(pdev, moduleIndex, channel) != 0)
		return 5;

	/* delay of 50 ms - as described in the specification */
	mdelay(50);

	/* set the sensor as initialised - else the access function will return an error */
	APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 1;

	/* select the memory space 0xB9 */
	if (i_APCI1711_EndatSelectMemorySpace(pdev, moduleIndex, channel, 0xB9) != 0)
	{
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 6;
	}

	/* read the alarm space */
	if (i_APCI1711_EndatSensorSendParameter(pdev, moduleIndex, channel, 0xB9, 0x0, &param) != 0)
	{
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 7;
	}

	/* read the warning space */
	if (i_APCI1711_EndatSensorSendParameter(pdev, moduleIndex, channel, 0xB9, 0x1, &param) != 0)
	{
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 8;
	}

	/* clear the alarm */
	if (i_APCI1711_EndatSensorReceiveParameter(pdev, moduleIndex, channel, 0xB9, 0x0, 0x0) != 0)
	{
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 9;
	}

	/* clear the warning */
	if (i_APCI1711_EndatSensorReceiveParameter(pdev, moduleIndex, channel, 0xB9, 0x1, 0x0) != 0)
	{
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 10;
	}

	/* select the memory space 0xA1 */
	if (i_APCI1711_EndatSelectMemorySpace(pdev, moduleIndex, channel, 0xA1) != 0)
	{
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 11;
	}

	/* read the with of a position value */
	if (i_APCI1711_EndatSensorSendParameter(pdev, moduleIndex, channel, 0xA1, 0xD, &param) != 0)
	{
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 12;
	}

	/* select the memory space 0xA5 */
	if (i_APCI1711_EndatSelectMemorySpace(pdev, moduleIndex, channel, 0xA5) != 0)
	{
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 13;
	}

	/* read the EnDat type */
	if (i_APCI1711_EndatSensorSendParameter(pdev, moduleIndex, channel, 0xA5, 0x5, &param) != 0)
	{
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 14;
	}

	/* assign freqValue for each frequency (counter value) */
	switch (freqValue)
	{
	case 250:
		registerValue = (79 << 8) + freqTstValue;
		break;
	case 500:
		registerValue = (39 << 8) + freqTstValue;
		break;
	case 800:
		registerValue = (24 << 8) + freqTstValue;
		break;
	case 1000:
		registerValue = (19 << 8) + freqTstValue;
		break;
	case 1250:
		registerValue = (15 << 8) + freqTstValue;
		break;
	case 2000:
		registerValue = (9 << 8) + freqTstValue;
		break;
	case 2500:
		registerValue = (7 << 8) + freqTstValue;
		break;
	case 4000:
		registerValue = (4 << 8) + freqTstValue;
		break;
	case 5000:
		registerValue = (3 << 8) + freqTstValue;
		break;
	case 6666:
		registerValue = (2 << 8) + freqTstValue;
		break;
	default:
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;
		return 15;
	}

	/* write it in the PLD */
	writel(registerValue, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 0) * WINDOWS_TO_LINUX_OFFSET);

	/* check the error bit */
	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);
	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}

	return 0;
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
int i_APCI1711_EndatSensorReceiveReset(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel)
{
	uint32_t error = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 3;

	/* reset the initialised state */
	APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_EndatModuleInfo.sensorInitialized[channel] = 0;

	/* reset the frequency */
	writel((124 << 8) + 124, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 0) * WINDOWS_TO_LINUX_OFFSET);

	/* reset information on mode command */
	writel(0, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET);

	/* send the 0x2A command */
	if (Primary_EndatSendCommand(pdev, moduleIndex, channel, 0x2A, 0, 0, (0x2A << 24)) != 0)
	{
		/* timeout */
		return 4;
	}

	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);
	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}

	return 0;
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
int i_APCI1711_EndatResetErrorBits(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel)
{
	uint32_t error = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 3;

	/* reset the error bits, by writing one */
	writel(1, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);

	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);
	printk ("error 0x%x\n", error);
	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}

	return 0;

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
int i_APCI1711_EndatGetErrorSources(struct pci_dev *pdev,
                                    uint8_t moduleIndex,
                                    uint8_t channel,
                                    uint8_t *errorSrc1,
                                    uint8_t *errorSrc2,
                                    uint8_t *errorSrc3,
                                    uint8_t *errorSrc4,
                                    uint8_t *errorSrc7,
                                    uint8_t *errorSrc8,
                                    uint8_t *errorSrc9,
                                    uint8_t *errorSrc10,
                                    uint8_t *errorSrc11,
                                    uint8_t *errorSrc12,
                                    uint8_t *errorSrc13)

{
	uint32_t registerContent = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 3;

	/* read the status register */
	registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);
	*errorSrc1 = (unsigned char) ((registerContent & 0x1));
	*errorSrc2 = (unsigned char) ((registerContent & 0x2) >> 1);
	*errorSrc3 = (unsigned char) ((registerContent & 0x4) >> 2);
	*errorSrc4 = (unsigned char) ((registerContent & 0x8) >> 3);
	*errorSrc7 = (unsigned char) ((registerContent & 0x10) >> 4);
	*errorSrc8 = (unsigned char) ((registerContent & 0x40) >> 6);
	*errorSrc9 = (unsigned char) ((registerContent & 0x80) >> 7);
	*errorSrc10 = (unsigned char) ((registerContent & 0x100) >> 8);
	*errorSrc11 = (unsigned char) ((registerContent & 0x200) >> 9);
	*errorSrc12 = (unsigned char) ((registerContent & 0x400) >> 10);
	*errorSrc13 = (unsigned char) ((registerContent & 0x800) >> 11);

	return 0;
}

/** Enable to execute the action "Select memory area" (see page 19/131 of EnDat specification)
 * The EnDat mode is 0xE
 * @param[in] pdev : Pointer to the device
 * @param[in] moduleIndex :	Index of the slave (0->3)
 * @param[in] channel :	Index of the EnDat channel (0,1)
 * @param[in] mrsCode :	The MRS-code corresponding to the memory area you want to select (see page 31/131 and 51/131 of EnDat specification)
 * \li 0xB9:	Operating status (address area: 0x0 - 0x3)
 * \li 0xA1:	Parameters of the encoder manufacturer - first part (address area: 0x4 - 0xF)
 * \li 0xA3:	Parameters of the encoder manufacturer - second part (address area: 0x0 - 0xF)
 * \li 0xA5:	Parameters of the encoder manufacturer - third part (address area: 0x0 - 0xF)
 * \li 0xA7:	Operating parameters (address area: 0x0 - 0xF)
 * \li 0xA9:	Parameters of the OEM - first part (address area: depending on the sensor)
 * \li 0xAB:	Parameters of the OEM - second part (address area: depending on the sensor)
 * \li 0xAD:	Parameters of the OEM - third part (address area: depending on the sensor)
 * \li 0xAF:	Parameters of the OEM - fourth part (address area: depending on the sensor)
 * \li 0xB1:	Compensation values of the encoder manufacturer - first part (address area: depending on the sensor)
 * \li 0xB3:	Compensation values of the encoder manufacturer - second part (address area: depending on the sensor)
 * \li 0xB5:	Compensation values of the encoder manufacturer - third part (address area: depending on the sensor)
 * \li 0xB7:	Compensation values of the encoder manufacturer - fourth part (address area: depending on the sensor)
 *
 * @retval 0 success
 * @retval 1 moduleIndex is incorrect
 * @retval 2 channel is incorrect
 * @retval 3 mrsCode is incorrect
 * @retval 4 the component is not programmed as EnDat
 * @retval 5 the sensor is not initialized (initialize it and recall this function)
 * @retval 6 timeout
 * @retval 20 transmission error
 */
int i_APCI1711_EndatSelectMemorySpace(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint32_t mrsCode)
{
	uint32_t error = 0;

	const int validMrsCodeSz = 13; /* if you change this value, please also change validMrsCode definition */
	const uint32_t validMrsCode[13] = {0xB9, 0xA1, 0xA3, 0xA5, 0xA7, 0xA9, 0xAB, 0xAD, 0xAF, 0xB1, 0xB3, 0xB5, 0xB7};
	int i = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	for (i = 0; i < validMrsCodeSz; i++)
	{
		if (mrsCode == validMrsCode[i])
		{
			break;
		}
	}

	if (i >= validMrsCodeSz)
	{
		return 3;
	}

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 4;

	/* check if the sensor is initialized */
	if (IsSensorInitialised(pdev, moduleIndex, channel) != 1)
	{
		return 5;
	}

	/* send the 0xE command */
	if (Primary_EndatSendCommand(pdev, moduleIndex, channel, 0xE, mrsCode, 0, ((0xE << 24) + (mrsCode << 16))) != 0)
	{
		/* timeout */
		return 6;
	}

	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);
	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}
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
int i_APCI1711_EndatSelectMemoryArea(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint32_t mrsCode)
{
	const int validMrsCodeSz = 13; /* if you change this value, please also change validMrsCode definition */
	const uint32_t validMrsCode[13] = {0xB9, 0xA1, 0xA3, 0xA5, 0xA7, 0xA9, 0xAB, 0xAD, 0xAF, 0xB1, 0xB3, 0xB5, 0xB7};
	int i = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	for (i = 0; i < validMrsCodeSz; i++)
	{
		if (mrsCode == validMrsCode[i])
		{
			break;
		}
	}

	if (i >= validMrsCodeSz)
	{
		return 3;
	}

	if (i_APCI1711_EndatSelectMemorySpace(pdev, moduleIndex, channel, mrsCode) != 0)
		return 4;

	return 0;
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
int i_APCI1711_EndatSensorSendPositionAndRecvMemArea(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint32_t mrsCode)
{
	uint32_t error = 0;

	const int validMrsCodeSz = 16; /* if you change this value, please also change validMrsCode definition */
	const uint32_t validMrsCode[16] = {0xB9, 0xA1, 0xA3, 0xA5, 0xA7, 0xA9, 0xAB, 0xAD, 0xAF, 0xB1, 0xB3, 0xB5, 0xB7, 0xBD, 0xBF, 0xBB};
	int i = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 3;

	for (i = 0; i < validMrsCodeSz; i++)
	{
		if (mrsCode == validMrsCode[i])
		{
			break;
		}
	}

	if (i >= validMrsCodeSz)
	{
		return 4;
	}

	/* check if the sensor is initialised */
	if (IsSensorInitialised(pdev, moduleIndex, channel) != 1)
	{
		return 5;
	}

	/* send the 0x09 command */
	if (Primary_EndatSendCommand(pdev, moduleIndex, channel, 0x09, mrsCode, 0, ((0x09 << 24) + (mrsCode << 16))) != 0)
	{
		/* timeout */
		return 6;
	}

	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);

	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}
	return 0;

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
int i_APCI1711_EndatSensorSendParameter(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint32_t mrsCode, uint32_t address, uint32_t *param)
{
	uint32_t error = 0;

	const int validMrsCodeSz = 16; /* if you change this value, please also change validMrsCode definition */
	const unsigned long validMrsCode[16] = {0xB9, 0xA1, 0xA3, 0xA5, 0xA7, 0xA9, 0xAB, 0xAD, 0xAF, 0xB1, 0xB3, 0xB5, 0xB7, 0xBD, 0xBF, 0xBB};
	int i = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	for (i = 0; i < validMrsCodeSz; i++)
	{
		if (mrsCode == validMrsCode[i]) // ---- value of mrsCode
		{
			break;
		}
	}

	if (i >= validMrsCodeSz)
	{
		return 3;
	}

	if (address > 0xFF)
	{
		return 4;
	}

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 5;

	/* check if the sensor is initialised */
	if (IsSensorInitialised(pdev, moduleIndex, channel) != 1)
	{
		return 6;
	}

	if ((!AllowEnDat22Command(pdev, moduleIndex, channel)) && ((mrsCode == 0xBD) || (mrsCode == 0xBF) || (mrsCode == 0xBB)))
	{
		return 7;
	}

	/* send the 0x23 command */
	if (Primary_EndatSendCommand(pdev, moduleIndex, channel, 0x23, mrsCode, address, ((0x23 << 24) + (address << 16))) != 0)
	{
		/* timeout */
		return 8;
	}

	/* delay of 10 ms - as described in the specification */
	mdelay(10);

	/* read the answer */
	*param = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 7) * WINDOWS_TO_LINUX_OFFSET);
	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);

	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}

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
int i_APCI1711_EndatSensorReceiveParameter(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint32_t mrsCode, uint32_t address, uint32_t param)
{
	uint32_t error = 0;

	const int validMrsCodeSz = 13; /* if you change this value, please also change validMrsCode definition */
	const uint32_t validMrsCode[13] = {0xB9, 0xA1, 0xA3, 0xA5, 0xA7, 0xA9, 0xAB, 0xAD, 0xAF, 0xB1, 0xB3, 0xB5, 0xB7};
	int i = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	for (i = 0; i < validMrsCodeSz; i++)
	{
		if (mrsCode == validMrsCode[i])
		{
			break;
		}
	}

	if (i >= validMrsCodeSz)
	{
		return 3;
	}

	if (address > 0xFF)
	{
		return 4;
	}

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 5;

	/* check if the sensor is initialised */
	if (IsSensorInitialised(pdev, moduleIndex, channel) != 1)
	{
		return 6;
	}

	/* send the 0x1C command */
	if (Primary_EndatSendCommand(pdev, moduleIndex, channel, 0x1C, mrsCode, address, ((0x1C << 24) + (address << 16)) + param) != 0)
	{
		/* timeout */
		return 7;
	}

	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);

	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}
	return 0;

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
int i_APCI1711_EndatSensorSendPositionValue(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint32_t * positionLow, uint32_t * positionHigh, uint32_t * positionSz)
{
	uint32_t error = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 3;

	/* check if the sensor is initialised */
	if (IsSensorInitialised(pdev, moduleIndex, channel) != 1)
	{
		return 4;
	}

	/* send the 0x07 command */
	if (Primary_EndatSendCommand(pdev, moduleIndex, channel, 0x7, 0, 0, (0x7 << 24)) != 0)
	{
		/* timeout */
		return 5;
	}

	/* read and return the values */
	*positionLow = (uint32_t) readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 5) * WINDOWS_TO_LINUX_OFFSET);
	*positionHigh = (uint32_t) readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 6) * WINDOWS_TO_LINUX_OFFSET);
	*positionSz = (uint32_t) readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 12) * WINDOWS_TO_LINUX_OFFSET);

	/* read the answer */
	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (64 * moduleIndex) + (32 * channel) + 13);

	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}

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
int i_APCI1711_EndatSelectAdditionalData(struct pci_dev *pdev, uint8_t moduleIndex, uint8_t channel, uint8_t addInfoCount, uint32_t mrsCodeAI1, uint32_t mrsCodeAI2)
{
	uint32_t registerContent = 0;
	uint32_t currentAddInfoCount = 0;
	uint8_t deactivateFirst = 0;
	uint8_t deactivateSecond = 0;
	uint8_t activateFirst = 0;
	uint8_t activateSecond = 0;
	uint32_t error = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	/* check addInfoCount */
	if (addInfoCount > 2)
		return 3;

	/*
	 * MRS-Code for add information are defined page 34/131 of the EnDat specification
	 * We do not allow the MRS-Code to deactivate the add information, since it is directly handled by the driver
	 * */
	if ((addInfoCount >= 1) && ((mrsCodeAI1 < 0x40) || (mrsCodeAI1 > 0x4E)))
	{
		return 4;
	}
	if ((addInfoCount == 2) && ((mrsCodeAI2 < 0x50) || (mrsCodeAI2 > 0x5B)))
	{
		return 5;
	}

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 6;

	/* check if the sensor allows 2.2 command */
	if (AllowEnDat22Command(pdev, moduleIndex, channel) != 1)
	{
		return 7;
	}

	/* check if the sensor is initialised */
	if (IsSensorInitialised(pdev, moduleIndex, channel) != 1)
	{
		return 8;
	}

	/* get the actual number of add info */
	registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET);
	currentAddInfoCount = registerContent & 0x3;

	/*
	 * activate or deactivate AI depending on what was selected before
	 * to deactivate the first add info, specification says to use code 0x4F
	 * to deactivate the second add info, specification says to use code 0x5F
	 * */
	switch (addInfoCount)
	{
	case 0:
		/* user does not want add info */
		if (currentAddInfoCount == 2)
		{
			deactivateFirst = 1;
			deactivateSecond = 1;
		}
		if (currentAddInfoCount == 1)
		{
			deactivateFirst = 1;
		}
		break;

	case 1:
		/* user only wants one info */
		if (currentAddInfoCount == 2)
		{
			deactivateSecond = 1;
		}
		activateFirst = 1;
		break;

	case 2:
		activateFirst = 1;
		activateSecond = 1;
		break;

	}

	/*
	 * do we have to deactivate the second add info ?
	 * deactivate means that we have to tell the sensor to not send the second add info anymore
	 * */
	if (deactivateSecond == 1)
	{
		/* the MRS-Code 0x5F tells the sensor to not send the add info 2 anymore */
		if (Primary_EndatSendCommandWithExtraCmd(pdev, moduleIndex, channel, 0x9, 0, 0, (0x9 << 24), (0x5F << 16)))
		{
			return 9;
		}

		/* set the number of ai to 1 */
		writel((0x9 << 2) + 1, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET);

		/* save the mrs code for the ai 2 */
		registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 8) * WINDOWS_TO_LINUX_OFFSET);
		writel((registerContent & 0xFF00) + 0x5F, (APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) (64 * moduleIndex + (32 * channel) + 8) * WINDOWS_TO_LINUX_OFFSET));
	}

	/*
	 * do we have to deactivate the first add info ?
	 * deactivate means that we have to tell the sensor to not send the second add info anymore
	 * */
	if (deactivateFirst == 1)
	{
		/* the MRS-Code 0x4F tells the sensor to not send the add info 1 anymore */
		if (Primary_EndatSendCommandWithExtraCmd(pdev, moduleIndex, channel, 0x9, 0, 0, (0x9 << 24), (0x4F << 16)))
		{
			return 10;
		}

		/* set the number of ai to 0 */
		writel((0x9 << 2), APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET);

		/* save the mrs code for the ai 1 */
		registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 8) * WINDOWS_TO_LINUX_OFFSET);
		writel((registerContent & 0xFF) + (0x4F << 8), APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) ((64 * moduleIndex + (32 * channel) + 8) * WINDOWS_TO_LINUX_OFFSET));
	}

	/* do we have to activate the first add info ? */
	if (activateFirst == 1)
	{
		if (Primary_EndatSendCommandWithExtraCmd(pdev, moduleIndex, channel, 0x9, 0, 0, (0x9 << 24), (mrsCodeAI1 << 16)))
		{
			return 11;
		}

		/* save the mrs code for the ai 1 */
		registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 8) * WINDOWS_TO_LINUX_OFFSET);
		writel((registerContent & 0xFF) + (mrsCodeAI1 << 8), APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) (64 * moduleIndex + (32 * channel) + 8) * WINDOWS_TO_LINUX_OFFSET);

		/* update current ai count ? */
		if (currentAddInfoCount == 0)
		{
			writel((0x9 << 2) + 1, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) (64 * moduleIndex + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET);
		}
	}

	/* do we have to activate the second add info ? */
	if (activateSecond == 1)
	{
		if (Primary_EndatSendCommandWithExtraCmd(pdev, moduleIndex, channel, 0x9, 0, 0, (0x9 << 24), (mrsCodeAI2 << 16)))
		{
			return 12;
		}

		/* save the mrs code for the ai 2 */
		registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 8)* WINDOWS_TO_LINUX_OFFSET);
		writel((registerContent & 0xFF00) + mrsCodeAI2, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) (64 * moduleIndex + (32 * channel) + 8 * 4) * WINDOWS_TO_LINUX_OFFSET);

		/* update current ai count ? */
		if ((currentAddInfoCount == 0) || (currentAddInfoCount == 1))
		{
			writel((0x9 << 2) + 2, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (uint32_t) (64 * moduleIndex + (32 * channel) + 3) * WINDOWS_TO_LINUX_OFFSET);
		}
	}

	/* reset the AI1 value */
	writel(0, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 9) * WINDOWS_TO_LINUX_OFFSET);
	/* reset the AI2 value */
	writel(0, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 10) * WINDOWS_TO_LINUX_OFFSET);

	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);

	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}
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
int i_APCI1711_EndatSensorSendPositionValueWithAdditionalData(struct pci_dev *pdev,
                                                              uint8_t moduleIndex,
                                                              uint8_t channel,
                                                              uint32_t *positionLow,
                                                              uint32_t *positionHigh,
                                                              uint32_t *positionSz,
                                                              uint32_t *addInfo1,
                                                              uint32_t *addInfo2)
{
	uint32_t registerContent = 0;
	uint32_t error = 0;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	/* check the channel */
	if (channel > 1)
		return 2;

	/* confirm that the slave is configured for Endat */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != PCIE1711_ENDAT)
		return 3;

	/* check if the sensor allows 2.2 command */
	if (AllowEnDat22Command(pdev, moduleIndex, channel) != 1)
	{
		return 4;
	}

	/* check if the sensor is initialised */
	if (IsSensorInitialised(pdev, moduleIndex, channel) != 1)
	{
		return 5;
	}

	/* send the 0x38 command */
	if (Primary_EndatSendCommand(pdev, moduleIndex, channel, 0x38, 0, 0, (0x38 << 24)) != 0)
	{
		/* timeout */
		return 6;
	}

	/* read and return the values */
	*positionLow = (uint32_t) readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 5) * WINDOWS_TO_LINUX_OFFSET);
	*positionHigh = (uint32_t) readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 6) * WINDOWS_TO_LINUX_OFFSET);
	*positionSz = (uint32_t) readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 12) * WINDOWS_TO_LINUX_OFFSET);

	/* then the add info */
	registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 9) * WINDOWS_TO_LINUX_OFFSET);
	*addInfo1 = (uint32_t) (registerContent & 0x1FFFFF);
	registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 10) * WINDOWS_TO_LINUX_OFFSET);
	*addInfo2 = (uint32_t) (registerContent & 0x1FFFFF);

	error = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + ((64 * moduleIndex) + (32 * channel) + 13) * WINDOWS_TO_LINUX_OFFSET);

	if ((error & 0x00000FDF) != 0)
	{
		return 20;
	}

	return 0;

}

