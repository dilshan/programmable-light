/*************************************************************************
* Title:	ATmega8 Firmware for programmable light controller.
* Author:	Dilshan R Jayakody <jayakody2000lk@gmail.com>
* Project:	Programmable LED controller.
* Homepage:	https://github.com/dilshan/programmable-light
* File:     main.c
* Info:		Main firmware module with applicaion entry point.
* Compiler: AVR GCC 5.4.0 (AVR 8-bit GNU Toolchain 3.6.1)
* Target:   ATmega8L / ATmega8A
**************************************************************************/

#include "sysbasedef.h"
#include "main.h"
#include "displaymodule.h"
#include "rtcmodule.h"
#include "memmodule.h"
#include "timemodule.h"

#include <avr/io.h>
#include <util/delay.h>	
#include <avr/interrupt.h>

INT main(VOID)
{
	UCHAR currentButtonState = 0x07;
	UCHAR lastButtonState = 0x07;
	UCHAR optionButtonCycles = 0;
	
	initSystem();
	
	// Load user defined start and end time from EEPROM.
	readTimeFromMemory(&_startTime, 0);
	readTimeFromMemory(&_endTime, 4);
	
	// Check for valid start and end time.
	IS_VALID_EEPROM_VALUE(_startTime.hours);
	IS_VALID_EEPROM_VALUE(_startTime.minutes);
	IS_VALID_EEPROM_VALUE(_startTime.seconds);
	
	IS_VALID_EEPROM_VALUE(_endTime.hours);
	IS_VALID_EEPROM_VALUE(_endTime.minutes);
	IS_VALID_EEPROM_VALUE(_endTime.seconds);
	
	while (1) 
    {
		currentButtonState = (PINB & 0x07);
		
		// Check for <option button> press event.
		if(IS_BUTTON_PRESSED(currentButtonState, 0x01, lastButtonState))
		{
			optionButtonCycles = 0;
			_ssdMode = SSD_DISPLAY_TIME;
			startSleepTimer();
		}
		
		// Check for <up button> press event.
		if(IS_BUTTON_PRESSED(currentButtonState, 0x02, lastButtonState))
		{
			optionButtonCycles = 0;
			_ssdMode = SSD_DISPLAY_START;
			startSleepTimer();
		}
		
		// Check for <down button> press event.
		if(IS_BUTTON_PRESSED(currentButtonState, 0x04, lastButtonState))
		{
			optionButtonCycles = 0;
			_ssdMode = SSD_DISPLAY_END;
			startSleepTimer();
		}
		
		// Clear timer if system is idle for long time.
		if(!_sleepTimer)
		{
			_ssdMode = SSD_DISPLAY_NONE;
		}
		
		// Count <option button> hold time.
		if((currentButtonState & 0x01) == 0x00)
		{
			optionButtonCycles++;
		}
		
		// Long press of <option button> should open settings menu.
		if(optionButtonCycles > LONG_PRESS_LIMIT)
		{
			optionButtonCycles = 0;

			updateSleepLED(FALSE);
			showConfigurationOption();
			
			// Update system time immediately. 
			getSystemTime(&_sysTime);
			
			// Restore blink status and related variables.
			setEditSegment(NO_EDIT_SEGMENT);
			setBlickState(TRUE);
			
			// Reset button status.
			currentButtonState = 0x07;
			lastButtonState = currentButtonState;
			continue;
		}
		
		// Update seven segment display based on current display mode.
		switch(_ssdMode)
		{
			case SSD_DISPLAY_TIME:
				sysTimeToDisplayBuffer(&_sysTime, &_displayBuffer);
				updateSleepLED(FALSE);
				break;
			case SSD_DISPLAY_NONE:
				// Clear entire display buffer and shutdown seven segment display.
				clearDisplay(_displayBuffer.valueBuffer, SSD_SIZE);
				updateSleepLED(TRUE);
				_displayBuffer.decimalPoint = 0xFF;
				break;
			case SSD_DISPLAY_START:
				sysTimeToDisplayBuffer(&_startTime, &_displayBuffer);
				updateSleepLED(FALSE);
				_displayBuffer.decimalPoint = 0x01;
				break;
			case SSD_DISPLAY_END:
				sysTimeToDisplayBuffer(&_endTime, &_displayBuffer);
				updateSleepLED(FALSE);
				_displayBuffer.decimalPoint = 0x01;
				break;
		}
		
		lastButtonState = currentButtonState;
		
		_delay_ms(60);
    }
}

