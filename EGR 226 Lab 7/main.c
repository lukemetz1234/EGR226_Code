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
}

void pin_init() {
    P6->SEL0 &= ~BIT1;  //Set up pins P6.1 (E)
    P6->SEL1 &= ~BIT1;
    P6->DIR |= BIT1;   //Output

    P4->SEL0 &= ~(BIT0 | BIT1 | BIT2 | BIT3);  //Set up pins
    P4->SEL1 &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    P4->DIR |= (BIT0 | BIT1 | BIT2 | BIT3);   //Output

}

void LCD_init() {
    write_command(3);
     Systick_ms_delay(100);
     write_command(3);
     Systick_us_delay(200);
     write_command(3);
     Systick_ms_delay(100);
     write_command(2);
     Systick_us_delay(100);
     write_command(2);
     Systick_us_delay(100);
     write_command(8);
     Systick_us_delay(100);
     write_command(0x0C);
     Systick_us_delay(100);
     write_command(1);
     Systick_us_delay(100);
     write_command(6);
     Systick_ms_delay(10);

}


void pushNibble(uint8_t nibble) {
    P4->OUT &=~0x0F;
    P5->OUT |= (nibble &0x0F);
}


void pushByte(uint8_t byte) {
    pushNibble(byte >> 4);
    pulseEnablePin();
    pushNibble(byte & 0x0F);
    pulseEnablePin();
}


void commandWrite(uint8_t command) {
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
