#include "msp.h"

void pin_init();
void LCD_init();
void pushNibble(uint8_t nibble);
void pushByte(uint8_t byte);
void commandWrite(uint8_t command);
void dataWrite(uint8_t command);
void delay_ms(int value);
void pulseEnablePin();
void delay_micro(int value);


/******************************
Added in Lecture (Not finished)
char display[65];           //what to put on the display
char currentDisplay[65];    //Currently on it
******************************/



void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	pin_init();
	LCD_init();

	char name1[4] = {'L', 'U', 'K', 'E'};        //Put whatever you want to print in a character array with the correct length
	char name2[5] = {'B', 'R', 'I', 'A', 'N'};
	char class[3] = {'E', 'G', 'R'};
	char classNum[3] = {'2', '2', '6'};
	int j = 1;
	int k = 0;
	while(1) {
	    if (j == 1) {                           //This is so that it dose not repeat forever
	        commandWrite(0x86);                 //address  06
	        for(k = 0; k < 4; k++) {
	            dataWrite(name1[k]);             //Write each letter
	            delay_ms(10);
	        }
	        commandWrite(0xC5);
	        for(k = 0; k < 5; k++) {
	            dataWrite(name2[k]);             //Write each letter
	            delay_ms(10);
	        }
	        commandWrite(0x96);
            for(k = 0; k < 3; k++) {
                dataWrite(class[k]);             //Write each letter
                delay_ms(10);
            }
            commandWrite(0xD6);
            for(k = 0; k < 3; k++) {
                dataWrite(classNum[k]);             //Write each letter
                delay_ms(10);
            }
	    }
	    j++;
	}
}

void pin_init() {

    /*************************
     Added in lecture
     Not finished

    for(int i = 0; i < 65; i++) {
        currentDisplay[i] = " ";
        display[i] = " ";
    }

    ************************/

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


void LCD_init(void)
{
  commandWrite(0x03);//REST 3 times
  delay_ms(10);
  commandWrite(0x03);
  delay_ms(10);
  commandWrite(0x03);
  delay_ms(10);

  commandWrite(0x02);//4bit mode
  delay_micro(100);
  commandWrite(0x02);
  commandWrite(0x06);//2 lin 5x7 format
  delay_micro(100);
  commandWrite(0x0F);//display on cursor blinking
  delay_micro(100);
  commandWrite(0x01);//clears display
  delay_micro(100);
  commandWrite(0x06);//incremnt cursor
  delay_micro(100);
}


void pushNibble(uint8_t nibble) {
    P4->OUT &=~0x0F;
    P4->OUT |= (nibble & 0x0F);
    pulseEnablePin();
}


void pushByte(uint8_t byte) {
    pushNibble(byte>>4);
    pushNibble(byte & 0x0F);
    delay_micro(100);
}


void commandWrite(uint8_t command) {
    P6->OUT &= ~BIT0;
    pushByte(command);
}

void dataWrite(uint8_t command) {
    P6->OUT |= BIT0;
    pushByte(command);
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
