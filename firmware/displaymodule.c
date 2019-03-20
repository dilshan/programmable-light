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

#include "sysbasedef.h"
#include "displaymodule.h"

#include <util/delay.h>
#include <avr/io.h>

// Define port outputs for all digits.
#define DIGIT_0		0x3F
#define DIGIT_1		0x06
#define DIGIT_2		0x5B
#define DIGIT_3		0x4F
#define DIGIT_4		0x66
#define DIGIT_5		0x6D
#define DIGIT_6		0x7D
#define DIGIT_7		0x07
#define DIGIT_8		0x7F
#define DIGIT_9		0x6F

// Define port outputs for English letters.
#define LETTER_N	0x54
#define LETTER_F	0x71
#define LETTER_E	0x79
#define LETTER_D	0x5E
#define LETTER_O	0x3F
#define LETTER_I	0x04
#define LETTER_T	0x78
#define LETTER_X	0x76
#define LETTER_Y	0x6E
#define LETTER__	0x40

// Define port output for SSD decimal indicator.
#define DECIMAL_POINT	0x80

UCHAR _editSegment = TRUE;
UCHAR _editBlink = TRUE;

/*************************************************************************
 Set currently active seven segment.

 segmentId: Active segment id starting from 0. Specify NO_EDIT_SEGMENT to
		    activate all the seven segment modules.
 
 Return: None
*************************************************************************/
VOID setEditSegment(UCHAR segmentId)
{
	_editSegment = segmentId;
}

/*************************************************************************
 Setup blink state of the seven segment display.

 isActive: Set this parameter to TRUE to blink the active seven segment.
 
 Return: None
*************************************************************************/
VOID setBlickState(UCHAR isActive)
{
	_editBlink = isActive;
}

/*************************************************************************
 Set display of the active seven segment display. 

 displayValue: Value to display on the seven segment display.

 isDeimal: set to TRUE / FALSE to activate the decimal indicator of the 
           seven segment display.
 
 Return: None
*************************************************************************/
VOID setDIsplaySegment(UCHAR displayValue, UCHAR isDeimal)
{
	switch(displayValue)	
	{
		// Digit mapping from 0 to 9.
		case 0x00:
			PORTD = DIGIT_0;
			break;
		case 0x01:
			PORTD = DIGIT_1;
			break;
		case 0x02:
			PORTD = DIGIT_2;
			break;
		case 0x03:
			PORTD = DIGIT_3;
			break;
		case 0x04:
			PORTD = DIGIT_4;
			break;
		case 0x05:
		case 0x53:
			PORTD = DIGIT_5;
			break;
		case 0x06:
			PORTD = DIGIT_6;
			break;
		case 0x07:
			PORTD = DIGIT_7;
			break;
		case 0x08:
			PORTD = DIGIT_8;
			break;
		case 0x09:
			PORTD = DIGIT_9;
			break;
			
		// Start English letter mappings.
		case 0x0A:
		case 0x44:
			PORTD = LETTER_D;
			break;
		case 0x45:
		case 0x0B:
			PORTD = LETTER_E;
			break;
		case 0x46:
		case 0x0C:
			PORTD = LETTER_F;
			break;
		case 0x49:
		case 0x0D:
			PORTD = LETTER_I;
			break;
		case 0x4E:
		case 0x0E:
		case 0x4D:
			PORTD = LETTER_N;
			break;
		case 0x0F:
		case 0x4F:
			PORTD = LETTER_O;
			break;
		case 0x10:
		case 0x54:
			PORTD = LETTER_T;
			break;
		case 0x11:
		case 0x58:
			PORTD = LETTER_X;
			break;
		case 0x12:
		case 0x59:
			PORTD = LETTER_Y;
			break;
		case 0x13:
		case 0x2D:
			PORTD = LETTER__;
			break;
			
		// For other values lets clear the segment.
		default:
			PORTD = 0x00;	
	}
	
	// Check to activate decimal indicator of the seven segment display.
	if(isDeimal)
	{
		PORTD |= DECIMAL_POINT;
	}
}

/*************************************************************************
 Set display of the complete seven segment display module. 

 valueSet: Instance of the display data structure.

 Return: None
*************************************************************************/
VOID setDisplayValueSet(PDISPLAY displayInfo)
{
	UCHAR segmentId = 0;
	
	for(segmentId = 0; segmentId < SSD_SIZE; segmentId++)
	{
		setDIsplaySegment(displayInfo->valueBuffer[segmentId], (displayInfo->decimalPoint == segmentId));
		if((_editSegment != segmentId) || (_editBlink == TRUE))
		{
			PORTC = (PORTC & 0xF0) | 1 << segmentId ;
		}
		
		_delay_us(400);
		PORTC = 0x00;
	}
}

/*************************************************************************
 Fill display buffer with specified character set.

 c1...c4: Character to fill the data buffer. Specify 0xFF to shutdown the 
		  appropriate segment of SSD module.
		  
 dataBuffer: Instance of the display data structure to fill with 
			 specified character set.
 
 Return: None
*************************************************************************/
VOID textToDisplay(UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4, PDISPLAY dataBuffer)
{
	dataBuffer->decimalPoint = 0xFF;
	
	dataBuffer->valueBuffer[0] = c1;
	dataBuffer->valueBuffer[1] = c2;
	dataBuffer->valueBuffer[2] = c3;
	dataBuffer->valueBuffer[3] = c4;
}

/*************************************************************************
 Clear value content of the specified display buffer. 

 valueSet: Instance of the display data structure.

 valueSize: Number of segments to clear.
 
 Return: None
*************************************************************************/
VOID clearDisplay(PUCHAR valueSet, UCHAR valueSize)
{
	UCHAR tempPos = 0;
	while(tempPos < valueSize)	
	{
		valueSet[tempPos] = 0xFF;
		tempPos++;
	}
}