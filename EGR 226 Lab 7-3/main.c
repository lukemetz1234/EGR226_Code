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

void shift_off(char message[]);
void shift_on(char message[]);

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    LCDpin_init();  //Set up pins connected to the LCD display
    LCD_init();     //Sequence to properly initialize LCD

    char message[15] = {'L', 'A', 'B', 'O', 'R', 'A', 'T', 'O', 'R', 'Y', ' ', 'O', 'V', 'E', 'R'};
    int i = 0;

    for(i = 0; i < 15; i++) {
                dataWrite(message[i]);
                delay_ms(10);
            }
    delay_ms(1000);

    while(1) {
        shift_off(message);
        shift_on(message);
    }

}

void shift_off(char message[])
{
    int j;
    commandWrite(0x0C);  //turns off cursor
    char space[1] = {' '};

    for(j=0; j<15; j++) {
        commandWrite(0x18);  //shifts addresses to the left 15 times
        delay_ms(1000);

        if (j==7) {
            commandWrite(0x80);
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
         }
        if (j==8) {
            commandWrite(0x81);
            delay_ms(100);
            dataWrite(space[0]);
            delay_ms(100);
         }
        if (j==9) {
            commandWrite(0x82);
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
        }
        if (j==10) {
            commandWrite(0x83);
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
        }
        if (j==11) {
            commandWrite(0x84);
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
        }
        if (j==12) {
            commandWrite(0x85);
            delay_ms(100);
            dataWrite(space[0]);
            delay_ms(10);
        }
        if (j==13) {
            commandWrite(0x86);
            delay_ms(10);
            dataWrite(space[0]);
            delay_ms(10);
        }
    }

    commandWrite(0x01);             //clears display
    delay_ms(100);
    commandWrite(0x02);             //returns addresses back to the original positions
    delay_ms(100);
}

void shift_on(char message[])
{
    int i;

    commandWrite(0x01);             //clears DDRAM
    delay_ms(100);

    for (i=0;i<15;i++)              //shifts cursor 15 addresses to the right
    {
        commandWrite(0x1C);         //shifts right
        delay_ms(10);
    }

    delay_ms(100);

    for(i=0; i<15; i++)
    {
        dataWrite(message[i]);  //prints element of message
        delay_ms(990);
        commandWrite(0x18);         //shifts address to the left
        delay_ms(10);
    }
    delay_ms(990);
}
