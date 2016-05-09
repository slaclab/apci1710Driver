/** @file vtable.h
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

#ifndef __apci1710_VTABLE_H__
#define __apci1710_VTABLE_H__

/** A vtable is an area of pointer to functions implementing the ioctl calls. 
 * The cmd are indexes from 1, the address from 0. 
 * Here we lose the field index 0 [sizeof(void*) of memory], 
 * but it avoid to decrement value during each call.
 * 
 * Type definition : allows latter use in board's private data, if needed
 * 
 * @note __apci1710_UPPER_IOCTL_CMD must be defined in apci1710.h
 * 
 * */
 
typedef __do_fcn* vtable_t[__APCI1710_UPPER_IOCTL_CMD+1]; 

/** nomber of elements in the vtable.
 * 
 * allows to be independant of any macro used in vtable's definition.
 * evaluation is one at compile time.
 * */
#define VTABLE_ELEMENT_NB(__vtable)  ( sizeof(__vtable) / (sizeof(__do_fcn*)) ) 

/** the actual vtable 
 * The actual vtable is declared is ioctl.c
 * */
extern vtable_t apci1710_vtable;

/** to be called in main.c 
 * 
 * defined in ioctl.c
 * add new ioctl handlers there 
 * */
void apci1710_init_vtable(vtable_t vtable);

/** set all entry of the vtable to apci1710_do_dummy()  */
void apci1710_dummyze_vtable(vtable_t vtable, __do_fcn * dummyfcn );

/** Affect an vtable entry.
 * 
 *  @return 0 if OK, -1 if problem */
extern int __APCI_1710_DECLARE_IOCTL_HANDLER(vtable_t vtable ,unsigned int cmd__,  __do_fcn * handler__ptr);

#endif // __VTABLE_H__
