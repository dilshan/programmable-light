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

#include "sysbasedef.h"
#include "timemodule.h"

/*************************************************************************
 Transform TIME data structure into DISPLAY data structure to render 
 values in seven segment display.
 
 timeData: Instance of the TIME data structure.
 
 displayData: Instance to hold transformed results. 
 
 Return: None
*************************************************************************/
VOID sysTimeToDisplayBuffer(PTIME timeData, PDISPLAY displayData)
{
	// Fill hour value into display buffer.
	displayData->valueBuffer[0] = timeData->hours / 10;
	displayData->valueBuffer[1] = timeData->hours % 10;
	
	// Fill minute value into display buffer.
	displayData->valueBuffer[2] = timeData->minutes / 10;
	displayData->valueBuffer[3] = timeData->minutes % 10;
	
	displayData->decimalPoint = ((timeData->seconds % 2) != 0) ? 0x01 : 0xFF;
}

/*************************************************************************
 Convert specified time structure to 32-bit unsigned integer.
 
 timeData: Instance of the TIME data structure.
 
 Return: Time as 32-bit unsigned integer.
*************************************************************************/
UINT32 timeToInt32(PTIME timeData)
{
	UINT32 tempHour = timeData->hours;
	UINT32 tempMinute = timeData->minutes;
	UINT32 tempSeconds = timeData->seconds;
	
	return (tempHour << 16) | (tempMinute << 8) | (tempSeconds);
}

/*************************************************************************
 Determine light on / off state by checking the specified time parameters.
 
 currTime: Current system time.
 
 onTime: Light on time.
 
 offTime: Light off time.
 
 Return: TRUE if light should open, otherwise this function return FALSE.
*************************************************************************/
UCHAR isLightActive(PTIME currTime, PTIME onTime, PTIME offTime)
{
	// Convert specified TIME data structures to 32-bit unsigned integers.
	UINT32 startTime = timeToInt32(onTime);
	UINT32 stopTime = timeToInt32(offTime);
	UINT32 sysTime = timeToInt32(currTime);
	
	if (stopTime == startTime)
	{
		// Nothing happens if start and end times are equal.
		return FALSE;
	}
	else if (stopTime > startTime)
	{
		// Start and end time are defined before midnight.
		return (sysTime >= startTime) && (sysTime < stopTime) ? TRUE : FALSE;
	}
	if (stopTime == 0x00)
	{
		// Stop time is set to mid night.
		return (sysTime < startTime) ? FALSE : TRUE;
	}
	else
	{
		// Start and end time are defined including midnight.
		return (sysTime >= startTime) || (sysTime < stopTime) ? TRUE : FALSE;
	}
}