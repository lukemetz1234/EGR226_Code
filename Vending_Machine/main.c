/*******************************************************
* Names:             Luke Metz
*                    Brian Koelzer
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Final Project, fall 2019
* Title:             Vending Machine
* Revision date:     12-05-2019
* Description:       This program runs a vending machine controlled by the MSP432 micro controller. The vending machine
*                    has 4 selectable items, which are selected by a keypad. The keypad is also used to traverse through the
*                    various menus of the display. A 4 line LCD display is used to display various information to the user.
*                    Two LED's are used to indicate if an item can be vended or not, and an IR LED is used to detect a coin
*                    being entered. Through an admin menu (which requires a password to be entered) the user can display
*                    a temperature reading, restock the inventory values, or withdraw all of the cash in the machine. An
*                    arduino was added in order to control a servo motor to dispense change back to the user.
*To fix:             -Change return synchronization.
*                    -Add something to detect when an item has been dropped and stop the motor.
*******************************************************/

#include "msp.h"
#include "LCD.h"
#include "keypad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//----Functions---------------
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
void PB_S_Init();
void PORT6_IRQHandler();
void print_inventory();
void tempSetup();
void PORT5_IRQHandler();
void setupTimerA1();
void TA1_N_IRQHandler();
void setupTimerA2();
void TA2_N_IRQHandler();
void setupTimerA3();
void TA3_N_IRQHandler();
//--------------------------

//--State machine states---
#define MAIN_MENU       1
#define ADMIN_PASS      2
#define ADMIN_MENU      3
#define TEMP_MENU       4
#define INVENTORY_MENU  5
#define CASH_MENU       6
//-------------------------

//------Item costs---------
#define ITEM_1_COST 1.00
#define ITEM_2_COST 1.50
#define ITEM_3_COST 1.25
#define ITEM_4_COST 1.00
//-------------------------

//------Item stocks--------
uint8_t item1_stock = 5;
uint8_t item2_stock = 5;
uint8_t item3_stock = 5;
uint8_t item4_stock = 5;
//-------------------------

//-------Global Variables----------
uint8_t state = 0;
double cash = 0;
double cashInMachine = 0.00;
int8_t returnCounter = 0;
int8_t key = -1;
uint8_t noKey = 0;
uint8_t motorCount = 0;
uint8_t count = 0;
uint8_t passPosition = 0;
uint8_t blink = 0;
double temp;
char tempBuffer[5] = {0};
char pin[6];
char password[6] = {1, 2, 3, 4, 5, 6};
//---------------------------------


