#include "msp.h"
#include "LCD.h"
#include "keypad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void dispense(int item);
void return_Change(int change);
void motorPinInit();
void ledPinInit();
void redLED(int state);
void greenLED(int state);
void displayWelcome();
void display_adminPass_menu();
void display_admin_menu();
void display_main_menu();
void readPassword(int keyPress);
void updateCash();
void dispense_menu(int selection, int enough, double price);
void outOfStock_menu();
void print_inventory();
void tempSetup();
void PORT5_IRQHandler();
void setupTimerA1();
void TA1_N_IRQHandler();
void setupTimerA2();
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

uint8_t item1_stock = 2;
uint8_t item2_stock = 3;
uint8_t item3_stock = 1;
uint8_t item4_stock = 5;


uint8_t state = 0;
double cash = 2.75;
double cashInMachine = 0.00;
int8_t key = -1;
uint8_t noKey = 0;
uint8_t motorCount = 0;
uint8_t count = 0;
uint8_t passPosition = 0;
uint8_t blink = 0;
double temp;
char tempBuffer[5] = {0};
char pin[4];
char password[4] = {1, 2, 3, 4};

void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//--------Pin initializations--------------
	LCDpin_init();
	LCD_init();
	ledPinInit();
	motorPinInit();
	setUpKeypad();
	tempSetup();
	//-----------------------------------------

	//-----------Welcome screen----------------
    displayWelcome();
	delay_ms(30000);
	//-----------------------------------------

	setupTimerA1();     //TA1 initialization
	setupTimerA2();     //TA2 initialization
    setupTimerA3();     //TA3 initialization

    NVIC_EnableIRQ(PORT5_IRQn);
    NVIC_EnableIRQ(PORT3_IRQn);
    __enable_interrupts();

    char totalCashBuffer[6] = {0};

	while(1) {
	    switch(state) {
	        case 0:
	            commandWrite(0x01);   //Clears display
	            display_main_menu();
	            state = MAIN_MENU;
	        break;
	        case MAIN_MENU:
	            if(count == 3) {
	                redLED(0);
	                greenLED(0);
	                display_main_menu();
	            }
	            if(key == 1) {
	                if(item1_stock <= 0) {
	                    outOfStock_menu();
	                }
	                else if(cash >= ITEM_1_COST) {
	                    dispense_menu(key, 1, ITEM_1_COST);
	                    dispense(1);
	                    --item1_stock;
	                    return_Change(cash - ITEM_1_COST);
	                    cash = 0;
	                }
	                else {
	                    dispense_menu(key, 0, ITEM_1_COST);
	                }
	            }
	            else if(key == 2) {
                    if(item2_stock <= 0) {
                        outOfStock_menu();
                    }
                    else if(cash >= ITEM_2_COST) {
	                    dispense_menu(key, 1, ITEM_2_COST);
	                    dispense(2);
	                    --item2_stock;
	                    return_Change(cash - ITEM_2_COST);
	                    cash = 0;
	                }
	                else {
	                    dispense_menu(key, 0, ITEM_2_COST);
	                }
	            }
	            else if(key == 3) {
                    if(item3_stock <= 0) {
                        outOfStock_menu();
                    }
                    else if(cash >= ITEM_3_COST) {
	                    dispense_menu(key, 1, ITEM_3_COST);
	                    dispense(3);
	                    --item3_stock;
	                    return_Change(cash - ITEM_3_COST);
	                    cash = 0;
	                }
                    else {
                        dispense_menu(key, 0, ITEM_3_COST);
                    }
	            }
	            else if(key == 4) {
                    if(item4_stock <= 0) {
                        outOfStock_menu();
                    }
                    else if(cash >= ITEM_4_COST) {
	                    dispense_menu(key, 1, ITEM_4_COST);
	                    dispense(4);
	                    --item4_stock;
	                    return_Change(cash - ITEM_4_COST);
	                    cash = 0;
	                }
                    else {
                        dispense_menu(key, 0, ITEM_4_COST);
                    }
	            }
	            else if(key == 10) {    // '*' key
	                blink = 0;
	                state = ADMIN_PASS;
	                display_adminPass_menu();
	            }
	        break;
	        case ADMIN_PASS:
	            if(key == 11) { //Accept (#)
	                passPosition = 0;
	                if((password[0] == pin[0]) && (password[0] == pin[0]) && (password[1] == pin[1]) && (password[2] == pin[2]) && (password[3] == pin[3])) {
	                    display_admin_menu();
	                    memset(pin,0,sizeof(pin));  //Clear password array to 0's
	                    state = ADMIN_MENU;
	                }
	                else {
	                    display_adminPass_menu();
	                }
	            }
	            else if(key == 10) {    //Cancel (*)
	                display_main_menu();
	                state = MAIN_MENU;
	            }
	            else if(key != -1 && key != 11 && key != 10) {
	                readPassword(key);
	            }

	        break;
	        case ADMIN_MENU:
	            if(key == 10) {    //Cancel (*)
	                display_main_menu();
	                state = MAIN_MENU;
	            }
	            else if(key == 1) {
	                commandWrite(0x01);   //Clears display
	                writeString("Admin: Temp");
	                commandWrite(0xC9);
	                dataWrite(0xDF);
	                writeString("C");
	                commandWrite(0xD0);
	                writeString("         [*]Back");
	                state = TEMP_MENU;
	            }
	            else if(key == 2) {
                    commandWrite(0x01);   //Clears display
                    writeString("Admin: Inventory");
                    print_inventory();
                    commandWrite(0xD0);
                    writeString("[#]Stock [*]Back");
	                state = INVENTORY_MENU;
	            }
                else if(key == 3) {
                    sprintf(totalCashBuffer, "$%4.2lf", cashInMachine);
                    commandWrite(0x01);   //Clears display
                    writeString("Admin: Cash");
                    commandWrite(0xC0);
                    writeString(totalCashBuffer);
                    commandWrite(0x90);
                    writeString("[#]Withdraw");
                    commandWrite(0xD0);
                    writeString("[*]Back");
                    state = CASH_MENU;
                }
	        break;
            case TEMP_MENU:
                if(key == 10) {
                    state = ADMIN_MENU;
                    display_admin_menu();
                }
            break;
            case INVENTORY_MENU:
                if(key == 10) {
                    state = ADMIN_MENU;
                    display_admin_menu();
                }
                else if (key == 11) {
                    item1_stock = 5;
                    item2_stock = 5;
                    item3_stock = 5;
                    item4_stock = 5;
                    print_inventory();
                }
            break;
            case CASH_MENU:
                if(key == 10) {
                    state = ADMIN_MENU;
                    display_admin_menu();
                }
            break;
            default:
                state = MAIN_MENU;
            break;
	    }
	}
}


