/*
 * LCD_handler.c
 *
 * Created: 2018-02-08 14:44:55
 *  Author: hugwan-6, termar-5
 */

#include "LCD_handler.h"
#include <avr/io.h>

#define SCC_0 0x1551
#define SCC_1 0x0110
#define SCC_2 0x1E11
#define SCC_3 0x1B11
#define SCC_4 0x0B50
#define SCC_5 0x1B41
#define SCC_6 0x1F41
#define SCC_7 0x0111
#define SCC_8 0x1F51
#define SCC_9 0x0B51

void printAt(long num, int pos) {
	int pp = pos;
	writeChar( (num % 100) / 10 + '0', pp);
	pp++;
	writeChar( num % 10 + '0', pp);
}

void LCD_init(void) {
	LCDCCR = ((0 << LCDCC2)| 0xF); // Set drive time to 300mcrs and ccv to 3,35V
	
	LCDCRB = (1 << LCDCS) // Use external asynchronous clocksource
	| (0 << LCD2B) // 1/3 bias
	| (1 << LCDMUX1) | (1 << LCDMUX0) // 1/4 duty cycle
	| (1 << LCDPM2) | (1 << LCDPM1) | (1 << LCDPM0); // 25 segments
	
	LCDFRR = 0;	// prescaler setting N = 16
	LCDFRR = 0x7; // clock divider D = 8
	
	LCDCRA = (1 << LCDEN) // LCD enabled
	| (1 << LCDAB) // Low power waveform
	| (0 << LCDIE) // No frame interrupt
	| (0 << LCDBL); // No blanking
}

void writeChar(char ch, int pos) {
	
	volatile uint8_t *regNibble0 = 0, *regNibble1 = 0, *regNibble2 = 0, *regNibble3 = 0;


	switch (pos)
	{
		case 0:
		case 1:
		regNibble0 = &LCDDR0;
		regNibble1 = &LCDDR5;
		regNibble2 = &LCDDR10;
		regNibble3 = &LCDDR15;
		break;
		case 2:
		case 3:
		regNibble0 = &LCDDR1;
		regNibble1 = &LCDDR6;
		regNibble2 = &LCDDR11;
		regNibble3 = &LCDDR16;
		break;
		case 4:
		case 5:
		regNibble0 = &LCDDR2;
		regNibble1 = &LCDDR7;
		regNibble2 = &LCDDR12;
		regNibble3 = &LCDDR17;
		break;
		default:
		return;
		break;
	}

	uint32_t SCCvalue;
	
	switch (ch)
	{
		case '0':
		SCCvalue = SCC_0;
		break;
		case '1':
		SCCvalue = SCC_1;
		break;
		case '2':
		SCCvalue = SCC_2;
		break;
		case '3':
		SCCvalue = SCC_3;
		break;
		case '4':
		SCCvalue = SCC_4;
		break;
		case '5':
		SCCvalue = SCC_5;
		break;
		case '6':
		SCCvalue = SCC_6;
		break;
		case '7':
		SCCvalue = SCC_7;
		break;
		case '8':
		SCCvalue = SCC_8;
		break;
		case '9':
		SCCvalue = SCC_9;
		break;
		default:
		SCCvalue = 0x8020;
		break;
	}

	if (pos % 2 == 1) { // If pos is odd we need to write to the high nibble
		// Clear the high nibbles
		if (pos == 3) {
			*regNibble0 &= 0x0F;
		} else {
			*regNibble0 &= 0x2F;	// Dont clear symbol bits
		}
		
		*regNibble1 &= 0x0F;
		*regNibble2 &= 0x0F;
		*regNibble3 &= 0x0F;
					
		// Assign nibble values at the high nibbles
		*regNibble0 |= ((SCCvalue & 0x000F) << 4);
		*regNibble1 |= (SCCvalue & 0x00F0);
		*regNibble2 |= ((SCCvalue & 0x0F00) >> 4);
		*regNibble3 |= ((SCCvalue & 0xF000) >> 8);

	} else { // If the pos is even we need to write to the lower nibble
		// Clear the lower nibbles
		if (pos == 2) {
			*regNibble0 &= 0xF0;
		} else {
			*regNibble0 &= 0xF2;	// Dont clear symbol bits
		}
		
		*regNibble1 &= 0xF0;
		*regNibble2 &= 0xF0;
		*regNibble3 &= 0xF0;
		
		// Assign nibble values at the low nibbles
		*regNibble0 |= (SCCvalue & 0x000F);
		*regNibble1 |= ((SCCvalue & 0x00F0) >> 4);
		*regNibble2 |= ((SCCvalue & 0x0F00) >> 8);
		*regNibble3 |= ((SCCvalue & 0xF000) >> 12);
	}
}