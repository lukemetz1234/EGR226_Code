/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 11 IR Emitting and Precise Input Timing with TimerA
* File:              main.c
* Description:       This program reads the current voltage on P5.1 which is connected to a temperature sensor. Based on
*                    the voltage reading, it is converted into degrees and printed out to the user via the LCD display
*                    used in a previous lab. A button is interfaced to allow for switching between degrees C and
*                    degrees F.
*******************************************************/

#include "msp.h"
#include <stdio.h>

uint16_t firstVal,secondVal,period;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int f = 0;

    //Capture input on P2.4
    P2->SEL0 |=  BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR  &= ~BIT4;

    P2->SEL0 &= ~(BIT0|BIT2);
    P2->SEL1 &= ~(BIT0|BIT2);
    P2->DIR  |= (BIT0|BIT2);
    P2->OUT &= ~(BIT0|BIT2);

    TIMER_A0->CTL = 0b1000100100;  //SMCLK, Continuous mode, divide by 1
    TIMER_A0->EX0 = 5;  //Divide by 6
    TIMER_A0->CCTL[1] = 0b0100100100010000;  //Capture Rising Edge, Synchronous, Interrupt

    NVIC_EnableIRQ(TA0_N_IRQn);
    __enable_interrupts();

    while(1){
        f = ((3000000/6)/period) + 1;    //Add one for some reason

        if(f == 10) {
            P2->OUT |= BIT2;
            P2->OUT &= ~BIT0;
        }
        else if(f == 14) {
            P2->OUT |= BIT0;
            P2->OUT &= ~BIT2;
        }
        else {
            P2->OUT &= ~BIT2;
            P2->OUT &= ~BIT0;
        }

        printf("frequency = %d Hz\n",f);
        __delay_cycles(1500000);
    }
}

void TA0_N_IRQHandler()
{
    if(TIMER_A0->CCTL[1] & BIT0)
    {
        firstVal = secondVal;
        secondVal = TIMER_A0->CCR[1];
        period = (secondVal - firstVal);
    }
    TIMER_A0->CCTL[1] &= ~(BIT1|BIT0);
}
