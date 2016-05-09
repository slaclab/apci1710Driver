 /* @file privadata.h
  *
  * @brief Holds definition for driver's private data and its manipulation functions.
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

#ifndef __APCI1710_PRIVDATA_H_
#define __APCI1710_PRIVDATA_H_

#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
	#define __user
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	#define __iomem
#endif

//------------------------------------------------------------------------------

/* Interrupt functionality: Timer, counter... */
typedef void (*v_APCI1710_InterruptFunctionality) (void *, uint8_t, uint8_t *);;

/* User kernel callback interrupt */
typedef void (*v_APCI1710_UserInterruptCallback) (struct pci_dev * pdev);

/* Interrupt infos */
typedef struct
{
	uint32_t ul_InterruptOccur;  /*   0 : No interrupt occur  */
                             /* > 0 : Interrupt occur     */
	unsigned int ui_Read;             /* Read FIFO                 */
	unsigned int ui_Write;            /* Write FIFO                */

	struct
    {
		uint8_t   b_OldModuleMask;
		uint32_t ul_OldInterruptMask;         /* Interrupt mask          */
		uint32_t ul_OldValue[2];              /* Index 0: Counter or ETM value
		                                         Index 1: ETM total time value
		                                       */
    }
    s_FIFOInterruptParameters [APCI1710_SAVE_INTERRUPT];

}
str_InterruptParameters;

typedef struct
{
   uint32_t dw_Functionality;									/* The associated functionality */
   v_APCI1710_InterruptFunctionality v_InterruptFunction;	/* The interrupt function associated to the module (e.g.: Counter, Timer, SSI...) */
}
str_InterruptFunctionality;

typedef struct
{
   v_APCI1710_UserInterruptCallback v_UserInterruptFunction;
}
str_UserInterruptCallback;


/***************************/
/* Interrupt routine infos */
/***************************/

typedef struct
{
   uint8_t b_InterruptInitialized;
}str_InterruptInfos;

typedef struct
{
	uint8_t   b_NumberOfModule;
	uint32_t dw_MolduleConfiguration [4]; /* Module configuration (also known as ID)  */

	struct
	{
    	uint8_t b_TimeBase;
		uint8_t b_Filter;
	}s_FilterInfo[4];

}str_BoardInfos;


