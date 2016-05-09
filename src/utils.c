/** @file utils.c
 
	General use utility functions
 
   @par CREATION  
   @author Sylvain Nahas
   @date   19.11.2007
   
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

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user 
#endif

//------------------------------------------------------------------------------

/** returns the content of the 4 modules configuration register (ID).
 * 
 * This information helps to identify the type of module that is installed on the board.
 * 
 * @param [in] argv                  : A pointer to an array of 4 unsigned long
 * 
 * @retval 0: No error.
 * @retval -EFAULT : Failed to copy data in user space (incorrect pointer). 
 * 
 * The 2 first bytes identify the functionality ( see enumeration below )
 * the two last byte is the firmware version 
 *  
 */ 
int do_CMD_APCI1710_GetModulesId (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)   
{
	uint32_t Functionalities[4];
	
	/* note: call to i_APCI1710_GetFunctionality() should not fail */

	i_APCI1710_ReadModulesConfiguration(pdev);

	/* module 0 */
	i_APCI1710_GetModuleId (pdev,0, &(Functionalities[0]) );
	/* module 1 */
	i_APCI1710_GetModuleId (pdev,1, &(Functionalities[1]) );
	/* module 2 */
	i_APCI1710_GetModuleId (pdev,2, &(Functionalities[2]) );
	/* module 3 */
	i_APCI1710_GetModuleId (pdev,3, &(Functionalities[3]) );
	
	if ( copy_to_user( (unsigned int __user *)arg , Functionalities, sizeof(Functionalities) ) )
		return -EFAULT;	

	return 0;
}

//------------------------------------------------------------------------------
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
int do_CMD_APCI1710_WRITE (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t dw_ArgArray[4];
	uint32_t address = 0;

	if ( copy_from_user (dw_ArgArray, (uint32_t __user *)arg, sizeof(dw_ArgArray) ) )
		return -EFAULT;

	/* Test the board type */
	switch (pdev->device)
	{
		case apci1710_BOARD_DEVICE_ID:
		{
			/* What is to be accessed */
			switch (dw_ArgArray[0])
			{
				case 0: address = GET_BAR2(pdev);
				break;

				case 1: address = GET_BAR1(pdev);
				break;

				case 2: address = GET_BAR0(pdev);
				break;

				default: return -EADDRNOTAVAIL;
			}

			/* How many bytes */
			switch (dw_ArgArray[1])
			{
				case 1:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						OUTP (address, dw_ArgArray[2], (uint8_t) dw_ArgArray[3]);
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				case 2:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						OUTPW (address, dw_ArgArray[2], (uint16_t) dw_ArgArray[3]);
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				case 4:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						OUTPDW (address, dw_ArgArray[2], (uint32_t) dw_ArgArray[3]);
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				default: return -EADDRNOTAVAIL;
			}
		}
		break;

		case apcie1711_BOARD_DEVICE_ID:
		{
			/* What is to be accessed */
			switch (dw_ArgArray[0])
			{
				case 0:
				break;

				default: return -EADDRNOTAVAIL;
			}

			/* How many bytes */
			switch (dw_ArgArray[1])
			{
				case 1:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
							iowrite8((uint8_t)dw_ArgArray[3], APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]);
						#else
							writeb((uint8_t)dw_ArgArray[3], (void __iomem *)(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]));
						#endif
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				case 2:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
							iowrite16((uint16_t)dw_ArgArray[3], APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]);
						#else
							writew((uint8_t)dw_ArgArray[3], (void __iomem *)(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]));
						#endif
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				case 4:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
							iowrite32(dw_ArgArray[3], APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]);
						#else
							writel((uint8_t)dw_ArgArray[3], (void __iomem *)(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]));
						#endif
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				default: return -EADDRNOTAVAIL;
			}
		}
		break;

		default: return -EFAULT;
	}

	return 0;
}

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
int do_CMD_APCI1710_READ (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t dw_ArgArray[3];
	uint32_t address = 0;

	if ( copy_from_user (dw_ArgArray, (uint32_t __user *)arg, sizeof(dw_ArgArray) ) )
		return -EFAULT;

	/* Test the board type */
	switch (pdev->device)
	{
		case apci1710_BOARD_DEVICE_ID:
		{
			/* What is to be accessed */
			switch (dw_ArgArray[0])
			{
				case 0: address = GET_BAR2(pdev);
				break;

				case 1: address = GET_BAR1(pdev);
				break;

				case 2: address = GET_BAR0(pdev);
				break;

				default: return -EADDRNOTAVAIL;
			}

			/* How many bytes */
			switch (dw_ArgArray[1])
			{
				case 1:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						INP (address, dw_ArgArray[2], (uint8_t *) &dw_ArgArray[0]);
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				case 2:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						INPW (address, dw_ArgArray[2], &dw_ArgArray[0]);
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				case 4:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						INPDW (address, dw_ArgArray[2], (uint32_t *) &dw_ArgArray[0]);
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				default: return -EADDRNOTAVAIL;
			}
		}
		break;

		case apcie1711_BOARD_DEVICE_ID:
		{
			/* What is to be accessed */
			switch (dw_ArgArray[0])
			{
				case 0:
				break;

				default: return -EADDRNOTAVAIL;
			}

			/* How many bytes */
			switch (dw_ArgArray[1])
			{
				case 1:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
							dw_ArgArray[0] = ioread8(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]);
						#else
							dw_ArgArray[0] = readb((void __iomem *)(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]));
						#endif
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				case 2:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
							dw_ArgArray[0] = ioread16(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]);
						#else
							dw_ArgArray[0] = readw((void __iomem *)(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]));
						#endif
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				case 4:
				{
					unsigned long irqstate;
					APCI1710_LOCK(pdev,&irqstate);
					{
						#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
							dw_ArgArray[0] = ioread32(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]);
						#else
							dw_ArgArray[0] = readl((void __iomem *)(APCI1710_PRIVDATA(pdev)->memBaseAddress3 + dw_ArgArray[2]));
						#endif
					}
					APCI1710_UNLOCK(pdev,irqstate);
				}
				break;

				default: return -EADDRNOTAVAIL;
			}
		}
		break;

		default: return -EFAULT;
	}

	if ( copy_to_user( (uint32_t __user *)arg , &dw_ArgArray[0], sizeof(uint32_t) ) )
		return -EFAULT;

	return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Get the board type
 *
 * @param [out] arg : Board type:
 * 					  - APCI1710_10K10_BOARD_TYPE
 * 					  - APCI1710_10K20_BOARD_TYPE
 * 					  - APCIE1711_BOARD_TYPE
 * 					  - UNKNOWN_BOARD_TYPE
 *
 * @retval 0: No error.
 * @retval -EFAULT : Failed to copy data in user space (incorrect pointer).
 */
int do_CMD_APCI1710_GetBoardType (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t boardType = 0;

	if (i_APCI1710_GetBoardType (pdev, &boardType) != 0)
		return -EFAULT;

	if ( copy_to_user( (uint32_t __user *)arg , &boardType, sizeof(uint32_t) ) )
		return -EFAULT;

	return 0;
}

//------------------------------------------------------------------------------

