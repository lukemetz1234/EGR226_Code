/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 06 Interfacing a keypad with the MSP432
* File:              main.c
* Description:       This program initializes the proper pins for the 12 key keypad and reads the key pressed by the user.
*                    For part one, the program simply prints out what the user has entered. For part two, the last 4 digits entered are
*                    recorded as a pin and printed to the user. The pin must be 4 digits and cannot include a *.
*******************************************************/

#include "msp.h"
#include <stdio.h>
#include <stdlib.h>

//---Function definitions----
void setUp();
int read_Keypad();
void delay_ms(int value);
void printKeyPress(int input);
int getPin(int input, int flag, int pin[4]);
void comparePin();
//----------------------------

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	setUp();        //Initializes all pins used for the keypad
	int flag = 1;   //Flag used to initialize the numbers of the 4 digit pin on reset
	int pin[4];     //Holds the 4 digit pin

	while(1) {
	    //------Part 1-------
        //printKeyPress(read_Keypad());         //Reads the number pressed on the keypad and prints it to the console
	    //------Part 1-------

	    //------Part 2-------
        getPin(read_Keypad(), flag, pin);       //Reads in a 4 digit pin from the user
        //------Part 2-------

        flag++;                                 //Increment flag so that initialization only happens once
    }
}


/***| setUp() |************************************//*
*Brief:    This function initializes all of the pins that the keypad is connected to. Each
*          pin is set as an input with a pull up resistor.
*Params:
*            null
*Returns:
*            null
********************************************************/
void setUp() {
    P6->SEL0 &= ~(BIT5|BIT4);  //Set up pins P6.4 and P6.5
    P6->SEL1 &= ~(BIT5|BIT4);
    P6->DIR &= ~(BIT5|BIT4);   //Input
    P6->REN |= (BIT5|BIT4);    //Enables internal resistor
    P6->OUT |= (BIT5|BIT4);    //Pull up resistor

    P4->SEL0 &= ~(BIT6|BIT3|BIT1);  //SSet up pins P4.1, P4.3, and P4.6
    P4->SEL1 &= ~(BIT6|BIT3|BIT1);
    P4->DIR &= ~(BIT6|BIT3|BIT1);   //Input
    P4->REN |= (BIT6|BIT3|BIT1);    //Enables internal resistor
    P4->OUT |= (BIT6|BIT3|BIT1);    //Pull up resistor

    P1->SEL0 &= ~BIT5;  //Set up pin P1.5
    P1->SEL1 &= ~BIT5;
    P1->DIR &= ~BIT5;   //Input
    P1->REN |= BIT5;    //Enables internal resistor
    P1->OUT |= BIT5;    //Pull up resistor

    P3->SEL0 &= ~BIT3;  //Set up pin P3.3
    P3->SEL1 &= ~BIT3;
    P3->DIR &= ~BIT3;   //Input
    P3->REN |= BIT3;    //Enables internal resistor
    P3->OUT |= BIT3;    //Pull up resistor
}

