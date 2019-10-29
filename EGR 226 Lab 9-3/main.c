/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 09-3 Using GPIO interrupts to control PWM and 7-Segment LED
* File:              main.c
* Description:       This program connects a 7 segment display to the board and counts up from 0-9.
*                    Two push buttons are used to either increment the displayed number by one, or derement by one.
*                    Interrupts are used on the buttons port.
*******************************************************/

#include "msp.h"

void PORT4_IRQHandler();
void pinSetup();
void writeSeg(int seg);

int i = 0;      //Current number being displayed

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    pinSetup();
    writeSeg(0);    //Initialize at zero

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
    P4->SEL0 &= ~(BIT0|BIT1|BIT2);
    P4->SEL1 &= ~(BIT0|BIT1|BIT2);
    P4->DIR &= ~(BIT0|BIT1|BIT2);
    P4->REN |= (BIT0|BIT1|BIT2);    //Enables internal resistor
    P4->OUT |= (BIT0|BIT1|BIT2);    //Pull up resistor
    P4->IE |= (BIT0|BIT1|BIT2);     //Turn on interrupt flag capture

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

    NVIC_EnableIRQ(PORT4_IRQn); //Enable interrupts on port 4
    __enable_interrupts();
}

/***| PORT4_IRQHandler() |************************************//*
*Brief:    This function runs every time an interrupt on port 4 is triggered. It either increments or decrements
*          the number by one.
*Params:
*            null
*Returns:
*            null
********************************************************/
void PORT4_IRQHandler()
{
    if(P4->IFG & BIT1) {        //Check for decrease
        __delay_cycles(30000);  //Debounce
        if(P4->IFG & BIT1) {
            if(i == 0) {
                i = 9;          //Loop back around
            }
            else {
                i--;            //Decrement by one
            }
        }
        P4->IFG &= ~BIT1;       //clear (Acknowledge) flag
    }

    if(P4->IFG & BIT2) {        //Check for increase
        __delay_cycles(30000);  //Debounce
        if(P4->IFG & BIT2) {
            if(i == 9) {
                i = 0;          //Loop
            }
            else {
                i++;            //Increment by one
            }
        }
        P4->IFG &= ~BIT2;       //clear (Acknowledge) flag
    }
    writeSeg(i);                //Write the number to the display
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
    case 0:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= (BIT4|BIT5);
        break;
    case 1:
        pinSetup();
        P4->OUT |= (BIT7|BIT5);
        break;

    case 2:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6);
        P5->OUT |= (BIT4|BIT5);
        break;
    case 3:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6|BIT7);
        P5->OUT |= BIT4;
        break;
    case 4:
        pinSetup();
        P4->OUT |= (BIT5|BIT6|BIT7);
        P1->OUT |= BIT5;
        break;
    case 5:
        pinSetup();
        P4->OUT |= (BIT4|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= BIT4;
        break;
    case 6:
        pinSetup();
        P4->OUT |= (BIT4|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= (BIT4|BIT5);
        break;
    case 7:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT7);
        break;
    case 8:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= (BIT4|BIT5);
        break;
    case 9:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= BIT4;
        break;
    }
}
