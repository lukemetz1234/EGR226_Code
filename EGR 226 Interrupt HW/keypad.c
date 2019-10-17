/*******************************************************
* Name:              Luke Metz
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Homework 2, part 3
* File:              keypad.c
* Description:       This program uses interrupts to read in key presses from the keypad
*                    It operates fully in the background so that main can run without interference.
*******************************************************/

#include "msp.h"
#include "keypad.h"

int8_t ROW = 0;         //Global variable of current row
int8_t COL = 0;         //Global variable of current column
int8_t BUTTON = -1;     //button pressed

int8_t LASTKEY = -1;    //Used to prevent filling array with a tap of the button

int8_t READ = 0;        //Array position for reading
int8_t STORE = 0;       //Array position for storing keys

int8_t NUMS[4] = {-1, -1, -1, -1};  //Array of values entered

void setupKeypad() {
    P6->SEL0 &= ~BIT1;  //Set up pins
    P6->SEL1 &= ~BIT1;
    P6->DIR &= ~BIT1;   //Input
    P6->REN |= BIT1;    //Enables internal resistor
    P6->OUT |= BIT1;    //Pull up resistor

    P4->SEL0 &= ~(BIT0|BIT2|BIT4);
    P4->SEL1 &= ~(BIT0|BIT2|BIT4);
    P4->DIR &= ~(BIT0|BIT2|BIT4);   //Input
    P4->REN |= (BIT0|BIT2|BIT4);    //Enables internal resistor
    P4->OUT |= (BIT0|BIT2|BIT4);    //Pull up resistor

    P4->SEL0 &= ~(BIT5|BIT7);
    P4->SEL1 &= ~(BIT5|BIT7);
    P4->DIR &= ~(BIT5|BIT7);    //Input
    P4->REN |= (BIT5|BIT7);     //Enables internal resistor
    P4->OUT |= (BIT5|BIT7);     //Pull up resistor
    P4->IE |= (BIT5|BIT7);      //Turn on interrupt flag capture

    P5->SEL0 &= ~BIT4;  //Set up pin
    P5->SEL1 &= ~BIT4;
    P5->DIR &= ~BIT4;   //Input
    P5->REN |= BIT4;    //Enables internal resistor
    P5->OUT |= BIT4;    //Pull up
    P5->IE |= BIT4;     //Turn on interrupt flag capture

    __enable_interrupts();      //Enables interrupts

    TA2_IRQ_Init();     //TA2 interrupt initialization
    TA3_IRQ_Init();     //TA3 interrupt initialization
}

/***| getKeypress() |************************************//*
*Brief:    This function is called by main every 5 seconds to return the last keypresses (up to 4)
*Params:
*            null
*Returns:
*            key
*
*            NOTE: Not working 100% properly because my READ and STORE positions are being messed up.
*                  The READ position is not always in sync with the very first STORE position, this means
*                  the READ position could be on a -1, causing main to believe that no key presses were hit.
*                  If 4 or more keys are pressed, they print out fine because there is no chance that the
*                  READ position will be on a -1.
********************************************************/
int getKeypress() {
    int numToReturn = NUMS[READ];
    NUMS[READ] = -1;

    if(READ == 3) {
        READ = 0;
    }
    else {
        READ++;
    }
    return numToReturn;
}


/***| TA2_IRQ_Init() |************************************//*
*Brief:    This function initializes TA2 as an interrupt
*Params:
*            null
*Returns:
*            null
********************************************************/
void TA2_IRQ_Init()
{
    TIMER_A2->EX0 = 2;                      //divide by 3
    TIMER_A2->CTL = 0b0000001011010110;     // interrupt enable, /8, up mode, SMCLK
    TIMER_A2->CCR[0] = 30;                  //Very small value so that readKeypress is continuously called
    NVIC_EnableIRQ(TA2_N_IRQn);             //Enable
}

/***| TA3_IRQ_Init() |************************************//*
*Brief:    This function initializes TA3 as an interrupt
*Params:
*            null
*Returns:
*            null
********************************************************/
void TA3_IRQ_Init()
{
    TIMER_A3->EX0 = 2;                      //divide by 3
    TIMER_A3->CTL = 0b0000001011010110;     // interrupt enable, /8, up mode, SMCLK
    TIMER_A3->CCR[0] = 60000;               //Close to half a second
    NVIC_EnableIRQ(TA3_N_IRQn);
}


/***| TA2_N_IRQHandler() |************************************//*
*Brief:    This function gets called when there is an interrupt on TA2
*Params:
*            null
*Returns:
*            null
********************************************************/
void TA2_N_IRQHandler()
{
    readKeypress();         //Reads a key press
    TIMER_A2->CTL &= ~BIT0; //clear interrupt
}

