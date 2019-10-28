/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 08-1 Creating a Pulse Width Modulated (PWM) Pseudo Analog Voltage Using
*                    Timer Peripherals of a Microcontroller
* File:              main.c
* Description:       This program controls the speed of a motor using the SysTick timer as a PWM signal.
*******************************************************/

#include "msp.h"

void pinSetup();
void delay_ms(int value);


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	int ONTIME = 1;
	int PERIOD = 1;
	pinSetup();


    P2->SEL0 |=  BIT6;
    P2->SEL1 &= ~BIT6;
    P2->DIR  |=  BIT6;  //Output


	while(1) {
	    P2->OUT |= BIT7;            //Turn on
	    delay_ms(ONTIME);           //Delay for ontime
	    P2->OUT &= ~BIT7;           //Turn off
	    delay_ms(PERIOD-ONTIME);    //Delay off time
	}

}

void pinSetup() {
    P2->SEL0 &= ~BIT7;  //Set up pin P6.1 (E)
    P2->SEL1 &= ~BIT7;
    P2->DIR |= BIT7;   //Output
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
