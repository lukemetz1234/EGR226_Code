#include "msp.h"


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer


	//------| 1 kHz, 50% duty cycle PWM on P6.6 |-----------------

    P6->SEL0 |=  BIT6;  //giving timerA control of P6.6
    P6->SEL1 &= ~BIT6;  //Now control it through TimerA2.3
    P6->DIR  |=  BIT6;

    TIMER_A2->CTL = 0b000001000010100;  //Divide by 1, up mode, SMCLK
    TIMER_A2->EX0 = 0b000;              //Divide by 1
    TIMER_A2->CCR[0] = 3000;            //Period (1 ms)
    TIMER_A2->CCTL[3] = 0b11100000;     //Reset/Set on TA2.3 (P6.6)
    TIMER_A2->CCR[3] = 1500;            //TA2.3 (P6.6)

    //------------------------------------------------------------



    //------| 100 kHz, 10% duty cycle PWM P5.6 |-----------------

    P5->SEL0 |=  BIT6;  //giving timerA control of P5.6
    P5->SEL1 &= ~BIT6;  //Now control it through TimerA2.1
    P5->DIR  |=  BIT6;

    TIMER_A2->CTL = 0b000001000010100;  //Divide by 1, up mode, SMCLK
    TIMER_A2->EX0 = 0b000;              //Divide by 1
    TIMER_A2->CCR[0] = 300;             //Period (.01 ms)
    TIMER_A2->CCTL[1] = 0b11100000;     //Reset/Set on TA2.1 (P5.6)
    TIMER_A2->CCR[1] = 30;              //TA2.1 (P5.6)

    //------------------------------------------------------------



    //------| 700 ms period, 7 ms on time PWM on P8.2 |-----------------

    P8->SEL0 |=  BIT2;  //giving timerA control of P8.2
    P8->SEL1 &= ~BIT2;  //Now control it through TimerA3.2
    P8->DIR  |=  BIT2;

    TIMER_A3->CTL = 0b000001011010100;  //Divide by 8, up mode, SMCLK
    TIMER_A3->EX0 = 0b100;              //Divide by 5
    TIMER_A3->CCR[0] = 52500;           //Period (700 ms)
    TIMER_A3->CCTL[2] = 0b11100000;     //Reset/Set on TA3.2 (P5.6)
    TIMER_A3->CCR[2] = 525;             //TA3.2 (P5.6)

    //------------------------------------------------------------



    //------| 20 ms period, 2.5 ms on time PWM on P2.7 |-----------------

    P2->SEL0 |=  BIT7;  //giving timerA control of P2.7
    P2->SEL1 &= ~BIT7;  //Now control it through TimerA0.4
    P2->DIR  |=  BIT7;

    TIMER_A0->CTL = 0b000001000010100;  //Divide by 1, up mode, SMCLK
    TIMER_A0->EX0 = 0b000;              //Divide by 1
    TIMER_A0->CCR[0] = 60000;           //Period (1 ms)
    TIMER_A0->CCTL[4] = 0b11100000;     //Reset/Set on TA0.4 (P2.7)
    TIMER_A0->CCR[4] = 7500;            //TA0.4 (P2.7)

    //------------------------------------------------------------

}


