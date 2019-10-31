#include "msp.h"
#include <stdio.h>
#include <math.h>

void SysTick_IRQ_Init();
void SysTick_Handler();

double temp = 0;

//Pg. 673


/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    P5->SEL0 |= BIT1;
    P5->SEL1 |= BIT1;
    P5->DIR &= ~BIT1;

    ADC14->CTL0 = 0;                                //Clears the register
    ADC14->CTL0 |= (BIT(26)|BIT(21)|BIT4);  //0b000001000010000000000000001000       //Bit 17 for multiple channels
    ADC14->CTL1 =0b110000;                          //Only Bit 4 and 5 on

    ADC14->MCTL[0] = 4;         //Read A4 (P5.1)
    ADC14->CTL0 |= BIT1;        //Enable conversion ADC14ENC
    ADC14->CTL0 |= BIT0;        //Start conversion


    SysTick_IRQ_Init();     //Initialize systick timer
    __enable_interrupts();  //Enable interrupts

    while(1) {


    }
}


/***| SysTick_IRQ_Init() |************************************//*
*Brief:    This function runs as an initialization for the systick timer
*Params:
*            null
*Returns:
*            null
********************************************************/
void SysTick_IRQ_Init()
{
    SysTick->CTRL |= (BIT1|BIT0);   //Enabled as interrupt
    SysTick->VAL = 123;             //clears
    SysTick->LOAD = 6000000;        //One second
    NVIC_EnableIRQ(SysTick_IRQn);   //Enable interrupt
}


/***| SysTick_Handler() |************************************//*
*Brief:    This function runs every time an interrupt on the systick timer is triggered
*Params:
*            null
*Returns:
*            null
********************************************************/
void SysTick_Handler()
{
    if(!(ADC14->CTL0 & BIT(16))) {              //Checks for conversion to be complete
        temp = (((ADC14->MEM[0]*(3.3/pow(2,14)))*1000)-500)/10;
        printf("P5.1 = %g\n", temp);
        ADC14->CTL0 |= BIT0;                    //Start conversion again     gets turned off once completed
    }
}