/*************************************************************************
 Set state of the sleep LED.
 
 isActive: Set this parameter to TRUE to turn on the LED.
 
 Return: None
*************************************************************************/
VOID updateSleepLED(UCHAR isActive)
{
	if((isActive == TRUE) && (_isLightActive == FALSE))
	{
		PORTB |= (1 << PB4);
	}
	else
	{
		PORTB &= ~(1 << PB4);
	}
}

/*************************************************************************
 Function to edit specified time data structure.
 
 editBuffer: Instance of the time structure to modify.
 
 Return: None
*************************************************************************/
VOID editTimeValue(PTIME editBuffer)
{
	UCHAR editSegmentId = 0;
	UCHAR tempSeconds;
	UCHAR currentButtonState;
	UCHAR lastButtonState = 0x07;
	UCHAR maxNumber;
	
	// Activate edit mode in display buffer.
	setEditSegment(editSegmentId);
	_isBlink = TRUE;
	_blinkState = TRUE;
	
	// Set seconds to odd number to activate the decimal indicator.
	tempSeconds = editBuffer->seconds;
	editBuffer->seconds = 1;
	sysTimeToDisplayBuffer(editBuffer, &_displayBuffer);
	
	// Wait until user release the pushed button(s).
	waitForButtonRelease();
	startSleepTimer();
	
	while(1)
	{
		currentButtonState = (PINB & 0x07);
		
		// Clear timer if system is idle for long time.
		if(!_sleepTimer)
		{
			// Exit edit mode without saving the changes.
			setEditSegment(NO_EDIT_SEGMENT);
			setBlickState(FALSE);
			_blinkCounter = 0;
			_isBlink = FALSE;
			// Restore the values of time data structure.
			editBuffer->seconds = tempSeconds;
			return;
		}
		
		// Check for <option button> press event.
		if(IS_BUTTON_PRESSED(currentButtonState, 0x01, lastButtonState))
		{
			startSleepTimer();
			
			if(editSegmentId < 3)
			{
				// Move to next segment.
				setEditSegment(++editSegmentId);
				
				// If hour is beyond 20hr we need to correct 2nd digit of the existing hour.
				if((editSegmentId == 1) && (_displayBuffer.valueBuffer[0] == 2) && (_displayBuffer.valueBuffer[1] >= 4))
				{
					_displayBuffer.valueBuffer[1] = 3;
				}
			}
			else
			{
				// Exit edit mode.
				setEditSegment(NO_EDIT_SEGMENT);
				setBlickState(FALSE);
				_blinkCounter = 0;
				_isBlink = FALSE;
				// Update time structure with modified value.
				editBuffer->hours = _displayBuffer.valueBuffer[1] + (_displayBuffer.valueBuffer[0] * 10);
				editBuffer->minutes = _displayBuffer.valueBuffer[3] + (_displayBuffer.valueBuffer[2] * 10);
				editBuffer->seconds = tempSeconds;
				return;
			}
		}
		
		// Check for <up button> press event.
		if(IS_BUTTON_PRESSED(currentButtonState, 0x02, lastButtonState))
		{
			startSleepTimer();
			
			switch(editSegmentId)
			{
				case 0:
					// Modify first digit of the hour and it allows only 0, 1 and 2.
					_displayBuffer.valueBuffer[0] = (_displayBuffer.valueBuffer[0] < 2) ? (_displayBuffer.valueBuffer[0] + 1) : 0;
					break;
				case 1:
					// Modify second digit of the hour. If first digit is 2 then this allows only 0..3, otherwise this allows 0..9
					maxNumber = (_displayBuffer.valueBuffer[0] == 2) ? 3 : 9;
					_displayBuffer.valueBuffer[1] = (_displayBuffer.valueBuffer[1] < maxNumber) ? (_displayBuffer.valueBuffer[1] + 1) : 0;
					break;
				case 2:
					// Modify first digit of the minutes.
					_displayBuffer.valueBuffer[2] = (_displayBuffer.valueBuffer[2] < 5) ? (_displayBuffer.valueBuffer[2] + 1) : 0;
					break;
				case 3:
					// Modify second digit of the minutes.
					_displayBuffer.valueBuffer[3] = (_displayBuffer.valueBuffer[3] < 9) ? (_displayBuffer.valueBuffer[3] + 1) : 0;
					break;
			}
		}
		
		// Check for <down button> press event.
		if(IS_BUTTON_PRESSED(currentButtonState, 0x04, lastButtonState))
		{
			startSleepTimer();
			
			switch(editSegmentId)
			{
				case 0:
					// Modify first digit of the hour and it allows only 0, 1 and 2.
					_displayBuffer.valueBuffer[0] = (_displayBuffer.valueBuffer[0] != 0) ? (_displayBuffer.valueBuffer[0] - 1) : 2;
					break;
				case 1:
					// Modify second digit of the hour. If first digit is 2 then this allows only 0..3, otherwise this allows 0..9
					maxNumber = (_displayBuffer.valueBuffer[0] == 2) ? 3 : 9;
					_displayBuffer.valueBuffer[1] = (_displayBuffer.valueBuffer[1] != 0) ? (_displayBuffer.valueBuffer[1] - 1) : maxNumber;
					break;
				case 2:
					// Modify first digit of the minutes.
					_displayBuffer.valueBuffer[2] = (_displayBuffer.valueBuffer[2] != 0) ? (_displayBuffer.valueBuffer[2] - 1) : 5;
					break;
				case 3:
					// Modify second digit of the minutes.
					_displayBuffer.valueBuffer[3] = (_displayBuffer.valueBuffer[3] != 0) ? (_displayBuffer.valueBuffer[3] - 1) : 9;
					break;
			}
		}
		
		lastButtonState = currentButtonState;
		_delay_ms(50);
	}
}

