#include "msp.h"

void PORT4_IRQHandler();
void pinSetup();

int ONTIME = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    pinSetup();

    while(1) {

    }
}


void pinSetup() {
    P4->SEL0 &= ~(BIT0|BIT1|BIT2);
    P4->SEL1 &= ~(BIT0|BIT1|BIT2);
    P4->DIR &= ~(BIT0|BIT1|BIT2);
    P4->REN |= (BIT0|BIT1|BIT2);    //Enables internal resistor
    P4->OUT |= (BIT0|BIT1|BIT2);    //Pull up resistor
    P4->IE |= (BIT0|BIT1|BIT2);  //Turn on interrupt flag capture

    P2->SEL0 |=  BIT6;  //giving timerA control of P6.7
    P2->SEL1 &= ~BIT6;  //Now control it through TimerA2.4
    P2->DIR  |=  BIT6;

    TIMER_A0->CTL = 0b000001001010100;      //Divide by 8, up mode, SMCLK
    TIMER_A0->EX0 = 0b000;                  //Divide by 1
    TIMER_A0->CCR[0] = 37500;               //Period (500 ms) 1500000/24=62500
    TIMER_A0->CCTL[3] = 0b11100000;         //Reset/Set on TA2.4 (P6.7)
    TIMER_A0->CCR[3] = ONTIME;              //TA2.4 (P6.7)

    NVIC_EnableIRQ(PORT4_IRQn);
    __enable_interrupts();
}


void PORT4_IRQHandler()
{
    if(P4->IFG & BIT0) {
        TIMER_A0->CCR[3] = 0;
        P4->IFG &= ~BIT0;  //clear (Acknowledge) flag
    }

    if(P4->IFG & BIT1) {
        __delay_cycles(15000);
        if(P4->IFG & BIT1) {
            if(TIMER_A0->CCR[3] == 0) {
                TIMER_A0->CCR[3] = 0;
            }
            else {
                TIMER_A0->CCR[3] -= 3750;
            }
        }
        P4->IFG &= ~BIT1;  //clear (Acknowledge) flag
    }

    if(P4->IFG & BIT2) {
        __delay_cycles(15000);
        if(P4->IFG & BIT2) {
            if(TIMER_A0->CCR[3] == 37500) {
                TIMER_A0->CCR[3] = 37500;
            }
            else {
                TIMER_A0->CCR[3] += 3750;
            }
        }
        P4->IFG &= ~BIT2;  //clear (Acknowledge) flag
    }
}
