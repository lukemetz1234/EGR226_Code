/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 10-3 Interfacing the MSP432 with a Temperature Sensor, Converting the Analog
*                    Voltage to Digital Samples, and Displaying on the LCD
* File:              main.c
* Description:       This program reads the current voltage on P5.1 which is connected to a temperature sensor. Based on
*                    the voltage reading, it is converted into degrees and printed out to the user via the LCD display
*                    used in a previous lab. A button is interfaced to allow for switching between degrees C and
*                    degrees F.
*******************************************************/

#include "msp.h"
#include <stdio.h>
#include <math.h>
#include "LCD.h"

void SysTick_IRQ_Init();
void SysTick_Handler();

double temp = 0;
char buffer[4];
int FLAG = 0;       //Indicates degrees C or degrees F

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    LCDpin_init();      //Set up LCD display
    LCD_init();

    P5->SEL0 |= BIT1;   //Set up P5.1
    P5->SEL1 |= BIT1;
    P5->DIR &= ~BIT1;   //Input

    P2->SEL0 &= ~BIT6;  //Button on P2.6
    P2->SEL1 &= ~BIT6;
    P2->DIR &= ~BIT6;   //Input
    P2->REN |= BIT6;    //Enables internal resistor
    P2->OUT |= BIT6;    //Pull up resistor
    P2->IE |= BIT6;     //Turn on interrupt flag capture

    ADC14->CTL0 = 0;                            //Clears the register
    ADC14->CTL0 |= (BIT(26)|BIT(21)|BIT4);      //0b000001000010000000000000001000       //Bit 17 for multiple channels
    ADC14->CTL1 =0b110000;                      //Only Bit 4 and 5 on

    ADC14->MCTL[0] = 4;         //Read A4 (P5.1)
    ADC14->CTL0 |= BIT1;        //Enable conversion ADC14ENC
    ADC14->CTL0 |= BIT0;        //Start conversion

    NVIC_EnableIRQ(PORT2_IRQn);     //Enable interrupts
    __enable_interrupts();

    char m[16] = {'C', 'U', 'R', 'R', 'E', 'N', 'T', ' ', 'T', 'E', 'M', 'P', '.', ' ', 'i', 's'};

    int i;
    for(i = 0; i < 16; i++) {
        dataWrite(m[i]);        //Print to LCD display "CURRENT TEMP. is"
    }
    commandWrite(0x0C);         //Turn off cursor

    while(1) {
        if(!(ADC14->CTL0 & BIT(16))) {              //Checks for conversion to be complete
            if(FLAG == 0) {                                                 //Degrees C
                temp = (((ADC14->MEM[0]*(3.3/pow(2,14)))*1000)-500)/10;     //Convert voltage reading to temp
            }
            else if(FLAG == 1) {                                            //Degrees F
                temp = (((ADC14->MEM[0]*(3.3/pow(2,14)))*1000)-500)/10;     //Convert voltage reading to temp
                temp = ((temp * (1.8)) + 32);                               //Convert to degrees F
            }
                sprintf(buffer, "%lf", temp);           //Parses out temp variable into an array
                ADC14->CTL0 |= BIT0;                    //Start conversion again     gets turned off once completed

                int j = 0;
                commandWrite(0xC5);             //Move cursor to middle of second row
                for(j = 0; j < 4; j++) {
                    dataWrite(buffer[j]);       //Print the temperature reading on the display
                }
                dataWrite(0xDF);
                if(FLAG == 0)
                    dataWrite('C');             //Check for which degrees and print accordingly
                else
                    dataWrite('F');
            }
    }
}

/***| PORT2_IRQHandler() |************************************//*
*Brief:    This function runs every time an interrupt on port 2 is triggered
*Params:
*            null
*Returns:
*            null
********************************************************/
void PORT2_IRQHandler()
{
    if(P2->IFG & BIT6) {            //Check for button press on P2.6
        __delay_cycles(6000);       //Delay for debounce
        if(P2->IFG & BIT6) {        //Check again
            if(FLAG == 0) {
                FLAG = 1;           //Toggle between C and F
            }
            else {
                FLAG = 0;
            }
            P2->IFG &= ~BIT6;       //clear (Acknowledge) flag
        }
    }
}
