/**********************************************************/
/* board-arduino.h                                        */
/* Copyright (c) 2010 by thomas seiler                    */
/* -------------------------------------------------------*/
/*                                                        */
/* This program is free software; you can redistribute it */
/* and/or modify it under the terms of the GNU General    */
/* Public License as published by the Free Software       */
/* Foundation; either version 2 of the License, or        */
/* (at your option) any later version.                    */
/*                                                        */
/* This program is distributed in the hope that it will   */
/* be useful, but WITHOUT ANY WARRANTY; without even the  */
/* implied warranty of MERCHANTABILITY or FITNESS FOR A   */
/* PARTICULAR PURPOSE.  See the GNU General Public        */
/* License for more details.                              */
/*                                                        */
/* You should have received a copy of the GNU General     */
/* Public License along with this program; if not, write  */
/* to the Free Software Foundation, Inc.,                 */
/* 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA */
/*                                                        */
/* Licence can be viewed at                               */
/* http://www.fsf.org/licenses/gpl.txt                    */
/**********************************************************/


#ifndef _board_h_
#define _board_h_

/* define BAUD_RATE
 * use 57600 if the processor is a atmega328p or atmega1280
 * use 19200 otherwise (atmega8 and atmega168)
 */
#if   defined(__AVR_ATmega328P__) || defined(__AVR_ATmega1280)
#define BAUD_RATE 57600
#else
#define BAUD_RATE 19200
#endif






#endif
