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

#ifndef MEMORY_MODULE_HEADER
#define MEMORY_MODULE_HEADER

VOID saveTimeToMemory(PTIME timeInfo, UCHAR offset);
VOID readTimeFromMemory(PTIME timeInfo, UCHAR offset);

#endif