void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//--------Pin initializations--------------
	LCDpin_init();
	LCD_init();
	ledPinInit();
	motorPinInit();
	setUpKeypad();
	tempSetup();
	PB_S_Init();
	//-----------------------------------------

	//-----------Welcome screen----------------
    displayWelcome();
	delay_ms(30000);
	//-----------------------------------------

	setupTimerA1();     //TA1 initialization
	setupTimerA2();     //TA2 initialization
    setupTimerA3();     //TA3 initialization

    NVIC_EnableIRQ(PORT5_IRQn); //Enable interrupts on port 5
    NVIC_EnableIRQ(PORT3_IRQn); //Enable interrupts on port 3
    NVIC_EnableIRQ(PORT6_IRQn); //Enable interrupts on port 6
    __enable_interrupts();

    char totalCashBuffer[6] = {0};  //Stores current cash in machine for printing to LCD


	while(1) {
	    switch(state) {     //STATE MACHINE
	        case 0:                 //"reset" state
	            commandWrite(0x01);     //Clears display
	            display_main_menu();    //Display the main menu
	            state = MAIN_MENU;      //Enter main menu state
	        break;
	        case MAIN_MENU:         //Main menu
	            if(count == 3) {    //Checks for three seconds passing
	                redLED(0);      //Turn off LED's
	                greenLED(0);
	                display_main_menu();    //Return to the main menu
	            }
	            if(key == 1) {          //Selection 1
	                if(item1_stock <= 0) {      //Check for out of stock
	                    redLED(1);              //Turn on red LED
	                    outOfStock_menu();      //Tell the user the item is out of stock
	                }
	                else if(cash >= ITEM_1_COST) {          //Check for enough money
	                    dispense_menu(key, 1, ITEM_1_COST); //Dispense menu
	                    dispense(1);                        //Turn on motor for item 1
	                    cashInMachine += ITEM_1_COST;       //Increment the total cash stored in the machine
	                    --item1_stock;                      //Decrease the items stock
	                    return_Change(cash - ITEM_1_COST);  //Return correct change
	                    cash = 0;   //Set the current cash entered back to zero
	                }
	                else {
	                    dispense_menu(key, 0, ITEM_1_COST); //Otherwise display the dispense menu with indication that there is not enough money
	                }
	            }
	            else if(key == 2) {     //Selectino 2
                    if(item2_stock <= 0) {      //Check for out of stock
                        redLED(1);              //Turn on red LED
                        outOfStock_menu();
                    }
                    else if(cash >= ITEM_2_COST) {          //If there is enough cash
	                    dispense_menu(key, 1, ITEM_2_COST); //Display dispense menu
	                    dispense(2);                        //Turn on motor for item 2
	                    cashInMachine += ITEM_2_COST;       //Increment the total cash stored in the machine
	                    --item2_stock;                      //Decrease item 2 stock
	                    return_Change(cash - ITEM_2_COST);  //Return correct change
	                    cash = 0;   //Reset cash to zero
	                }
	                else {
	                    dispense_menu(key, 0, ITEM_2_COST); //Not enough money
	                }
	            }
	            else if(key == 3) {         //Selection 3
                    if(item3_stock <= 0) {  //Check for out of stock
                        redLED(1);
                        outOfStock_menu();
                    }
                    else if(cash >= ITEM_3_COST) {              //If theres enough cash, dispense item...
	                    dispense_menu(key, 1, ITEM_3_COST);
	                    dispense(3);
	                    cashInMachine += ITEM_3_COST;
	                    --item3_stock;
	                    return_Change(cash - ITEM_3_COST);
	                    cash = 0;
	                }
                    else {
                        dispense_menu(key, 0, ITEM_3_COST);     //Not enough money
                    }
	            }
	            else if(key == 4) {         //Selection 4
                    if(item4_stock <= 0) {      //Check for out of stock
                        redLED(1);
                        outOfStock_menu();
                    }
                    else if(cash >= ITEM_4_COST) {              //If there is enough money, dispense item
	                    dispense_menu(key, 1, ITEM_4_COST);
	                    dispense(4);
	                    cashInMachine += ITEM_4_COST;
	                    --item4_stock;
	                    return_Change(cash - ITEM_4_COST);
	                    cash = 0;
	                }
                    else {
                        dispense_menu(key, 0, ITEM_4_COST);     //Not enough money
                    }
	            }
	            else if(key == 10) {    // '*' key Enters the admin menu
	                blink = 0;          //Stops selection cursor from blinking
	                state = ADMIN_PASS; //Admin password state
	                display_adminPass_menu();   //Display admin password screen
	            }
	        break;
	        case ADMIN_PASS:
	            if(key == 11) { //Accept (#)
	                passPosition = 0;       //Position to display *'s and store password keys reset to 0
	                if((password[0] == pin[0]) && (password[0] == pin[0]) && (password[1] == pin[1]) && (password[2] == pin[2])
	                        && (password[3] == pin[3]) && (password[4] == pin[4]) && (password[5] == pin[5])) {         //Check for correct password
	                    display_admin_menu();       //Display the admin menu
	                    memset(pin,0,sizeof(pin));  //Clear password array to 0's
	                    state = ADMIN_MENU;         //Admin menu state
	                }
	                else {
	                    display_adminPass_menu();   //If the password is incorrect, just reload the password screen
	                }
	            }
	            else if(key == 10) {    //Cancel (*)
	                display_main_menu();    //Go back to main menu
	                state = MAIN_MENU;
	            }
	            else if(key != -1 && key != 11 && key != 10) {
	                readPassword(key);      //Otherwise read the key in for the password
	            }

	        break;
	        case ADMIN_MENU:
	            if(key == 10) {    //Cancel (*)
	                display_main_menu();        //Go back to main menu
	                state = MAIN_MENU;
	            }
	            else if(key == 1) {         //Admin temp menu
	                commandWrite(0x01);     //Clears display
	                writeString("Admin: Temp");     //Print temperature menu items
	                commandWrite(0xC9);
	                dataWrite(0xDF);        //Degree symbol
	                writeString("C");
	                commandWrite(0xD0);
	                writeString("         [*]Back");
	                state = TEMP_MENU;      //Temp menu state
	            }
	            else if(key == 2) {         //Inventory menu
                    commandWrite(0x01);     //Clears display
                    writeString("Admin: Inventory");    //Print inventory menu
                    print_inventory();
                    commandWrite(0xD0);
                    writeString("[#]Stock [*]Back");
	                state = INVENTORY_MENU;     //Inventory menu state
	            }
                else if(key == 3) {     //Cash menu
                    sprintf(totalCashBuffer, "$%4.2lf", cashInMachine); //Cash array to be printed to the LCD
                    commandWrite(0x01);   //Clears display
                    writeString("Admin: Cash");     //Print cash menu items
                    commandWrite(0xC0);
                    writeString(totalCashBuffer);
                    commandWrite(0x90);
                    writeString("[#]Withdraw");
                    commandWrite(0xD0);
                    writeString("[*]Back");
                    state = CASH_MENU;      //Cash menu state
                }
	        break;
            case TEMP_MENU:                 //Temperature updates in the background using a 1 second timerA interrupt
                if(key == 10) {             //Check for back (*) key
                    state = ADMIN_MENU;     //Go back to admin menu
                    display_admin_menu();
                }
            break;
            case INVENTORY_MENU:
                if(key == 10) {         //Check for back (*) key
                    state = ADMIN_MENU; //Return to admin
                    display_admin_menu();
                }
                else if (key == 11) {   //Check for (#) key
                    item1_stock = 5;    //Restock all items to 5
                    item2_stock = 5;
                    item3_stock = 5;
                    item4_stock = 5;
                    print_inventory();  //Update the inventory
                }
            break;
            case CASH_MENU:
                if(key == 10) {         //Check for back (*) key
                    state = ADMIN_MENU; //Return to admin menu
                    display_admin_menu();
                }
                else if(key == 11) {        //Return all change in machine
                    returnCounter = 20;     //Run coin return for 20 seconds (~20 coins)
                    cashInMachine = 0;
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
            default:
                state = MAIN_MENU;
            break;
	    }
	}
}



