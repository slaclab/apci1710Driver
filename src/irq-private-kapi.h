#ifndef APCI1710_INT_PRIVATE_H_
#define APCI1710_INT_PRIVATE_H_

//------------------------------------------------------------------------------

/** Incremental counter interrupt function management.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ul_InterruptMask  : Interrupt mask.
 *
 * @param [out] pb_InterruptFlag : 0 -> No interrupt generated.
 *                                 1 -> Interrupt generated.
 */
void	v_APCI1710_IncrementalCounter_InterruptFunction (struct pci_dev *pdev,
                                                         uint8_t b_Module,
                                                         uint8_t * pb_InterruptFlag);

/** Impuls counter interrupt function management.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ul_InterruptMask  : Interrupt mask.
 *
 * @param [out] pb_InterruptFlag : 0 -> No interrupt generated.
 *                                 1 -> Interrupt generated.
 */
void	v_APCI1710_ImpulsCounter_InterruptFunction (struct pci_dev *pdev,
                                                    uint8_t b_Module,
                                                    uint8_t * pb_InterruptFlag);

/** Chronos interrupt function management.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ul_InterruptMask  : Interrupt mask.
 *
 * @param [out] pb_InterruptFlag : 0 -> No interrupt generated.
 *                                 1 -> Interrupt generated.
 */
void	v_APCI1710_Chronos_InterruptFunction (struct pci_dev *pdev,
                                                    uint8_t b_Module,
                                                    uint8_t * pb_InterruptFlag);

/** ETM interrupt function management.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ul_InterruptMask  : Interrupt mask.
 *
 * @param [out] pb_InterruptFlag : 0 -> No interrupt generated.
 *                                 1 -> Interrupt generated.
 */
void v_APCI1710_ETM_InterruptFunction	(struct pci_dev *pdev,
						 uint8_t b_Module,
						 uint8_t * pb_InterruptFlag);

#ifdef WITH_IDV_OPTION
/** IDV interrupt function management.
 *
 * @param [in] pdev              : The device to initialize.
 * @param [in] b_ModulNbr        : Module number to configure (0 to 3).
 * @param [in] ul_InterruptMask  : Interrupt mask.
 *
 * @param [out] pb_InterruptFlag : 0 -> No interrupt generated.
 *                                 1 -> Interrupt generated.
 */
void v_APCI1710_IDV_InterruptFunction (struct pci_dev *pdev,
                                      uint8_t b_Module,
                                      uint8_t * pb_InterruptFlag);
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** User interrupt function call management.
 *
 * @param [in] pdev                : The device to initialize.
 * @param [in] b_ModulNbr          : Module number to configure (0 to 3).
 * @param [in] ul_InterruptMask    : Interrupt mask.
 * @param [in] ul_Value            : Values.
 *
 */
static __inline__ void v_APCI1710_UserInterruptManagement (struct pci_dev *pdev,
                                            uint8_t b_Module,
                                            uint32_t ul_InterruptMask,
                                            uint32_t *ul_Value)
	{

	/***************************/
	/* Save the Interrupt mask */
	/***************************/

    APCI1710_PRIVDATA(pdev)->
	s_InterruptParameters.
	s_FIFOInterruptParameters [APCI1710_PRIVDATA(pdev)->
				   s_InterruptParameters.
				   ui_Write].
	ul_OldInterruptMask = ul_InterruptMask;

	/**************************/
	/* Save the Module number */
	/**************************/

	APCI1710_PRIVDATA(pdev)->
	s_InterruptParameters.
	s_FIFOInterruptParameters [APCI1710_PRIVDATA(pdev)->
				   s_InterruptParameters.
				   ui_Write].
	b_OldModuleMask = b_Module;

	/**************************/
	/* Save the counter value */
	/**************************/

	APCI1710_PRIVDATA(pdev)->
	s_InterruptParameters.
	s_FIFOInterruptParameters [APCI1710_PRIVDATA(pdev)->
				   s_InterruptParameters.
				   ui_Write].ul_OldValue[0] = ul_Value[0];

	// If ETM interrupt informations
	if ((0x60000UL & ul_InterruptMask) != 0)
	{
		APCI1710_PRIVDATA(pdev)->
		s_InterruptParameters.
		s_FIFOInterruptParameters [APCI1710_PRIVDATA(pdev)->
					   s_InterruptParameters.
					   ui_Write].ul_OldValue[1] = ul_Value[1];
	}

	/***********************************/
	/* Increment the interrupt counter */
	/***********************************/

	APCI1710_PRIVDATA(pdev)->
	s_InterruptParameters.
	ul_InterruptOccur ++;

	/****************************/
	/* Increment the write FIFO */
	/****************************/

	APCI1710_PRIVDATA(pdev)->
	s_InterruptParameters.
	ui_Write = (APCI1710_PRIVDATA(pdev)->
		    s_InterruptParameters.
		    ui_Write + 1) % APCI1710_SAVE_INTERRUPT;

	/**********************/
	/* Call user function */
	/**********************/

	/* Call the user kernel callback */
	if (APCI1710_PRIVDATA(pdev)->s_UserInterruptCallback.v_UserInterruptFunction != NULL)
		(APCI1710_PRIVDATA(pdev)->s_UserInterruptCallback.v_UserInterruptFunction ) (pdev);

	/* Asynchronous IO signal implementation - send a SIGIO to all registered processes*/
	if (APCI1710_PRIVDATA(pdev)->async_queue)
		{
		kill_fasync( &(APCI1710_PRIVDATA(pdev)->async_queue), SIGIO, POLL_IN);
		}
	}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Global variable to register functionality interrupts */
extern str_InterruptFunctionality s_InterruptFunctionality[];

#endif /*APCI1710_INT_PRIVATE_H_*/
