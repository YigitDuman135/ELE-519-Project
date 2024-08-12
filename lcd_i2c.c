/*
 * lcd_i2c.c
 *
 * Created: 8/10/2024 11:21:56 PM
 *  Author: yigit
 */ 
#define F_CPU 16000000UL
#include <util/delay.h>
#include "lcd_i2c.h"

lcd_initializer initializer;

uint8_t display_function = 0x00;
uint8_t display_control = 0x00;
uint8_t display_mode = 0x00;

/************ low level data pushing commands **********/
// Helper function to pulse the enable pin
void lcd_pulseEnable(uint8_t data) {
	lcd_write_byte(data | En);  // En high
	_delay_us(1);          // Enable pulse must be >450ns
	lcd_write_byte(data & ~En); // En low
	_delay_us(50);         // Commands need > 37us to settle
}

void lcd_write_byte(uint8_t data) {
	I2C_Start();
	I2C_Write((initializer.device.slave_address<<1)|0);
	I2C_Write(data | initializer.backlight_val);
	I2C_Stop();
}

// Write 4 bits to the LCD via I2C
void lcd_write4bits(uint8_t value) {
	lcd_write_byte(value); // Send the data
	lcd_pulseEnable(value);    // Pulse the enable pin
}

// write either command or data
void lcd_send(uint8_t value, uint8_t mode) {
	uint8_t highnib = value & 0xF0;       // Extract the high nibble (upper 4 bits)
	uint8_t lownib = (value << 4) & 0xF0; // Shift the lower nibble into the high nibble's place
	// Send the high nibble with the mode (RS bit) included
	lcd_write4bits(highnib | mode);
	// Send the low nibble with the mode (RS bit) included
	lcd_write4bits(lownib | mode);
}

/*********** mid level commands, for sending data/cmds */
void lcd_command(uint8_t value) {
	lcd_send(value, 0);
}

/********** high level commands, for the user! */
void lcd_clear(void){
	lcd_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	_delay_us(2000);  // this command takes a long time!
}

void lcd_home(void){
	lcd_command(LCD_RETURNHOME);  // set cursor position to zero
	_delay_us(2000);  // this command takes a long time!
}

void lcd_setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > initializer.rows ) {
		row = initializer.rows-1;    // we count rows starting w/0
	}
	lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void lcd_noDisplay(void) {
	display_control &= ~LCD_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL | display_control);
}
void lcd_display(void) {
	display_control |= LCD_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL | display_control);
}

// Turns the underline cursor on/off
void lcd_noCursor(void) {
	display_control &= ~LCD_CURSORON;
	lcd_command(LCD_DISPLAYCONTROL | display_control);
}
void lcd_cursor(void) {
	display_control |= LCD_CURSORON;
	lcd_command(LCD_DISPLAYCONTROL | display_control);
}

// Turn on and off the blinking cursor
void lcd_noBlink(void) {
	display_control &= ~LCD_BLINKON;
	lcd_command(LCD_DISPLAYCONTROL | display_control);
}
void lcd_blink(void) {
	display_control |= LCD_BLINKON;
	lcd_command(LCD_DISPLAYCONTROL | display_control);
}

// These commands scroll the display without changing the RAM
void lcd_scrollDisplayLeft(void) {
	lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcd_crollDisplayRight(void) {
	lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_leftToRight(void) {
	display_mode |= LCD_ENTRYLEFT;
	lcd_command(LCD_ENTRYMODESET | display_mode);
}

// This is for text that flows Right to Left
void lcd_rightToLeft(void) {
	display_mode &= ~LCD_ENTRYLEFT;
	lcd_command(LCD_ENTRYMODESET | display_mode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(void) {
	display_mode |= LCD_ENTRYSHIFTINCREMENT;
	lcd_command(LCD_ENTRYMODESET | display_mode);
}

// This will 'left justify' text from the cursor
void lcd_noAutoscroll(void) {
	display_mode &= ~LCD_ENTRYSHIFTINCREMENT;
	lcd_command(LCD_ENTRYMODESET | display_mode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	lcd_command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		lcd_send(charmap[i], Rs);
	}
}

// Turn the (optional) backlight off/on
void lcd_noBacklight(void) {
	initializer.backlight_val = LCD_BACKLIGHT;
	lcd_write_byte(0);
}

void lcd_backlight(void) {
	initializer.backlight_val = LCD_BACKLIGHT;
	lcd_write_byte(0);
}

// Alias functions

void lcd_cursor_on(void) {
	lcd_cursor();
}

void lcd_cursor_off(void) {
	lcd_noCursor();
}

void lcd_blink_on(void) {
	lcd_blink();
}

void lcd_blink_off(void) {
	lcd_noBlink();
}

void lcd_load_custom_character(uint8_t char_num, uint8_t *rows) {
	lcd_createChar(char_num, rows);
}

void lcd_setBacklight(uint8_t new_val) {
	if(new_val){
		lcd_backlight();		// turn backlight on
		}else{
		lcd_noBacklight();		// turn backlight off
	}
}

void lcd_print(const char *str) {
	while (*str) {
		lcd_send(*str, Rs);  // Ensure RS is set to 1 for data
		_delay_ms(1);        // Small delay to ensure timing
		str++;               // Move to the next character
	}
}


void lcd_init(lcd_initializer Initializer) {
	initializer = Initializer;
	initializer.backlight_val = LCD_NOBACKLIGHT;
	
	DDRC = ((1<<DDC4)|(1<<DDC5));
	PORTC = ((1<<PORTC4)|(1<<PORTC5));
	
	I2C_Init(initializer.device);
	
	if (initializer.rows > 1) {
		display_function |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((initializer.dot_size != 0) && (initializer.rows == 1)) {
		display_function |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	_delay_ms(50);
	
	// Now we pull both RS and R/W low to begin commands
	lcd_write_byte(initializer.backlight_val);	// reset expanderand turn backlight off (Bit 8 =1)
	_delay_ms(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	// we start in 8bit mode, try to set 4 bit mode
	lcd_write4bits(0x03 << 4);
	_delay_us(4500); // wait min 4.1ms
	
	// second try
	lcd_write4bits(0x03 << 4);
	_delay_us(4500); // wait min 4.1ms
	
	// third go!
	lcd_write4bits(0x03 << 4);
	_delay_us(150);
	
	// finally, set to 4-bit interface
	lcd_write4bits(0x02 << 4);


	// set # lines, font size, etc.
	lcd_command(LCD_FUNCTIONSET | display_function);
	
	// turn the display on with no cursor or blinking default
	display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_display();
	
	// clear it off
	lcd_clear();
	
	// Initialize to default text direction (for roman languages)
	display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	lcd_command(LCD_ENTRYMODESET | display_mode);
	
	lcd_home();
}