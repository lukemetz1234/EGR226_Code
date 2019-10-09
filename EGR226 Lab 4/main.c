#include "msp.h"


void delay_ms(int del);
int checkButtonPress();


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer


	P1->SEL0 &= ~BIT1;  //Set up button in P1.1
	P1->SEL1 &= ~BIT1;
	P1->DIR &= ~BIT1;   //Input
	P1->REN |= BIT1;    //Enables internal resistor
	P1->OUT |= BIT1;    //Pull up resistor

	P2->SEL0 &= ~BIT0;  //Set up Red LED
	P2->SEL1 &= ~BIT0;
	P2->DIR |= BIT0;    //Output
	P2->OUT &= ~BIT0;   //Initiate off

	P2->SEL0 &= ~BIT1;  //Set up Green LED
	P2->SEL1 &= ~BIT1;
	P2->DIR |= BIT1;    //Output
	P2->OUT &= ~BIT1;   //Initiate off

	P2->SEL0 &= ~BIT2;  //Set up Blue LED
	P2->SEL1 &= ~BIT2;
	P2->DIR |= BIT2;    //Output
	P2->OUT &= ~BIT2;   //Off

	int counter = 1;    //Used to switch cases

	while(1) {
	    switch(counter)
	    {
	    case 1:
	        if (checkButtonPress() == 1) {      //Check for the button press
	            while(checkButtonPress() == 1); //Wait until the button is released
	            counter = 2;    //Enter case 2
	        }
	        break;
	    case 2:
	        P2->OUT |= BIT0;    //Red LED on
	        P2->OUT &= ~BIT1;   //Green off
	        P2->OUT &= ~BIT2;   //Blue off
	        if (checkButtonPress() == 1) {      //Check for button press
	            while(checkButtonPress() == 1); //Wait until the button is released
	            counter = 3;    //Enter case 3
	        }
	        break;
	    case 3:
	        P2->OUT &= ~BIT0;   //Red off
	        P2->OUT |= BIT1;    //Green on
	        P2->OUT &= ~BIT2;   //Blue off
	        if (checkButtonPress() == 1) {      //Check for button press
	            while(checkButtonPress() == 1); //Wait until the button is released
	            counter = 4;    //Enter case 4
	        }
	        break;
	    case 4:
            P2->OUT &= ~BIT0;   //Red off
            P2->OUT &= ~BIT1;   //Green off
            P2->OUT |= BIT2;    //Blue on
            if (checkButtonPress() == 1) {      //Check for button press
                while(checkButtonPress() == 1); //Wait until the button is released
                counter = 2;    //Loop back to case 2
            }
            break;

	    }

	}

}

int checkButtonPress() {
    if((P1->IN & BIT1) == 0) {      //Check for the button being pressed
        delay_ms(5);                //Delay 5 ms for debounce
        if ((P1->IN & BIT1) == 0) { //Check agian for the button still being pressed
                return 1;           //If its still pressed, return a 1
        }
        else {
                return 0;           //Otherwise return a 0
        }
    }
}

void delay_ms(int value)
{
    SysTick->LOAD = value*3000;  //3000 cycles in a millisecond
    SysTick->VAL = 91719;  //Set Val to anything to clear //STCVR
    SysTick->CTRL |= BIT0;  //Enable   //Status and Control Register STCSR
    while((SysTick->CTRL&BIT(16))==0);  //Wait for SysTick to be complete
}