/*************************************************************************
 Function to handle options menu and related user inputs.
 
 Return: None
*************************************************************************/
VOID showConfigurationOption()
{
	MENU_MODE currentMode = SSD_MENU_TIME;
	UCHAR currentButtonState;
	UCHAR lastButtonState = 0x07;

	// Update display buffer with current mode (SSD_MENU_TIME).
	textToDisplay('S','Y','S',' ', &_displayBuffer);
	
	// Wait until user release the pushed button(s).
	waitForButtonRelease();
	startSleepTimer();
	
	// Start main service loop to handle options related activities.
	while(1)
	{
		currentButtonState = (PINB & 0x07);
		
		// Check for menu timeouts to clear the menu.
		if(!_sleepTimer)
		{
			return;
		}
		
		// Check for <option button> press event.
		if(IS_BUTTON_PRESSED(currentButtonState, 0x01, lastButtonState))
		{
			startSleepTimer();
			
			switch(currentMode)
			{
				case SSD_MENU_TIME:
					// Configure RTC with modified value.
					editTimeValue(&_sysTime);
					setSystemTime(&_sysTime);
					break;
				case SSD_MENU_ON_TIME:
					editTimeValue(&_startTime);
					saveTimeToMemory(&_startTime, 0);
					break;
				case SSD_MENU_OFF_TIME:
					editTimeValue(&_endTime);
					saveTimeToMemory(&_endTime, 4);
					break;
				case SSD_MENU_EXIT:
					// Exit from options menu.
					return;
			}
			
			// Reset button status.
			currentButtonState = 0x07;
			lastButtonState = currentButtonState;
			continue;
		}
		
		// Check for <up button> press event.
		if(IS_BUTTON_PRESSED(currentButtonState, 0x02, lastButtonState))
		{
			startSleepTimer();
			
			switch(currentMode)
			{
				case SSD_MENU_TIME:
					currentMode = SSD_MENU_ON_TIME;
					break;
				case SSD_MENU_ON_TIME:
					currentMode = SSD_MENU_OFF_TIME;
					break;
				case SSD_MENU_OFF_TIME:
					currentMode = SSD_MENU_EXIT;
					break;
				case SSD_MENU_EXIT:
					currentMode = SSD_MENU_TIME;
					break;
			}
		}
		
		// Check for <down button> press event.
		if(IS_BUTTON_PRESSED(currentButtonState, 0x04, lastButtonState))
		{
			startSleepTimer();
			
			switch(currentMode)
			{
				case SSD_MENU_TIME:
					currentMode = SSD_MENU_EXIT;
					break;
				case SSD_MENU_ON_TIME:
					currentMode = SSD_MENU_TIME;
					break;
				case SSD_MENU_OFF_TIME:
					currentMode = SSD_MENU_ON_TIME;
					break;
				case SSD_MENU_EXIT:
					currentMode = SSD_MENU_OFF_TIME;
					break;
			}
		}
			
		// Update seven segment display based on current menu mode.
		switch(currentMode)
		{
			case SSD_MENU_TIME:
				textToDisplay('S','Y','S',' ', &_displayBuffer);
				break;
			case SSD_MENU_ON_TIME:
				textToDisplay('O','N',' ',' ', &_displayBuffer);
				break;
			case SSD_MENU_OFF_TIME:
				textToDisplay('O','F','F',' ', &_displayBuffer);
				break;
			case SSD_MENU_EXIT:
				textToDisplay(' ','-','-',' ', &_displayBuffer);
				break;
		}
		
		lastButtonState = currentButtonState;
		_delay_ms(50);
	}
}

