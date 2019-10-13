

void LCDpin_init();
void LCD_init();
void pushNibble(uint8_t nibble);
void pushByte(uint8_t byte);
void commandWrite(uint8_t command);
void dataWrite(uint8_t command);
void delay_ms(int value);
void pulseEnablePin();
void delay_micro(int value);