typedef union
{
    /* 82X54 timer infos */

    struct
	{
    	struct
	    {
			uint8_t    b_82X54Init;
			uint8_t    b_InputClockSelection;
			uint8_t    b_InputClockLevel;
			uint8_t    b_OutputLevel;
			uint8_t    b_HardwareGateLevel;
			uint32_t  dw_ConfigurationWord;
	    }s_82X54TimerInfo [3];

	    uint8_t b_InterruptMask;

	 }s_82X54ModuleInfo;

	 /* Chronometer infos */
    struct
    {
        uint8_t  b_ChronoInit;
        uint8_t  b_InterruptMask;
        uint8_t  b_PCIInputClock;
        uint8_t  b_TimingUnit;
        uint8_t  b_CycleMode;
        uint32_t dw_ConfigReg;
    } s_ChronoModuleInfo;

    /* BiSS infos */
	struct
    {
        uint8_t singleCycleInitStatus;
        uint8_t initialisedSlaveCount;
        struct
        {
            uint8_t channel;
            uint8_t channelMode;
            uint8_t channelBISSMode; /* 0: A/B, 1: C*/
            uint8_t dataLength;
            uint8_t grayEncode;
            uint8_t CRCPolynom;
            uint8_t CRCInvert;
            uint8_t dataSlaveIndex;
            uint8_t registerSlaveID;
        } slaveInfo[6];
    } s_BissModuleInfo;

    /* Endat infos */
    struct
    {
    	uint8_t sensorInitialized[2]; /* set to 1 if the sensor is initialized */
 	} s_EndatModuleInfo;

	/* Incremental counter infos */
	struct
	{
    	union
        {
        	struct
            {
		        uint8_t  b_ModeRegister1;
		        uint8_t  b_ModeRegister2;
		        uint8_t  b_ModeRegister3;
		        uint8_t  b_ModeRegister4;
            } s_ByteModeRegister;

			uint32_t dw_ModeRegister1_2_3_4;

		} s_ModeRegister;

		struct
	    {
			unsigned int b_IndexInit	              : 1;
			unsigned int b_CounterInit	              : 1;
			unsigned int b_ReferenceInit              : 1;
			unsigned int b_IndexInterruptOccur        : 1;
			unsigned int b_CompareLogicInit           : 1;
			unsigned int b_FrequencyMeasurementInit   : 1;
			unsigned int b_FrequencyMeasurementEnable : 1;
		} s_InitFlag;

		union
	    {
	    	struct
	        {
	        	unsigned int b_EnableTimer	         : 1;
	            unsigned int b_TriggerTimer	         : 1;
	            unsigned int b_Port0Selection        : 1;
	            unsigned int b_Port1Selection        : 1;
	        } s_BitRegister;

			uint32_t dw_DWordRegister;

	    } s_InitCompare;
	} s_SiemensCounterInfo;

	/* TTL I/O infos */
    struct
    {
        uint8_t b_TTLInit;
        uint8_t b_PortConfiguration[4];
    } s_TTLIOInfo;

	/* Digital I/O infos */
    struct
    {
        uint8_t  b_DigitalInit;
        uint8_t  b_ChannelAMode;
        uint8_t  b_ChannelBMode;
        uint8_t  b_OutputMemoryEnabled;
        uint32_t dw_OutputMemory; /* 32-bit, written in memory */
    } s_DigitalIOInfo;

	/* SSI */
    struct
    {
        uint8_t b_SSIProfile;
        uint8_t b_PositionTurnLength;
        uint8_t b_TurnCptLength;
        uint8_t b_SSIInit;
        uint8_t b_SSICountingMode;
    } s_SSICounterInfo;

	/* Pulse encoder infos */
	struct
    {
        struct
        {
            uint8_t b_PulseEncoderInit;
        } s_PulseEncoderInfo[4];
        uint32_t dw_SetRegister;
        uint32_t dw_ControlRegister;
        uint32_t dw_StatusRegister;
    } s_PulseEncoderModuleInfo;

    /* ETM infos */
    struct
    {
        struct
        {
            uint8_t b_ETMEnable;
            uint8_t b_ETMInterrupt;
        } s_ETMInfo[2];
        uint8_t  b_ETMInit;
        uint8_t  b_TimingUnit;
        uint8_t  b_ClockSelection;
        double   d_TimingInterval;
        uint32_t ul_Timing;
    } s_ETMModuleInfo;

     /* IDV infos */
#ifdef WITH_IDV_OPTION
	struct
	{
		uint8_t b_IDVInit;
		uint8_t b_Stueck;
		uint8_t b_OutputH;
		uint8_t b_Irq;
		struct // Holds values like a buffer to avoid overwriting the data in the next measurement cycle whilst reading it into user space
	  {
		  uint32_t dw_Status;
	    uint32_t dw_MessUnterkante;
	    uint32_t dw_MessOffset;
	    uint8_t  dw_MessZaehler; // Counts, how aften the driver has written data which haven't been read
	   }s_IDVInfo;
	 }s_IDVModuleInfo;
#endif
} str_ModuleInfo;



//------------------------------------------------------------------------------

/* internal driver data */
struct apci1710_str_BoardInformations
{
	spinlock_t lock; /**< protect the board data */

	str_BoardInfos s_BoardInfos;
	str_ModuleInfo s_ModuleInfo[4];
	str_InterruptInfos s_InterruptInfos;
	str_InterruptParameters s_InterruptParameters;
	str_InterruptFunctionality  s_InterruptFunctionality [4];
	/* field used to implement linked list */
	struct pci_dev * previous; /**< previous in known-devices linked list */
	struct pci_dev * next; /**< next in known-devices linked list */

	str_UserInterruptCallback s_UserInterruptCallback; /* One user interrupt for each board */

    struct fasync_struct * async_queue; /* asynchronous readers */

	void __iomem * memBaseAddress3;
};

