#include "msp.h"

void pin_init();
void LCD_init();
void pushNibble(uint8_t nibble);
void pushByte(uint8_t byte);
void commandWrite(uint8_t command);
void delay_ms(int value);
void pulseEnablePin();


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//test test
}

void pin_init() {
    P6->SEL0 &= ~BIT1;  //Set up pins P6.1 (E)
    P6->SEL1 &= ~BIT1;
    P6->DIR |= BIT1;   //Output

}

void LCD_init() {


}


void pushNibble(uint8_t nibble) {



}


void pushByte(uint8_t byte) {

}


void commandWrite(uint8_t command) {


}

void pulseEnablePin() {
    P6->OUT |= BIT1;
    delay_ms(10);
    P6->OUT &= ~BIT1;
}

void delay_ms(int value)
{
    SysTick->LOAD = value*3000;  //3000 cycles in a millisecond
    SysTick->VAL = 91719;  //Set Val to anything to clear //STCVR
    SysTick->CTRL |= BIT0;  //Enable   //Status and Control Register STCSR
    while((SysTick->CTRL&BIT(16))==0);  //Wait for SysTick to be complete
}
