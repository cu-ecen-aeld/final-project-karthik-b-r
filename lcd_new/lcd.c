/*
 * Reference: https://forums.raspberrypi.com/viewtopic.php?t=93613
 * Modified by Karthik Baggaon Rajendra
 * lcd.c:
 *        Simple program to send a string to the LCD
 */

#include <stdio.h>
#include <stdlib.h>
#include "wiringPi.h"
#include "lcd.h"

#define LCD_E 24
#define LCD_RS 25
#define LCD_D4 23
#define LCD_D5 17
#define LCD_D6 18
#define LCD_D7 22

int lcdAddr[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
                 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8,0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF};


void pulseEnable ()
{
   digitalWrite (LCD_E, HIGH) ;
   delay(0.5); //  1/2 microsecond pause - enable pulse must be > 450ns
   digitalWrite (LCD_E, LOW) ;
}

/*
  send a byte to the lcd in two nibbles
  before calling use SetChrMode or SetCmdMode to determine whether to send character or command
*/
void lcd_byte(char bits)
{
  digitalWrite (LCD_D4,(bits & 0x10)) ;
  digitalWrite (LCD_D5,(bits & 0x20)) ;
  digitalWrite (LCD_D6,(bits & 0x40)) ;
  digitalWrite (LCD_D7,(bits & 0x80)) ;
  pulseEnable();

  digitalWrite (LCD_D4,(bits & 0x1)) ;
  digitalWrite (LCD_D5,(bits & 0x2)) ;
  digitalWrite (LCD_D6,(bits & 0x4)) ;
  digitalWrite (LCD_D7,(bits & 0x8)) ;
  pulseEnable();
}

void SetCmdMode()
{
  digitalWrite (LCD_RS, 0); // set for commands
}

void SetChrMode()
{
  digitalWrite (LCD_RS, 1); // set for characters
}

void lcd_text(char *s)
{
  while(*s)
        lcd_byte(*s++);
 }

void lcd_init()
{
   wiringPiSetupGpio () ; // use BCIM numbering
   // set up pi pins for output
   pinMode (LCD_E,  OUTPUT);
   pinMode (LCD_RS, OUTPUT);
   pinMode (LCD_D4, OUTPUT);
   pinMode (LCD_D5, OUTPUT);
   pinMode (LCD_D6, OUTPUT);
   pinMode (LCD_D7, OUTPUT);

   // initialise LCD
   SetCmdMode(); // set for commands
   lcd_byte(0x33); // full init
   lcd_byte(0x32); // 4 bit mode
   lcd_byte(0x28); // 2 line mode
   lcd_byte(0x0C); // display on, cursor off, blink off
   lcd_byte(0x01);  // clear screen
   delay(3);        // clear screen is slow!
}


void lcd_print1(char *msg) 
{
	SetCmdMode();
	lcd_byte(0x80);
//	lcd_byte(0x01);
	delay(3);
	SetChrMode(); 
	lcd_text(msg);
	delay(1000);
}

void lcd_print2(char *msg)
{
	SetCmdMode();
//	lcd_byte(0x01);
	lcd_byte(0xC0);
	delay(3);
	SetChrMode(); 
	lcd_text(msg);
	delay(1000);
}

/*int main (int argc, char *argv [])
{
  lcd_init();

  SetChrMode();
  if (argc>1)
     lcd_text(argv[1]);
  else
    {
	while(1)
{
static int line_count=0;
		if (line_count == 0)		
                {
                       // SetChrMode(); 
			lcd_print1("Hello world");
			line_count=1;
		}
		else
		{
			lcd_print2("sko buffs");
			line_count=0;
		}
//     lcd_print1("hello world!");
//     lcd_print2("hello line2!");
    }
}
  return 0 ;
}
*/

void printChar(char c, int addr)
{
        SetCmdMode();
        lcd_byte(addr);
        delay(3);
        SetChrMode();
        lcd_byte(c);
}

/*int main (int argc, char *argv [])
{
  lcd_init();

       char *str = "AESD Project Work by us at CU ";
       char *p = str;
       int i = 0;
       while (*p != '\0')
       {
          printChar(*p, lcdAddr[i]);
          i++;
          p++;
          if(31 == i)
             i = 0;
       }
  
  return 0 ;
}*/

