/** @file vtable.c
*
* @author Sylvain Nahas
*
* A vtable is an area of pointers to ioctl-implementing functions
*/

/** @par LICENCE
* @verbatim
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

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details. 
        
    You shoud find the complete GPL in the COPYING file accompanying 
    this source code. 
* @endverbatim
*/

#include "apci1710-private.h"
#include "vtable.h"

EXPORT_NO_SYMBOLS;


//------------------------------------------------------------------------------

vtable_t apci1710_vtable __cacheline_aligned; 

//------------------------------------------------------------------------------
/** Affect an vtable entry.
 * 
 * @return 0 if OK, -1 if problem 
 */
int __APCI_1710_DECLARE_IOCTL_HANDLER(vtable_t vtable ,unsigned int cmd__,  __do_fcn * handler__ptr) 
{
	
	/* better to be paranoid - catches bugs at run time */
	if (!vtable)
	{
		printk("%s: ioctl %d: vtable is NULL\n",__FUNCTION__,_IOC_NR(cmd__));
		return -1;
	}
	
	/* check boundaries - vtable size is known at compile time - 
	 * using it avoid code dependancy on macro definition */
	if ( _IOC_NR(cmd__) > (VTABLE_ELEMENT_NB(vtable_t)-1) )
	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)
		printk("%s: ioctl %d: vtable entry index is over table boundaries (%lu elements)\n",__FUNCTION__,_IOC_NR(cmd__),VTABLE_ELEMENT_NB(vtable_t) );
#else
		printk("%s: ioctl %d: vtable entry index is over table boundaries (%u elements)\n",__FUNCTION__,_IOC_NR(cmd__),VTABLE_ELEMENT_NB(vtable_t) );
#endif
		return -1;		
	}		
	
	/* vtable entries must be set to apci1710_do_dummy before affectation with clear_vtable */
	
	vtable[_IOC_NR(cmd__)] = handler__ptr;
	
	return 0;
}

//------------------------------------------------------------------------------
/** This function initialise a vtable (set all entry to dummy)
 * 
 * @param vtable VTABLE to intialise
 * 
 * */
void apci1710_dummyze_vtable(vtable_t vtable, __do_fcn * dummyfcn )
{
	int i;
	for ( i=0; i< VTABLE_ELEMENT_NB(vtable_t) ; i++ )
	{
		vtable[i] = dummyfcn;
	}
}
