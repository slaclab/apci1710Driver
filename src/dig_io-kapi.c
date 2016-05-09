/** @file kapi_dig_io.c
 
   Contains digital io kernel functions.
 
   @par CREATION  
   @author Krauth Julien
   @date   30.05.06
   
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

EXPORT_SYMBOL(i_APCI1710_InitDigitalIO);
EXPORT_SYMBOL(i_APCI1710_ReadDigitalIOChlValue);
EXPORT_SYMBOL(i_APCI1710_ReadDigitalIOPortValue);
EXPORT_SYMBOL(i_APCI1710_SetDigitalIOMemoryOn);
EXPORT_SYMBOL(i_APCI1710_SetDigitalIOMemoryOff);
EXPORT_SYMBOL(i_APCI1710_SetDigitalIOChlOn);
EXPORT_SYMBOL(i_APCI1710_SetDigitalIOChlOff);
EXPORT_SYMBOL(i_APCI1710_SetDigitalIOPortOn);
EXPORT_SYMBOL(i_APCI1710_SetDigitalIOPortOff);
		
EXPORT_NO_SYMBOLS;
//------------------------------------------------------------------------------

/** Configure the digital I/O operating mode.
 * 
 * Configure the digital I/O operating mode from selected
 * module  (b_ModulNbr). You must calling this function be
 * for you call any other function witch access of digital I/O.                                                   
 * 
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_ChannelAMode        : Channel A mode selection <br>
 *                                     0 : Channel used for digital input <br>
 *                                     1 : Channel used for digital output
 * @param [in] b_ChannelBMode        : Channel B mode selection <br>
 *                                     0 : Channel used for digital input
 *                                     1 : Channel used for digital output
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Bi-directional channel A configuration error.
 * @retval 5: Bi-directional channel B configuration error.
 */
