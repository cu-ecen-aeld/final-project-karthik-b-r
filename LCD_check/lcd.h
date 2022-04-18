#ifndef __LCD_H__
#define __LCD_H__

#include "wiringPi.h"

void pulseEnable();
void lcd_byte(char bits);
void SetCmdMode();
void SetChrMode();
void lcd_text(char *s);
void lcd_init();
void lcd_print1(char *msg);
void lcd_print2(char *msg);

#endif