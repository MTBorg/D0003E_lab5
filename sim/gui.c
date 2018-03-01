#include "gui.h"

#include <stdio.h>

#define ESCAPE(str) "\e["#str
#define ESCAPE_COLOR(color) ESCAPE(color)
#define TEST(str) "HEllo"#str

#define COLOR_RED 31
#define COLOR_GREEN 32
#define COLOR_WHITE 37
#define COLOR_YELLOW 33

#define SYMBOL_LIGHT 79 // O
#define SYMBOL_BRIDGE_BEGIN 124 // |
#define SYMBOL_BRIDGE_FLOOR 95 // _

#define SYMBOL_CAR_NORTH 60 // <
#define SYMBOL_CAR_SOUTH 62 // >

#define COLOR_CAR COLOR_YELLOW
#define COLOR_BRIDGE COLOR_WHITE

#define NORTH 0
#define SOUTH 1

#define BRIDGE_POSITION 20

void ClearTerminal();
void SetColor();

void displayBridge(unsigned char southLight, unsigned char northLight,
 unsigned char southCars, unsigned char northCars, unsigned char bridge,
 unsigned char lastGreenLight){

	printf(ESCAPE(35C));
	
	for(unsigned char i = 0; i < southCars + 1; i++){
		printf(ESCAPE(1D));
	}
 
    //Paint the cars on the south side
    SetColor(COLOR_CAR);
    for(unsigned char i = 0; i < southCars; i++){
        printf("%c", SYMBOL_CAR_SOUTH);
    }

    //Paint the south light
    SetColor(southLight == GREEN ? COLOR_GREEN : COLOR_RED);
    printf("%c", SYMBOL_LIGHT);

    //Paint the beginning of the bridge
    SetColor(COLOR_BRIDGE);
    printf("%c", SYMBOL_BRIDGE_BEGIN);

    //Paint the cars on the bridge (and the bridge)
    for(unsigned char i = 0; i < 5; i++){
        if((bridge >> (4- i)) & 0x1){ //If there's a car in this position
			SetColor(COLOR_CAR);
			if(lastGreenLight == NORTH){
				printf("%c", SYMBOL_CAR_NORTH);
			}else{
				printf("%c", SYMBOL_CAR_SOUTH);
			}
		}else{
			SetColor(COLOR_BRIDGE);
			printf("%c", SYMBOL_BRIDGE_FLOOR);
		}
    }

    //Paint the beginning of the bridge (from the north side)
	SetColor(COLOR_WHITE);
    printf("%c", SYMBOL_BRIDGE_BEGIN);

    //Paint the north light
    SetColor(northLight == GREEN ? COLOR_GREEN : COLOR_RED);
    printf("%c", SYMBOL_LIGHT);

    //Paint the cars on the north side
    SetColor(COLOR_CAR);
    for(unsigned char i = 0; i < northCars; i++){
        printf("%c", SYMBOL_CAR_NORTH);
    }

	SetColor(COLOR_WHITE);
	
    printf("\n");
}

void clearTerminal(){
    printf(ESCAPE(2J));
    printf(ESCAPE(r));
}

void SetColor(unsigned int color){
    switch(color){
        case COLOR_RED:
            printf(ESCAPE(31m)); break;
        case COLOR_GREEN:
            printf(ESCAPE(32m)); break;
        case COLOR_WHITE:
            printf(ESCAPE(37m)); break;
		case COLOR_YELLOW:
			printf(ESCAPE(33m)); break;
    }
}