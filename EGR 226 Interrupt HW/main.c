#include "msp.h"
#include <stdio.h>
#include <stdlib.h>

#include "keypad.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    setupKeypad();
    __enable_interrupt();
    int key;

    SysTick->LOAD = 15000000;  //5 sec at 3MHz  //Reload register STRVR
    SysTick->VAL = 226; //any value clears count
    SysTick->CTRL |= BIT0;  //Enable   //Status and Control Register STCSR

    while(1)
    {
        key = getKeypress();  //get keypress
        while(key != -1)
        {
            printf("Key = %d\n",key);
            key = getKeypress();  //see if any more presses exist
        }
        while(!(SysTick->CTRL&BIT(16)));
        printf("5 seconds have passed\n\n");
    }
}
