/** @file biss.c

   Contains BiSS ioctl functions.

   @par CREATION
   @author Lambert Sartory
   @date   09.01.2013

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
    77836 Rheinmünster
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

/** Initialise the master and the slave(s) for single cycle read / write.
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
int do_CMD_APCI1710_BissMasterInitSingleCycle(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t argArray[18];
	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;
	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
		return i_APCI1711_BissMasterInitSingleCycle(pdev,
		                                            (uint8_t)argArray[0],  // moduleIndex
		                                            (uint16_t)argArray[1], // sensorDataFreqDivisor
		                                            (uint16_t)argArray[2], // registerDataFreqDivisor
		                                            (uint8_t)argArray[3],  // channel0BISSSSIMode
		                                            (uint8_t)argArray[4],  // channel0BissMode
		                                            (uint8_t)argArray[5],  // channel1BISSSSIMode
		                                            (uint8_t)argArray[6],  // channel1BissMode
		                                            (uint8_t)argArray[7],  // nbrOfSlave
		                                            (uint8_t *)&argArray[8],   // channel[6]
		                                            (uint8_t *)&argArray[10],  // dataLength[6]
		                                            (uint8_t *)&argArray[12],  // option[6]
		                                            (uint8_t *)&argArray[14],  // CRCPolynom[6]
		                                            (uint8_t *)&argArray[16]); // CRCInvert[6]
	return -ENOSYS;	// BiSS for APCI-1710 is not yet implemented
}

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
int do_CMD_APCI1710_BissMasterSingleCycleDataRead(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int returnValue;
	uint32_t argArray[4];
	if (copy_from_user(argArray, (uint32_t __user *)arg, sizeof(argArray)))
		return -EFAULT;
	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
	{
		returnValue = i_APCI1711_BissMasterSingleCycleDataRead(pdev,
		                                                       (uint8_t)argArray[0],    // moduleIndex
		                                                       (uint8_t)argArray[1],    // slaveInde
		                                                       &argArray[2],  // dataLow
		                                                       &argArray[3]); // dataHigh
		if (returnValue != 0)
			return returnValue;
		if (copy_to_user((uint32_t __user *)arg , argArray, sizeof(argArray)))
			return -EFAULT;
		return 0;
	}
	return -ENOSYS;	// BiSS for APCI-1710 is not yet implemented
}

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
int do_CMD_APCI1710_BissMasterSingleCycleRegisterRead(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int returnValue;
	uint8_t argArray[68];
	if (copy_from_user(argArray, (uint8_t __user *)arg, sizeof(argArray)))
		return -EFAULT;
	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
	{
		returnValue = i_APCI1711_BissMasterSingleCycleRegisterRead(pdev,
		                                                           argArray[0],   // moduleIndex
		                                                           argArray[1],   // slaveIndex
		                                                           argArray[2],   // address
		                                                           argArray[3],   // size
		                                                           &argArray[4]); // data[64]
		if (returnValue != 0)
			return returnValue;
		if (copy_to_user((uint8_t __user *)arg , argArray, sizeof(argArray)))
			return -EFAULT;
		return 0;
	}
	return -ENOSYS;	// BiSS for APCI-1710 is not yet implemented
}

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
int do_CMD_APCI1710_BissMasterSingleCycleRegisterWrite(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t argArray[68];
	if (copy_from_user(argArray, (uint8_t __user *)arg, sizeof(argArray)))
		return -EFAULT;
	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
	{
		return i_APCI1711_BissMasterSingleCycleRegisterWrite(pdev,
			                                                 argArray[0],   // moduleIndex
			                                                 argArray[1],   // slaveIndex
			                                                 argArray[2],   // address
			                                                 argArray[3],   // size
			                                                 &argArray[4]); // data[64]
	}
	return -ENOSYS;	// BiSS for APCI-1710 is not yet implemented
}

/** release the master, the slave(s) for single cycle read / write.
 * @param[in] deviceData				Pointer to the device
 * @param[in] moduleIndex				Index of the slave (0->3)
 * @retval 0: success
 * @retval 1 : moduleIndex is incorrect
 * @retval 2 : the component is not programmed as Biss
 * */
int do_CMD_APCI1710_BissMasterReleaseSingleCycle(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t moduleIndex;
	if (copy_from_user(&moduleIndex, (uint8_t __user *)arg, sizeof(moduleIndex)))
		return -EFAULT;
	if (pdev->device == apcie1711_BOARD_DEVICE_ID)
		return i_APCI1711_BissMasterReleaseSingleCycle(pdev, moduleIndex);
	return -ENOSYS;	// BiSS for APCI-1710 is not yet implemented
}
