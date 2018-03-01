/*
 * Controller.h
 *
 * Created: 2018-02-21 13:58:17
 *  Author: hugwan-6, termar-5
 */ 

#include "TinyTimber.h"

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

typedef struct {
	Object super;
	unsigned char northLight, southLight;
	unsigned char lastGreenLight;	// 1 =  north, 0 = south
	unsigned int carsWaitSouth, carsWaitNorth;
	unsigned int southCarsOnBridge, northCarsOnBridge;
	unsigned int carsPassedSinceSwitch;
} Controller;

void update(Controller * self, unsigned char input);
void carLeftBridgeSouth(Controller * self);
void carLeftBridgeNorth(Controller * self);
void decide(Controller * self);

void USARTInit();
void USARTreceived(Controller *self);
void USARTTransmit(Controller *self, unsigned char signal);

#define initController(USARTHand) {initObject(), 0, 1};

#endif /* CONTROLLER_H_ */