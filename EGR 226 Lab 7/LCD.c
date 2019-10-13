/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab
* File:              LCD.c
* Description:       This program initializes the proper pins for an LCD display and runs a sequence to set up the display.
*******************************************************/


#include "msp.h"
#include "LCD.h"


/***| LCDpin_init() |************************************//*
*Brief:    This function initializes all of the pins that the LCD display is connected to
*Params:
*            null
*Returns:
*            null
********************************************************/
void LCDpin_init() {

    P6->SEL0 &= ~BIT1;  //Set up pin P6.1 (E)
    P6->SEL1 &= ~BIT1;
    P6->DIR |= BIT1;   //Output

    P6->SEL0 &= ~BIT0;  //Set up RS pin
    P6->SEL1 &= ~BIT0;
    P6->DIR |= BIT0;   //Output

    P4->SEL0 &= ~(BIT0 | BIT1 | BIT2 | BIT3);   //Set up data pins
    P4->SEL1 &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    P4->DIR |= (BIT0 | BIT1 | BIT2 | BIT3);     //Output
    P4->OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);    //Initialize off
}


/***| LCD_init(void) |************************************//*
*Brief:    This function initializes the LCD display per the sequence described in the data sheet
*Params:
*            null
*Returns:
*            null
********************************************************/
void LCD_init(void)
{
  commandWrite(0x03);   //Reset 3 times
  delay_ms(10);
  commandWrite(0x03);
  delay_ms(10);
  commandWrite(0x03);
  delay_ms(10);

  commandWrite(0x02);   //4 bit mode
  delay_micro(100);
  commandWrite(0x02);
  commandWrite(0x06);   //2 line 5x7 format
  delay_micro(100);
  commandWrite(0x0F);   //Display on, cursor on, blinking
  delay_micro(100);
  commandWrite(0x01);   //Clears display
  delay_micro(100);
  commandWrite(0x06);   //Increment cursor mode
  delay_micro(100);
}


/***| pushNibble(uint8_t nibble) |************************************//*
*Brief:    This function pushes a nibble (4 bits) to the LCD display by sending the data over the lines and
*          pulsing the enable pin to store the data.
*Params:
*            nibble: 4 bits to be pushed
*Returns:
*            null
********************************************************/
void pushNibble(uint8_t nibble) {
    P4->OUT &=~0x0F;            //Clears data lines
    P4->OUT |= (nibble & 0x0F); //Pushes bottom 4 bits
    pulseEnablePin();           //Pulses the enable pin to lock in data
}


/***| pushByte(uint8_t byte) |************************************//*
*Brief:    This function pushes a byte (8 bits) to the LCD display by sending the first 4 bits, followed by the bottom 4.
*Params:
*            byte: 8 bits to be pushed
*Returns:
*            null
********************************************************/
void pushByte(uint8_t byte) {
    pushNibble(byte>>4);        //Top 4 bits
    pushNibble(byte & 0x0F);    //Bottom 4 bits
    delay_micro(100);
}


/***| commandWrite(uint8_t command) |************************************//*
*Brief:    This function pushes a command to the LCD display, the RS pin must be set to 0 for a command to be pushed
*Params:
*            command: 8 bit command to be pushed
*Returns:
*            null
********************************************************/
void commandWrite(uint8_t command) {
    P6->OUT &= ~BIT0;   //Set RS pin to 0
    pushByte(command);  //Push the command
}


/***| dataWrite(uint8_t data) |************************************//*
*Brief:    This function pushes data to the LCD display, the RS pin must be set to 1 for data to be pushed
*Params:
*            data: 8 bit data to be pushed
*Returns:
*            null
********************************************************/
void dataWrite(uint8_t data) {
    P6->OUT |= BIT0;    //Set RS pin to 1
    pushByte(data);     //Push data
}


/***| pulseEnablePin() |************************************//*
*Brief:    This function pulses the enable pin (P6.1) in order to lock the data sent to the display
*Params:
*            null
*Returns:
*            null
********************************************************/
void pulseEnablePin() {
    P6->OUT |= BIT1;    //Set P6.1 to 1
    delay_ms(10);       //Small delay
    P6->OUT &= ~BIT1;   //Set P6.1 to 0
}


/***| delay_ms() |************************************//*
*Brief:    Given a value representing number of ms, wait for that amount
*          of time using the SysTick timer
*Params:
*            (int) value:    ms to delay by
*Returns:
*            null
********************************************************/
void delay_ms(int value)
{
    SysTick->LOAD = value*3000;  //3000 cycles in a millisecond
    SysTick->VAL = 91719;  //Set Val to anything to clear //STCVR
    SysTick->CTRL |= BIT0;  //Enable   //Status and Control Register STCSR
    while((SysTick->CTRL&BIT(16))==0);  //Wait for SysTick to be complete
}


/***| delay_micro() |************************************//*
*Brief:    Given a value representing number of micro seconds, wait for that amount
*          of time using the SysTick timer
*Params:
*            (int) value:    micro seconds to delay by
*Returns:
*            null
********************************************************/
void delay_micro(int value) {
    SysTick->LOAD = value*3;  //3000 cycles in a millisecond
    SysTick->VAL = 91719;  //Set Val to anything to clear //STCVR
    SysTick->CTRL |= BIT0;  //Enable   //Status and Control Register STCSR
    while((SysTick->CTRL&BIT(16))==0);  //Wait for SysTick to be complete
}