/***| TA3_N_IRQHandler() |************************************//*
*Brief:    This function gets called when there is an interrupt on TA3
*Params:
*            null
*Returns:
*            null
********************************************************/
void TA3_N_IRQHandler()
{
    LASTKEY = -1;           //Resets last key
    TIMER_A3->CTL &= ~BIT0; //clear interrupt
}

/***| readKeypress() |************************************//*
*Brief:    This function gets called when there is an interrupt on TA2, it reads
*          a key press from the keypad
*Params:
*            null
*Returns:
*            null
********************************************************/
void readKeypress() {
    ROW = 0;
    COL = 0;

    P6->DIR |= BIT1;        //Output
    P6->OUT &= ~BIT1;       //Output a 0
    ROW = 1;                //Row one checking
    if((P5->IN & BIT4) == 0) {  //Check for which column
        COL = 3;
    }
    if((P4->IN & BIT7) == 0) {
        COL = 2;
    }
    if((P4->IN & BIT5) == 0) {
        COL = 1;
    }
    BUTTON = decode(COL, ROW);  //Decode which key
    P6->DIR &= ~BIT1;           //Reset P6.1
    P6->OUT |= BIT1;
    if(BUTTON != -1) {          //Store the key press if one exists
        storeKey(BUTTON);
    }
    ROW = 0;
    COL = 0;


    P4->DIR |= BIT0;        //Output
    P4->OUT &= ~BIT0;       //Output a 0
    ROW = 2;                //Row 2 checking
    if((P5->IN & BIT4) == 0) {      //Column checking
        COL = 3;
    }
    if((P4->IN & BIT7) == 0) {
        COL = 2;
    }
    if((P4->IN & BIT5) == 0) {
        COL = 1;
    }
    BUTTON = decode(COL, ROW);      //Decode what key was ;ressed
    P4->DIR &= ~BIT0;
    P4->OUT |= BIT0;
    if(BUTTON != -1) {
        storeKey(BUTTON);       //Store the keypress in an array
    }
    ROW = 0;
    COL = 0;

    P4->DIR |= BIT2;            //Same function as previous two rows
    P4->OUT &= ~BIT2;
    ROW = 3;                    //Row 3
    if((P5->IN & BIT4) == 0) {
        COL = 3;
    }
    if((P4->IN & BIT7) == 0) {
        COL = 2;
    }
    if((P4->IN & BIT5) == 0) {
        COL = 1;
    }
    BUTTON = decode(COL, ROW);
    P4->DIR &= ~BIT2;
    P4->OUT |= BIT2;
    if(BUTTON != -1) {
        storeKey(BUTTON);
    }
    ROW = 0;
    COL = 0;

    P4->DIR |= BIT4;            //Same function as previous rows
    P4->OUT &= ~BIT4;
    ROW = 4;                    //Row 4
    if((P5->IN & BIT4) == 0) {
        COL = 3;
    }
    if((P4->IN & BIT7) == 0) {
        COL = 2;
    }
    if((P4->IN & BIT5) == 0) {
        COL = 1;
    }
    BUTTON = decode(COL, ROW);
    P4->DIR &= ~BIT4;
    P4->OUT |= BIT4;
    if(BUTTON != -1) {
        storeKey(BUTTON);
    }
    ROW = 0;
    COL = 0;
}


/***| decode(int col, int row) |************************************//*
*Brief:    This function takes inputs for which row and column a key was pressed on, and deciphers it
*Params:
*            col: column
*            row: row
*Returns:
*            correct key
********************************************************/
int decode(int col, int row) {
    if (col == 1 && row == 1) {         //Cycle through all possibilities
        return 1;
    }
    else if (col == 2 && row == 1) {
        return 2;
    }
    else if (col == 3 && row == 1) {
        return 3;
    }

    else if (col == 1 && row == 2) {
        return 4;
    }
    else if (col == 2 && row == 2) {
        return 5;
    }
    else if (col == 3 && row == 2) {
        return 6;
    }

    else if (col == 1 && row == 3) {
        return 7;
    }
    else if (col == 2 && row == 3) {
        return 8;
    }
    else if (col == 3 && row == 3) {
        return 9;
    }

    else if (col == 1 && row == 4) {
        return 20;
    }
    else if (col == 2 && row == 4) {
        return 0;
    }
    else if (col == 3 && row == 4) {
        return 30;
    }
    return -1;
}

/***| storeKey(int key) |************************************//*
*Brief:    This function takes input of a key and stores it in the correct position of the array
*Params:
*            key: key to be stored
*Returns:
*            null
********************************************************/
void storeKey(int key) {
    if(key != LASTKEY) {        //Make sure we aren't holding the button down
        NUMS[STORE] = key;      //Store the key in the array
        LASTKEY = key;          //Update last key
        if(STORE == 3) {
            STORE = 0;      //loop back to 0
        }
        else {
            STORE++;
        }
    }
}










