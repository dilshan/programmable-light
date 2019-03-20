/*************************************************************************
* Title:	ATmega8 Firmware for programmable light controller.
* Author:	Dilshan R Jayakody <jayakody2000lk@gmail.com>
* Project:	Programmable LED controller.
* Homepage:	TBD
* File:     rtcmodule.c
* Info:		DS1307 RTC module related routines.
* Compiler: AVR GCC 5.4.0 (AVR 8-bit GNU Toolchain 3.6.1)
* Target:   ATmega8L / ATmega8A
**************************************************************************/

#include "sysbasedef.h"
#include "rtcmodule.h"
#include "i2cmaster.h"

#include <avr/io.h>
#include <util/delay.h>	

#define DS1307_ADDRESS	0xD0

#define DS1307_SECONDS	0x00
#define DS1307_MINUTES	0x01
#define DS1307_HOURS	0x02
#define DS1307_CONTROL	0x07

/*************************************************************************
 Initialize DS1307 RTC module and start timer.

 Return: None
*************************************************************************/
VOID initRTCModule()
{
	// Initialize I2C module and library.
	i2c_init();
	_delay_ms(5);
	
	// Update CH bit of address 0x00.
	i2c_start_wait(DS1307_ADDRESS + I2C_WRITE);
	i2c_write(DS1307_SECONDS);
	i2c_write(0x00);
	i2c_stop();
}

/*************************************************************************
 Get system time from RTC.

 timeInfo: Data structure to fill current time.

 Return: None
*************************************************************************/
VOID getSystemTime(PTIME timeInfo)
{
	// Request beginning of the time registers of DS1307 RTC.
	i2c_start_wait(DS1307_ADDRESS + I2C_WRITE);
	i2c_write(DS1307_SECONDS);
	
	// Read time values from DS1307 RTC.
	i2c_start_wait(DS1307_ADDRESS + I2C_READ);
	timeInfo->seconds = bcdToDec(i2c_readAck() & 0x7F);
	timeInfo->minutes = bcdToDec(i2c_readAck());
	timeInfo->hours = bcdToDec(i2c_readNak());
	i2c_stop();
}

VOID setSystemTime(PTIME timeInfo)
{
	// Request beginning of the time registers of DS1307 RTC.
	i2c_start_wait(DS1307_ADDRESS + I2C_WRITE);
	i2c_write(DS1307_SECONDS);
	
	// Stop RTC oscillator and reset seconds.
	i2c_write(0x80);
	i2c_write(decToBcd(timeInfo->minutes));
	i2c_write(decToBcd(timeInfo->hours));
	
	// We don't care about date, month and year. Lets configure some fixed values to those parameters.
	i2c_write(decToBcd(0x01));
	i2c_write(decToBcd(0x01));
	i2c_write(decToBcd(0x01));
	i2c_write(decToBcd(0x015));
	i2c_stop();
	
	// Restart oscillator.
	i2c_start_wait(DS1307_ADDRESS + I2C_WRITE);
	i2c_write(DS1307_SECONDS);
	i2c_write(0x00);
	i2c_stop();
}

/*************************************************************************
 Convert specified BCD number to decimal number. 

 inVal: Number to convert in BCD format.

 Return: Converted number in decimal format.
*************************************************************************/
UCHAR bcdToDec(UCHAR inVal)
{
	UCHAR retVal = (inVal >> 4) * 10;
	retVal += inVal & (~(0b11110000));
	return retVal;
}

/*************************************************************************
 Convert specified decimal number to BCD number. 

 inVal: Number to convert in decimal format.

 Return: Converted number in BCD format.
*************************************************************************/
UCHAR decToBcd(UCHAR inVal)
{
	UCHAR retVal = ((inVal / 10) << 4);
	retVal += inVal % 10;
	return retVal;
}