/** initialise board's private data - fill it when adding new members and ioctl handlers */
static __inline__ void apci1710_init_priv_data(struct apci1710_str_BoardInformations * data)
{
	memset (data, 0, sizeof(struct apci1710_str_BoardInformations));

	spin_lock_init(& (data->lock) );

	/*
	 * This driver is only for the APCI-1710,
	 * this board has 4 modules.
	 */
	data->s_BoardInfos.b_NumberOfModule = 4;

	data->s_InterruptFunctionality[0].v_InterruptFunction = NULL;
	data->s_InterruptFunctionality[1].v_InterruptFunction = NULL;
	data->s_InterruptFunctionality[2].v_InterruptFunction = NULL;
	data->s_InterruptFunctionality[3].v_InterruptFunction = NULL;
	data->s_UserInterruptCallback.v_UserInterruptFunction = NULL;
}


/** return the private data field of a pci_dev structure.
 * @note The implementation of this function differs in 2.4 and 2.6 kernel
 */
static __inline__ struct apci1710_str_BoardInformations * APCI1710_PRIVDATA(struct pci_dev * pdev)
{
	return (struct apci1710_str_BoardInformations *) pci_get_drvdata(pdev);
}


/** lock the board */
static __inline__ void APCI1710_LOCK(struct pci_dev * pdev, unsigned long * flags)
{
	spin_lock_irqsave(& (APCI1710_PRIVDATA(pdev)->lock) , *flags );
}

/** unlock the board */
static __inline__ void APCI1710_UNLOCK(struct pci_dev * pdev, unsigned long flags)
{
	spin_unlock_irqrestore(& (APCI1710_PRIVDATA(pdev)->lock) , flags );
}

/* returns the functionality of a module (first 2 bytes of configuration) */
static __inline__ uint32_t APCI1710_MODULE_FUNCTIONALITY(struct pci_dev * pdev, unsigned char module)
{
	return ( (APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [(int)module]) >> 16) & 0x0000FFFFUL;
}

/* returns the version (revision) of the firmware of a module (last 2 bytes of configuration) */
static __inline__ uint32_t APCI1710_MODULE_VERSION(struct pci_dev * pdev, unsigned char module)
{
	return (APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [(int)module]) & 0x0000FFFFUL;
}

/* returns the ID ( = CONFIGURATION) of a module (FUNCTIONALITY/VERSION) */
static __inline__ uint32_t APCI1710_MODULE_ID(struct pci_dev * pdev, unsigned char module)
{
	return (APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [(int)module]);
}

/* offset to access module b_ModulNbr */
static __inline__ uint32_t MODULE_OFFSET(uint8_t b_ModulNbr)
{
	return ((uint32_t) b_ModulNbr) * 64;
}

/* returns 1 if the interrupt function has not been initialised yet */
static __inline__ int INTERRUPT_FUNCTION_NOT_INITIALISED(struct pci_dev * pdev)
{
	return ( (APCI1710_PRIVDATA(pdev)->s_InterruptInfos.b_InterruptInitialized) == 0 );
}

/* returns 1 if given module _is_ an incremental counter */
static __inline__ int IS_A_COUNTER(struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	return (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_INCREMENTAL_COUNTER) ;
}

/* returns 1 if given module is _not_ an incremental counter */
static __inline__ int NOT_A_COUNTER(struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	return ! IS_A_COUNTER(pdev, b_ModulNbr);
}

/* returns 1 if the module index is not in the range of the board */
static __inline__ int NUMBER_OF_MODULE(struct pci_dev * pdev)
{
	return (APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule);
}

/* returns 1 if the module index is not in the range of the board */
static __inline__ int NOT_A_MODULE(struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	return (b_ModulNbr >= NUMBER_OF_MODULE(pdev));
}

/* returns 1 if a user handler has been installed for a given module */
static __inline__ int MODULE_HAS_USER_HANDLER(struct pci_dev * pdev, uint8_t b_ModulNbr)
{
	return (APCI1710_PRIVDATA(pdev)->s_InterruptFunctionality [b_ModulNbr].v_InterruptFunction != NULL);
}
#endif // __APCI1710_PRIVDATA_H_
