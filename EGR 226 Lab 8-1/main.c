#include "msp.h"

void pinSetup();
void delay_ms(int value);


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	int ONTIME = 1;
	int PERIOD = 1;
	pinSetup();


    P2->SEL0 |=  BIT6;  //giving timerA control of P6.7
    P2->SEL1 &= ~BIT6;  //Now control it through TimerA2.4
    P2->DIR  |=  BIT6;


	while(1) {
	    P2->OUT |= BIT7;
	    delay_ms(ONTIME);
	    P2->OUT &= ~BIT7;
	    delay_ms(PERIOD-ONTIME);
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
