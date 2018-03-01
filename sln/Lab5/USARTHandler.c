/*
 * USARTHandler.c
 *
 * Created: 2018-02-21 14:43:00
 *  Author: hugwan-6, termar-5
 */ 

#include "USARTHandler.h"
#include "LCD_handler.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC 8000000 // Clock speed??
#define BAUD 9600
#define MYUBRR (FOSC/(16*BAUD) - 1)

void USARTInit(){
	cli(); //Disable global interrupts
	
	// Set baud rate
	UBRR0H = (unsigned char)(MYUBRR >> 8); // 4 highest bits from ubrr and preserved bits will be 0
	UBRR0L = (unsigned char)MYUBRR;	// 8 lowest bits from ubrr
	
	// 5-bit data size with 1 stop bit, transmission and receiving enabled
	UCSR0C &= ~((1 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00));	
	UCSR0B &= ~(1 << UCSZ02);
	UCSR0B |= ((1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0));
	
	sei(); //Reenable global interrupts
}

void USARTreceived(USARTHandler *self) {	// Read the input from USART and call update in controller
	static char test = '0';
	writeChar(test, 0);
	test++;

	if (test > '9') {
		test = '0';
	}
	
	unsigned char dummy = UDR0;
}

void USARTTransmit(USARTHandler *self, unsigned char signal) {
	while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
	
	UDR0 = signal;	// Transmit
}