/***| dispense(int item) |******************************
*Brief:    This function turns on the proper motor to dispense the corresponding item
*Params:
*            item : item number to dispense
*Returns:
*            null
********************************************************/
void dispense(int item) {
    if(item == 1) {
        P4->OUT |= BIT4;    //Enable motor for item 1
    }
    else if(item == 2) {
        P4->OUT |= BIT5;    //Enable motor for item 2
    }
    else if(item == 3) {
        P4->OUT |= BIT6;    //Enable motor for item 3
    }
    else if(item == 4) {
        P4->OUT |= BIT7;    //Enable motor for item 4
    }
    motorCount = 0;         //Motor count increments every second, used to run the motors for the desired amount of time
}


/***| return_Change(int change) |***********************
*Brief:    This function returns the correct change to the user
*Params:
*            change : change to dispense
*Returns:
*            null
********************************************************/
void return_Change(int change) {
    returnCounter = (change/.25);   //Stores number of quarters to return

    if(returnCounter > 0) {     //If there is change to return
        P3->OUT |= BIT6;        //Output a one to the arduino which runs the coin return servo until this pin is driven low again
    }
}


/***| redLED(int state) |*******************************
*Brief:    This function controls the red LED
*Params:
*            state : LED state, 1 for on, 0 for off
*Returns:
*            null
********************************************************/
void redLED(int state) {
    if(state == 0) {
        P1->OUT &= ~BIT7;   //LED off
    }
    else if(state == 1) {   //LED on
        P1->OUT |= BIT7;
    }
}


/***| greenLED(int state) |*****************************
*Brief:    This function controls the green LED
*Params:
*            state : LED state, 1 for on, 0 for off
*Returns:
*            null
********************************************************/
void greenLED(int state) {
    if(state == 0) {
        P1->OUT &= ~BIT6;   //LED off
    }
    else if(state == 1) {
        P1->OUT |= BIT6;    //LED on
    }
}


/***| motorPinInit() |**********************************
*Brief:    This function initializes the pins used by each motor
*Params:
*            null
*Returns:
*            null
********************************************************/
void motorPinInit() {
    P4->SEL0 &= ~(BIT4|BIT5|BIT6|BIT7);
    P4->SEL1 &= ~(BIT4|BIT5|BIT6|BIT7);
    P4->DIR |=   (BIT4|BIT5|BIT6|BIT7);     //Output
    P4->OUT &=  ~(BIT4|BIT5|BIT6|BIT7);     //Off
}