int   i_APCI1710_InitDigitalIO (struct pci_dev *pdev,
								uint8_t b_ModulNbr,
								uint8_t b_ChannelAMode,
								uint8_t b_ChannelBMode)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_WriteConfig = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if digital I/O counter */
	   /*******************************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_DIGITAL_IO)
	      {
	      /***************************************************/
	      /* Test the bi-directional channel A configuration */
	      /***************************************************/

	      if ((b_ChannelAMode == 0) || (b_ChannelAMode == 1))
		 {
		 /***************************************************/
		 /* Test the bi-directional channel B configuration */
		 /***************************************************/

		 if ((b_ChannelBMode == 0) || (b_ChannelBMode == 1))
		    {
		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [(int)b_ModulNbr].
		    s_DigitalIOInfo.
		    b_DigitalInit = 1;

		    /********************************/
		    /* Save channel A configuration */
		    /********************************/

		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [(int)b_ModulNbr].
		    s_DigitalIOInfo.
		    b_ChannelAMode = b_ChannelAMode;

		    /********************************/
		    /* Save channel B configuration */
		    /********************************/

		    APCI1710_PRIVDATA(pdev)->
		    s_ModuleInfo [(int)b_ModulNbr].
		    s_DigitalIOInfo.
		    b_ChannelBMode = b_ChannelBMode;

		    /*****************************************/
		    /* Set the channel A and B configuration */
		    /*****************************************/

		    dw_WriteConfig = (uint32_t) (b_ChannelAMode | (b_ChannelBMode * 2));

		    /***************************/
		    /* Write the configuration */
		    /***************************/

		    OUTPDW (GET_BAR2(pdev),
			    4 + MODULE_OFFSET(b_ModulNbr),
			    dw_WriteConfig);
		    }
		 else
		    {
		    /************************************************/
		    /* Bi-directional channel B configuration error */
		    /************************************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /************************************************/
		 /* Bi-directional channel A configuration error */
		 /************************************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a digital I/O module */
	      /******************************************/

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

/** Read the status from selected digital I/O digital input (b_InputChannel).
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_InputChannel        : Selection from digital input (0 to 6) <br>
 *                                     0 : Channel C <br>         
 *                                     1 : Channel D <br>         
 *                                     2 : Channel E <br>         
 *                                     3 : Channel F <br>         
 *                                     4 : Channel G <br>         
 *                                     5 : Channel A <br>         
 *                                     6 : Channel B         
 *
 * @param [out] pb_ChannelStatus     : Digital input channel status <br>
 *                                     0 : Channle is not active <br>
 *                                     1 : Channle is active    
 *
 * @retval 0: No error.                                            
 * @retval 1: The handle parameter of the board is wrong.          
 * @retval 2: The module parameter is wrong.                       
 * @retval 3: The module is not a digital I/O module.              
 * @retval 4: The selected digital I/O digital input is wrong.     
 * @retval 5: Digital I/O not initialised.                         
 * @retval 6: The digital channel A is used for output.            
 * @retval 7: The digital channel B is used for output.            
 */
int   i_APCI1710_ReadDigitalIOChlValue (struct pci_dev *pdev,
										uint8_t    b_ModulNbr,
										uint8_t    b_InputChannel,
										uint8_t *  pb_ChannelStatus)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_StatusReg;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if digital I/O counter */
	   /*******************************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_DIGITAL_IO)
	      {
	      /******************************************/
	      /* Test the digital imnput channel number */
	      /******************************************/

	      if (b_InputChannel <= 6)
		 {
		 /**********************************************/
		 /* Test if the digital I/O module initialised */
		 /**********************************************/

		 if (APCI1710_PRIVDATA(pdev)->
		     s_ModuleInfo [(int)b_ModulNbr].
		     s_DigitalIOInfo.
		     b_DigitalInit == 1)
		    {
		    /**********************************/
		    /* Test if channel A or channel B */
		    /**********************************/

		    if (b_InputChannel > 4)
		       {
		       /*********************/
		       /* Test if channel A */
		       /*********************/

		       if (b_InputChannel == 5)
			  {
			  /***************************/
			  /* Test the channel A mode */
			  /***************************/

			  if (APCI1710_PRIVDATA(pdev)->
			      s_ModuleInfo [(int)b_ModulNbr].
			      s_DigitalIOInfo.
			      b_ChannelAMode != 0)
			     {
			     /********************************************/
			     /* The digital channel A is used for output */
			     /********************************************/

			     i_ReturnValue = -6;
			     }
			  } // if (b_InputChannel == 5)
		       else
			  {
			  /***************************/
			  /* Test the channel B mode */
			  /***************************/

			  if (APCI1710_PRIVDATA(pdev)->
			      s_ModuleInfo [(int)b_ModulNbr].
			      s_DigitalIOInfo.
			      b_ChannelBMode != 0)
			     {
			     /********************************************/
			     /* The digital channel B is used for output */
			     /********************************************/

			     i_ReturnValue = -7;
			     }
			  } // if (b_InputChannel == 5)
		       } // if (b_InputChannel > 4)

		    /***********************/
		    /* Test if error occur */
		    /***********************/

		    if (i_ReturnValue == 0)
		       {
		       /**************************/
		       /* Read all digital input */
		       /**************************/

		       INPDW (GET_BAR2(pdev),
			      MODULE_OFFSET(b_ModulNbr),
			      &dw_StatusReg);

		       *pb_ChannelStatus = (uint8_t) ((dw_StatusReg ^ 0x1C) >> b_InputChannel) & 1;
		       } // if (i_ReturnValue == 0)
		    }
		 else
		    {
		    /*******************************/
		    /* Digital I/O not initialised */
		    /*******************************/

		    i_ReturnValue = 5;
		    }
		 }
	      else
		 {
		 /********************************/
		 /* Selected digital input error */
		 /********************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a digital I/O module */
	      /******************************************/

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

/** Read the status from digital input port.               
 * 
 * Read the status from digital input port from selected 
 * digital I/O module (b_ModulNbr).
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 *
 * @param [out] pb_PortValue         : Digital I/O inputs port status.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Digital I/O not initialised.
 */
int   i_APCI1710_ReadDigitalIOPortValue  (struct pci_dev *pdev,
											uint8_t     b_ModulNbr,
											uint8_t *   pb_PortValue)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_StatusReg;
	
    if (!pdev) return 1;	

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if digital I/O counter */
	   /*******************************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_DIGITAL_IO)
	      {
	      /**********************************************/
	      /* Test if the digital I/O module initialised */
	      /**********************************************/

	      if (APCI1710_PRIVDATA(pdev)->
		  s_ModuleInfo [(int)b_ModulNbr].
		  s_DigitalIOInfo.
		  b_DigitalInit == 1)
		 {
		 /**************************/
		 /* Read all digital input */
		 /**************************/

		 INPDW (GET_BAR2(pdev),
			MODULE_OFFSET(b_ModulNbr),
			&dw_StatusReg);

		 *pb_PortValue = (uint8_t) (dw_StatusReg ^ 0x1C);
		 }
	      else
		 {
		 /*******************************/
		 /* Digital I/O not initialised */
		 /*******************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a digital I/O module */
	      /******************************************/

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

/** Activates the digital output memory. 
 * 
 * After having called up this function, the output you have previously
 * activated with the function "i_APCI1710_SetDigitalIOOutputXOn" are not reset.
 * You can reset them with the function "i_APCI1710_SetDigitalIOOutputXOff".                  
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Digital I/O not initialised.
 */
int   i_APCI1710_SetDigitalIOMemoryOn (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int    i_ReturnValue = 0;
	
    if (!pdev) return 1;	

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if digital I/O counter */
	   /*******************************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_DIGITAL_IO)
	      {
	      /**********************************************/
	      /* Test if the digital I/O module initialised */
	      /**********************************************/

	      if (APCI1710_PRIVDATA(pdev)->
		  s_ModuleInfo [(int)b_ModulNbr].
		  s_DigitalIOInfo.
		  b_DigitalInit == 1)
		 {
		 /****************************/
		 /* Set the output memory on */
		 /****************************/

		 APCI1710_PRIVDATA(pdev)->
		 s_ModuleInfo [(int)b_ModulNbr].
		 s_DigitalIOInfo.
		 b_OutputMemoryEnabled = 1;

		 /***************************/
		 /* Clear the output memory */
		 /***************************/

		 APCI1710_PRIVDATA(pdev)->
		 s_ModuleInfo [(int)b_ModulNbr].
		 s_DigitalIOInfo.
		 dw_OutputMemory = 0;
		 }
	      else
		 {
		 /*******************************/
		 /* Digital I/O not initialised */
		 /*******************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a digital I/O module */
	      /******************************************/

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

/** Deactivates the digital output memory.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * 
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Digital I/O not initialised.
 */
int   i_APCI1710_SetDigitalIOMemoryOff (struct pci_dev *pdev, uint8_t b_ModulNbr)
	{
	int    i_ReturnValue = 0;

    if (!pdev) return 1;

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if digital I/O counter */
	   /*******************************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_DIGITAL_IO)
	      {
	      /**********************************************/
	      /* Test if the digital I/O module initialised */
	      /**********************************************/

	      if (APCI1710_PRIVDATA(pdev)->
		  s_ModuleInfo [(int)b_ModulNbr].
		  s_DigitalIOInfo.
		  b_DigitalInit == 1)
		 {
		 /*****************************/
		 /* Set the output memory off */
		 /*****************************/

		 APCI1710_PRIVDATA(pdev)->
		 s_ModuleInfo [(int)b_ModulNbr].
		 s_DigitalIOInfo.
		 b_OutputMemoryEnabled = 0;
		 }
	      else
		 {
		 /*******************************/
		 /* Digital I/O not initialised */
		 /*******************************/

		 i_ReturnValue = 4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a digital I/O module */
	      /******************************************/

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

/** Sets the output. 
 * 
 * Sets the output witch has been passed with the
 * parameter b_Channel. Setting an output means setting
 * an ouput high.                                         
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_OutputChannel       : Selection from digital output <br>
 *                                     channel (0 to 2) <br>               
 *                                     0 : Channel H <br>
 *                                     1 : Channel A <br>
 *                                     2 : Channel B
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: The selected digital output is wrong.
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".
 * @retval 6: The digital channel A is used for input.
 * @retval 7: The digital channel B is used for input.
 */
int   i_APCI1710_SetDigitalIOChlOn (struct pci_dev *pdev,
									uint8_t b_ModulNbr,
									uint8_t b_OutputChannel)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_WriteValue;

    if (!pdev) return 1;
    
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if digital I/O counter */
	   /*******************************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_DIGITAL_IO)
	      {
	      /**********************************************/
	      /* Test if the digital I/O module initialised */
	      /**********************************************/

	      if (APCI1710_PRIVDATA(pdev)->
		  s_ModuleInfo [b_ModulNbr].
		  s_DigitalIOInfo.
		  b_DigitalInit == 1)
		 {
		 /******************************************/
		 /* Test the digital output channel number */
		 /******************************************/

		 if (b_OutputChannel == 1)
		    {
		    if (APCI1710_PRIVDATA(pdev)->
		        s_ModuleInfo [(int)b_ModulNbr].
		        s_DigitalIOInfo.
		        b_ChannelAMode != 1)
		       {
		       /*******************************************/
		       /* The digital channel A is used for input */
		       /*******************************************/

		       i_ReturnValue = 6;
		       }
		    }

		    /*************/
		    /* Channel B */
		    /*************/

		 if (b_OutputChannel == 2)
		    {
		    if (APCI1710_PRIVDATA(pdev)->
		        s_ModuleInfo [(int)b_ModulNbr].
		        s_DigitalIOInfo.
		        b_ChannelBMode != 1)
		       {
		       /*******************************************/
		       /* The digital channel B is used for input */
		       /*******************************************/

		       i_ReturnValue = 7;
		       }
		    }

		 if (b_OutputChannel > 2)
		    {
	            /****************************************/
		    /* The selected digital output is wrong */
		    /****************************************/

		    i_ReturnValue = 4;
		    }

		 /***********************/
		 /* Test if error occur */
		 /***********************/

		 if (!i_ReturnValue)
		    {
		    /*********************************/
		    /* Test if output memory enabled */
		    /*********************************/

		    if (APCI1710_PRIVDATA(pdev)->
			s_ModuleInfo [(int)b_ModulNbr].
			s_DigitalIOInfo.
			b_OutputMemoryEnabled == 1)
		       {
		       dw_WriteValue = APCI1710_PRIVDATA(pdev)->
				       s_ModuleInfo [(int)b_ModulNbr].
				       s_DigitalIOInfo.
				       dw_OutputMemory | (1 << b_OutputChannel);

		       APCI1710_PRIVDATA(pdev)->
		       s_ModuleInfo [(int)b_ModulNbr].
		       s_DigitalIOInfo.
		       dw_OutputMemory = dw_WriteValue;
		       }
		    else
		       {
		       dw_WriteValue = 1 << b_OutputChannel;
		       }

		    /*******************/
		    /* Write the value */
		    /*******************/

		    OUTPDW (GET_BAR2(pdev),
			    MODULE_OFFSET(b_ModulNbr),
			    dw_WriteValue);
		    }
		 }
	      else
		 {
		 /*******************************/
		 /* Digital I/O not initialised */
		 /*******************************/

		 i_ReturnValue = 5;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a digital I/O module */
	      /******************************************/

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

/** Resets the output. 
 * 
 * Resets the output witch has been passed with the
 * parameter b_Channel. Resetting an output means setting 
 * an ouput low.                                          
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_OutputChannel       : Selection from digital output channel (0 to 2) <br>              
 *                                     0 : Channel H <br>              
 *                                     1 : Channel A <br>              
 *                                     2 : Channel B               
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
 */
int   i_APCI1710_SetDigitalIOChlOff (struct pci_dev *pdev,
									uint8_t b_ModulNbr,
									uint8_t b_OutputChannel)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_WriteValue = 0;

    if (!pdev) return 1;
    
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if digital I/O counter */
	   /*******************************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_DIGITAL_IO)
	      {
	      /**********************************************/
	      /* Test if the digital I/O module initialised */
	      /**********************************************/

	      if (APCI1710_PRIVDATA(pdev)->
		  s_ModuleInfo [(int)b_ModulNbr].
		  s_DigitalIOInfo.
		  b_DigitalInit == 1)
		 {
		 /******************************************/
		 /* Test the digital output channel number */
		 /******************************************/

		    /*************/
		    /* Channel A */
		    /*************/

		 if (b_OutputChannel == 1)
		    {
		    if (APCI1710_PRIVDATA(pdev)->
		        s_ModuleInfo [(int)b_ModulNbr].
		        s_DigitalIOInfo.
		        b_ChannelAMode != 1)
		       {
		       /*******************************************/
		       /* The digital channel A is used for input */
		       /*******************************************/

		       i_ReturnValue = 6;
		       }
		    }

		    /*************/
		    /* Channel B */
		    /*************/

		 if (b_OutputChannel == 2)
		    {
	            if (APCI1710_PRIVDATA(pdev)->
                        s_ModuleInfo [(int)b_ModulNbr].
                        s_DigitalIOInfo.
                        b_ChannelBMode != 1)
                       {
                       /*******************************************/
                       /* The digital channel B is used for input */
                       /*******************************************/

                       i_ReturnValue = 7;
                       }
                    }

		 if (b_OutputChannel > 2)
		    {
	            /****************************************/
	            /* The selected digital output is wrong */
	            /****************************************/

	            i_ReturnValue = 4;
		    }

		 /***********************/
		 /* Test if error occur */
		 /***********************/

		 if (!i_ReturnValue)
		    {
		    /*********************************/
		    /* Test if output memory enabled */
		    /*********************************/

		    if (APCI1710_PRIVDATA(pdev)->
			s_ModuleInfo [(int)b_ModulNbr].
			s_DigitalIOInfo.
			b_OutputMemoryEnabled == 1)
		       {
		       dw_WriteValue = APCI1710_PRIVDATA(pdev)->
				       s_ModuleInfo [(int)b_ModulNbr].
				       s_DigitalIOInfo.
				       dw_OutputMemory & (0xFFFFFFFFUL - (1 << b_OutputChannel));

		       APCI1710_PRIVDATA(pdev)->
		       s_ModuleInfo [(int)b_ModulNbr].
		       s_DigitalIOInfo.
		       dw_OutputMemory = dw_WriteValue;
		       }
		    else
		       {
		       /*****************************/
		       /* Digital Output Memory OFF */
		       /*****************************/

		       i_ReturnValue = 8;
		       }

		    /*******************/
		    /* Write the value */
		    /*******************/

		    OUTPDW (GET_BAR2(pdev),
			    MODULE_OFFSET(b_ModulNbr),
			    dw_WriteValue);
		    }
		 }
	      else
		 {
		 /*******************************/
		 /* Digital I/O not initialised */
		 /*******************************/

		 i_ReturnValue = 5;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a digital I/O module */
	      /******************************************/

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

/** Sets one or several outputs from port.
 *
 * Setting an output means setting an output high.        
 * If you have switched OFF the digital output memory     
 * (OFF), all the other output are set to "0".            
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_PortValue           : Output Value ( 0 To 7 ).
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 * @retval 2: The module parameter is wrong.
 * @retval 3: The module is not a digital I/O module.
 * @retval 4: Output value wrong.
 * @retval 5: digital I/O not initialised see function "i_APCI1710_InitDigitalIO".
 * @retval 6: The digital channel A is used for input.
 * @retval 7: The digital channel B is used for input.
 */
int   i_APCI1710_SetDigitalIOPortOn (struct pci_dev *pdev,
									uint8_t b_ModulNbr,
									uint8_t b_PortValue)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_WriteValue;

    if (!pdev) return 1;
    
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if digital I/O counter */
	   /*******************************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_DIGITAL_IO)
	      {
	      /**********************************************/
	      /* Test if the digital I/O module initialised */
	      /**********************************************/

	      if (APCI1710_PRIVDATA(pdev)->
		  s_ModuleInfo [(int)b_ModulNbr].
		  s_DigitalIOInfo.
		  b_DigitalInit == 1)
		 {
		 /***********************/
		 /* Test the port value */
		 /***********************/

		 if (b_PortValue <= 7)
		    {
		    /***********************************/
		    /* Test the digital output channel */
		    /***********************************/

		    /**************************/
		    /* Test if channel A used */
		    /**************************/

		    if ((b_PortValue & 2) == 2)
		       {
		       if (APCI1710_PRIVDATA(pdev)->
			   s_ModuleInfo [(int)b_ModulNbr].
			   s_DigitalIOInfo.
			   b_ChannelAMode != 1)
			  {
			  /*******************************************/
			  /* The digital channel A is used for input */
			  /*******************************************/

			  i_ReturnValue = 6;
			  }
		       } // if ((b_PortValue & 2) == 2)

		    /**************************/
		    /* Test if channel B used */
		    /**************************/

		    if ((b_PortValue & 4) == 4)
		       {
		       if (APCI1710_PRIVDATA(pdev)->
			   s_ModuleInfo [(int)b_ModulNbr].
			   s_DigitalIOInfo.
			   b_ChannelBMode != 1)
			  {
			  /*******************************************/
			  /* The digital channel B is used for input */
			  /*******************************************/

			  i_ReturnValue = 7;
			  }
		       } // if ((b_PortValue & 4) == 4)

		    /***********************/
		    /* Test if error occur */
		    /***********************/

		    if (!i_ReturnValue)
		       {
		       /*********************************/
		       /* Test if output memory enabled */
		       /*********************************/

		       if (APCI1710_PRIVDATA(pdev)->
			   s_ModuleInfo [(int)b_ModulNbr].
			   s_DigitalIOInfo.
			   b_OutputMemoryEnabled == 1)
			  {
			  dw_WriteValue = APCI1710_PRIVDATA(pdev)->
					  s_ModuleInfo [(int)b_ModulNbr].
					  s_DigitalIOInfo.
					  dw_OutputMemory | b_PortValue;

			  APCI1710_PRIVDATA(pdev)->
			  s_ModuleInfo [(int)b_ModulNbr].
			  s_DigitalIOInfo.
			  dw_OutputMemory = dw_WriteValue;
			  }
		       else
			  {
			  dw_WriteValue = b_PortValue;
			  }

		       /*******************/
		       /* Write the value */
		       /*******************/

		       OUTPDW (GET_BAR2(pdev),
			       MODULE_OFFSET(b_ModulNbr),
			       dw_WriteValue);
		       }
		    }
		 else
		    {
		    /**********************/
		    /* Output value wrong */
		    /**********************/

		    i_ReturnValue = 4;
		    }
		 }
	      else
		 {
		 /*******************************/
		 /* Digital I/O not initialised */
		 /*******************************/

		 i_ReturnValue = 5;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a digital I/O module */
	      /******************************************/

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

/** Resets one or several output from port.
 *
 * Resetting means setting low.
 *
 * @param [in] pdev                  : The device to initialize.
 * @param [in] b_ModulNbr            : Module number to configure (0 to 3).
 * @param [in] b_PortValue           : Output Value ( 0 To 7 ).
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
 */
int   i_APCI1710_SetDigitalIOPortOff (struct pci_dev *pdev,
										uint8_t b_ModulNbr,
										uint8_t b_PortValue)
	{
	int    i_ReturnValue = 0;
	uint32_t dw_WriteValue = 0;

    if (!pdev) return 1;
    
	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModulNbr < NUMBER_OF_MODULE(pdev))
	   {
	   /*******************************/
	   /* Test if digital I/O counter */
	   /*******************************/

	   if ( APCI1710_MODULE_FUNCTIONALITY(pdev,b_ModulNbr) == APCI1710_DIGITAL_IO)
	      {
	      /**********************************************/
	      /* Test if the digital I/O module initialised */
	      /**********************************************/

	      if (APCI1710_PRIVDATA(pdev)->
		  s_ModuleInfo [(int)b_ModulNbr].
		  s_DigitalIOInfo.
		  b_DigitalInit == 1)
		 {
		 /***********************/
		 /* Test the port value */
		 /***********************/

		 if (b_PortValue <= 7)
		    {
		    /***********************************/
		    /* Test the digital output channel */
		    /***********************************/

		    /**************************/
		    /* Test if channel A used */
		    /**************************/

		    if ((b_PortValue & 2) == 2)
		       {
		       if (APCI1710_PRIVDATA(pdev)->
			   s_ModuleInfo [(int)b_ModulNbr].
			   s_DigitalIOInfo.
			   b_ChannelAMode != 1)
			  {
			  /*******************************************/
			  /* The digital channel A is used for input */
			  /*******************************************/

			  i_ReturnValue = 6;
			  }
		       } // if ((b_PortValue & 2) == 2)

		    /**************************/
		    /* Test if channel B used */
		    /**************************/

		    if ((b_PortValue & 4) == 4)
		       {
		       if (APCI1710_PRIVDATA(pdev)->
			   s_ModuleInfo [(int)b_ModulNbr].
			   s_DigitalIOInfo.
			   b_ChannelBMode != 1)
			  {
			  /*******************************************/
			  /* The digital channel B is used for input */
			  /*******************************************/

			  i_ReturnValue = 7;
			  }
		       } // if ((b_PortValue & 4) == 4)

		    /***********************/
		    /* Test if error occur */
		    /***********************/

		    if (!i_ReturnValue)
		       {
		       /*********************************/
		       /* Test if output memory enabled */
		       /*********************************/

		       if (APCI1710_PRIVDATA(pdev)->
			   s_ModuleInfo [(int)b_ModulNbr].
			   s_DigitalIOInfo.
			   b_OutputMemoryEnabled == 1)
			  {
			  dw_WriteValue = APCI1710_PRIVDATA(pdev)->
					  s_ModuleInfo [(int)b_ModulNbr].
					  s_DigitalIOInfo.
					  dw_OutputMemory & (0xFFFFFFFFUL - b_PortValue);

			  APCI1710_PRIVDATA(pdev)->
			  s_ModuleInfo [(int)b_ModulNbr].
			  s_DigitalIOInfo.
			  dw_OutputMemory = dw_WriteValue;
			  }
		       else
			  {
			  /*****************************/
			  /* Digital Output Memory OFF */
			  /*****************************/

			  i_ReturnValue = 8;
			  }

		       /*******************/
		       /* Write the value */
		       /*******************/

		       OUTPDW (GET_BAR2(pdev),
			       MODULE_OFFSET(b_ModulNbr),
			       dw_WriteValue);
		       }
		    }
		 else
		    {
		    /**********************/
		    /* Output value wrong */
		    /**********************/

		    i_ReturnValue = 4;
		    }
		 }
	      else
		 {
		 /*******************************/
		 /* Digital I/O not initialised */
		 /*******************************/

		 i_ReturnValue = 5;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a digital I/O module */
	      /******************************************/

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
