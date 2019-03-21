/*************************************************************************
* Title:	ATmega8 Firmware for programmable light controller.
* Author:	Dilshan R Jayakody <jayakody2000lk@gmail.com>
* Project:	Programmable LED controller.
* Homepage:	https://github.com/dilshan/programmable-light
* File:     rtcmodule.h
* Info:		ATmega8 EEPROM module related routines.
* Compiler: AVR GCC 5.4.0 (AVR 8-bit GNU Toolchain 3.6.1)
* Target:   ATmega8L / ATmega8A
**************************************************************************/

#include "sysbasedef.h"
#include "memmodule.h"

#include <avr/eeprom.h>

/*************************************************************************
 Write specified time data structure to EEPROM.
 
 timeInfo: Instance of the time data structure.
 
 offset: Memory address offset.
 
 Return: None
*************************************************************************/
VOID saveTimeToMemory(PTIME timeInfo, UCHAR offset)
{
	eeprom_write_byte((UCHAR*)(offset + 0), timeInfo->seconds);
	eeprom_write_byte((UCHAR*)(offset + 1), timeInfo->minutes);
	eeprom_write_byte((UCHAR*)(offset + 2), timeInfo->hours);
}

/*************************************************************************
 Read specified time data structure from EEPROM.
 
 timeInfo: Instance of the time data structure to load content.
 
 offset: Memory address offset.
 
 Return: None
*************************************************************************/
VOID readTimeFromMemory(PTIME timeInfo, UCHAR offset)
{
	timeInfo->seconds = eeprom_read_byte((UCHAR*)(offset + 0));
	timeInfo->minutes = eeprom_read_byte((UCHAR*)(offset + 1));
	timeInfo->hours = eeprom_read_byte((UCHAR*)(offset + 2));
}
