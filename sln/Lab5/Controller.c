/*
 * Controller.c
 *
 * Created: 2018-02-21 13:58:06
 *  Author: hugwan-6, termar-5
 */ 

#include "Controller.h"
#include "LCD_handler.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define RED 0
#define GREEN 1

#define FOSC 8000000UL // Clock speed
#define BAUD 9600UL
#define MYUBRR ((FOSC/(16*BAUD)) - 1)	// Calculation of oscillator frequency

#define CARS_PASSED_COUNT_LIMIT 5

static unsigned char computeSignal(Controller *self);

void update(Controller * self, unsigned char input){
	unsigned char northArrival = input & 0x1;
	unsigned char southArrival = (input >> 2) & 0x1;
	unsigned char northEntry = (input >> 1) & 0x1;
	unsigned char southEntry = (input >> 3) & 0x1;
 	
	//Find out the number of cars in the queue
	self->carsWaitNorth += northArrival; //Northbound car arrival
	self->carsWaitSouth += southArrival; //Southbound car arrival
	self->carsWaitNorth -= northEntry; //Northbound bridge entry
	self->carsWaitSouth -= southEntry; //Southbound bridge entry
	
	//Count the cars on the bridge
	self->northCarsOnBridge += northEntry; //Northbound bridge entry
	self->southCarsOnBridge += southEntry; //Southbound bridge entry
	self->carsPassedSinceSwitch += northEntry + southEntry;
	
	printAt(self->carsWaitSouth, 0); //DEBUG
	printAt(self->carsWaitNorth, 2); //DEBUG
	printAt(self->northCarsOnBridge != 0 ? self->northCarsOnBridge : self->southCarsOnBridge, 4); //DEBUG
	
	if(northEntry){
		AFTER(SEC(5), self, carLeftBridgeNorth, 0);
	}
	else if(southEntry){
		//writeChar('2', 5); //DEBUG
		AFTER(SEC(5), self, carLeftBridgeSouth, 0);
	}
	
	ASYNC(self, decide, 0);
}

void carLeftBridgeSouth(Controller * self){
	//writeChar('7', 0); //DEBUG

	printAt(self->carsWaitSouth, 0); //DEBUG
	printAt(self->carsWaitNorth, 2); //DEBUG
	self->southCarsOnBridge--;
	printAt(self->northCarsOnBridge != 0 ? self->northCarsOnBridge : self->southCarsOnBridge, 4); //DEBUG
	
	if(self->southCarsOnBridge == 0){
		self->carsPassedSinceSwitch = 0;
	}
	
	ASYNC(self, decide, 0);
}

void carLeftBridgeNorth(Controller * self){
	//writeChar('9', 3); //DEBUG	
	self->northCarsOnBridge--;
	printAt(self->carsWaitSouth, 0); //DEBUG
	printAt(self->carsWaitNorth, 2); //DEBUG
	printAt(self->northCarsOnBridge != 0 ? self->northCarsOnBridge : self->southCarsOnBridge, 4); //DEBUG

	if(self->northCarsOnBridge == 0){
		self->carsPassedSinceSwitch = 0;
	}

	ASYNC(self, decide, 0);
}

void decide(Controller * self){
	//If the bridge is empty
	if(!self->southCarsOnBridge && !self->northCarsOnBridge){
		if(self->carsWaitNorth && !self->carsWaitSouth){//If there's cars waiting on the north side only
			self->northLight = GREEN;
			self->southLight = RED;
			//self->lastGreenLight = 1;
		}else if(self->carsWaitSouth && !self->carsWaitNorth){//If there's cars waiting on the south side only
			self->northLight = RED;
			self->southLight = GREEN;
			//self->lastGreenLight = 0;
		}else{ //If there's cars waiting on both sides
			if(self->southLight == RED && self->northLight == RED){	// If both lights are red
				if (self->lastGreenLight) {	// If the last cars came from north
					self->southLight = GREEN;
					self->northLight = RED;
					//self->lastGreenLight = 0;
				} else {	// If the last cars came from south
					self->southLight = RED;
					self->northLight = GREEN;
					//self->lastGreenLight = 1;
				}
			}/*else{
				//Flip the lights
				self->northLight ^= 1; 
				self->southLight ^= 1;
				self->lastGreenLight ^= 1;
			}*/
		}
	}else{//The bridge is not empty
		if(self->carsPassedSinceSwitch >= CARS_PASSED_COUNT_LIMIT){ // If more cars than allowed limit has crossed the bridge from one side
			if(self->northLight == GREEN && self->carsWaitSouth) {	// If cars waiting on south side, empty the bridge
				self->northLight = RED;
				self->lastGreenLight = 1;
			} else if(self->southLight == GREEN && self->carsWaitNorth) {	// If cars waiting on north side, empty the bridge
				self->southLight = RED;
				self->lastGreenLight = 0;
			}
		}
	}
	
	ASYNC(self, USARTTransmit, computeSignal(self));
}

static unsigned char computeSignal(Controller *self) {
	unsigned char northLight = (self->northLight & 0x1);
	unsigned char southLight = (self->southLight & 0x1);
	
	return (
	((southLight == RED		? 1 : 0) << 3)		|
	((southLight == GREEN	? 1 : 0) << 2)		|
	((northLight == RED		? 1 : 0) << 1)		|
	((northLight == GREEN	? 1 : 0) << 0));
	
}


// --------------- USART --------------
void USARTreceived(Controller *self) {	// Read the input from USART and call update in controller
	
	//////DEBUG
	/*static char test = '0';
	writeChar(test, 0);
	test++;
	if (test > '9') {
		test = '0';
	}*/
	///////////
	
	unsigned char input = UDR0 & 0xF;
	
	//printAt(input, 3); //DEBUG
	
	ASYNC(self, update, input);
}

void USARTTransmit(Controller *self, unsigned char signal) {
	while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
	//writeChar('2', 2); //DEBUG
	static unsigned char test = 0x4; //DEBUG
	UDR0 = (unsigned char)signal;	// Transmit
	test ^= 0x5; //DEBUG
}

void USARTInit(Controller *self){
	cli(); //Disable global interrupts
	
	// Set baud rate
	//unsigned int MYUBRR = 0x33;
	UBRR0H = (unsigned char)(MYUBRR >> 8); // 4 highest bits from ubrr and preserved bits will be 0
	UBRR0L = (unsigned char)MYUBRR;	// 8 lowest bits from ubrr
	
	// 5-bit data size with 1 stop bit, transmission and receiving enabled
	UCSR0C &= ~((1 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00));
	UCSR0B &= ~(1 << UCSZ02);
	UCSR0B |= ((1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0));
	
	//ASYNC(self, carLeftBridgeNorth, 0);
	
	sei(); //Reenable global interrupts
}