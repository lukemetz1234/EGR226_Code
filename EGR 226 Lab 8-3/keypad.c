#include "msp.h"
#include "keypad.h"

/***| setUpKeypad() |************************************//*
*Brief:    This function initializes all of the pins that the keypad is connected to. Each
*          pin is set as an input with a pull up resistor.
*Params:
*            null
*Returns:
*            null
********************************************************/
void setUpKeypad() {
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


int read_Keypad() {
    setUpKeypad();        //Calls setUpKeypad to ensure all pins are input, pull up resistor
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
    setUpKeypad();        //Call setUpKeypad again to reset P3.3
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
    setUpKeypad();

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
    setUpKeypad();

    return -1;      //If no button is pressed, return a -1
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
