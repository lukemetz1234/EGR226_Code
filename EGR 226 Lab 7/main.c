/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 07 Interfacing the MSP432 with a Liquid Crystal Display (LCD)
* File:              main.c
* Description:       This program initializes the proper pins for an LCD display and runs a sequence to set up the display.
*                    Each group members name is printed to the display, the first name on the top line and the second name on the
*                    line below. EGR is printed in the line below that, followed by 226 on the very bottom line. Each segment
*                    printed to the display is centered.
*******************************************************/

#include "msp.h"
#include "LCD.h"


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	LCDpin_init();  //Set up pins connected to the LCD display
	LCD_init();     //Sequence to properly initialize LCD

	char name1[4] = {'L', 'U', 'K', 'E'};       //Partner 1 name
	char name2[5] = {'B', 'R', 'I', 'A', 'N'};  //Partner 2 name
	char class[3] = {'E', 'G', 'R'};            //Course
	char classNum[3] = {'2', '2', '6'};         //Course number

	int k = 0;

	commandWrite(0x86);         //Address  86
	for(k = 0; k < 4; k++) {
	    dataWrite(name1[k]);    //Write each letter of LUKE
	    delay_ms(10);
	}

	commandWrite(0xC5);         //Address  C5
	for(k = 0; k < 5; k++) {
	    dataWrite(name2[k]);    //Write each letter of BRIAN
	    delay_ms(10);
	}

	commandWrite(0x96);         //Address 96
    for(k = 0; k < 3; k++) {
        dataWrite(class[k]);     //Write each letter of EGR
        delay_ms(10);
    }

    commandWrite(0xD6);         //Address D6
    for(k = 0; k < 3; k++) {
        dataWrite(classNum[k]);  //Write each number of 226
        delay_ms(10);
    }

    commandWrite(0x0C);     //Turn off cursor
}


