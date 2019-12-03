#include "msp.h"
#include "keypad.h"
#include <stdio.h>

int keyP = -1;
uint8_t reg;

void setUpKeypad();
void PORT5_IRQHandler();
void defaultState();

int getKey() {
    if(reg != 0x00)
        keyP = -1;
    if(keyP != -1) {
        printf("Key = %d\n", keyP);
    }
    reg = reg << 1;
    reg |= 1;
    return keyP;
}

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

void defaultState() {
    P5->DIR &= ~(BIT6|BIT1|BIT2);   //Input
    P5->REN |= (BIT6|BIT1|BIT2);    //Enables internal resistor
    P5->OUT |= (BIT6|BIT1|BIT2);    //Pull up resistor

    P2->DIR |= (BIT4|BIT5|BIT6|BIT7);   //Output
    P2->OUT &= ~(BIT4|BIT5|BIT6|BIT7);    //Output a 0
}

void PORT5_IRQHandler(){
    setUpKeypad();

    if((P5->IN & BIT6) ==  0) {
        //Column 1
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
                    keyP = 11;
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