/***| ledPinInit() |**********************************
*Brief:    This function initializes the pins used by the red, green, and IR leds
*Params:
*            null
*Returns:
*            null
********************************************************/
void ledPinInit() {
    P1->SEL0 &= ~(BIT6|BIT7);   //Red and green LED's
    P1->SEL1 &= ~(BIT6|BIT7);
    P1->DIR |= (BIT6|BIT7);     //Output
    P1->OUT &= ~(BIT6|BIT7);    //Off

    P3->SEL0 &= ~BIT0;          //IR LED
    P3->SEL1 &= ~BIT0;
    P3->DIR &= ~BIT0;           //input
    P3->REN |= BIT0;            //Enable pull up resistor
    P3->OUT |= BIT0;
    P3->IE |= BIT0;             //Enable interrupts in P3.0
}


/***| display_adminPass_menu() |************************
*Brief:    This function prints the admin password menu to the LCD
*Params:
*            null
*Returns:
*            null
********************************************************/
void display_adminPass_menu() {
    passPosition = 0;
    commandWrite(0x01);             //Clears display
    writeString("Admin passcode: ");
    commandWrite(0xC0);             //Second line
    writeString("______");
    commandWrite(0x90);             //Third line
    writeString("[#]Accept");
    commandWrite(0xD0);             //Fourth line
    writeString("[*]Cancel");
}


/***| readPassword(int keyPress) |**********************
*Brief:    This function prints an "*" in when a key is entered for the password and stores the keypress
           in the array in the correct position
*Params:
*            keyPress : key pressed
*Returns:
*            null
********************************************************/
void readPassword(int keyPress) {
    if(passPosition == 6) {     //If 6 keys have been entered, loop back to zero
        passPosition = 0;
    }
    pin[passPosition] = keyPress;       //Store key in correct position
    commandWrite(0xC0 + passPosition);  //Move to the correct address
    writeString("*");                   //Print an asterisk
    passPosition++;
}


/***| display_admin_menu() |************************
*Brief:    This function prints the admin menu to the display
*Params:
*            null
*Returns:
*            null
********************************************************/
void display_admin_menu() {
    commandWrite(0x01);     //Clears display
    commandWrite(0x85);
    writeString("Admin");
    commandWrite(0xC0);
    writeString("[1]Temperature");
    commandWrite(0x90);
    writeString("[2]Inventory");
    commandWrite(0xD0);
    writeString("[3]Cash  [*]Back");
}


/***| display_main_menu() |************************
*Brief:    This function prints the main selection menu to the display
*Params:
*            null
*Returns:
*            null
********************************************************/
void display_main_menu() {
    noKey = 0;          //Key presses will be read
    count = 4;          //Set count higher than three
    char cashBuffer[5] = {0};   //Array to store cash
    commandWrite(0x01); //Clear display

    sprintf(cashBuffer, "%4.2lf", cash);    //Parse cash into array
    commandWrite(0x80);         //Address  80

    writeString("Cash:  $");
    writeString(cashBuffer);

    commandWrite(0xC0);         //Address  C0
    writeString("Selection: _");

    commandWrite(0xD0);         //Address D0
    writeString("[*]Admin");
    blink = 1;                  //Blink cursor for selection
    return;
}


/***| updateCash() |************************************
*Brief:    This function updates the cash amount displayed when a coin is entered
*Params:
*            null
*Returns:
*            null
********************************************************/
void updateCash() {
    char cashBuffer[5] = {0};

    if(state == MAIN_MENU) {        //Only print if in the main menu
        sprintf(cashBuffer, "%4.2lf", cash);
        commandWrite(0x80);         //Address  80
        writeString("Cash:  $");
        writeString(cashBuffer);    //Print cash amount to display
    }
}


