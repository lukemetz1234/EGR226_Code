/*******************************************************
* Name:              Luke Metz
* Course:            EGR 226 - Microcontroller Programming and Applications
* Project:           Homework 2, part 3
* File:              keypad.h
* Description:       Header file for keypad.c
*******************************************************/
void setupKeypad();
void PORT5_IRQHandler();
void PORT4_IRQHandler();
int getKeypress();
void readKeypress();
int decode(int col, int row);
void storeKey(int key);
void TA2_IRQ_Init();
void TA2_N_IRQHandler();
void TA3_N_IRQHandler();
void TA3_IRQ_Init();
