/*
 * USARTHandler.h
 *
 * Created: 2018-02-21 14:38:25
 *  Author: hugwan-6, termar-5
 */ 

#include "TinyTimber.h"
#include "Controller.h"

#ifndef USARTHANDLER_H_
#define USARTHANDLER_H_

typedef struct{
	Object super;
	Controller *ctrl;
} USARTHandler;

void USARTInit();
void USARTreceived(USARTHandler *self);
void USARTTransmit(USARTHandler *self, unsigned char signal);

#define initUSARTHandler(cntrl) {initObject(), cntrl};

#endif /* USARTHANDLER_H_ */