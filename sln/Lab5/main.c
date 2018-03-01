/*
 * Lab5.c
 *
 * Created: 2018-02-21 13:44:20
 * Author : hugwan-6, termar-5
 */ 

#include <avr/io.h>
#include "TinyTimber.h"
#include "Controller.h"

#include "LCD_handler.h" // bug fix


int main(void) {
	LCD_init();

	Controller controller = initController();
	
	INSTALL(&controller, USARTreceived, IRQ_USART0_RX);
	//writeChar('3', 4);
	
	//AFTER(SEC(1), &controller, carLeftBridgeNorth, 0);
	
	return TINYTIMBER(&controller, USARTInit, 0);
}