//Dispenses desired item
void dispense(int item) {
    if(item == 1) {
        P4->OUT |= BIT4;
    }
    else if(item == 2) {
        P4->OUT |= BIT5;
    }
    else if(item == 3) {
        P4->OUT |= BIT6;
    }
    else if(item == 4) {
        P4->OUT |= BIT7;
    }
    motorCount = 0;
}

//Returns correct change
void return_Change(int change) {

}

//Turns on red LED
void redLED(int state) {
    if(state == 0) {
        P1->OUT &= ~BIT7;
    }
    else if(state == 1) {
        P1->OUT |= BIT7;
    }
}

//Turns on green LED
void greenLED(int state) {
    if(state == 0) {
        P1->OUT &= ~BIT6;
    }
    else if(state == 1) {
        P1->OUT |= BIT6;
    }
}


void motorPinInit() {
    P4->SEL0 &= ~(BIT4|BIT5|BIT6|BIT7);
    P4->SEL1 &= ~(BIT4|BIT5|BIT6|BIT7);
    P4->DIR |=   (BIT4|BIT5|BIT6|BIT7);     //Output
    P4->OUT &=  ~(BIT4|BIT5|BIT6|BIT7);     //Off
}

void ledPinInit() {
    P1->SEL0 &= ~(BIT6|BIT7);
    P1->SEL1 &= ~(BIT6|BIT7);
    P1->DIR |= (BIT6|BIT7);   //Output
    P1->OUT &= ~(BIT6|BIT7);

    P3->SEL0 &= ~BIT0;  //IR LED
    P3->SEL1 &= ~BIT0;
    P3->DIR &= ~BIT0;   //input
    P3->REN |= BIT0;
    P3->OUT |= BIT0;
    P3->IE |= BIT0;
}

void display_adminPass_menu() {
    passPosition = 0;
    commandWrite(0x01);   //Clears display
    writeString("Admin passcode: ");
    commandWrite(0xC0);
    writeString("____ ");
    commandWrite(0x90);
    writeString("[#]Accept");
    commandWrite(0xD0);
    writeString("[*]Cancel");
}

void readPassword(int keyPress) {
    if(passPosition == 4) {
        passPosition = 0;
    }
    pin[passPosition] = keyPress;
    commandWrite(0xC0 + passPosition);
    writeString("*");
    passPosition++;
}

void display_admin_menu() {
    commandWrite(0x01);   //Clears display
    commandWrite(0x85);
    writeString("Admin");
    commandWrite(0xC0);
    writeString("[1]Temperature");
    commandWrite(0x90);
    writeString("[2]Inventory");
    commandWrite(0xD0);
    writeString("[3]Cash  [*]Back");
}

void display_main_menu() {
    noKey = 0;
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
    blink = 1;
    return;
}

