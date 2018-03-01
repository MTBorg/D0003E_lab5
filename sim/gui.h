#ifndef GUI_H
#define GUI_H

#define GREEN 1
#define RED 0

void clearTerminal();
void displayBridge(unsigned char southLight, unsigned char northLight,
  unsigned char southCars, unsigned char northCars, unsigned char bridge,
  unsigned char lastGreenLight);

#endif