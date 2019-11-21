/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab
* File:              LCD.h
* Description:       This file holds the headers for the LCD.c file
*******************************************************/

void LCDpin_init();
void LCD_init();
void pushNibble(uint8_t nibble);
void pushByte(uint8_t byte);
void commandWrite(uint8_t command);
void dataWrite(uint8_t data);
void delay_ms(int value);
void pulseEnablePin();
void delay_micro(int value);
void writeString(char *string);
