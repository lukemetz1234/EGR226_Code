/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 10-2 Interfacing the MSP432 with a Temperature Sensor, Converting the Analog
*                    Voltage to Digital Samples, and Displaying on the LCD
* File:              main.c
* Description:       This program reads the current voltage on P5.1 which is connected to a temperature sensor. Based on
*                    the voltage reading, it is converted into degrees and printed out to the user via the console
*                    window.
*******************************************************/

#include "msp.h"
#include <stdio.h>
#include <math.h>

void SysTick_IRQ_Init();
void SysTick_Handler();

double temp = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    P5->SEL0 |= BIT1;       //Set up P5.1
    P5->SEL1 |= BIT1;
    P5->DIR &= ~BIT1;       //Input

    ADC14->CTL0 = 0;                            //Clears the register
    ADC14->CTL0 |= (BIT(26)|BIT(21)|BIT4);      //0b000001000010000000000000001000       //Bit 17 for multiple channels
    ADC14->CTL1 =0b110000;                      //Only Bit 4 and 5 on

    ADC14->MCTL[0] = 4;         //Read A4 (P5.1)
    ADC14->CTL0 |= BIT1;        //Enable conversion ADC14ENC
    ADC14->CTL0 |= BIT0;        //Start conversion


    SysTick_IRQ_Init();     //Initialize systick timer
    __enable_interrupts();  //Enable interrupts

    while(1) {      //Loop forever

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
    if(!(ADC14->CTL0 & BIT(16))) {                                  //Checks for conversion to be complete
        temp = (((ADC14->MEM[0]*(3.3/pow(2,14)))*1000)-500)/10;     //Converts the voltage reading to degrees
        printf("P5.1 = %g\n", temp);            //Prints the temperature reading
        ADC14->CTL0 |= BIT0;                    //Start conversion again, gets turned off once completed
    }
}
