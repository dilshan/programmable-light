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