/*************************************************************************
 Interrupt service routine for Timer2. This timer is mainly used to call 
 RTC with specific intervals to update the system time.
 
 Return: None
*************************************************************************/
ISR(TIMER2_OVF_vect)
{
	// Disable timer2 interrupt and start requesting data from RTC.
	TIMSK &= ~( 1 << TOIE2);
	
	if((++_rtcRefreshCounter) == 20)
	{
		getSystemTime(&_sysTime);
		
		// Check for light on condition.
		_isLightActive = isLightActive(&_sysTime, &_startTime, &_endTime);
		if(_isLightActive == TRUE)
		{
			// Turn on master light.
			PORTB |= (1 << PB3);
		}
		else 
		{
			// Turn off master light.
			PORTB &= ~(1 << PB3);
		}
		
		_rtcRefreshCounter = 0;
	}
	
	if(_isBlink == TRUE)
	{
		if((++_blinkCounter) == 10)
		{
			_blinkState = ~_blinkState;
			setBlickState(_blinkState);
			_blinkCounter = 0;
		}
	}
	
	// Reset timer2 counter and enable the interrupt.
	TIMSK |= (1 << TOIE2);
	TCNT2 = 0;
}

/*************************************************************************
 Interrupt service routine for Timer0. This timer is used to update 
 seven segment display with display buffer content.
 
 Return: None
*************************************************************************/
ISR(TIMER0_OVF_vect)
{
	// Disable timer0 interrupt and refresh seven segment display.
	TIMSK &= ~( 1 << TOIE0);
	
	// Update seven segment display based on current display buffer content.
	setDisplayValueSet(&_displayBuffer);
	
	// Reset timer0 counter and enable the interrupt.
	TIMSK |= (1 << TOIE0);
	TCNT0 = 232;
}

