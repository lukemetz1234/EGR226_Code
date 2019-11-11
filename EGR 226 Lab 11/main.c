#include "msp.h"
#include <stdio.h>

/**
 * main.c
 */

uint16_t firstVal,secondVal,period;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int f = 0;

    //Capture input on P2.4
    P2->SEL0 |=  BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR  &= ~BIT4;

    P2->SEL0 &= ~(BIT0|BIT2);
    P2->SEL1 &= ~(BIT0|BIT2);
    P2->DIR  |= (BIT0|BIT2);
    P2->OUT &= ~(BIT0|BIT2);

    TIMER_A0->CTL = 0b1000100100;  //SMCLK, Continuous mode, divide by 1
    TIMER_A0->EX0 = 5;  //Divide by 6
    TIMER_A0->CCTL[1] = 0b0100100100010000;  //Capture Rising Edge, Synchronous, Interrupt

    NVIC_EnableIRQ(TA0_N_IRQn);
    __enable_interrupts();

    while(1){
        f = ((3000000/6)/period) + 1;    //Add one for some reason

        if(f == 10) {
            P2->OUT |= BIT2;
            P2->OUT &= ~BIT0;
        }
        else if(f == 14) {
            P2->OUT |= BIT0;
            P2->OUT &= ~BIT2;
        }
        else {
            P2->OUT &= ~BIT2;
            P2->OUT &= ~BIT0;
        }

        printf("frequency = %d Hz\n",f);
        __delay_cycles(1500000);
    }
}

void TA0_N_IRQHandler()
{
    if(TIMER_A0->CCTL[1] & BIT0)
    {
        firstVal = secondVal;
        secondVal = TIMER_A0->CCR[1];
        period = (secondVal - firstVal);
    }
    TIMER_A0->CCTL[1] &= ~(BIT1|BIT0);
}
