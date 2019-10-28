/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 08-3 Creating a Pulse Width Modulated (PWM) Pseudo Analog Voltage Using
*                    Timer Peripherals of a Microcontroller
* File:              main.c
* Description:       This program controls the speed of a motor using TimerA as a PWM output. This code also integrates
*                    the keypad used in a previous lab to change the speed of the motor at the press of a button.
*******************************************************/

#include "msp.h"
#include "keypad.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int ONTIME = 37500;

    P2->SEL0 |=  BIT6;  //giving timerA
    P2->SEL1 &= ~BIT6;
    P2->DIR  |=  BIT6;  //Output

    TIMER_A0->CTL = 0b000001001010100;      //Divide by 8, up mode, SMCLK
    TIMER_A0->EX0 = 0b000;                  //Divide by 1
    TIMER_A0->CCR[0] = 37500;               //Period
    TIMER_A0->CCTL[3] = 0b11100000;         //Reset/Set
    TIMER_A0->CCR[3] = ONTIME;              //TA0.3


    while(1) {
        ONTIME = read_Keypad();     //Read the keypress
        //printf("%d\n", ONTIME);
        if(ONTIME != -1) {              //If no key was pressed
            ONTIME = (ONTIME * 3750);   //Set the motor speed to the key as a percentage
            TIMER_A0->CCR[3] = ONTIME;
        }
    }
}

