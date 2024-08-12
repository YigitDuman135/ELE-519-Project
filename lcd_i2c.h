/*
 * lcd_i2c.h
 *
 * Created: 8/10/2024 11:22:17 PM
 *  Author: yigit
 */ 
#define F_CPU 16000000UL
#include <util/delay.h>
#include "i2c.h"

#ifndef LCD_I2C_H_
#define LCD_I2C_H_

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

typedef struct LCD_INITIALIZER {
	I2C_Initialiser device;
	uint8_t rows;
	uint8_t columns;
	uint8_t dot_size;
	uint8_t backlight_val;
} lcd_initializer;

/************ low level data pushing commands **********/
// Helper function to pulse the enable pin
void lcd_pulseEnable(uint8_t data);

void lcd_write_byte(uint8_t data);

// Write 4 bits to the LCD via I2C
void lcd_write4bits(uint8_t value);

// write either command or data
void lcd_send(uint8_t value, uint8_t mode);

/*********** mid level commands, for sending data/cmds */
void lcd_command(uint8_t value);

/********** high level commands, for the user! */
void lcd_clear(void);

void lcd_home(void);

void lcd_setCursor(uint8_t col, uint8_t row);

// Turn the display on/off (quickly)
void lcd_noDisplay(void);
void lcd_display(void);

// Turns the underline cursor on/off
void lcd_noCursor(void);
void lcd_cursor(void);

// Turn on and off the blinking cursor
void lcd_noBlink(void);
void lcd_blink(void);

// These commands scroll the display without changing the RAM
void lcd_scrollDisplayLeft(void);
void lcd_crollDisplayRight(void);

// This is for text that flows Left to Right
void lcd_leftToRight(void);

// This is for text that flows Right to Left
void lcd_rightToLeft(void);

// This will 'right justify' text from the cursor
void lcd_autoscroll(void);

// This will 'left justify' text from the cursor
void lcd_noAutoscroll(void);

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_createChar(uint8_t location, uint8_t charmap[]);

// Turn the (optional) backlight off/on
void lcd_noBacklight(void);
void lcd_backlight(void);

// Alias functions
void lcd_cursor_on(void);
void lcd_cursor_off(void);

void lcd_blink_on(void);
void lcd_blink_off(void);

void lcd_load_custom_character(uint8_t char_num, uint8_t *rows);

void lcd_setBacklight(uint8_t new_val);

void lcd_print(const char *str);

void lcd_init(lcd_initializer Initializer);

#endif /* LCD_I2C_H_ */