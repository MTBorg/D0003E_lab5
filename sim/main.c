/*
	D0003E
	Lab5 simulation
	
	lab5_sim.c
	hugwan-6, termar-5
*/

//TODO
// - The avr crashes when the keyboard is spammed (FIXED)
// - When the lights are switched and all cars on the bridge have left, two cars are painted on the bridge (FIXED)
// - Sending cars to the side that's red for the first time after starting the program does not make that side green

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#include "gui.h"

#define BAUDRATE B9600
#define COM1FILE "/dev/ttyS0"

#define NORTH 0
#define SOUTH 1

void *keyboard();
void *bridgeController();
unsigned char USARTWrite(unsigned char writeBits);
void *USARTRead();
void USARTInit();
void *bridgeController();

int COM1;
unsigned char northLight, southLight;
unsigned char northCars, southCars;
unsigned char bridge;
unsigned char lastGreenLight;

int main() {
	pthread_t keyboardThread;
	pthread_t USARTReadThread;
	pthread_t bridgeControllerThread;
	
	USARTInit();
	
	// Create keyboard thread
	if(pthread_create(&keyboardThread, NULL, &keyboard, NULL) != 0) {
		printf("create write thread error. \n");
	}
	
	// Create USART read thread
	if(pthread_create(&USARTReadThread, NULL, &USARTRead, NULL) != 0) {
		printf("create read thread error. \n");
	}
	
	//Create a bridge controller thread
	if(pthread_create(&bridgeControllerThread, NULL, &bridgeController, NULL) != 0){
		printf("ERROR: Failed to create bridge controller thread");
	}
	
	
	// Wait
	pthread_join(keyboardThread, NULL);
	pthread_join(USARTReadThread, NULL);
}

void USARTInit() {
	struct termios t;
	COM1 = open(COM1FILE, O_RDWR); // Open com port
	
	tcgetattr(COM1, &t);
	cfsetispeed(&t, B9600);
	cfsetospeed(&t, B9600);
	t.c_cflag &= ~CSIZE; 	// Clear char size
	t.c_cflag |= CS5;	// set 5 bit char size
	t.c_cflag &= ~CSTOPB; // 1 stop bit
	t.c_cflag &= ~PARENB; // no parity
	t.c_cflag |= CREAD;	// Enable receive
	t.c_cc[VMIN] =  1; // Read at least 1 char
	//t.c_cc[VTIME] = 10;	// ???
	
	if (tcsetattr(COM1, TCSANOW, &t) != 0) {
       printf("Error from tcsetattr.");
    }
}

void *USARTRead() {
	unsigned char input;
	fd_set rfds;
	FD_ZERO(&rfds); // emtpy set
	FD_SET(COM1, &rfds); // include com1:

	while(1){
		tcflush(COM1, TCIFLUSH);
		select(4, &rfds, NULL, NULL, NULL);
		if(FD_ISSET(COM1, &rfds)) {
				
			clearTerminal();
			
			if(read(COM1, &input, sizeof(char)) == -1) {
				printf("Error on read");
			}
			
			//printf("Input from USART: %d \n", input); //DEBUG
			
			if(input & 0x1) {	// northbound green light status
				//printf("Received north green light\n"); //DEBUG
				
				//If the lights are switched then the bridge is empty and the bridge bits have to be cleared
				if(lastGreenLight != NORTH){
					bridge = 0;
				}
				
				northLight = GREEN;
				lastGreenLight = NORTH;
			} else {
				//printf("Received north red light\n"); //DEBUG
				northLight = RED;
			}
			
			if(input & 0x4) {	// southbound green light status
				//printf("Received south green light\n"); //DEBUG
				
				//If the lights are switched then the bridge is empty and the bridge bits have to be cleared
				if(lastGreenLight != SOUTH){
					bridge = 0;
				}
				southLight = GREEN;
				lastGreenLight = SOUTH;
			} else {
				//printf("Received south red light\n"); //DEBUG
				southLight = RED;
			}

			FD_ZERO(&rfds);
			FD_SET(COM1, &rfds);
			
			displayBridge(southLight, northLight, southCars, northCars, bridge, lastGreenLight);
		}
	}
}

void *keyboard() {
	while(1) {
		unsigned char keyboardInput = getchar();
		
		while(getchar() != '\n'); //Make sure to only read one character
		
		if(keyboardInput == '\n') {
			continue;
		}
		
		if(keyboardInput == 'a') {	// Spawn car arriving on the south side of the bridge
				int bytes = USARTWrite(0b0100);
				//printf("Wrote %d bytes to port %s \n", bytes, COM1FILE); //DEBUG
				southCars++;
				
		} else if(keyboardInput == 'd') { 	// Spawn car arriving on the north side of the bridge
				int bytes = USARTWrite(0b001);
				//printf("Wrote %d bytes to port %s \n", bytes, COM1FILE); //DEBUG
				northCars++;
		}
	}
}

unsigned char USARTWrite(unsigned char writeBits) {
	return write(COM1, &writeBits, sizeof(unsigned char));
}

void *bridgeController() {	// Handles the light signals and lets cars cross the bridge
	while(1){
		//printf("Bridge updating\n"); //DEBUG
		
		if(lastGreenLight == SOUTH){
			bridge = bridge >> (unsigned char)1; //Rotate the car symbols one step to the right (north)
			bridge &= 0x1f; //Make sure only the first five bits remain valid
			if((southCars > 0) && (southLight == GREEN)){
				bridge |= 1 << 4;
				southCars--;
				USARTWrite(0b1000); //Signal to the avr that a southbound car has entered the bridge
			}
		}else if(lastGreenLight == NORTH){
			bridge = bridge << (unsigned char)1; //Rotate the car symbols one step to the left (south)
			bridge &= 0x1f; //Make sure only the first five bits remain valid
			if((northCars > 0) && (northLight == GREEN)){
				bridge |= 1;
				northCars--;
				USARTWrite(0b0010); //Signal to the avr that a northbound car has entered the bridge
			}
		}
		
		//printf("Cars on south side %d", southCars); //DEBUG
		clearTerminal();
		displayBridge(southLight, northLight, southCars, northCars, bridge, lastGreenLight);
		
		
		clock_t startTime = clock();
		while(clock() - startTime <= CLOCKS_PER_SEC); //Wait for one second before checking again
	}
}