/***| dispense_menu() |************************************
*Brief:    This function displays the dispense menu when an item is selected
*Params:
*            selection : item selected
*            enough : if there is enough money for the item or not
*            price : the price of the selected item
*Returns:
*            null
********************************************************/
void dispense_menu(int selection, int enough, double price) {
    blink = 0;      //Dont blink cursor any more
    noKey = 1;      //Dont read key presses while displayed
    char cashBuffer[5] = {0};           //Stores cash
    char changeBuffer[5] = {0};         //Change to be returned
    char needBuffer[6] = {0};           //Money needed to purchase selection
    char selectionBuffer[1] = {0};      //Item selection

    double totChange = 0.0;     //Total change
    double totNeed = 0.0;       //Total amount needed
    totChange = cash - price;
    totNeed = (price - cash);

    sprintf(cashBuffer, "%4.2lf", cash);            //Parse all values into arrays
    sprintf(changeBuffer, "%4.2lf", totChange);
    sprintf(needBuffer, "%4.2lf", totNeed);
    sprintf(selectionBuffer, "%d", selection);

    commandWrite(0x80);         //Address  80
    writeString("Cash:  $");
    writeString(cashBuffer);
    commandWrite(0xC0);         //Address  C0
    writeString("Selection: ");
    writeString(selectionBuffer);

    if(enough == 1) {               //If there is enough money
        redLED(0);
        greenLED(1);                //Turn on green LED
        commandWrite(0x90);
        writeString("Dispensed ");  //Indicate the item was dispensed
        commandWrite(0xD0);
        writeString("Change: $");
        writeString(changeBuffer);  //Print the amount of change to be returned
    }
    else {                              //Otherwise there is not enough money
        redLED(1);                      //Turn on red LED
        greenLED(0);
        commandWrite(0x90);
        writeString("Not enough money");    //Indicate there is not enough money
        commandWrite(0xD0);
        writeString("Need  $");
        writeString(needBuffer);            //Print the amount of money needed
        writeString("   ");
    }
    count = 0;      //Set count to zero in order to display this menu for 3 seconds
}


/***| outOfStock_menu() |*******************************
*Brief:    This function writes a message to the display indicating an item is out of stock
*Params:
*            null
*Returns:
*            null
********************************************************/
void outOfStock_menu() {
    blink = 0;
    commandWrite(0x01);     //Clear display
    commandWrite(0x80);
    writeString(" Out of stock!");
    commandWrite(0x90);
    writeString("  Make another");
    commandWrite(0xD0);
    writeString("   selection.");
    count = 0;              //Set count to zero to display screen for three seconds
}


/***| PB_S_Init() |*************************************
*Brief:    This function initializes the pins for the pushbutton and servo motor
*Params:
*            null
*Returns:
*            null
********************************************************/
void PB_S_Init() {
    P6->SEL0 &= ~BIT7;  //Pushbutton
    P6->SEL1 &= ~BIT7;
    P6->DIR &= ~BIT7;   //Input
    P6->REN |= BIT7;    //Enables internal resistor
    P6->OUT |= BIT7;    //Pull up resistor
    P6->IE  |= BIT7;    //Interrupt enable

    P3->SEL0 &= ~BIT6;  //Servo enable
    P3->SEL1 &= ~BIT6;
    P3->DIR |= BIT6;    //Output
    P3->OUT &= ~BIT6;
}


/***| print_inventory() |*******************************
*Brief:    This function initializes the pins for the pushbutton and servo motor
*Params:
*            null
*Returns:
*            null
********************************************************/
void print_inventory() {
    char inv1Buffer[16] = {0};  //First line
    char inv2Buffer[16] = {0};  //Second line

    sprintf(inv1Buffer, "%d Reeses %d Hers", item1_stock, item2_stock);     //Print items and their stock
    sprintf(inv2Buffer, "%d KitK   %d Snik", item3_stock, item4_stock);

    commandWrite(0xC0);
    writeString(inv1Buffer);
    commandWrite(0x90);
    writeString(inv2Buffer);
}

/***| displayWelcome() |*******************************
*Brief:      This function displays the welcome message to the vending machine
*Params:
*            null
*Returns:
*            null
********************************************************/
void displayWelcome() {
    commandWrite(0x86);
    writeString("LUKE");
    commandWrite(0xC5);
    writeString("BRIAN");
    commandWrite(0xD0);
    writeString("VENDING MACHINE!");
}


/***| tempSetup() |**************************************
*Brief:    This function sets up the ADC for temp reading and its pin
*Params:
*            null
*Returns:
*            null
********************************************************/
void tempSetup() {
    P5->SEL0 |= BIT5;   //Set up P5.1
    P5->SEL1 |= BIT5;
    P5->DIR &= ~BIT5;   //Input

    //ADC conversion set up
    ADC14->CTL0 = 0;                            //Clears the register
    ADC14->CTL0 |= (BIT(26)|BIT(21)|BIT4);      //0b000001000010000000000000001000
    ADC14->CTL1 =0b110000;                      //Only Bit 4 and 5 on

    ADC14->MCTL[0] = 0;         //Read A0 (P5.5)
    ADC14->CTL0 |= BIT1;        //Enable conversion ADC14ENC
    ADC14->CTL0 |= BIT0;        //Start conversion
}


