#include "msp.h"

void LCD_init();
void pushNibble(uint8_t nibble);
void pushByte(uint8_t byte);
void commandWrite(uint8_t command);
void delay_ms(int value);


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//test test
}

void delay_ms(int value)
{
    SysTick->LOAD = value*3000;  //3000 cycles in a millisecond
    SysTick->VAL = 91719;  //Set Val to anything to clear //STCVR
    SysTick->CTRL |= BIT0;  //Enable   //Status and Control Register STCSR
    while((SysTick->CTRL&BIT(16))==0);  //Wait for SysTick to be complete
}
