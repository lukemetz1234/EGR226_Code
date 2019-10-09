/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 05 Part 3 - Digital Inputs and Outputs – Interfacing External Switches and LEDs to the MSP432
* File:              main.c
* Description:       This program initializes the proper GPIO pins for three LEDs and two buttons.
*                    Each button is checked for a press, and when the forward button is pressed, the red LED lights up, upon the
*                    next press the green LED lights up, followed by the yellow. When the forward button is held, this
*                    cycle happens switching between each LED every second.This same thing happens in reverse LED color order with
*                    the reverse button.
*******************************************************/

/***************
*    PART 3
***************/

#include "msp.h"
#include <stdio.h>
#include <stdlib.h>


void delay_ms(int del);
int checkButtonPress();
int checkBB();

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer


    P3->SEL0 &= ~BIT7;  //Set up button on P3.7 (forward)
    P3->SEL1 &= ~BIT7;
    P3->DIR &= ~BIT7;   //Input
    P3->REN |= BIT7;    //Enables internal resistor
    P3->OUT |= BIT7;    //Pull up resistor

    P5->SEL0 &= ~BIT5;  //Set up button on P5.5 (backward)
    P5->SEL1 &= ~BIT5;
    P5->DIR &= ~BIT5;   //Input
    P5->REN |= BIT5;    //Enables internal resistor
    P5->OUT |= BIT5;    //Pull up resistor

    P5->SEL0 &= ~BIT0;  //Set up Red LED
    P5->SEL1 &= ~BIT0;
    P5->DIR |= BIT0;    //Output
    P5->OUT &= ~BIT0;   //Initiate off

    P6->SEL0 &= ~BIT6;  //Set up Green LED
    P6->SEL1 &= ~BIT6;
    P6->DIR |= BIT6;    //Output
    P6->OUT &= ~BIT6;   //Initiate off

    P1->SEL0 &= ~BIT7;  //Set up Yellow LED
    P1->SEL1 &= ~BIT7;
    P1->DIR |= BIT7;    //Output
    P1->OUT &= ~BIT7;   //Off

    int counter = 1;    //Used to switch cases
    int flag = 0;       //Flags to stop the LED from immediately switching when the button is released after a button hold down
    int flagB = 0;

    while(1) {
        switch(counter)
        {
        case 1:
            if (checkButtonPress() == 1 || checkBB() == 1) {      //Check for either button press
                counter = 2;    //Enter case 2
            }
            break;

        case 2:
            P5->OUT |= BIT0;    //Red LED on
            P6->OUT &= ~BIT6;   //Green off
            P1->OUT &= ~BIT7;   //Yellow off

            if (checkButtonPress() == 1) {  //Check for forward button press
                delay_ms(200);              //Wait 200 ms
                if (checkButtonPress() == 0 && flag == 0) { //Check to see if the button has been released within the 200 ms delay
                    counter = 3; //next case in sequence    //If the button was released, then the user pressed the button (did not hold it down)
                    flag = 0;                               //and the program jumps immediately to the next case
                }
                else {
                    delay_ms(1000);     //otherwise if the button is still being held down wait for one second
                    flag = 0;
                    if (checkButtonPress() == 1) {      //Then check if the button is still being held down
                        counter = 3;                    //If it is, switch to the next case.
                        flag = 1;                       //Set the flag here to one, in the next case the flag is checked for a zero
                    }                        //If the flag is a one in the next case, this will prevent the program from detecting a button
                }                            //release after the first 200 ms delay and immediately switching.
            }


            else if (checkBB() == 1) {  //Check for reverse button press
                delay_ms(200);          //Wait 200 ms
                if (checkBB() == 0 && flagB == 0) { //Check to see if the button has been released within the 200 ms delay
                    counter = 4;    //Go to previous case in sequence
                    flagB = 0;
                }
                else {
                    delay_ms(1000);     //Otherwise wait for one second
                    flagB = 0;
                    if (checkBB() == 1) {     //Then check if the button is still being held down
                        counter = 4;            //If it is, switch to the previous case
                        flagB = 1;
                    }
                }
            }
            break;
        case 3:
            //Everything in case 3 functions exactly the same as case 2, besides the next case in the sequence
            P5->OUT &= ~BIT0;   //Red off
            P6->OUT |= BIT6;    //Green on
            P1->OUT &= ~BIT7;   //Yellow off

            //Forward button
            if (checkButtonPress() == 1) {
                delay_ms(200);
                if (checkButtonPress() == 0 && flag == 0) {
                    counter = 4;
                    flag = 0;
                }
                else {
                    delay_ms(1000);
                    flag = 0;
                    if (checkButtonPress() == 1) {
                        counter = 4;
                        flag = 1;
                    }
                }
            }

            //Backward button
            else if (checkBB() == 1) {
                delay_ms(200);
                if (checkBB() == 0 && flagB == 0) {
                    counter = 2;
                    flagB = 0;
                }
                else {
                    delay_ms(1000);
                    flagB = 0;
                    if (checkBB() == 1) {
                        counter = 2;
                        flagB = 1;
                    }
                }
            }
            break;
        case 4:
            //Everything in case 4 functions exactly the same as case 2 and 3, besides the next case in the sequence
            P5->OUT &= ~BIT0;   //Red off
            P6->OUT &= ~BIT6;   //Green off
            P1->OUT |= BIT7;    //Yellow on

            //Forward button
            if (checkButtonPress() == 1) {
                delay_ms(200);
                if (checkButtonPress() == 0 && flag == 0) {
                    counter = 2;
                    flag = 0;
                }
                else {
                    delay_ms(1000);
                    flag = 0;
                    if (checkButtonPress() == 1) {
                        counter = 2;
                        flag = 1;
                    }
                }
            }

            //Backward button
            else if (checkBB() == 1) {
                delay_ms(200);
                if (checkBB() == 0 && flagB == 0) {
                    counter = 3;
                    flagB = 0;
                }
                else {
                    delay_ms(1000);
                    flagB = 0;
                    if (checkBB() == 1) {
                        counter = 3;
                        flagB = 1;
                    }
                }
            }
            break;
        }
    }
}

/***| checkButtonPress() |************************************//*
*Brief:    This function checks the button on P3.7 (forward button) being pressed
*          and returns a 1 if it is, or 0 if it is not
*Params:
*            null
*Returns:
*            1 if button pressed, 0 if not
********************************************************/
int checkButtonPress() {
    if((P3->IN & BIT7) == 0) {      //Check for the button being pressed
        delay_ms(5);                //Delay 5 ms for debounce
        if ((P3->IN & BIT7) == 0) { //Check agian for the button still being pressed
                return 1;           //If its still pressed, return a 1
        }
        else {
                return 0;           //Otherwise return a 0
        }
    }
    return 0;
}

/***| checkBB() |************************************//*
*Brief:    This function checks the button on P5.5 (backward button) being pressed
*          and returns a 1 if it is, or 0 if it is not
*Params:
*            null
*Returns:
*            1 if button pressed, 0 if not
********************************************************/
int checkBB() {
    if((P5->IN & BIT5) == 0) {
        delay_ms(5);
        if((P5->IN & BIT5) == 0) {
            return 1;
        }
        else {
            return 0;
        }
    }
    return 0;
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
