#include "msp.h"
#include "LCD.h"


/******************************
Added in Lecture (Not finished)
char display[65];           //what to put on the display
char currentDisplay[65];    //Currently on it
******************************/



void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	LCDpin_init();
	LCD_init();

	char name1[4] = {'L', 'U', 'K', 'E'};        //Put whatever you want to print in a character array with the correct length
	char name2[5] = {'B', 'R', 'I', 'A', 'N'};
	char class[3] = {'E', 'G', 'R'};
	char classNum[3] = {'2', '2', '6'};

	int k = 0;

	commandWrite(0x86);                 //address  06
	for(k = 0; k < 4; k++) {
	    dataWrite(name1[k]);             //Write each letter
	    delay_ms(10);
	}

	commandWrite(0xC5);
	for(k = 0; k < 5; k++) {
	    dataWrite(name2[k]);             //Write each letter
	    delay_ms(10);
	}

	commandWrite(0x96);
    for(k = 0; k < 3; k++) {
        dataWrite(class[k]);             //Write each letter
        delay_ms(10);
    }

    commandWrite(0xD6);
    for(k = 0; k < 3; k++) {
        dataWrite(classNum[k]);             //Write each letter
        delay_ms(10);
    }

    commandWrite(0x0C);
}


