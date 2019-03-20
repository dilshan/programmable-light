/*************************************************************************
* Title:	ATmega8 Firmware for programmable light controller.
* Author:	Dilshan R Jayakody <jayakody2000lk@gmail.com>
* Project:	Programmable LED controller.
* Homepage:	TBD
* File:     displaymodule.c
* Info:		Seven segment display related routines.
* Compiler: AVR GCC 5.4.0 (AVR 8-bit GNU Toolchain 3.6.1)
* Target:   ATmega8L / ATmega8A
**************************************************************************/

#ifndef DISPLAY_MODULE_HEADER
#define DISPLAY_MODULE_HEADER

#include "sysbasedef.h"

#define NO_EDIT_SEGMENT 0xFF

VOID setDIsplaySegment(UCHAR displayValue, UCHAR isDeimal);
VOID setDisplayValueSet(PDISPLAY displayInfo);

VOID textToDisplay(UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4, PDISPLAY dataBuffer);
VOID clearDisplay(PUCHAR valueSet, UCHAR valueSize);

VOID setEditSegment(UCHAR segmentId);
VOID setBlickState(UCHAR isActive);

#endif