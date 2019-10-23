#include "msp.h"

void pinSetup();
void writeSeg(int seg);
void SysTick_IRQ_Init();
void SysTick_Handler();

int i = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	pinSetup();
	SysTick_IRQ_Init();
	__enable_interrupts();

	while(1) {
	}
}


void pinSetup() {
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

void SysTick_IRQ_Init()
{
    SysTick->CTRL |= (BIT1|BIT0);  //Enabled as interrupt
    SysTick->VAL = 123;
    SysTick->LOAD = 3000000;
    NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler()
{
    if(i == 10) {
        i = 0;
    }
    writeSeg(i);
    i++;
}