/*************************************************************************
 Interrupt service routine for Timer1. This timer is used to detect 
 system idle state.
 
 Return: None
*************************************************************************/
ISR(TIMER1_OVF_vect)
{
	// Disable timer1 interrupt and refresh seven segment display.
	TIMSK &= ~( 1 << TOIE1);
	
	// Update sleep counter for display / menu timeouts.
	if(_sleepTimer > 0)
	{
		_sleepTimer--;
		
		// Reset timer1 counter and enable the interrupt.
		TIMSK |= (1 << TOIE1);
		TCNT1 = 0;
	}
}

/*************************************************************************
 Enable sleep timer to determine system idle state.
 
 Return: None
*************************************************************************/
VOID startSleepTimer()
{
	_sleepTimer = SLEEP_TIMEOUT;
	TIMSK |= (1 << TOIE1);
	TCNT1 = 0;
}

/*************************************************************************
 Wait until user release the input button(s).
 
 Return: None
*************************************************************************/
VOID waitForButtonRelease()
{
	while(1)
	{
		if((PINB & 0x07) == 0x07)
		{
			_delay_ms(500);
			break;
		}
		
		_delay_ms(50);
	}
	
	_delay_ms(150);
}

/*************************************************************************
 Initialize MCU by setting up the default values for ports and
 system registers.  
 
 Return: None
*************************************************************************/
VOID initSystem()
{
	// Shutdown unused peripherals of this MCU.
	ADCSRA = 0x00;
	
	// Set PORTC and PORTD as an output ports (mainly used for seven segment display).
	DDRD = 0xFF;
	DDRC = 0xFF;
	DDRB = 0xF8;
	
	// Set default state of the ports.
	PORTD = 0x00;
	PORTC = 0x00;
	PORTB = 0x07;
	
	// Enable timer0 with to handle button inputs.
	TCNT0 = 232;
	TCCR0 |= (1 << CS02);
	
	// Enable timer2 to handle RTC scans.
	TCNT2 = 0;
	TCCR2 |= (1 << CS22) | (1 << CS21) | (1 << CS22);
	
	// Enable timer1 with long time durations.
	TCNT1 = 0;
	TCCR1B |= (1 << CS12);
	
	_delay_ms(10);
	
	_rtcRefreshCounter = 0;
	_ssdMode = SSD_DISPLAY_NONE;
	
	_blinkCounter = 0;
	_isBlink = FALSE;
	_sleepTimer = 0;
	_isLightActive = FALSE;
	
	// Clear seven segment related data structures.
	clearDisplay(_displayBuffer.valueBuffer, SSD_SIZE);
	_displayBuffer.decimalPoint = 0xFF;
	
	// Reset editor related parameters.
	setEditSegment(NO_EDIT_SEGMENT);
	setBlickState(TRUE);
	
	// Initialize I2C to communicate with DS1307 RTC.
	initRTCModule();
	_delay_ms(20);
	
	// Make sure that RTC runs correctly. If RTC battery backup fails it returns
	// garbage values, and this code block fix it by reset the RTC to 00:00:00.
	getSystemTime(&_sysTime);
	if((_sysTime.hours > 23) || (_sysTime.minutes > 59) || (_sysTime.seconds > 59))
	{
		// Ignore time setup on brownout resets. This check is required to work with some PSUs. 
		if((MCUCSR & 0x04) == 0x00)
		{
			// Reset RTC time to 00:00:00.
			_sysTime.hours = 0;
			_sysTime.minutes = 0;
			_sysTime.seconds = 0;
					
			setSystemTime(&_sysTime);
			_delay_ms(20);
		}
	}
	
	// Setup required interrupts.
	TIMSK |= (1 << TOIE0) | (1 << TOIE2) | (1 << TOIE1);
	sei();
}

