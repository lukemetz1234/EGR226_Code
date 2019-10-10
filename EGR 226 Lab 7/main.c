#include "msp.h"

void pin_init();
void LCD_init();
void pushNibble(uint8_t nibble);
void pushByte(uint8_t byte);
void commandWrite(uint8_t command);
void delay_ms(int value);
void pulseEnablePin();
void delay_micro(int value);


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	pin_init();
	LCD_init();

	while(1) {
	    //commandWrite(0xF);
	}
}

void pin_init() {
    P6->SEL0 &= ~BIT1;  //Set up pins P6.1 (E)
    P6->SEL1 &= ~BIT1;
    P6->DIR |= BIT1;   //Output

    P6->SEL0 &= ~BIT0;  //Set up RS
    P6->SEL1 &= ~BIT0;
    P6->DIR |= BIT0;   //Output

    P4->SEL0 &= ~(BIT0 | BIT1 | BIT2 | BIT3);  //Set up data pins
    P4->SEL1 &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    P4->DIR |= (BIT0 | BIT1 | BIT2 | BIT3);   //Output
    P4->OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);

}

void LCD_init() {
     commandWrite(0x30);
     delay_ms(1000);
     commandWrite(0x30);
     delay_micro(2000);
     commandWrite(0x30);
     delay_ms(1000);

     commandWrite(2);
     delay_micro(1000);
     commandWrite(2);
     delay_micro(1000);

     commandWrite(8);
     delay_micro(1000);
     commandWrite(0x0C);
     delay_micro(1000);
     commandWrite(1);
     delay_micro(1000);
     commandWrite(6);
     delay_ms(1000);

     commandWrite(0xF);
     delay_ms(100);
     commandWrite(0xF);
     delay_ms(100);
     //commandWrite(0xF);
     //delay_ms(100);
}


void pushNibble(uint8_t nibble) {
    P4->OUT &=~0x0F;
    P4->OUT |= (nibble & 0x0F);
}


void pushByte(uint8_t byte) {
    pushNibble(byte >> 4);
    pulseEnablePin();
    pushNibble(byte & 0x0F);
    pulseEnablePin();
}


void commandWrite(uint8_t command) {
    P6->OUT &= ~BIT0;
    pushByte(command);
    pulseEnablePin();
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

void delay_micro(int value) {
    SysTick->LOAD = value*3;  //3000 cycles in a millisecond
    SysTick->VAL = 91719;  //Set Val to anything to clear //STCVR
    SysTick->CTRL |= BIT0;  //Enable   //Status and Control Register STCSR
    while((SysTick->CTRL&BIT(16))==0);  //Wait for SysTick to be complete
}
