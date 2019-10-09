/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 05 Part 1 - Digital Inputs and Outputs – Interfacing External Switches and LEDs to the MSP432
* File:              main.c
* Description:       This program initializes the proper GPIO pins for three LEDs and one button.
*                    The button is checked for a press, and when it is, the red LED lights up, upon the
*                    next press the green LED lights up, followed by the yellow. This cycle continues
*                    for each press of the button.
*******************************************************/

/***************
*    PART 1
***************/

#include "msp.h"

void delay_ms(int del);
int checkButtonPress();

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer


    P3->SEL0 &= ~BIT7;  //Set up button on P3.7
    P3->SEL1 &= ~BIT7;
    P3->DIR &= ~BIT7;   //Input
    P3->REN |= BIT7;    //Enables internal resistor
    P3->OUT |= BIT7;    //Pull up resistor

    P5->SEL0 &= ~BIT0;  //Set up Red LED on P5.0
    P5->SEL1 &= ~BIT0;
    P5->DIR |= BIT0;    //Output
    P5->OUT &= ~BIT0;   //Initiate off

    P1->SEL0 &= ~BIT6;  //Set up Green LED on P1.6
    P1->SEL1 &= ~BIT6;
    P1->DIR |= BIT6;    //Output
    P1->OUT &= ~BIT6;   //Initiate off

    P1->SEL0 &= ~BIT7;  //Set up Yellow LED on P1.7
    P1->SEL1 &= ~BIT7;
    P1->DIR |= BIT7;    //Output
    P1->OUT &= ~BIT7;   //Off

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
            P5->OUT |= BIT0;    //Red LED on
            P1->OUT &= ~BIT6;   //Green off
            P1->OUT &= ~BIT7;   //Yellow off
            if (checkButtonPress() == 1) {      //Check for button press
                while(checkButtonPress() == 1); //Wait until the button is released
                counter = 3;    //Enter case 3
            }
            break;
        case 3:
            P5->OUT &= ~BIT0;   //Red off
            P1->OUT |= BIT6;    //Green on
            P1->OUT &= ~BIT7;   //Yellow off
            if (checkButtonPress() == 1) {      //Check for button press
                while(checkButtonPress() == 1); //Wait until the button is released
                counter = 4;    //Enter case 4
            }
            break;
        case 4:
            P5->OUT &= ~BIT0;   //Red off
            P1->OUT &= ~BIT6;   //Green off
            P1->OUT |= BIT7;    //Yellow on
            if (checkButtonPress() == 1) {      //Check for button press
                while(checkButtonPress() == 1); //Wait until the button is released
                counter = 2;    //Loop back to case 2
            }
            break;
        }
    }
}


/***| checkButtonPress() |************************************//*
*Brief:    This function checks the button on P3.7 being pressed
*          and returns a 1 if it is, or 0 if it is not
*Params:
*            null
*Returns:
*            1 if button pressed, 0 if not
********************************************************/
int checkButtonPress() {
    if((P3->IN & BIT7) == 0) {      //Check for the button being pressed
        delay_ms(5);                //Delay 5 ms for de-bounce
        if ((P3->IN & BIT7) == 0) { //Check again for the button still being pressed
                return 1;           //If its still pressed, return a 1
        }
        else {
                return 0;           //Otherwise return a 0
        }
    }
}


/***| delay_ms() |************************************//*
*Brief:    Given a value representing number of ms, wait for that amount
*          of time using the SysTick timer
*Params:
*            (int) value:    ms to delay by
*Returns:
*            null
********************************************************/
void delay_ms(int value)
{
    SysTick->LOAD = value*3000;  //3000 cycles in a millisecond
    SysTick->VAL = 91719;  //Set Val to anything to clear //STCVR
    SysTick->CTRL |= BIT0;  //Enable   //Status and Control Register STCSR
    while((SysTick->CTRL&BIT(16))==0);  //Wait for SysTick to be complete
}
