/*************************************************************************
* Title:	ATmega8 Firmware for programmable light controller.
* Author:	Dilshan R Jayakody <jayakody2000lk@gmail.com>
* Project:	Programmable LED controller.
* Homepage:	https://github.com/dilshan/programmable-light
* File:     main.h
* Info:		Main firmware module with applicaion entry point.
* Compiler: AVR GCC 5.4.0 (AVR 8-bit GNU Toolchain 3.6.1)
* Target:   ATmega8L / ATmega8A
**************************************************************************/

#ifndef MAIN_HEADER
#define MAIN_HEADER

#include "sysbasedef.h"

#define LONG_PRESS_LIMIT	21
#define SLEEP_TIMEOUT	5

#define IS_BUTTON_PRESSED(s,p,l) (((s & p) == p) && (l & p) == 0x00)
#define IS_VALID_EEPROM_VALUE(p) p=(p==0xFF)?0:p 

// System wide data structures and variables.
DISPLAY _displayBuffer;
DISPLAY_MODE _ssdMode;
TIME _sysTime;
TIME _startTime;
TIME _endTime;

UCHAR _rtcRefreshCounter;
UCHAR _blinkCounter;
UCHAR _isBlink;
UCHAR _blinkState;
UCHAR _sleepTimer;
UCHAR _isLightActive;

VOID initSystem();
VOID waitForButtonRelease();
VOID startSleepTimer();
VOID updateSleepLED(UCHAR isActive);

VOID showConfigurationOption();
VOID setupSystemTime();

#endif
