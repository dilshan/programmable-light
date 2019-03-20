/*************************************************************************
* Title:	ATmega8 Firmware for programmable light controller.
* Author:	Dilshan R Jayakody <jayakody2000lk@gmail.com>
* Project:	Programmable LED controller.
* Homepage:	TBD
* File:     rtcmodule.h
* Info:		Collection of routines to manipulate time data structure.
* Compiler: AVR GCC 5.4.0 (AVR 8-bit GNU Toolchain 3.6.1)
* Target:   ATmega8L / ATmega8A
**************************************************************************/

#ifndef TIME_MODULE_HEADER
#define TIME_MODULE_HEADER

VOID sysTimeToDisplayBuffer(PTIME timeData, PDISPLAY displayData);
UINT32 timeToInt32(PTIME timeData);
UCHAR isLightActive(PTIME currTime, PTIME onTime, PTIME offTime);

#endif