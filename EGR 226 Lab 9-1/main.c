/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 09-1 Using GPIO interrupts to control PWM and 7-Segment LED
* File:              main.c
* Description:       This program controls the speed of a motor using TimerA as a PWM output. The speed can be increased
*                    by 10% by pushing a button, decreased by 10% by another button, and stopped by a third button.
*                    Interrupts are used on each button.
*******************************************************/

#include "msp.h"

void PORT4_IRQHandler();
void pinSetup();

int ONTIME = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    pinSetup();

    while(1) {      //Run program continuously

    }
}


void pinSetup() {
    P4->SEL0 &= ~(BIT0|BIT1|BIT2);
    P4->SEL1 &= ~(BIT0|BIT1|BIT2);
    P4->DIR &= ~(BIT0|BIT1|BIT2);
    P4->REN |= (BIT0|BIT1|BIT2);    //Enables internal resistor
    P4->OUT |= (BIT0|BIT1|BIT2);    //Pull up resistor
    P4->IE |= (BIT0|BIT1|BIT2);     //Turn on interrupt flag capture

    P2->SEL0 |=  BIT6;  //giving timerA control
    P2->SEL1 &= ~BIT6;
    P2->DIR  |=  BIT6;

    TIMER_A0->CTL = 0b000001001010100;      //Up mode, SMCLK
    TIMER_A0->EX0 = 0b000;                  //Divide by 1
    TIMER_A0->CCR[0] = 37500;               //Period
    TIMER_A0->CCTL[3] = 0b11100000;         //Reset/Set
    TIMER_A0->CCR[3] = ONTIME;

    NVIC_EnableIRQ(PORT4_IRQn);     //Enable interrupts
    __enable_interrupts();
}

void PORT4_IRQHandler()
{
    if(P4->IFG & BIT0) {        //Check for stop button
        TIMER_A0->CCR[3] = 0;   //0% duty cycle
        P4->IFG &= ~BIT0;  //clear (Acknowledge) flag
    }

    if(P4->IFG & BIT1) {            //Check for decrease
        __delay_cycles(15000);      //For debounce
        if(P4->IFG & BIT1) {
            if(TIMER_A0->CCR[3] == 0) {
                TIMER_A0->CCR[3] = 0;       //Cant go below zero
            }
            else {
                TIMER_A0->CCR[3] -= 3750;   //Decrement the speed by 10% (3750)
            }
        }
        P4->IFG &= ~BIT1;  //clear (Acknowledge) flag
    }

    if(P4->IFG & BIT2) {            //Check for increase
        __delay_cycles(15000);      //Debounce
        if(P4->IFG & BIT2) {
            if(TIMER_A0->CCR[3] == 37500) {     //Cant go above 100% duty cycle
                TIMER_A0->CCR[3] = 37500;
            }
            else {
                TIMER_A0->CCR[3] += 3750;       //Increment speed by 10% (3750)
            }
        }
        P4->IFG &= ~BIT2;  //clear (Acknowledge) flag
    }
}
