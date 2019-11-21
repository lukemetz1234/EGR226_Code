#include "msp.h"
#include "LCD.h"
#include <stdio.h>
#include <string.h>

/**
 * Student Name
 * Date
 * Description:
 * This is lab 3.1 for ENGR 226.  The final code will update the
 * display by writing to the 'nextDisplay' memory location.
 */

char currentDisplay[65];
char nextDisplay[65];

enum displayStates {
    DATALOOP_CHECK,
    SET_RS_COMMAND,
    SET_TOP_NIBBLE_DATA,
    SET_ENABLE_TOP_NIBBLE,
    CLEAR_ENABLE_TOP_NIBBLE,
    SET_BOTTOM_NIBBLE_DATA,
    SET_ENABLE_BOTTOM_NIBBLE,
    CLEAR_ENABLE_BOTTOM_NIBBLE,
    CLEAR_RS_COMMAND
};

enum displayStates displayState = DATALOOP_CHECK;

void LCD_init()
{
    setupLCD();
    setupTimerA3();

    initializeDisplay();
    commandWrite(0x0C);  //remove cursor

    int i;
    for(i=0;i<64;i++)  //completely clear arrays to begin
    {
        if(i<16)
        {
            commandWrite(0x80+i);
            dataWrite(' ');
        }
        else if(i > 16 && i < 32)
        {
            commandWrite(0xC0+i);
            dataWrite(' ');
        }
        else if(i > 32 && i < 48)
        {
            commandWrite(0x90+i);
            dataWrite(' ');
        }
        else
        {
            commandWrite(0xD0+i);
            dataWrite(' ');
        }
        currentDisplay[i] = ' ';
        nextDisplay[i] = ' ';
    }
}
/*
    char buffer[31] = {0};
    double val = 2.75;

    sprintf(buffer, "Value of x: %4.2lf", val);

    strcpy(nextDisplay,"   Welcome to      Project 1!");
    __delay_cycles(3000000);
    while(1)
    {
        __delay_cycles(3000000);
        strcpy(nextDisplay, buffer);
    }
}*/


void writeString(char *string)
{
    int i = 0;
    while(string[i] != '\0')
    {
        dataWrite(string[i]);
        i++;
    }
}

void pulseEnable() //enable on P4.0
{
    P4->OUT |= BIT1;
    delay_us(20);
    P4->OUT &= ~BIT1;
}

void pushByte(uint8_t data)
{
    pushNibble((data&0xF0)>>4);  //top four bits as Nibble
    pulseEnable();
    pushNibble((data&0x0F));  //bottom four bits as Nibble
    pulseEnable();
}

void pushNibble(uint8_t nibble) //P2.3-P2.0
{
    P4->OUT &=~ 0xF0; //clears P2.3-P2.0
    P4->OUT |= (nibble<<4 & 0xF0);  //just for safety, remove top four bits and put on output
}

void commandWrite(uint8_t data) //RS on P4.1
{
    P4->OUT &= ~BIT0;
    delay_us(100);
    pushByte(data);
}

void dataWrite(uint8_t data) //RS on P4.1
{
    P4->OUT |= BIT0;
    delay_us(100);
    pushByte(data);
}

void initializeDisplay()
{
    commandWrite(0x03);   //Reset 3 times
    delay_ms(10);
    commandWrite(0x03);
    delay_ms(10);
    commandWrite(0x03);
    delay_ms(10);

    commandWrite(0x02);   //4 bit mode
    delay_us(100);
    commandWrite(0x02);
    commandWrite(0x06);   //2 line 5x7 format
    delay_us(100);
    commandWrite(0x0F);   //Display on, cursor on, blinking
    delay_us(100);
    commandWrite(0x01);   //Clears display
    delay_us(100);
    commandWrite(0x06);   //Increment cursor mode
    delay_us(100);
}

void delay_ms(int ms) {
    SysTick->CTRL = 0;
    SysTick->LOAD = (ms * 3000) - 1;
    SysTick->VAL = 39;
    SysTick->CTRL = 0b101;
    while(!(SysTick->CTRL & BIT(16)));
}

