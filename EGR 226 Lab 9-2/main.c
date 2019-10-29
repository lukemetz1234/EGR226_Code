/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 09-2 Using GPIO interrupts to control PWM and 7-Segment LED
* File:              main.c
* Description:       This program connects a 7 segment display to the board and counts up from 0-9.
*                    The systick timer is set up with an interrupt enabled to happen every second. This increments
*                    the current number on the display by one once a second.
*******************************************************/

#include "msp.h"

void pinSetup();
void writeSeg(int seg);
void SysTick_IRQ_Init();
void SysTick_Handler();

int i = 0;  //Current number being displayed

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	pinSetup();             //Set up pins
	SysTick_IRQ_Init();     //Initialize systick timer
	__enable_interrupts();  //Enable interrupts

	while(1) {
	}
}

/***| pinSetup() |************************************//*
*Brief:    This function initializes all of the pins that the 7 segment display are connected to
*Params:
*            null
*Returns:
*            null
********************************************************/
void pinSetup() {
    P4->SEL0 &= ~(BIT3|BIT4|BIT5|BIT6|BIT7);
    P4->SEL1 &= ~(BIT3|BIT4|BIT5|BIT6|BIT7);
    P4->DIR |= (BIT3|BIT4|BIT5|BIT6|BIT7);
    P4->OUT &= ~(BIT3|BIT4|BIT5|BIT6|BIT7);

    P1->SEL0 &= ~BIT5;
    P1->SEL1 &= ~BIT5;
    P1->DIR |= BIT5;
    P1->OUT &= ~BIT5;

    P5->SEL0 &= ~(BIT4|BIT5);
    P5->SEL1 &= ~(BIT4|BIT5);
    P5->DIR |= (BIT4|BIT5);
    P5->OUT &= ~(BIT4|BIT5);
}

/***| writeSeg(int seg) |************************************//*
*Brief:    This function alters the proper outputs to the 7 segment display in order to display the correct number
*Params:
*            seg: Number to be displayed
*Returns:
*            null
********************************************************/
void writeSeg(int seg) {
    switch (seg) {
    case 0:                             //Displays a zero
        pinSetup();                     //Resets all pins to output a zero
        P4->OUT |= (BIT4|BIT5|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= (BIT4|BIT5);
        break;
    case 1:                             //Displays a one
        pinSetup();
        P4->OUT |= (BIT7|BIT5);
        break;

    case 2:                             //Displays a two
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6);
        P5->OUT |= (BIT4|BIT5);
        break;
    case 3:                             //Displays a three
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6|BIT7);
        P5->OUT |= BIT4;
        break;
    case 4:                             //Displays a four
        pinSetup();
        P4->OUT |= (BIT5|BIT6|BIT7);
        P1->OUT |= BIT5;
        break;
    case 5:                             //Displays a five
        pinSetup();
        P4->OUT |= (BIT4|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= BIT4;
        break;
    case 6:                             //Displays a six
        pinSetup();
        P4->OUT |= (BIT4|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= (BIT4|BIT5);
        break;
    case 7:                             //Displays a seven
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT7);
        break;
    case 8:                             //Displays a eight
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= (BIT4|BIT5);
        break;
    case 9:                             //Displays a nine
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= BIT4;
        break;
    }
}

/***| SysTick_IRQ_Init() |************************************//*
*Brief:    This function runs as an initialization for the systick timer
*Params:
*            null
*Returns:
*            null
********************************************************/
void SysTick_IRQ_Init()
{
    SysTick->CTRL |= (BIT1|BIT0);   //Enabled as interrupt
    SysTick->VAL = 123;             //clears
    SysTick->LOAD = 3000000;        //One second
    NVIC_EnableIRQ(SysTick_IRQn);   //Enable interrupt
}


/***| SysTick_Handler() |************************************//*
*Brief:    This function runs every time an interrupt on the systick timer is triggered
*Params:
*            null
*Returns:
*            null
********************************************************/
void SysTick_Handler()
{
    if(i == 10) {   //Loop back to zero
        i = 0;
    }
    writeSeg(i);    //Display the current number
    i++;            //Increment each time
}


