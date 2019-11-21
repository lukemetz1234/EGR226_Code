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
double cash = 0.75;
int8_t key = -1;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	LCD_init();
	setUpKeypad();

	//Welcome screen
    displayWelcome();
	delay_ms(3000);
	//-------------

    TA2_IRQ_Init();         //TA2 interrupt initialization
    NVIC_EnableIRQ(PORT5_IRQn);
    __enable_interrupts();

	while(1) {
	    switch(state) {
	        case 0:
	            commandWrite(0x01);   //Clears display
	            state = MAIN_MENU;
	        break;
	        case MAIN_MENU:
	            //display_main_menu();
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
    char str1[8] = {'C', 'a', 's', 'h', ':', ' ', ' ', '$'};
    char str2[12] = {'S', 'e', 'l', 'e', 'c', 't', 'i', 'o', 'n', ':', ' ', '_'};
    char str3[8] = {'[', '*', ']', 'A', 'd', 'm', 'i', 'n'};
    char cashBuffer[5] = {0};

    //sprintf(cashBuffer, "%4.2lf/0", cash);

    int k = 0;
    commandWrite(0x80);         //Address  80

    for(k = 0; k < 8; k++) {
        dataWrite(str1[k]);
        delay_us(1);
    }
    for(k = 0; k < 4; k++) {
        dataWrite(cashBuffer[k]);
        delay_us(1);
    }

    commandWrite(0xC0);         //Address  C0
    for(k = 0; k < 12; k++) {
        dataWrite(str2[k]);
        delay_us(1);
    }

    commandWrite(0xD0);         //Address D0
    for(k = 0; k < 8; k++) {
        dataWrite(str3[k]);
        delay_us(1);
    }
    return;
}

void dispense_menu(int selection, int enough, double price) {
    double totChange = 0.0;
    double totNeed = 0.0;

    totChange = cash - price;
    totNeed = (price - cash);

    char dispMenu[65] = {0};

    if(enough == 1) {
        sprintf(dispMenu, "Cash:  $%4.2lf     Selection: %d    Dispensed       Change: %4.2lf", cash, key, totChange);
        strcpy(nextDisplay, dispMenu);
    }
    else {
        sprintf(dispMenu, "Cash:  $%4.2lf     Selection: %d", cash, key);
        strcpy(nextDisplay, dispMenu);
    }
}

void displayWelcome() {
    commandWrite(0x02);
    char name1[4] = {'L', 'U', 'K', 'E'};       //Partner 1 name
    char name2[5] = {'B', 'R', 'I', 'A', 'N'};  //Partner 2 name
    char vend[16] = {'V', 'E', 'N', 'D', 'I', 'N', 'G', ' ', 'M', 'A', 'C', 'H', 'I', 'N', 'E', '!'};

    strcpy(nextDisplay, "    Luke         Brian       Vending Machine!");
    /*
    int k = 0;
    commandWrite(0x86);         //Address  86

    for(k = 0; k < 4; k++) {
        dataWrite(name1[k]);    //Write each letter of LUKE
        delay_ms(1);
    }

    commandWrite(0xC5);         //Address  C5
    for(k = 0; k < 5; k++) {
        dataWrite(name2[k]);    //Write each letter of BRIAN
        delay_ms(1);
    }

    commandWrite(0xD0);         //Address D0
    for(k = 0; k < 16; k++) {
        dataWrite(vend[k]);     //Write each letter of VENDING MACHINE
        delay_ms(1);
    }
*/
    commandWrite(0x0C);     //Turn off cursor
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
