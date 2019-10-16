#include "msp.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int ONTIME = 37500;

    P2->SEL0 |=  BIT6;  //giving timerA control of P6.7
    P2->SEL1 &= ~BIT6;  //Now control it through TimerA2.4
    P2->DIR  |=  BIT6;

    TIMER_A0->CTL = 0b000001001010100;      //Divide by 8, up mode, SMCLK
    TIMER_A0->EX0 = 0b000;                  //Divide by 1
    TIMER_A0->CCR[0] = 37500;               //Period (500 ms) 1500000/24=62500
    TIMER_A0->CCTL[3] = 0b11100000;         //Reset/Set on TA2.4 (P6.7)
    TIMER_A0->CCR[3] = ONTIME;                //TA2.4 (P6.7)


    while(1) {
        TIMER_A0->CCR[3] = ONTIME;                //TA2.4 (P6.7)
    }

}

