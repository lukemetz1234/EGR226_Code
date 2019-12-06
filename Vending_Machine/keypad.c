/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab
* File:              keypad.c
* Description:       This file contains the functions to initialize the pins for a keypad connected to the MSP432 board, read
*                    from the keypad on interrpts, and return the correct debounced key press.
*******************************************************/


#include "msp.h"
#include "keypad.h"
#include <stdio.h>

int keyP = -1;
uint8_t reg;


/***| getKey() |*****************************************
*Brief:    This function returns the debounced key
*Params:
*            null
*Returns:
*            key
********************************************************/
int getKey() {
    if(reg != 0x00)     //If the shift register is not all 0's
        keyP = -1;      //The key has not been debounced, so return a -1
    if(keyP != -1) {
        printf("Key = %d\n", keyP);
    }
    reg = reg << 1;     //Shift register by one
    reg |= 1;           //Add one to it
    return keyP;
}


/***| setUpKeypad() |************************************
*Brief:    This function initializes the pins used by the keypad
*Params:
*            null
*Returns:
*            null
********************************************************/
void setUpKeypad() {
    //Columns
    P5->SEL0 &= ~(BIT6|BIT1|BIT2);
    P5->SEL1 &= ~(BIT6|BIT1|BIT2);
    P5->DIR &= ~(BIT6|BIT1|BIT2);   //Input
    P5->REN |= (BIT6|BIT1|BIT2);    //Enables internal resistor
    P5->OUT |= (BIT6|BIT1|BIT2);    //Pull up resistor
    P5->IE  |= (BIT6|BIT1|BIT2);
    P5->IES |= (BIT6|BIT1|BIT2);    //Edge select

    //Rows
    P2->SEL0 &= ~(BIT4|BIT5|BIT6|BIT7);  //Set up pin P1.5
    P2->SEL1 &= ~(BIT4|BIT5|BIT6|BIT7);
    P2->DIR |= (BIT4|BIT5|BIT6|BIT7);   //Output
    P2->OUT &= ~(BIT4|BIT5|BIT6|BIT7);    //Output a 0
}


/***| defaultState() |**********************************
*Brief:    This function returns the keypad pins to their initially set up state
*Params:
*            null
*Returns:
*            null
********************************************************/
void defaultState() {
    P5->DIR &= ~(BIT6|BIT1|BIT2);   //Input
    P5->REN |= (BIT6|BIT1|BIT2);    //Enables internal resistor
    P5->OUT |= (BIT6|BIT1|BIT2);    //Pull up resistor

    P2->DIR |= (BIT4|BIT5|BIT6|BIT7);   //Output
    P2->OUT &= ~(BIT4|BIT5|BIT6|BIT7);    //Output a 0
}


/***| PORT5_IRQHandler() |**********************************
*Brief:    This interrupt handler is called when a key is pressed and cycles through the columns and rows
*          to determine the key pressed
*Params:
*            null
*Returns:
*            null
********************************************************/
void PORT5_IRQHandler(){
    setUpKeypad();
    if((P5->IN & BIT6) ==  0) {
        //Column 1
        //Change rows to inputs and columns to outputs to read the correct key
        P5->DIR |= (BIT6|BIT1|BIT2);        //Output
        P5->OUT &= ~(BIT6|BIT1|BIT2);       //Output a 0

        P2->DIR &= ~(BIT4|BIT5|BIT6|BIT7);  //Input
        P2->REN |= (BIT4|BIT5|BIT6|BIT7);   //Enables internal resistor
        P2->OUT |= (BIT4|BIT5|BIT6|BIT7);   //Pull up

        if((P2->IN & BIT4) == 0) {          //First key
            while(1) {                      //Short while loop for debounce
                if((P2->IN & BIT4) == 0)
                    reg = reg << 1;         //While the key is read as pressed, shift in zeros to the register
                else {
                    reg = reg << 1;         //Otherwise shift in ones
                    reg |= 0b1;
                }
                if(reg == 0xFF) {           //Once the register is all ones
                    keyP = -1;              //The key has been released
                    break;
                }
                else if(reg == 0x00) {      //If the register is filled with zeros, the key has been debounced
                    keyP = 11;
                    break;
                }
            }
        }
        else if((P2->IN & BIT5) == 0) {       //--------| Process above is identical for all columns and rows |-------------
            while(1) {
                if((P2->IN & BIT5) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 9;
                    break;
                }
            }
        }
        else if((P2->IN & BIT6) == 0) {
            while(1) {
                if((P2->IN & BIT6) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 6;
                    break;
                }
            }
        }
        else if((P2->IN & BIT7) == 0) {
            while(1) {
                if((P2->IN & BIT7) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 3;
                    break;
                }
            }
        }
    }
    else if((P5->IN & BIT1) == 0) {
        //Column 2
        P5->DIR |= (BIT6|BIT1|BIT2);   //Output
        P5->OUT &= ~(BIT6|BIT1|BIT2);    //Output a 0

        P2->DIR &= ~(BIT4|BIT5|BIT6|BIT7);   //Input
        P2->REN |= (BIT4|BIT5|BIT6|BIT7);    //Enables internal resistor
        P2->OUT |= (BIT4|BIT5|BIT6|BIT7);    //Pull up

        if((P2->IN & BIT4) == 0) {
            while(1) {
                if((P2->IN & BIT4) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 0;
                    break;
                }
            }
        }
        else if((P2->IN & BIT5) == 0) {
            while(1) {
                if((P2->IN & BIT5) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 8;
                    break;
                }
            }
        }
        else if((P2->IN & BIT6) == 0) {
            while(1) {
                if((P2->IN & BIT6) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 5;
                    break;
                }
            }
        }
        else if((P2->IN & BIT7) == 0) {
            while(1) {
                if((P2->IN & BIT7) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 2;
                    break;
                }
            }
        }
    }
    else if((P5->IN & BIT2) == 0) {
        //Column 3
        P5->DIR |= (BIT6|BIT1|BIT2);   //Output
        P5->OUT &= ~(BIT6|BIT1|BIT2);    //Output a 0

        P2->DIR &= ~(BIT4|BIT5|BIT6|BIT7);   //Input
        P2->REN |= (BIT4|BIT5|BIT6|BIT7);    //Enables internal resistor
        P2->OUT |= (BIT4|BIT5|BIT6|BIT7);    //Pull up

        if((P2->IN & BIT4) == 0) {
            while(1) {
                if((P2->IN & BIT4) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 10;
                    break;
                }
            }
        }
        else if((P2->IN & BIT5) == 0) {
            while(1) {
                if((P2->IN & BIT5) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 7;
                    break;
                }
            }
        }
        else if((P2->IN & BIT6) == 0) {
            while(1) {
                if((P2->IN & BIT6) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 4;
                    break;
                }
            }
        }
        else if((P2->IN & BIT7) == 0) {
            while(1) {
                if((P2->IN & BIT7) == 0)
                    reg = reg << 1;
                else {
                    reg = reg << 1;
                    reg |= 0b1;
                }
                if(reg == 0xFF) {
                    keyP = -1;
                    break;
                }
                else if(reg == 0x00) {
                    keyP = 1;
                    break;
                }
            }
        }
    }
    else {
        keyP = -1;
    }
    setUpKeypad();
    P5->IFG &= ~(BIT6|BIT1|BIT2);       //clear (Acknowledge) flag
}
