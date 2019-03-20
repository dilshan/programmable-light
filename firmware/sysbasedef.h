/*************************************************************************
* Title:	ATmega8 Firmware for programmable light controller.
* Author:	Dilshan R Jayakody <jayakody2000lk@gmail.com>
* Project:	Programmable LED controller.
* Homepage:	TBD
* File:     sysbasedef.h
* Info:		Core system definitions.
* Compiler: AVR GCC 5.4.0 (AVR 8-bit GNU Toolchain 3.6.1)
* Target:   ATmega8L / ATmega8A
**************************************************************************/

#ifndef SYSTEM_DEFINITION_HEADER
#define SYSTEM_DEFINITION_HEADER

// Define base data types.
#define VOID	void
#define CHAR	signed char
#define UCHAR	unsigned char
#define PUCHAR	unsigned char*
#define INT		int
#define UINT	unsigned int
#define UINT32	unsigned long

// Definition for logical TRUE and FALSE
#define TRUE	0xFF
#define FALSE	0x00

// For this system clock frequency should be 4 MHz.
#ifndef F_CPU
#define F_CPU	4000000UL
#endif

// Seven segment display related parameters.
#define SSD_SIZE	4

// Data structure to hold Seven segment related data.
struct displayBufferStruct
{
	UCHAR valueBuffer[SSD_SIZE];
	UCHAR decimalPoint;
};

#define DISPLAY		struct displayBufferStruct
#define PDISPLAY	DISPLAY*

// Time structure.
struct timeStruct
{
	UCHAR hours;
	UCHAR minutes;
	UCHAR seconds;	
};

#define TIME	struct timeStruct
#define PTIME	TIME*

// Main seven segment display modes.
enum displayMode
{
	SSD_DISPLAY_TIME,
	SSD_DISPLAY_START,
	SSD_DISPLAY_END,
	SSD_DISPLAY_NONE	
};

#define DISPLAY_MODE enum displayMode

// Options menu modes.
enum menuMode
{
	SSD_MENU_TIME,
	SSD_MENU_ON_TIME,
	SSD_MENU_OFF_TIME,
	SSD_MENU_EXIT
};

#define MENU_MODE enum menuMode

#endif