/***| PORT6_IRQHandler() |*******************************
*Brief:    Interrupt handler for port 6, pushbutton
*Params:
*            null
*Returns:
*            null
********************************************************/
void PORT6_IRQHandler(){
    if((P6->IN & BIT7) == 0) {      //If the button is pressed
        if(state == MAIN_MENU) {
            cash = 0;               //Set the cash back to zero (simulating change return)
            updateCash();
        }
    }
    P6->IFG &= ~(BIT7);       //clear (Acknowledge) flag
}


/***| PORT3_IRQHandler() |*******************************
*Brief:    Interrupt handler for port 3, IR sensor
*Params:
*            null
*Returns:
*            null
********************************************************/
void PORT3_IRQHandler(){
    if((P3->IN & BIT0) == 0) {  //If the IR beam was tripped by a quarter
        cash = cash + 0.25;     //Increase cash by 0.25
        updateCash();           //Update the cash printed in the main menu
    }
    P3->IFG &= ~(BIT0);       //clear (Acknowledge) flag
}


/***| setupTimerA1() |**********************************
*Brief:    This function initializes TA1 as an interrupt
*Params:
*            null
*Returns:
*            null
********************************************************/
void setupTimerA1()
{
    TIMER_A1->CTL = 0b0000001011010110;
    TIMER_A1->EX0 = 0b111;          //111 = divide by 8
    TIMER_A1->CCR[0] = 46875;       //One second interrupt

    TIMER_A1->CTL &= ~BIT0;         // Clear flag
    NVIC_EnableIRQ(TA1_N_IRQn);     //Enable interrupts
}


/***| TA1_N_IRQHandler() |*******************************
*Brief:    Interrupt handler for port 3, IR sensor
*Params:
*            null
*Returns:
*            null
********************************************************/
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

/***| setupTimerA2() |***********************************
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

/***| TA2_N_IRQHandler() |******************************
*Brief:    This function gets called when there is an interrupt on TA2
*Params:
*            null
*Returns:
*            null
********************************************************/
void TA2_N_IRQHandler() {
    if(noKey == 0) {        //If we want to read key presses
        key = getKey();     //Read the keypress into key
    }
    else {
        key = -1;           //Otherwise key is -1
    }
    TIMER_A2->CTL &= ~BIT0; //clear interrupt
}


/***| setupTimerA3() |******************************
*Brief:    This function initializes timer A3
*Params:
*            null
*Returns:
*            null
********************************************************/
void setupTimerA3() {
    TIMER_A3->CTL = 0b0000001011010110;
    TIMER_A3->EX0 = 0b111; //111 = divide by 8
    TIMER_A3->CCR[0] = 46875; //One second

    TIMER_A3->CTL &= ~BIT0;  // Clear flag
    NVIC_EnableIRQ(TA3_N_IRQn);
}


/***| TA3_N_IRQHandler() |******************************
*Brief:    This handler is called when there is an interrupt on TA3
*Params:
*            null
*Returns:
*            null
********************************************************/
void TA3_N_IRQHandler() {
    if(state == TEMP_MENU) {
        if(!(ADC14->CTL0 & BIT(16))) {                                  //Checks for conversion to be complete
            temp = (((ADC14->MEM[0]*(3.3/pow(2,14)))*1000)-500)/10;     //Convert voltage reading to temp (C)
            sprintf(tempBuffer, "%4.2lf", temp);                         //Parses out temp variable into an array
            commandWrite(0xC4);
            writeString(tempBuffer);
            ADC14->CTL0 |= BIT0;                    //Start conversion again, gets turned off once completed
        }
    }

    count = count + 1;                          //Increment count (used to display menus for 3 seconds

    if(state == MAIN_MENU && blink == 1) {      //Blink the cursor
        if((count %2) == 0) {                   //Every other second
            commandWrite(0xCB);
            writeString(" ");
        }
        else {
            commandWrite(0xCB);
            writeString("_");
        }
    }

    if(state == MAIN_MENU || state == CASH_MENU) {
        if(returnCounter > 0) {     //If the return counter is greater than zero (we need to return change)
            P3->OUT |= BIT6;        //Set the servo enable pin high
        }
        else {
            P3->OUT &= ~BIT6;       //Once return counter reaches zero, turn the servo off
        }
    }
    returnCounter--;        //Decrement every second (dispensing a single coin takes one second)

    printf("%d\n", returnCounter);

    TIMER_A3->CTL &= ~BIT0;     //Clear interrupt flag
}

