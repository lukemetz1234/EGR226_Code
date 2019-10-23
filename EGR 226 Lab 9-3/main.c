#include "msp.h"

void PORT4_IRQHandler();
void pinSetup();
void writeSeg(int seg);

int i = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    pinSetup();
    writeSeg(0);

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

    P4->SEL0 &= ~(BIT3|BIT4|BIT5|BIT6|BIT7);
    P4->SEL1 &= ~(BIT3|BIT4|BIT5|BIT6|BIT7);
    P4->DIR |= (BIT3|BIT4|BIT5|BIT6|BIT7);
    P4->OUT &= ~(BIT3|BIT4|BIT5|BIT6|BIT7);

    P1->SEL0 &= ~BIT5;
    P1->SEL1 &= ~BIT5;
    P1->DIR |= BIT5;
    P1->OUT &= ~BIT5;

    P5->SEL0 &= ~(BIT4|BIT5);
    P5->SEL1 &= ~(BIT4|BIT5);
    P5->DIR |= (BIT4|BIT5);
    P5->OUT &= ~(BIT4|BIT5);

    NVIC_EnableIRQ(PORT4_IRQn);
    __enable_interrupts();
}


void PORT4_IRQHandler()
{
    if(P4->IFG & BIT1) {
        __delay_cycles(30000);
        if(P4->IFG & BIT1) {
            if(i == 0) {
                i = 9;
            }
            else {
                i--;
            }
        }
        P4->IFG &= ~BIT1;  //clear (Acknowledge) flag
    }

    if(P4->IFG & BIT2) {
        __delay_cycles(30000);
        if(P4->IFG & BIT2) {
            if(i == 9) {
                i = 0;
            }
            else {
                i++;
            }
        }
        P4->IFG &= ~BIT2;  //clear (Acknowledge) flag
    }
    writeSeg(i);
}

void writeSeg(int seg) {
    switch (seg) {
    case 0:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= (BIT4|BIT5);
        break;
    case 1:
        pinSetup();
        P4->OUT |= (BIT7|BIT5);
        break;

    case 2:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6);
        P5->OUT |= (BIT4|BIT5);
        break;
    case 3:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6|BIT7);
        P5->OUT |= BIT4;
        break;
    case 4:
        pinSetup();
        P4->OUT |= (BIT5|BIT6|BIT7);
        P1->OUT |= BIT5;
        break;
    case 5:
        pinSetup();
        P4->OUT |= (BIT4|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= BIT4;
        break;
    case 6:
        pinSetup();
        P4->OUT |= (BIT4|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= (BIT4|BIT5);
        break;
    case 7:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT7);
        break;
    case 8:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= (BIT4|BIT5);
        break;
    case 9:
        pinSetup();
        P4->OUT |= (BIT4|BIT5|BIT6|BIT7);
        P1->OUT |= BIT5;
        P5->OUT |= BIT4;
        break;
    }
}
