/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab
* File:              LCD.h
* Description:       This file holds the headers for the LCD.c file
*******************************************************/

extern char nextDisplay[65];

void LCD_init();
void pulseEnable();
void pushByte(uint8_t data);
void pushNibble(uint8_t nibble);
void commandWrite(uint8_t data);
void initializeDisplay();
void delay_ms(int ms);
void delay_us(int us);
void setupLCD();
void setupTimerA3();
void writeString(char *string);
void dataWrite(uint8_t data);