void delay_us(int us) {
    SysTick->CTRL = 0;
    SysTick->LOAD = (us * 3) - 1;
    SysTick->VAL = 39;
    SysTick->CTRL = 0b101;
    while(!(SysTick->CTRL & BIT(16)));
}

void setupLCD() {
    P4->SEL0 &= ~(BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);
    P4->SEL1 &= ~(BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);
    P4->DIR |= (BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);
    P4->OUT &= ~(BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);
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
    //TIMER_A1->EX0 = 0b111; //111 = divide by 8
    TIMER_A3->CCR[0] = 3749; // 10 ms to start
    //TIMER_A0->CCTL[0] = 0;

    TIMER_A3->CTL &= ~BIT0;  // Clear flag
    NVIC_EnableIRQ(TA3_N_IRQn);

}
void TA3_N_IRQHandler()
{
    static int databit = 0;
    static int command = 0;
    switch(displayState) {
        case DATALOOP_CHECK:
            if(currentDisplay[databit]==nextDisplay[databit])
            {
                databit++;
                if(databit == 63)
                    databit = 0;
            }
            else {
                if(nextDisplay[databit]=='\0')
                    nextDisplay[databit]=currentDisplay[databit];
                else {
                    displayState = SET_RS_COMMAND;
                }
            }
            break;
        case SET_RS_COMMAND:
            P4->OUT &= ~BIT0;
            TIMER_A3->CCR[0] = 4; // ~10 us delay
            command = 1;
            displayState = SET_TOP_NIBBLE_DATA;
            break;
        case SET_TOP_NIBBLE_DATA:
            if(command == 1) {
                P4->OUT &=~ 0xF0;
                if(databit < 16)
                    P4->OUT |= (0x80);
                else if(databit > 16 && databit < 32)
                    P4->OUT |= (0xC0);
                else if(databit > 32 && databit < 48)
                    P4->OUT |= (0x90);
                else
                    P4->OUT |= (0xD0);
            }
            else {
                P4->OUT &=~ 0xF0;
                P4->OUT |= (nextDisplay[databit]&0xF0);
            }
            TIMER_A3->CCR[0] = 1; // ~2 us delay
            displayState = SET_ENABLE_TOP_NIBBLE;
            break;
        case SET_ENABLE_TOP_NIBBLE:
            P4->OUT |= BIT1;
            TIMER_A3->CCR[0] = 8; // ~20 us delay
            displayState = CLEAR_ENABLE_TOP_NIBBLE;
            break;
        case CLEAR_ENABLE_TOP_NIBBLE:
            P4->OUT &= ~BIT1;
            displayState = SET_BOTTOM_NIBBLE_DATA;
            break;
        case SET_BOTTOM_NIBBLE_DATA:
            if(command == 1) {
                P4->OUT &=~ 0xF0;
                P4->OUT |= (databit << 4);
            }
            else {
                P4->OUT &=~ 0xF0;
                P4->OUT |= ((nextDisplay[databit]<<4)&0xF0);
            }
            TIMER_A3->CCR[0] = 1; // ~2 us delay
            displayState = SET_ENABLE_BOTTOM_NIBBLE;
            break;
        case SET_ENABLE_BOTTOM_NIBBLE:
            P4->OUT |= BIT1;
            TIMER_A3->CCR[0] = 8; // ~20 us delay
            displayState = CLEAR_ENABLE_BOTTOM_NIBBLE;
            break;
        case CLEAR_ENABLE_BOTTOM_NIBBLE:
            P4->OUT &= ~BIT1;
            if(command)
                displayState = CLEAR_RS_COMMAND;
            else
            {
                currentDisplay[databit] = nextDisplay[databit];
                TIMER_A3->CCR[0] = 3749; // 10 ms to start
                displayState = DATALOOP_CHECK;
            }
            break;
        case CLEAR_RS_COMMAND:
            P4->OUT |= BIT0;
            TIMER_A3->CCR[0] = 4; // ~10 us delay
            command = 0;
            displayState = SET_TOP_NIBBLE_DATA;
            break;

    }
    TIMER_A3->CTL &= ~BIT0;
}
