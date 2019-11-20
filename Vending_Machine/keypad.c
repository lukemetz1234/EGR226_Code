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
    P6->SEL0 &= ~(BIT6|BIT7);  //Set up pins P6.4 and P6.5
    P6->SEL1 &= ~(BIT6|BIT7);
    P6->DIR &= ~(BIT6|BIT7);   //Input
    P6->REN |= (BIT6|BIT7);    //Enables internal resistor
    P6->OUT |= (BIT6|BIT7);    //Pull up resistor

    P5->SEL0 &= ~(BIT6|BIT1);  //Set up pins P4.1, P4.3, and P4.6
    P5->SEL1 &= ~(BIT6|BIT1);
    P5->DIR &= ~(BIT6|BIT1);   //Input
    P5->REN |= (BIT6|BIT1);    //Enables internal resistor
    P5->OUT |= (BIT6|BIT1);    //Pull up resistor

    P2->SEL0 &= ~BIT3;  //Set up pin P1.5
    P2->SEL1 &= ~BIT3;
    P2->DIR &= ~BIT3;   //Input
    P2->REN |= BIT3;    //Enables internal resistor
    P2->OUT |= BIT3;    //Pull up resistor

    P3->SEL0 &= ~(BIT5|BIT7);  //Set up pin P3.3
    P3->SEL1 &= ~(BIT5|BIT7);
    P3->DIR &= ~(BIT5|BIT7);   //Input
    P3->REN |= (BIT5|BIT7);    //Enables internal resistor
    P3->OUT |= (BIT5|BIT7);    //Pull up resistor
}


int read_Keypad() {
    setUpKeypad();        //Calls setUpKeypad to ensure all pins are input, pull up resistor
    __delay_cycles(30000);   //Small delay to ensure the pins are properly set

    P3->DIR |= BIT7;    //Set P3.3 to an output
    P3->OUT &= ~BIT7;   //Output a 0
    __delay_cycles(30000);       //Delay to ensure pin state change
    if((P2->IN & BIT3) == 0) {          //Check for the pound key being pressed
        while((P2->IN & BIT3) == 0);
        return 10; //pound
    }
    if((P6->IN & BIT7) == 0) {          //Check for 9 button
        while((P6->IN & BIT7) == 0);
        return 9;
    }
    if((P6->IN & BIT6) == 0) {          //Check for 6 button
        while((P6->IN & BIT6) == 0);
        return 6;
    }
    if((P5->IN & BIT6) == 0) {          //Check for 3 button
        while((P5->IN & BIT6) == 0);
        return 3;
    }
    setUpKeypad();        //Call setUpKeypad again to reset P3.3
    __delay_cycles(30000);

    P3->DIR |= BIT5;    //Set P4.1 as an output
    P3->OUT &= ~BIT5;   //Output a 0
    delay_ms(10);
    if((P2->IN & BIT3) == 0) {          //Check for 0 button
        while((P2->IN & BIT3) == 0);
        return 0;
    }
    if((P6->IN & BIT7) == 0) {          //Check for 8 button
        while((P6->IN & BIT7) == 0);
        return 8;
    }
    if((P6->IN & BIT6) == 0) {          //Check for 5 button
        while((P6->IN & BIT6) == 0);
        return 5;
    }
    if((P5->IN & BIT6) == 0) {          //Check for 2 buton
        while((P5->IN & BIT6) == 0);
        return 2;
    }
    setUpKeypad();

    P5->DIR |= BIT1;    //Set up P4.3 as an output
    P5->OUT &= ~BIT1;   //Output a 0
    __delay_cycles(30000);

    if((P2->IN & BIT3) == 0) {          //Check for * button
        while((P2->IN & BIT3) == 0);
        return 11;  //*
    }
    if((P6->IN & BIT7) == 0) {          //Check for 7 button
        while((P6->IN & BIT7) == 0);
        return 7;
    }
    if((P6->IN & BIT6) == 0) {          //Check for 4 button
        while((P6->IN & BIT6) == 0);
        return 4;
    }
    if((P5->IN & BIT6) == 0) {          //Check for 1 button
        while((P5->IN & BIT6) == 0);
        return 1;
    }
    setUpKeypad();

    return -1;      //If no button is pressed, return a -1
}
