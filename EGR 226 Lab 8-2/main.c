/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 08-2 Creating a Pulse Width Modulated (PWM) Pseudo Analog Voltage Using
*                    Timer Peripherals of a Microcontroller
* File:              main.c
* Description:       This program controls the speed of a motor using TimerA as a PWM output.
*******************************************************/

#include "msp.h"
#include <stdio.h>

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int ONTIME = 375;

    P2->SEL0 |=  BIT6;  //giving timerA control
    P2->SEL1 &= ~BIT6;
    P2->DIR  |=  BIT6;  //Output

    TIMER_A0->CTL = 0b000001001010100;      //Divide by 8, up mode, SMCLK
    TIMER_A0->EX0 = 0b000;                  //Divide by 1
    TIMER_A0->CCR[0] = 7500;               //Period
    TIMER_A0->CCTL[3] = 0b11100000;         //Reset/Set
    //TIMER_A0->CCR[3] = ONTIME;              //TA0.3
    int i = 0;

    while(1) {
        __delay_cycles(6000000);

        if (i == 0) {
            TIMER_A0->CCR[3] = ONTIME;      //Sets the on time of the motor, able to be changed
            __delay_cycles(3000000);
            TIMER_A0->CCR[3] = 0;
        }
        else if (i == 3) {
            TIMER_A0->CCR[3] = 562;      //Sets the on time of the motor, able to be changed
            __delay_cycles(3000000);
            TIMER_A0->CCR[3] = 0;
        }

        i++;

        if (i == 5) {
            i = 0;
        }
    }
}