/***| read_Keypad() |************************************//*
*Brief:    This function reads the key pressed on the keypad and returns it as an int
*Params:
*            null
*Returns:
*            int representing the key pressed
********************************************************/
int read_Keypad() {
    setUp();        //Calls setup to ensure all pins are input, pull up resistor
    delay_ms(10);   //Small delay to ensure the pins are properly set

    P3->DIR |= BIT3;    //Set P3.3 to an output
    P3->OUT &= ~BIT3;   //Output a 0
    delay_ms(10);       //Delay to ensure pin state change
    if((P1->IN & BIT5) == 0) {          //Check for the pound key being pressed
        while((P1->IN & BIT5) == 0);
        return 10; //pound
    }
    if((P4->IN & BIT6) == 0) {          //Check for 9 button
        while((P4->IN & BIT6) == 0);
        return 9;
    }
    if((P6->IN & BIT5) == 0) {          //Check for 6 button
        while((P6->IN & BIT5) == 0);
        return 6;
    }
    if((P6->IN & BIT4) == 0) {          //Check for 3 button
        while((P6->IN & BIT4) == 0);
        return 3;
    }
    setUp();        //Call setup again to reset P3.3
    delay_ms(10);

    P4->DIR |= BIT1;    //Set P4.1 as an output
    P4->OUT &= ~BIT1;   //Output a 0
    delay_ms(10);
    if((P1->IN & BIT5) == 0) {          //Check for 0 button
        while((P1->IN & BIT5) == 0);
        return 0;
    }
    if((P4->IN & BIT6) == 0) {          //Check for 8 button
        while((P4->IN & BIT6) == 0);
        return 8;
    }
    if((P6->IN & BIT5) == 0) {          //Check for 5 button
        while((P6->IN & BIT5) == 0);
        return 5;
    }
    if((P6->IN & BIT4) == 0) {          //Check for 2 buton
        while((P6->IN & BIT4) == 0);
        return 2;
    }
    setUp();

    P4->DIR |= BIT3;    //Set up P4.3 as an output
    P4->OUT &= ~BIT3;   //Output a 0
    delay_ms(10);
    if((P1->IN & BIT5) == 0) {          //Check for * button
        while((P1->IN & BIT5) == 0);
        return 11;  //*
    }
    if((P4->IN & BIT6) == 0) {          //Check for 7 button
        while((P4->IN & BIT6) == 0);
        return 7;
    }
    if((P6->IN & BIT5) == 0) {          //Check for 4 button
        while((P6->IN & BIT5) == 0);
        return 4;
    }
    if((P6->IN & BIT4) == 0) {          //Check for 1 button
        while((P6->IN & BIT4) == 0);
        return 1;
    }
    setUp();

    return -1;      //If no button is pressed, return a -1
}

/***| printKeyPress() |************************************//*
*Brief:    This function prints the correct character based on which key
*          was pressed on the keypad
*Params:
*            input
*Returns:
*            null
********************************************************/
void printKeyPress(int input) {
    if(input > -1) {        //Make sure a key was pressed
        if (input == 10)    //Pound key
            printf("#\n");
        else if (input == 11)   //* key
            printf("*\n");
        else
            printf("%d\n", input);  //Otherwise print the corresponding number
    }
}

/***| getPin() |************************************//*
*Brief:    This function takes key press inputs until the user hits the # key, it then prints out the last
*          4 digits as a pin. Error checking is in place for incorrect length or keypresses
*Params:
*            input      input from the keypad
*            flag       flag to initialize on reset
*            pin[4]     array to hold pin
*Returns:
*            null
********************************************************/
int getPin(int input, int flag, int pin[4]) {
    int temp, temp2, temp3; //Temporary ints for shifting

    if(flag == 1) {     //Initialize upon reset in order to properly check for errors
        printf("Enter a 4 digit pin followed by the pound(#) symbol.\n");
        pin[0] = 25;
        pin[1] = 25;
        pin[2] = 25;        //A 25 showing up in the pin means an invalid key was pressed, or that space has not
        pin[3] = 25;        //yet been filled by a key press, indicating less than 4 digits
     }

    if (input == 11) {      //If a * is pressed
        input = 25;
    }

    if(input > -1) {
        if (input == 10) {
            if (pin[0] != 25 && pin[1] != 25 && pin[2] != 25 && pin[3] != 25) {         //Check for a 25 (error) at any pin digit
                printf("You entered the pin: %d %d %d %d\n", pin[3], pin[2], pin[1], pin[0]);   //If there are no errors, print the pin
                pin[0] = 25;        //Reset each value to null
                pin[1] = 25;
                pin[2] = 25;
                pin[3] = 25;
                input = 25;
            }
            else {
                printf("Invalid pin entry, please enter four numbers.\n");  //Otherwise print an error and ask for a new pin
                pin[0] = 25;
                pin[1] = 25;        //Reset each value again
                pin[2] = 25;
                pin[3] = 25;
                input = 25;
            }
        }
        temp = pin[0];       //Shift each digit in the pin over and fill in the next user input
        temp2 = pin[1];
        temp3 = pin[2];
        pin[1] = temp;
        pin[2] = temp2;
        pin[3] = temp3;
        pin[0] = input;
    }
}

void comparePin() {
    //int pin1[4];
    //int pin2[4];


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
