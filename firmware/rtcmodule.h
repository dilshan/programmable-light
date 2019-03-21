/*************************************************************************
* Title:	ATmega8 Firmware for programmable light controller.
* Author:	Dilshan R Jayakody <jayakody2000lk@gmail.com>
* Project:	Programmable LED controller.
* Homepage:	https://github.com/dilshan/programmable-light
* File:     rtcmodule.h
* Info:		DS1307 RTC module related routines.
* Compiler: AVR GCC 5.4.0 (AVR 8-bit GNU Toolchain 3.6.1)
* Target:   ATmega8L / ATmega8A
**************************************************************************/

#ifndef DS1307_RTC_MODULE_HEADER
#define DS1307_RTC_MODULE_HEADER

VOID initRTCModule();
VOID getSystemTime(PTIME timeInfo);
VOID setSystemTime(PTIME timeInfo);

UCHAR bcdToDec(UCHAR inVal);
UCHAR decToBcd(UCHAR inVal);

#endif
