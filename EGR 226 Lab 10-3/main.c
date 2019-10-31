#include "msp.h"
#include <stdio.h>
#include <math.h>
#include "LCD.h"

void SysTick_IRQ_Init();
void SysTick_Handler();

double temp = 0;
char buffer[4];
int FLAG = 0;

//Pg. 673


/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    LCDpin_init();
    LCD_init();

    P5->SEL0 |= BIT1;
    P5->SEL1 |= BIT1;
    P5->DIR &= ~BIT1;

    P2->SEL0 &= ~BIT6;
    P2->SEL1 &= ~BIT6;
    P2->DIR &= ~BIT6;
    P2->REN |= BIT6;    //Enables internal resistor
    P2->OUT |= BIT6;    //Pull up resistor
    P2->IE |= BIT6;     //Turn on interrupt flag capture

    ADC14->CTL0 = 0;                                //Clears the register
    ADC14->CTL0 |= (BIT(26)|BIT(21)|BIT4);  //0b000001000010000000000000001000       //Bit 17 for multiple channels
    ADC14->CTL1 =0b110000;                          //Only Bit 4 and 5 on

    ADC14->MCTL[0] = 4;         //Read A4 (P5.1)
    ADC14->CTL0 |= BIT1;        //Enable conversion ADC14ENC
    ADC14->CTL0 |= BIT0;        //Start conversion

    NVIC_EnableIRQ(PORT2_IRQn);     //Enable interrupts
    __enable_interrupts();

    char m[16] = {'C', 'U', 'R', 'R', 'E', 'N', 'T', ' ', 'T', 'E', 'M', 'P', '.', ' ', 'i', 's'};

    int i;
    for(i = 0; i < 16; i++) {
        dataWrite(m[i]);
    }
    commandWrite(0x0C);     //Turn off cursor


    while(1) {

        if(!(ADC14->CTL0 & BIT(16))) {              //Checks for conversion to be complete
            if(FLAG == 0) {
                temp = (((ADC14->MEM[0]*(3.3/pow(2,14)))*1000)-500)/10;
            }
            else if(FLAG == 1) {
                temp = (((ADC14->MEM[0]*(3.3/pow(2,14)))*1000)-500)/10;
                temp = ((temp * (1.8)) + 32);
            }
                sprintf(buffer, "%lf", temp);
                ADC14->CTL0 |= BIT0;                    //Start conversion again     gets turned off once completed

                int j = 0;
                commandWrite(0xC5);
                for(j = 0; j < 4; j++) {
                    dataWrite(buffer[j]);
                }
                dataWrite(0xDF);
                if(FLAG == 0)
                    dataWrite('C');
                else
                    dataWrite('F');
            }
    }
}

void PORT2_IRQHandler()
{
    if(P2->IFG & BIT6) {
        __delay_cycles(6000);
        if(P2->IFG & BIT6) {        //Check for stop button
            if(FLAG == 0) {
                FLAG = 1;
            }
            else {
                FLAG = 0;
            }
            P2->IFG &= ~BIT6;  //clear (Acknowledge) flag
        }
    }
}
