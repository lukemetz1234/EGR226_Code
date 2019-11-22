#include "msp.h"
#include "LCD.h"
#include "keypad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void dispense(int item);
void return_Change(int change);
void redLED(int state);
void greenLED(int state);
void displayWelcome();
void display_main_menu();
void dispense_menu(int selection, int enough, double price);
void TA2_IRQ_Init();
void TA2_N_IRQHandler();
void setupTimerA3();
void TA3_N_IRQHandler();


#define MAIN_MENU       1
#define ADMIN_PASS      2
#define ADMIN_MENU      3
#define TEMP_MENU       4
#define INVENTORY_MENU  5
#define CASH_MENU       6

#define ITEM_1_COST 1.50
#define ITEM_2_COST 0.50
#define ITEM_3_COST 0.25
#define ITEM_4_COST 1.75


uint8_t state = 0;
double cash = 2.75;
int8_t key = -1;
uint8_t count = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	LCDpin_init();
	LCD_init();
	setUpKeypad();

	//Welcome screen
    displayWelcome();
	delay_ms(30000);
	//-------------

    TA2_IRQ_Init();         //TA2 interrupt initialization
    setupTimerA3();
    NVIC_EnableIRQ(PORT5_IRQn);
    __enable_interrupts();

	while(1) {
	    switch(state) {
	        case 0:
	            commandWrite(0x01);   //Clears display
	            display_main_menu();
	            state = MAIN_MENU;
	        break;
	        case MAIN_MENU:
	            if(count == 3) {
	                display_main_menu();
	            }
	            if(key == 1) {
	                if(cash >= ITEM_1_COST) {
	                    dispense_menu(key, 1, ITEM_1_COST);
	                    dispense(1);
	                    return_Change(cash - ITEM_1_COST);
	                    cash = 0;
	                }
	                else {
	                    dispense_menu(key, 0, ITEM_1_COST);
	                }
	            }
	            else if(key == 2) {
	                if(cash >= ITEM_2_COST) {
	                    dispense_menu(key, 1, ITEM_2_COST);
	                    dispense(2);
	                    return_Change(cash - ITEM_2_COST);
	                    cash = 0;
	                }
	                else {
	                    dispense_menu(key, 0, ITEM_2_COST);
	                }
	            }
	            else if(key == 3) {
	                if(cash >= ITEM_3_COST) {
	                    dispense_menu(key, 1, ITEM_3_COST);
	                    dispense(3);
	                    return_Change(cash - ITEM_3_COST);
	                    cash = 0;
	                }
                    else {
                        dispense_menu(key, 0, ITEM_3_COST);
                    }
	            }
	            else if(key == 4) {
	                if(cash >= ITEM_4_COST) {
	                    dispense_menu(key, 1, ITEM_4_COST);
	                    dispense(4);
	                    return_Change(cash - ITEM_4_COST);
	                    cash = 0;
	                }
                    else {
                        dispense_menu(key, 0, ITEM_4_COST);
                    }
	            }
	            else if(key == 11) {    // '*' key
	                state = ADMIN_PASS;
	            }
	        break;
	        case ADMIN_PASS:

	        break;
	        case ADMIN_MENU:

	        break;
            case TEMP_MENU:

            break;
            case INVENTORY_MENU:

            break;
            case CASH_MENU:

            break;
            default:
                state = MAIN_MENU;
            break;

	    }

	}

}


//Dispenses desired item
void dispense(int item) {


}

//Returns correct change
void return_Change(int change) {


}

void redLED(int state) {

}

void greenLED(int state) {

}

void display_main_menu() {
    count = 4;
    char cashBuffer[5] = {0};
    commandWrite(0x01); //Clear display

    sprintf(cashBuffer, "%4.2lf", cash);

    commandWrite(0x80);         //Address  80

    writeString("Cash:  $");
    writeString(cashBuffer);

    commandWrite(0xC0);         //Address  C0
    writeString("Selection: _");

    commandWrite(0xD0);         //Address D0
    writeString("[*]Admin");
    return;
}

void dispense_menu(int selection, int enough, double price) {
    char cashBuffer[5] = {0};
    char changeBuffer[5] = {0};
    char needBuffer[6] = {0};
    char selectionBuffer[1] = {0};

    double totChange = 0.0;
    double totNeed = 0.0;

    totChange = cash - price;
    totNeed = (price - cash);

    sprintf(cashBuffer, "%4.2lf", cash);
    sprintf(changeBuffer, "%4.2lf", totChange);
    sprintf(needBuffer, "%4.2lf", totNeed);
    sprintf(selectionBuffer, "%d", selection);

    commandWrite(0x80);         //Address  80
    writeString("Cash:  $");
    writeString(cashBuffer);
    commandWrite(0xC0);         //Address  C0
    writeString("Selection: ");
    writeString(selectionBuffer);

    if(enough == 1) {
        commandWrite(0x90);
        writeString("Dispensed ");
        commandWrite(0xD0);
        writeString("Change: $");
        writeString(changeBuffer);
    }
    else {
        commandWrite(0x90);
        writeString("Broke bitch");
        commandWrite(0xD0);
        writeString("Need  $");
        writeString(needBuffer);
        writeString("   ");
    }
    count = 0;
}

void displayWelcome() {
    commandWrite(0x86);
    writeString("LUKE");
    commandWrite(0xC5);
    writeString("BRIAN");
    commandWrite(0xD0);
    writeString("VENDING MACHINE!");
}

/***| TA2_IRQ_Init() |************************************//*
*Brief:    This function initializes TA2 as an interrupt
*Params:
*            null
*Returns:
*            null
********************************************************/
void TA2_IRQ_Init()
{
    TIMER_A2->EX0 = 2;                      //divide by 3
    TIMER_A2->CTL = 0b0000001011010110;     // interrupt enable, /8, up mode, SMCLK
    TIMER_A2->CCR[0] = 3000;                  //Very small value so that readKeypress is continuously called
    NVIC_EnableIRQ(TA2_N_IRQn);             //Enable
}

/***| TA2_N_IRQHandler() |************************************//*
*Brief:    This function gets called when there is an interrupt on TA2
*Params:
*            null
*Returns:
*            null
********************************************************/
void TA2_N_IRQHandler()
{
    key = getKey();
    TIMER_A2->CTL &= ~BIT0; //clear interrupt
}

void setupTimerA3()
{
    //Four TimerAs exist, using the first one
    // Set up a 1 second timer
    // bits 15-10 are reserved = 000000
    // bits 9-8 are for clock = 10 (SMCLK)
    // bits 7-6 are for divider of 8 = 11
    // bits 5-4 are for mode, up mode = 01
    // bit 3 is reserved = 0
    // bit 2 is clear count value TAR = 1
    // bit 1 is for interrupt enable = 1
    // bit 0 is for the interrupt flag = 0
    TIMER_A3->CTL = 0b0000001011010110;
    TIMER_A3->EX0 = 0b111; //111 = divide by 8
    TIMER_A3->CCR[0] = 46875; //
    //TIMER_A0->CCTL[0] = 0;

    TIMER_A3->CTL &= ~BIT0;  // Clear flag
    NVIC_EnableIRQ(TA3_N_IRQn);
}

void TA3_N_IRQHandler() {
    count++;
    printf("%d\n", count);
    TIMER_A3->CTL &= ~BIT0;
}

