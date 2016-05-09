/** @file biss_1711-kapi.c

   Contains BiSS kernel functions.

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

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,11)
	static spinlock_t spinlock_biss = SPIN_LOCK_UNLOCKED;
#else
	DEFINE_SPINLOCK(spinlock_biss);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
static __inline__ unsigned long msecs_to_jiffies(unsigned long msecs)
{
	return ((HZ * msecs + 999) / 1000);
}
#endif

EXPORT_SYMBOL(i_APCI1711_BissMasterInitSingleCycle);
EXPORT_SYMBOL(i_APCI1711_BissMasterSingleCycleDataRead);
EXPORT_SYMBOL(i_APCI1711_BissMasterSingleCycleRegisterRead);
EXPORT_SYMBOL(i_APCI1711_BissMasterSingleCycleRegisterWrite);
EXPORT_SYMBOL(i_APCI1711_BissMasterReleaseSingleCycle);

EXPORT_NO_SYMBOLS;

static int WaitMemReadyBit(void *address, uint32_t bit, uint32_t bitValue, uint32_t ms)
{
    // Loop until the correct value is read or until a timeout is reached
    unsigned long timeout = jiffies + msecs_to_jiffies(ms);
    for (;;)
    {
        if (((readl(address) >> bit) & 1) == bitValue)
            return 0;
        if (jiffies > timeout)
            return -1;
    }
}

static int BreakCommand(struct pci_dev *pdev)
{
    /* Send the break command and wait for acknowledgment */
    writel(0x80, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 244);
    return WaitMemReadyBit(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240, 0, 1, 500) != 0 ? 1 : 0;
}

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
int i_APCI1711_BissMasterInitSingleCycle(struct   pci_dev *pdev,
                                         uint8_t  moduleIndex,
                                         uint16_t sensorDataFreqDivisor,
                                         uint16_t registerDataFreqDivisor,
                                         uint8_t  channel0BISSSSIMode,
                                         uint8_t  channel0BissMode,
                                         uint8_t  channel1BISSSSIMode,
                                         uint8_t  channel1BissMode,
                                         uint8_t  nbrOfSlave,
                                         uint8_t  channel[6],
                                         uint8_t  dataLength[6],
                                         uint8_t  option[6],
                                         uint8_t  CRCPolynom[6],
                                         uint8_t  CRCInvert[6])
{
	uint8_t cpt = 0;
	uint8_t ch0NbrOfSlaveSave = 0;
	uint8_t ch0NbrOfSlave = 0;
	uint8_t ch1NbrOfSlave = 0;
	uint8_t ch0RegisterSlaveIndex = 0;
	uint8_t ch1RegisterSlaveIndex = 0;
	uint8_t lastChannel = 0;
	unsigned long flags;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	if (sensorDataFreqDivisor > 31 || sensorDataFreqDivisor == 16)
		return 2;

	if (registerDataFreqDivisor > 7)
		return 3;

	if (channel0BISSSSIMode != 0 && channel0BISSSSIMode != 1)
		return 4;

	if (channel0BISSSSIMode == 0 && channel0BissMode != 0 && channel0BissMode != 1)
		return 5;

	if (channel1BISSSSIMode != 0 && channel1BISSSSIMode != 1)
		return 6;

	if (channel1BISSSSIMode == 0 && channel1BissMode != 0 && channel1BissMode != 1)
		return 7;

	if (nbrOfSlave < 1 || nbrOfSlave > 6)
		return 8;

    if (channel[0] != 0)
        return 9;

	for (cpt = 0; cpt < nbrOfSlave; cpt++)
	{
		if (lastChannel == 1 && channel[cpt] == 0)
			return 10;

		if (channel[cpt] != 0 && channel[cpt] != 1)
			return 11;

		if (dataLength[cpt] > 64)
			return 12;
		
		if (CRCPolynom[cpt] > 255)
			return 13;
		
		if (CRCInvert[cpt] != 0 && CRCInvert[cpt] != 1)
			return 14;

		/* internal computation */
		if (channel[cpt] == 0)
		{
			ch0NbrOfSlave++;
			ch0NbrOfSlaveSave++;
		}
		else
		{
			ch1NbrOfSlave++;
		}

		/* save the last channel */
		lastChannel = channel[cpt];
	}

	/* confirm that the slave is configured for BiSS */
	if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != APCI1710_BISS_MASTER)
		return 15;

	/* lock with interrupt  */
	spin_lock_irqsave(&spinlock_biss, flags);
	{
		uint32_t registerContent = 0;
		uint32_t slaveloc = 0;

        /* stop all previous communication */
        BreakCommand(pdev);

        /* set the frequency */
        writel((sensorDataFreqDivisor << 16) | ( registerDataFreqDivisor << 21), APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 228);

        /* set the FREQAGS */
        registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 232);
        registerContent = (registerContent & 0xFFFF0000) | 124;
        writel(registerContent, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 232);

        /* clear slaves & channel */
        for (cpt = 0; cpt < 6; cpt++)
            writel(0, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 192 + 4*cpt);

        /* clear channel configuration */
        writel(0, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 236);

        /* add the slave(s) */
        for (cpt = 0; cpt < nbrOfSlave; cpt++)
        {
            slaveloc |= (channel[cpt] << cpt);

            /* slave configuration */
            registerContent = (CRCInvert[cpt] << 15) | ((CRCPolynom[cpt] & 0xFE) << 7) | (option[cpt] << 7) | (1 << 6) | ((dataLength[cpt]-1) & 0x3F);
            writel(registerContent, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 192 + 4*cpt);
        }

        /* channel configuration */
        /* note: actnsens stay to 0 at the moment */
        registerContent = (0/*actnsens*/ << 25) | (channel1BISSSSIMode << 11) | (channel1BissMode << 10 ) | (channel0BISSSSIMode << 9) | (channel0BissMode << 8 ) | slaveloc;
        writel(registerContent, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 236);

        /* select biss model C and register communication */
		registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 228);
		registerContent = (registerContent & 0xFFFF0000) | (1 << 15) | (1 << 14);
		writel(registerContent, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 228);

		/* initialise master - init command */
		writel(0x10, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 244);

		/* wait */
		if (WaitMemReadyBit(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240, 0, 1, 500) != 0)
		{
			/* timeout */
			BreakCommand(pdev);
			spin_unlock_irqrestore(&spinlock_biss, flags);
			return 17;
		}

		/* check the error bit */
		registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240);
		if ((registerContent & 0x80) == 0)
		{
			spin_unlock_irqrestore(&spinlock_biss, flags);
			return 17;
		}

		/*Save the initialization in the structure*/
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.singleCycleInitStatus = 1;
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.initialisedSlaveCount = nbrOfSlave;

		/*Save the slaves information*/
		for (cpt = 0; cpt < nbrOfSlave; cpt ++)
		{
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].channel = channel[cpt];

			if (channel[cpt] == 0)
			{
				APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].channelMode = channel0BISSSSIMode;
				APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].channelBISSMode = channel0BissMode;
				APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].registerSlaveID = ch0RegisterSlaveIndex++;
				APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].dataSlaveIndex = ch0NbrOfSlave-- - 1;
			}
			else
			{
				APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].channelMode = channel1BISSSSIMode;
				APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].channelBISSMode = channel1BissMode;
				APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].registerSlaveID = ch1RegisterSlaveIndex ++;
				APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].dataSlaveIndex = ch0NbrOfSlaveSave + ch1NbrOfSlave-- - 1;
			}

			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].dataLength = dataLength[cpt];
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].grayEncode = option[cpt];
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].CRCPolynom = CRCPolynom[cpt];
			APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[cpt].CRCInvert = CRCInvert[cpt];
		}
		spin_unlock_irqrestore(&spinlock_biss, flags);
	}
	return 0;
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
int i_APCI1711_BissMasterSingleCycleDataRead(struct  pci_dev *pdev,
                                             uint8_t moduleIndex,
                                             uint8_t slaveIndex,
                                             uint32_t * dataLow,
                                             uint32_t * dataHigh)
{
	unsigned long flags;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	if (slaveIndex >= APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.initialisedSlaveCount)
		return 2;

    /* confirm that the slave is configured for BiSS */
    if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != APCI1710_BISS_MASTER)
        return 3;

	/* test if the single cycle acquisition is initialised */
	if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.singleCycleInitStatus == 0)
		return 4;

	/* lock with interrupt */
    spin_lock_irqsave(&spinlock_biss, flags);
	{
	    uint32_t registerContent = 0;
		uint8_t dataLength = APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].dataLength;

        /* command register COMMAND_GETSENS0 */
        writel(0x4, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 244);

        /* Wait EOT or TIMEOUT */
        if (WaitMemReadyBit(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240, 0, 1, 500) != 0)
        {
            /* stop communication */
            BreakCommand(pdev);
            spin_unlock_irqrestore(&spinlock_biss, flags);
            return 5;
        }

        /* check the error bit */
        registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240);
        if ((registerContent & 0x80) == 0)
        {
            spin_unlock_irqrestore(&spinlock_biss, flags);
            return 5;
        }

        *dataLow  = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].dataSlaveIndex * 8));
        *dataHigh = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 4 + (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].dataSlaveIndex * 8));

		if (dataLength > 32)
		{
			*dataHigh = *dataHigh & (uint32_t)2 << ((dataLength - 32)-1);
		}
		else
		{
			*dataLow = *dataLow & (uint32_t)2 << ((dataLength)-1);
			*dataHigh = 0;
		}
	}
    spin_unlock_irqrestore(&spinlock_biss, flags);

	return 0;
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
int i_APCI1711_BissMasterSingleCycleRegisterRead(struct  pci_dev *pdev,
                                                 uint8_t moduleIndex,
                                                 uint8_t slaveIndex,
                                                 uint8_t address,
                                                 uint8_t size,
                                                 uint8_t data[64])
{
	unsigned long flags;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	if (slaveIndex >= APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.initialisedSlaveCount)
		return 2;

	if (address > 127)
		return 3;

	if (size < 1 || size > 64)
		return 4;

    /* confirm that the slave is configured for BiSS */
    if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != APCI1710_BISS_MASTER)
        return 5;

	/* test if the single cycle acquisition is initialized */
	if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.singleCycleInitStatus == 0)
		return 6;

	/* test if the channel is configured as BiSS */
	if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].channelMode != 0 )
		return 7;

	/* interrupt lock */
    spin_lock_irqsave(&spinlock_biss, flags);
	{
		uint32_t registerContent = 0;
		uint8_t  cpt = 0;

		/* select the address and the size that we want to read */
		writel((((size-1) & 0x3F) << 24) | ((address & 0x7F) << 16), APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 224);

		/* command communication configuration. HOLDCMD = 0, MSEL = 1 */
		registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 228);
		registerContent = (registerContent & 0xFFFF0000)
			| (1 << 15)
			| (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].channelBISSMode << 14)
			| ((APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].registerSlaveID & 0x7) << 11)
			| (1 << APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].channel);
		writel(registerContent, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 228);

		/* mode A/B ? */
		if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].channelBISSMode == 0)
		{
			/* Command register access */
			writel(0x8, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 244);

			/* Wait EOT or TIMEOUT */
			if (WaitMemReadyBit(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240, 2, 1, 500) != 0)
			{
				/* stop communication */
				BreakCommand(pdev);
				spin_unlock_irqrestore(&spinlock_biss, flags);
				return 8;
			}

			registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240);
		}
		/* mode C */
		else
		{
			unsigned long timeout = jiffies + msecs_to_jiffies(500);
			for (;;)
			{
				/* Command register access */
				writel(0x8, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 244);

				/* Wait EOT or TIMEOUT */
				if (WaitMemReadyBit(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240, 0, 1, 500) != 0)
				{
					/* stop communication */
					BreakCommand(pdev);
					spin_unlock_irqrestore(&spinlock_biss, flags);
					return 8;
				}

				registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240);
				if ((registerContent & 5) == 5)
					break;
				if (jiffies > timeout)
				{
					/* stop communication */
					BreakCommand(pdev);
					spin_unlock_irqrestore(&spinlock_biss, flags);
					return 8;
				}
			}
		}

		/* check error bit */
		if ((registerContent & 0x80) == 0)
		{
			/* stop communication */
			BreakCommand(pdev);
			spin_unlock_irqrestore(&spinlock_biss, flags);
			return 8;
		}

		/* read the answer */
		for (cpt = 0; cpt < size; cpt++)
		{
			if ((cpt % 4) == 0)
				registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 128 + cpt);
			data[cpt] = (uint8_t)(registerContent & 0xFF);
			registerContent = registerContent >> 8;
		}
	}
    spin_unlock_irqrestore(&spinlock_biss, flags);

	return 0;
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
int i_APCI1711_BissMasterSingleCycleRegisterWrite(struct  pci_dev *pdev,
                                                  uint8_t moduleIndex,
                                                  uint8_t slaveIndex,
                                                  uint8_t address,
                                                  uint8_t size,
                                                  uint8_t data[64])
{
	unsigned long flags;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

	if (slaveIndex >= APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.initialisedSlaveCount)
		return 2;

	if (address > 127)
		return 3;

	if (size < 1 || size > 64)
		return 4;

    /* confirm that the slave is configured for BiSS */
    if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != APCI1710_BISS_MASTER)
        return 5;

	/* test if the single cycle acquisition is initialized */
	if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.singleCycleInitStatus == 0)
		return 6;

	/* test if the channel is configured as biss */
	if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].channelMode != 0)
		return 7;

	/* interrupt lock */
	spin_lock_irqsave(&spinlock_biss, flags);
	{
		uint32_t registerContent = 0;
		uint8_t  cpt = 0;

		/*
		 * write the data that we will send
		 * since we have 32 bits registers, we have to write them at the correct place
		 * */
		for (cpt = 0; cpt < size; cpt++)
		{
			registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 128 + cpt);
			switch (cpt % 4)
			{
				case 0:
					registerContent = (registerContent & 0xFFFFFF00) | data[cpt];
					break;
				case 1:
					registerContent = (registerContent & 0xFFFF00FF) | (data[cpt] << 8);
					break;
				case 2:
					registerContent = (registerContent & 0xFF00FFFF) | (data[cpt] << 16);
					break;
				case 3:
					registerContent = (registerContent & 0x00FFFFFF) | (data[cpt] << 24);
					break;
			}
			writel(registerContent, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 128 + cpt);
	   }

		/* select the address and the size */
		writel((((size-1) & 0x3F) << 24) | 0x00800000 | ((address & 0x7F) << 16), APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 224);
		/* command communication configuration. HOLDCMD = 0, MSEL = 1*/
		registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 228);
		registerContent = (registerContent & 0xFFFF0000)
			| (1 << 15)
			| (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].channelBISSMode << 14)
			| ((APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].registerSlaveID & 0x7) << 11)
			| (1 << APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].channel);
		writel(registerContent, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 228);

		/* mode A/B ? */
		if (APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.slaveInfo[slaveIndex].channelBISSMode == 0)
		{
			/* Command register access */
			writel(0x8, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 244);

			/* Wait EOT or TIMEOUT */
			if (WaitMemReadyBit(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240, 2, 1, 500) != 0)
			{
				/* stop communication */
				BreakCommand(pdev);
				spin_unlock_irqrestore(&spinlock_biss, flags);
				return 8;
			}

			registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240);
		}
		/* mode C */
		else
		{
			unsigned long timeout = jiffies + msecs_to_jiffies(500);
			for (;;)
			{
				/* Command register access */
				writel(0x8, APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 244);

				/* Wait EOT or TIMEOUT */
				if (WaitMemReadyBit(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240, 0, 1, 500) != 0)
				{
					/* stop communication */
					BreakCommand(pdev);
					spin_unlock_irqrestore(&spinlock_biss, flags);
					return 8;
				}

				registerContent = readl(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + 240);
				if ((registerContent & 5) == 5)
					break;
				if (jiffies > timeout)
				{
					/* stop communication */
					BreakCommand(pdev);
					spin_unlock_irqrestore(&spinlock_biss, flags);
					return 8;
				}
			}
		}

		/* check error bit */
		if ((registerContent & 0x80) == 0)
		{
			/* stop communication */
			BreakCommand(pdev);
			spin_unlock_irqrestore(&spinlock_biss, flags);
			return 8;
		}
	}
	spin_unlock_irqrestore(&spinlock_biss, flags);

	return 0;
}

/** release the master, the slave(s) for single cycle read / write.
 * @param[in] deviceData				Pointer to the device
 * @param[in] moduleIndex				Index of the slave (0->3)
 * @retval 0: success
 * @retval 1 : moduleIndex is incorrect
 * @retval 2 : the component is not programmed as Biss
 * */
int i_APCI1711_BissMasterReleaseSingleCycle(struct pci_dev *pdev, uint8_t moduleIndex)
{
	unsigned long flags;

	/* check the parameters */
	if (moduleIndex > 3)
		return 1;

    /* confirm that the slave is configured for BiSS */
    if (APCI1710_MODULE_FUNCTIONALITY(pdev, moduleIndex) != APCI1710_BISS_MASTER)
        return 2;

	spin_lock_irqsave(&spinlock_biss, flags);
	{
		/* save the initialization data in the structure */
        APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.singleCycleInitStatus = 0;
		APCI1710_PRIVDATA(pdev)->s_ModuleInfo[moduleIndex].s_BissModuleInfo.initialisedSlaveCount = 0;
	}
	spin_unlock_irqrestore(&spinlock_biss, flags);

	return 0;
}