//Updates the cash displayed in the main menu when a coin is detected
void updateCash() {
    char cashBuffer[5] = {0};

    if(state == MAIN_MENU) {
        sprintf(cashBuffer, "%4.2lf", cash);
        commandWrite(0x80);         //Address  80
        writeString("Cash:  $");
        writeString(cashBuffer);
    }
}

void dispense_menu(int selection, int enough, double price) {
    blink = 0;
    noKey = 1;
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
        redLED(0);
        greenLED(1);
        commandWrite(0x90);
        writeString("Dispensed ");
        commandWrite(0xD0);
        writeString("Change: $");
        writeString(changeBuffer);
    }
    else {
        redLED(1);
        greenLED(0);
        commandWrite(0x90);
        writeString("Not enough money");
        commandWrite(0xD0);
        writeString("Need  $");
        writeString(needBuffer);
        writeString("   ");
    }
    count = 0;
}

void outOfStock_menu() {
    blink = 0;
    commandWrite(0x01);     //Clear display
    commandWrite(0x80);
    writeString(" Out of stock!");
    commandWrite(0x90);
    writeString("  Make another");
    commandWrite(0xD0);
    writeString("   selection.");
    count = 0;
}

void print_inventory() {
    char inv1Buffer[16] = {0};
    char inv2Buffer[16] = {0};

    sprintf(inv1Buffer, "%d Reeses %d Twix", item1_stock, item2_stock);
    sprintf(inv2Buffer, "%d M&M    %d York", item3_stock, item4_stock);

    commandWrite(0xC0);
    writeString(inv1Buffer);
    commandWrite(0x90);
    writeString(inv2Buffer);
}

void displayWelcome() {
    commandWrite(0x86);
    writeString("LUKE");
    commandWrite(0xC5);
    writeString("BRIAN");
    commandWrite(0xD0);
    writeString("VENDING MACHINE!");
}

void tempSetup() {
    P5->SEL0 |= BIT5;   //Set up P5.1
    P5->SEL1 |= BIT5;
    P5->DIR &= ~BIT5;   //Input

    ADC14->CTL0 = 0;                            //Clears the register
    ADC14->CTL0 |= (BIT(26)|BIT(21)|BIT4);      //0b000001000010000000000000001000       //Bit 17 for multiple channels
    ADC14->CTL1 =0b110000;                      //Only Bit 4 and 5 on

    ADC14->MCTL[0] = 0;         //Read A0 (P5.5)
    ADC14->CTL0 |= BIT1;        //Enable conversion ADC14ENC
    ADC14->CTL0 |= BIT0;        //Start conversion
}

void PORT3_IRQHandler(){
    if((P3->IN & BIT0) == 0) {
        cash = cash + 0.25;
        updateCash();
    }
    P3->IFG &= ~BIT0;       //clear (Acknowledge) flag
}

void setupTimerA1()
{
    TIMER_A1->CTL = 0b0000001011010110;
    TIMER_A1->EX0 = 0b111; //111 = divide by 8
    TIMER_A1->CCR[0] = 46875; //

    TIMER_A1->CTL &= ~BIT0;  // Clear flag
    NVIC_EnableIRQ(TA1_N_IRQn);
}

void TA1_N_IRQHandler()
{
    if(motorCount == 2) {
        P4->OUT &= ~(BIT4|BIT5|BIT6|BIT7);      //Stop motor
        motorCount = 9;
    }
    printf("M count = %d\n", motorCount);
    ++motorCount;
    TIMER_A1->CTL &= ~BIT0; //clear interrupt
}

/***| setupTimerA2() |************************************//*
*Brief:    This function initializes TA2 as an interrupt
*Params:
*            null
*Returns:
*            null
********************************************************/
void setupTimerA2()
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
    if(noKey == 0) {
        key = getKey();
    }
    else {
        key = -1;
    }
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

    TIMER_A3->CTL &= ~BIT0;  // Clear flag
    NVIC_EnableIRQ(TA3_N_IRQn);
}

void TA3_N_IRQHandler() {
    if(state == TEMP_MENU) {
        if(!(ADC14->CTL0 & BIT(16))) {              //Checks for conversion to be complete
            temp = (((ADC14->MEM[0]*(3.3/pow(2,14)))*1000)-500)/10;     //Convert voltage reading to temp
            sprintf(tempBuffer, "%4.2lf", temp);           //Parses out temp variable into an array
            commandWrite(0xC4);
            writeString(tempBuffer);
            ADC14->CTL0 |= BIT0;                    //Start conversion again     gets turned off once completed
        }
    }
    count = count + 1;
    if(state == MAIN_MENU && blink == 1) {
        if((count %2) == 0) {
            commandWrite(0xCB);
            writeString(" ");
        }
        else {
            commandWrite(0xCB);
            writeString("_");
        }
    }
    printf("%d\n", count);

    TIMER_A3->CTL &= ~BIT0;
}

