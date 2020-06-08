#pragma once
#include "include.h"

#ifdef __cplusplus
extern "C" {
#endif

	// flags for backlight control
#define LCD_BACKLIGHT   0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04 // Enable bit
#define Rw 0x02 // Read/Write bit
#define Rs 0x01 // Register select bit

	// HD44780U Commands
#define	LCD_CLEAR	0x01
#define	LCD_HOME	0x02
#define	LCD_ENTRY	0x04
#define	LCD_CTRL	0x08
#define	LCD_CDSHIFT	0x10
#define	LCD_FUNC	0x20
#define	LCD_CGRAM	0x40
#define	LCD_DGRAM	0x80

	// Bits in the entry register
#define	LCD_ENTRYRIGHT          0x00
#define	LCD_ENTRYLEFT           0x02
#define	LCD_ENTRYSHIFTINCREMENT 0x01
#define	LCD_ENTRYSHIFTDECREMENT 0x00

	// Bits in the control register
#define	LCD_BLINK_CTRL		0x01
#define	LCD_CURSOR_CTRL		0x02
#define	LCD_DISPLAY_CTRL	0x04

	// Bits in the function register
#define	LCD_8BITMODE 0x10
#define	LCD_4BITMODE 0x00
#define	LCD_2LINE    0x08
#define	LCD_1LINE    0x00
#define	LCD_5x10DOTS 0x04
#define	LCD_5x8DOTS  0x00

	//extended function set
#define LCD_4LINE 0x09

#define	LCD_CDSHIFT_RL	0x04

/*#define LCD_L0 128
#define LCD_L1 192
#define LCD_L2 148
#define LCD_L3 212

#define CLEAR 1*/

//#define LCD_SYNC

void initialize_lcd();
void lcd_putchar(unsigned char data);
void lcd_putstr(const char *string);
void lcd_fflush();
void lcd_setline(unsigned char data);
void lcd_setcursor(unsigned char data);
int lcd_working();

//advance
void writeCommand(unsigned char cmd);

#define lcdprintf(ARGS...){char buff[128];sprintf(buff,ARGS);lcd_putstr(buff);}

#ifdef __cplusplus
} // closing brace for extern "C"
#endif