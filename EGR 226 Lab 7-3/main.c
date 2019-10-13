/*******************************************************
* Name:              Luke Metz, Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Lab 07-3 Interfacing the MSP432 with a Liquid Crystal Display (LCD)
* File:              main.c
* Description:       This program initializes the proper pins for an LCD display and runs a sequence to set up the display.
*                    The message "LABORATORY OVER" is printed across the top of the display, and then shifted from right
*                    to left one character per second until the display is completely empty. It then shifts the message
*                    back onto the display right to left one character at a time, repeating this sequence forever.
*******************************************************/

#include "msp.h"
#include "LCD.h"

//---Function declarations---
void shift_off(char message[]);
void shift_on(char message[]);

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    LCDpin_init();  //Set up pins connected to the LCD display
    LCD_init();     //Sequence to properly initialize LCD

    char message[15] = {'L', 'A', 'B', 'O', 'R', 'A', 'T', 'O', 'R', 'Y', ' ', 'O', 'V', 'E', 'R'}; //Message to be printed
    int i = 0;

    for(i = 0; i < 15; i++) {   //Loops through every character
        dataWrite(message[i]);  //Write message out to display initially
        delay_ms(10);
    }

    delay_ms(1000);

    while(1) {
        shift_off(message);     //Shift the message off to the left
        shift_on(message);      //Shift the message on from the right
    }
}


/***| shift_off(char message[]) |************************************//*
*Brief:    This function shifts the message off of the display to the left, shifting by one character
*          every second.
*Params:
*            message: message to be printed to the display and shifted
*Returns:
*            null
********************************************************/
void shift_off(char message[])
{
    int j;
    commandWrite(0x0C);     //Turns off cursor
    char space[1] = {' '};  //Holds space character

    for(j=0; j<15; j++) {
        commandWrite(0x18);     //Shifts addresses to the left
        delay_ms(1000);         //Delay one second

/*The following if statements are used to replace characters with spaces after they shift off of the display.
* Once the message has shifted left enough address spaces, it begins printing on from the left on the third line
* of the display. These if statements simply cover those characters with spaces so that they are not seen. */

        if (j==7) {
            commandWrite(0x80);     //Replace address 80 with a space
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
         }
        if (j==8) {
            commandWrite(0x81);     //Replace address 81 with a space
            delay_ms(100);
            dataWrite(space[0]);
            delay_ms(100);
         }
        if (j==9) {
            commandWrite(0x82);     //Replace address 82 with a space
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
        }
        if (j==10) {
            commandWrite(0x83);     //Replace address 83 with a space
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
        }
        if (j==11) {
            commandWrite(0x84);     //Replace address 84 with a space
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
        }
        if (j==12) {
            commandWrite(0x85);     //Replace address 85 with a space
            delay_ms(100);
            dataWrite(space[0]);
            delay_ms(10);
        }
        if (j==13) {
            commandWrite(0x86);     //Replace address 86 with a space
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
        }
    }

    commandWrite(0x01);             //Clears the display
    delay_ms(100);
    commandWrite(0x02);             //Returns addresses back to their original positions
    delay_ms(100);
}


/***| shift_on(char message[]) |************************************//*
*Brief:    This function shifts the message on to the display from the left, shifting by one character
*          every second.
*Params:
*            message: message to be printed to the display and shifted
*Returns:
*            null
********************************************************/
void shift_on(char message[])
{
    int i;

    commandWrite(0x01);             //Clears DDRAM
    delay_ms(100);

    for (i=0;i<15;i++) {            //Shifts cursor 15 addresses to the right
        commandWrite(0x1C);         //Shift right
        delay_ms(10);
    }

    delay_ms(100);

    for(i=0; i<15; i++) {           //Loops 15 times to print every character
        dataWrite(message[i]);      //Prints one character at a time
        delay_ms(990);
        commandWrite(0x18);         //Shifts all addresses one to the left
        delay_ms(10);
    }
    delay_ms(990);
}
