/** @file kapi_dig_io.c
 
   Contains ttl io kernel functions.
 
   @par CREATION  
   @author Krauth Julien
   @date   26.10.2007
   
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

EXPORT_SYMBOL(i_APCI1710_InitTTLIO);
EXPORT_SYMBOL(i_APCI1710_InitTTLIODirection);
EXPORT_SYMBOL(i_APCI1710_ReadTTLIOChannelValue);
EXPORT_SYMBOL(i_APCI1710_SetTTLIOChlOn);
EXPORT_SYMBOL(i_APCI1710_SetTTLIOChlOff);

//------------------------------------------------------------------------------

/** Configure the TTL I/O operating mode on the selected module.
 *  You must call this function before you call any other TTL function.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 */
int i_APCI1710_InitTTLIO (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int    i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /**************************/
	   /* Test if TTL I/O module */
	   /**************************/

	   if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_TTL_IO)
	      {
		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [b_ModulNbr].
		    s_TTLIOInfo.
		    b_TTLInit = 1;

	      /***************************/
	      /* Set TTL port A to input */
	      /***************************/

		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [b_ModulNbr].
		    s_TTLIOInfo.
		    b_PortConfiguration [0] = 0;

	      /***************************/
	      /* Set TTL port B to input */
	      /***************************/

		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [b_ModulNbr].
		    s_TTLIOInfo.
		    b_PortConfiguration [1] = 0;

	      /***************************/
	      /* Set TTL port C to input */
	      /***************************/

		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [b_ModulNbr].
		    s_TTLIOInfo.
		    b_PortConfiguration [2] = 0;

	      /****************************/
	      /* Set TTL port D to output */
	      /****************************/

		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [b_ModulNbr].
		    s_TTLIOInfo.
		    b_PortConfiguration [3] = 1;

	      /*************************/
	      /* Set the configuration */
	      /*************************/

		   OUTPDW (GET_BAR2(pdev), 20 + (64 * b_ModulNbr), 0x8);
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a TTL module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Configure the TTL I/O operating mode on the selected module.
 *  You must call this function before you call any other TTL function.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_PortAMode           : 1 = output, 0 = input.
 * @param [in] b_PortBMode           : 1 = output, 0 = input.
 * @param [in] b_PortCMode           : 1 = output, 0 = input.
 * @param [in] b_PortDMode           : 1 = output, 0 = input.
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
 */
int i_APCI1710_InitTTLIODirection (struct pci_dev *pdev,
                                   uint8_t b_ModulNbr,
                                   uint8_t b_PortAMode,
                                   uint8_t b_PortBMode,
                                   uint8_t b_PortCMode,
                                   uint8_t b_PortDMode)
	{
	int i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /**************************/
	   /* Test if TTL I/O module */
	   /**************************/

	   if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_TTL_IO)
	      {
	      /********************/
	      /* Test the version */
	      /********************/

	      if ((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) >= 0x3230)
				{
		 		/************************/
		 		/* Test the port A mode */
		 		/************************/

				if ((b_PortAMode == 0) || (b_PortAMode == 1))
					{
					/************************/
					/* Test the port B mode */
					/************************/
	
					if ((b_PortBMode == 0) || (b_PortBMode == 1))
						{
						/************************/
						/* Test the port C mode */
						/************************/
	
						if ((b_PortCMode == 0) || (b_PortCMode == 1))
							{
							/************************/
							/* Test the port D mode */
							/************************/

							if ((b_PortDMode == 0) || (b_PortDMode == 1))
								{
								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
								b_TTLInit = 1;

								/***********************/
								/* Set TTL port A mode */
								/***********************/
				
								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
								b_PortConfiguration [0] = b_PortAMode;
				
								/***********************/
								/* Set TTL port B mode */
								/***********************/
				
								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
								b_PortConfiguration [1] = b_PortBMode;
				
								/***********************/
								/* Set TTL port C mode */
								/***********************/
				
								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
								b_PortConfiguration [2] = b_PortCMode;
				
								/***********************/
								/* Set TTL port D mode */
								/***********************/
				
								APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
								b_PortConfiguration [3] = b_PortDMode;
				
								/*************************/
								/* Set the configuration */
								/*************************/
				
		   					OUTPDW (GET_BAR2(pdev), 20 + (64 * b_ModulNbr), (b_PortAMode << 0) |
													(b_PortBMode << 1) |
													(b_PortCMode << 2) |
													(b_PortDMode << 3));
			     }
			  else
			     {
			     /**********************************/
			     /* Port D mode selection is wrong */
			     /**********************************/

			     i_ReturnValue = 8;
			     }
			  }
		       else
			  {
			  /**********************************/
			  /* Port C mode selection is wrong */
			  /**********************************/

			  i_ReturnValue = 7;
			  }
		       }
		    else
		       {
		       /**********************************/
		       /* Port B mode selection is wrong */
		       /**********************************/

		       i_ReturnValue = 6;
		       }
		    }
		 else
		    {
		    /**********************************/
		    /* Port A mode selection is wrong */
		    /**********************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /*******************************************/
		 /* Function not available for this version */
		 /*******************************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a TTL module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}


//------------------------------------------------------------------------------

/** Read the status from selected TTL digital input.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_SelectedPort,       : Selection from TTL I/O port (0 to 2):<br>
 *                                     0 : Port A selection<br>
 *                                     1 : Port B selection<br>
 *                                     2 : Port C selection<br>
 *                                     3 : Port D selection
 * @param [in] b_InputChannel        : Selection from digital input ( 0 to 2).
 *
 * @param [out] pb_ChannelStatus     : Digital input channel status:<br>
 *                                     0 : Channle is not active<br>
 *                                     1 : Channle is active
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 * @retval 4: The selected TTL input port is wrong.
 * @retval 5: The selected TTL digital input is wrong.
 * @retval 6: TTL I/O not initialised.
 */
int i_APCI1710_ReadTTLIOChannelValue (struct pci_dev *pdev,
                                      uint8_t b_ModulNbr,
                                      uint8_t b_SelectedPort,
                                      uint8_t b_InputChannel,
                                      uint8_t *b_ChannelStatus)
	{
	int i_ReturnValue = 0;
	uint32_t  dw_StatusReg = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /**************************/
	   /* Test if TTL I/O module */
	   /**************************/

	   if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_TTL_IO)
	      {
	      /********************************/
	      /* Test the TTL I/O port number */
	      /********************************/

	      if (((b_SelectedPort <= 2) && ((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) == 0x3130)) ||
		  ((b_SelectedPort <= 3) && ((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) >= 0x3230)))
		 {
		 /******************************************/
		 /* Test the digital imnput channel number */
		 /******************************************/

		 if (((b_InputChannel <= 7) && (b_SelectedPort < 3)) ||
		     ((b_InputChannel <= 1) && (b_SelectedPort == 3)))
		    {
		    /******************************************/
		    /* Test if the TTL I/O module initialised */
		    /******************************************/

		    if (APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
								b_TTLInit == 1)
		       {
		       /***********************************/
		       /* Test if TTL port used for input */
		       /***********************************/

		       if (((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) == 0x3130) ||
			   (((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) >= 0x3230) &&
			    (APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
			     b_PortConfiguration [b_SelectedPort] == 0)))
			  {
			  /**************************/
			  /* Read all digital input */
			  /**************************/

				INPDW (GET_BAR2(pdev),
			      (64 * b_ModulNbr),
			      &dw_StatusReg);

			  *b_ChannelStatus = (uint8_t) ((dw_StatusReg >> (8 * b_SelectedPort)) >> b_InputChannel) & 1;
			  }
		       else
			  {
			  /*******************************/
			  /* Selected TTL I/O port error */
			  /*******************************/

			  i_ReturnValue = 4;
			  }
		       }
		    else
		       {
		       /***************************/
		       /* TTL I/O not initialised */
		       /***************************/

		       i_ReturnValue = 6;
		       }
		    }
		 else
		    {
		    /********************************/
		    /* Selected digital input error */
		    /********************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /*******************************/
		 /* Selected TTL I/O port error */
		 /*******************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /**********************************/
	      /* The module is not a TTL module */
	      /**********************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Set the output witch has been passed with the
 *  parameter b_Channel. Setting an output means setting
 *  an ouput high.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_OutputChannel       : Selection from digital output channel (0 or 1):<br>
 *                                     0       : PD0<br>
 *                                     1       : PD1<br>
 *                                     2 to 9  : PA<br>
 *                                     10 to 17: PB<br>
 *                                     8 to 25 : PC
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 * @retval 4: The selected digital output is wrong. 
 * @retval 5: TTL I/O not initialised see function i_APCI1710_InitTTLIO.
 */
int i_APCI1710_SetTTLIOChlOn (struct pci_dev *pdev,
                              uint8_t b_ModulNbr,
                              uint8_t b_OutputChannel)
	{
	int i_ReturnValue = 0;
	uint32_t dw_StatusReg = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /**************************/
	   /* Test if TTL I/O module */
	   /**************************/

	   if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_TTL_IO)
	      {
	      /******************************************/
	      /* Test if the TTL I/O module initialised */
	      /******************************************/

	      if (APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
								b_TTLInit == 1)
		 {
		 /***********************************/
		 /* Test the TTL I/O channel number */
		 /***********************************/

		 if (((b_OutputChannel <= 1) && ((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) == 0x3130)) ||
		     ((b_OutputChannel <= 25) && ((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) >= 0x3230)))
		    {
		    /****************************************************/
		    /* Test if the selected channel is a output channel */
		    /****************************************************/

		    if (((b_OutputChannel <= 1) && (APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_TTLIOInfo.b_PortConfiguration [3] == 1)) ||
			     ((b_OutputChannel >= 2) && (b_OutputChannel <= 9) && (APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_TTLIOInfo.b_PortConfiguration [0] == 1)) || ((b_OutputChannel >= 10) && (b_OutputChannel <= 17) && (APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_TTLIOInfo.b_PortConfiguration [1] == 1)) || ((b_OutputChannel >= 18) && (b_OutputChannel <= 25) && (APCI1710_PRIVDATA(pdev)->s_ModuleInfo [b_ModulNbr].s_TTLIOInfo.b_PortConfiguration [2] == 1)))
		       {
		       /************************/
		       /* Test if PD0 selected */
		       /************************/

		       if (b_OutputChannel == 0)
			  {
				OUTPDW (GET_BAR2(pdev), (64 * b_ModulNbr), 1);
			  }
		       else
                          {
			  /************************/
			  /* Test if PD1 selected */
			  /************************/

			  if (b_OutputChannel == 1)
			     {
				OUTPDW (GET_BAR2(pdev), 4 + (64 * b_ModulNbr), 1);
			     }
			  else
			     {
			     b_OutputChannel = b_OutputChannel - 2;

			     /********************/
			     /* Read all channel */
			     /********************/
					
					INPDW (GET_BAR2(pdev),
			      (64 * b_ModulNbr),
			      &dw_StatusReg);

			     dw_StatusReg = (dw_StatusReg >> ((b_OutputChannel / 8) * 8)) & 0xFF;
			     dw_StatusReg = dw_StatusReg | (1 << (b_OutputChannel % 8));

			     /****************************/
			     /* Set the new output value */
			     /****************************/
				OUTPDW (GET_BAR2(pdev), 8 + ((b_OutputChannel / 8) * 4) + (64 * b_ModulNbr), dw_StatusReg);
			     }
			  }
		       }
		    else
		       {
		       /************************************/
		       /* The selected TTL output is wrong */
		       /************************************/

		       i_ReturnValue = 4;
		       }
		    }
		 else
		    {
		    /************************************/
		    /* The selected TTL output is wrong */
		    /************************************/

		    i_ReturnValue = 4;
		    }
		 }
	      else
		 {
		 /***************************/
		 /* TTL I/O not initialised */
		 /***************************/

		 i_ReturnValue = 5;
		 }
	      }
	   else
	      {
	      /**************************************/
	      /* The module is not a TTL I/O module */
	      /**************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}

//------------------------------------------------------------------------------

/** Reset the output witch has been passed with the
 *  parameter b_Channel. Resetting an output means setting
 *  an ouput low.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_OutputChannel       : Selection from digital output channel (0 or 1):<br>
 *                                     0       : PD0<br>
 *                                     1       : PD1<br>
 *                                     2 to 9  : PA<br>
 *                                     10 to 17: PB<br>
 *                                     8 to 25 : PC
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a ttl I/O module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: TTL I/O not initialised see function i_APCI1710_InitTTLIO.
 */
int i_APCI1710_SetTTLIOChlOff (struct pci_dev *pdev,
                              uint8_t b_ModulNbr,
                              uint8_t b_OutputChannel)
	{
	int i_ReturnValue = 0;
	uint32_t dw_StatusReg = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < APCI1710_PRIVDATA(pdev)->s_BoardInfos.b_NumberOfModule)
	   {
	   /**************************/
	   /* Test if TTL I/O module */
	   /**************************/

	   if (APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_TTL_IO)
	      {
	      /******************************************/
	      /* Test if the TTL I/O module initialised */
	      /******************************************/

	      if (APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
								b_TTLInit == 1)
		 {
		 /***********************************/
		 /* Test the TTL I/O channel number */
		 /***********************************/

		 if (((b_OutputChannel <= 1) && ((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) == 0x3130)) ||
		     ((b_OutputChannel <= 25) && ((APCI1710_PRIVDATA(pdev)->s_BoardInfos.dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) >= 0x3230)))
		    {
		    /****************************************************/
		    /* Test if the selected channel is a output channel */
		    /****************************************************/

		    if (((b_OutputChannel <= 1) && (APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
									      b_PortConfiguration [3] == 1)) ||
			((b_OutputChannel >= 2) && (b_OutputChannel <= 9) && (APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
									      b_PortConfiguration [0] == 1)) ||
			((b_OutputChannel >= 10) && (b_OutputChannel <= 17) && (APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
										b_PortConfiguration [1] == 1)) ||
			((b_OutputChannel >= 18) && (b_OutputChannel <= 25) && (APCI1710_PRIVDATA(pdev)->
								s_ModuleInfo [b_ModulNbr].
								s_TTLIOInfo.
										b_PortConfiguration [2] == 1)))
		       {
		       /************************/
		       /* Test if PD0 selected */
		       /************************/

		       if (b_OutputChannel == 0)
			  {
				OUTPDW (GET_BAR2(pdev), (64 * b_ModulNbr), 0);
			  }
		       else
                          {
			  /************************/
			  /* Test if PD1 selected */
			  /************************/

			  if (b_OutputChannel == 1)
			     {
				OUTPDW (GET_BAR2(pdev), 4 + (64 * b_ModulNbr), 0);
			     }
			  else
			     {
			     b_OutputChannel = b_OutputChannel - 2;

			     /********************/
			     /* Read all channel */
			     /********************/
					
					INPDW (GET_BAR2(pdev),
			      (64 * b_ModulNbr),
			      &dw_StatusReg);

			     dw_StatusReg = (dw_StatusReg >> ((b_OutputChannel / 8) * 8)) & 0xFF;
			     dw_StatusReg = dw_StatusReg & (0xFF - (1 << (b_OutputChannel % 8)));

			     /****************************/
			     /* Set the new output value */
			     /****************************/
				OUTPDW (GET_BAR2(pdev), 8 + ((b_OutputChannel / 8) * 4) + (64 * b_ModulNbr), dw_StatusReg);
			     }
			  }
		       }
		    else
		       {
		       /************************************/
		       /* The selected TTL output is wrong */
		       /************************************/

		       i_ReturnValue = 4;
		       }
		    }
		 else
		    {
		    /************************************/
		    /* The selected TTL output is wrong */
		    /************************************/

		    i_ReturnValue = 4;
		    }
		 }
	      else
		 {
		 /***************************/
		 /* TTL I/O not initialised */
		 /***************************/

		 i_ReturnValue = 5;
		 }
	      }
	   else
	      {
	      /**************************************/
	      /* The module is not a TTL I/O module */
	      /**************************************/

	      i_ReturnValue = 3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = 2;
	   }

	return (i_ReturnValue);
	}










