/*
 * LCD_handler.h
 *
 * Created: 2018-02-08 14:45:20
 *  Author: hugwan-6, termar-5
 */ 


#ifndef LCD_HANDLER_H_
#define LCD_HANDLER_H_

void printAt(long num, int pos);
void LCD_init(void);
void writeChar(char ch, int pos);

#endif /* LCD_HANDLER_H_ */