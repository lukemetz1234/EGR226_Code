/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 11 IR Emitting and Precise Input Timing with TimerA
* File:              main.c
* Description:       This program uses TimerA to capture a frequency on P2.4. This frequency is coming from an IR
*                    receiving LED that is pointed at an IR emitting LED connected to a function generator. When
*                    a frequency of 10 Hz is detected, the on-board blue LED is illuminated, and the red LED is
*                    illuminated when a frequency of 14 Hz is detected.
*******************************************************/

#include "msp.h"
#include <stdio.h>

uint16_t firstVal,secondVal,period;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int f = 0;  //Frequency

    //Capture input on P2.4
    P2->SEL0 |=  BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR  &= ~BIT4;

    P2->SEL0 &= ~(BIT0|BIT2);   //Blue and Green LED setup
    P2->SEL1 &= ~(BIT0|BIT2);
    P2->DIR  |= (BIT0|BIT2);    //Output
    P2->OUT &= ~(BIT0|BIT2);    //Initially off

    TIMER_A0->CTL = 0b1000100100;   //SMCLK, Continuous mode, divide by 1
    TIMER_A0->EX0 = 5;              //Divide by 6
    TIMER_A0->CCTL[1] = 0b0100100100010000;  //Capture Rising Edge, Synchronous, Interrupt

    NVIC_EnableIRQ(TA0_N_IRQn);     //Interrupt enable
    __enable_interrupts();

    while(1){
        f = ((3000000/6)/period) + 1;    //Calculates the frequency, add 1 because of integer math

        if(f == 10) {           //If the frequency is equal to 10
            P2->OUT |= BIT2;
            P2->OUT &= ~BIT0;
        }
        else if(f == 14) {      //If the frequency is equal to 14
            P2->OUT |= BIT0;
            P2->OUT &= ~BIT2;
        }
        else {
            P2->OUT &= ~BIT2;   //Otherwise both LED's off
            P2->OUT &= ~BIT0;
        }

        printf("frequency = %d Hz\n",f);    //Prints the frequency to the console window
        __delay_cycles(1500000);            //Half a second
    }
}

void TA0_N_IRQHandler()
{
    if(TIMER_A0->CCTL[1] & BIT0)        //Interrupt flag check
    {
        firstVal = secondVal;
        secondVal = TIMER_A0->CCR[1];       //Time since last interrupt
        period = (secondVal - firstVal);    //Time between rising edges
    }
    TIMER_A0->CCTL[1] &= ~(BIT1|BIT0);      //Clear interrupt flag
}
