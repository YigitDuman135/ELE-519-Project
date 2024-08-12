/*
 * Project_Final_wTCP.c
 *
 * Created: 8/12/2024 7:07:48 AM
 * Author : yigit
 */ 
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

// Externally Included Libraries
#include "lcd_i2c.h"
#include "adc.h"
#include "dht.h"
#include "eeprom.h"
#include "usart.h"

// definition constants
#define LCD_ADDRESS 0x27
#define AIR_QUALITY_ADDRESS 0x00
#define TEMPERATURE_ADDRESS 0x02
#define HUMIDITY_ADDRESS 0x03

void enable_interrupt(void);

volatile uint8_t mode = 0x02;

int main(void) {
	lcd_initializer myLCD = {
		.device.slave_address = LCD_ADDRESS,  // I2C address of the LCD
		.device.baud_rate = F_100kHz,  // I2C baud rate of 100kHz
		.rows = 2,                     // 2 rows
		.columns = 16,                 // 16 columns
		.dot_size = LCD_5x8DOTS        // 5x8 dot matrix
	};
	
	Dht11 dht11 = {
		.port = 'B',
		.pin = 1
	};
	
	uint16_t adc_result;
	uint8_t temperature = 0, humidity = 0, result = 0;
	uint8_t uart_buffer[56];
	
	//Buffers to print lcd screen
	char buffer1[15];
	char buffer2[8];
	
	// LCD Initialize and Turn on backlight
	lcd_init(myLCD);
	lcd_backlight();
	// Initialize DHT11
	dht11_init(dht11);
	// Initialize ADC
	adc_init();
	// Enable Interrupt
	enable_interrupt();
	// Initialize UART
	usart_init();

    /* Replace with your application code */
    while(1) {
		sprintf((char*)uart_buffer, "Temperature: %d%d", eeprom_read(TEMPERATURE_ADDRESS) / 10, eeprom_read(TEMPERATURE_ADDRESS) % 10);
		usart_send_string((char *)uart_buffer);
		
		sprintf((char*)uart_buffer, "C, Humidity: %d%d%%, Air Quality: %d PPM\r\n",
		eeprom_read(HUMIDITY_ADDRESS) / 10, eeprom_read(HUMIDITY_ADDRESS) % 10,
		((uint16_t)(eeprom_read(AIR_QUALITY_ADDRESS)) << 8) | (uint16_t)eeprom_read(AIR_QUALITY_ADDRESS + 1));
		usart_send_string((char *)uart_buffer);
		
		result = dht11_read(&temperature, &humidity);
		adc_result = adc_read(0);
		
		eeprom_write(AIR_QUALITY_ADDRESS, (uint8_t)((adc_result & 0xFF00)>>8));
		eeprom_write(AIR_QUALITY_ADDRESS + 1, (uint8_t)(adc_result & 0x00FF));
		eeprom_write(TEMPERATURE_ADDRESS, temperature);
		eeprom_write(HUMIDITY_ADDRESS, humidity);
		
		if(result == 0) {
			sprintf(buffer1, "T: %d%d", temperature / 10, temperature % 10);
			lcd_print(buffer1);
			lcd_send(0xDF, Rs);
			sprintf(buffer1, "C, H: %d%d%%", humidity / 10, humidity % 10);
			lcd_print(buffer1);
			_delay_ms(1);
		} else {
			sprintf(buffer1, "Error");
			lcd_print(buffer1);
		}
		sprintf(buffer2, "Q: %d PPM", adc_result);
		
		lcd_setCursor(0, 1);
		_delay_ms(1);
		
		lcd_print(buffer2);
		_delay_ms(1);
		
		if(mode == 1) {
			_delay_ms(2000);
		} else if(mode == 2) {
			_delay_ms(1000);	
		} else if(mode == 3) {
			_delay_ms(500);
		}
		
		lcd_setCursor(0, 0);
    }
}

void enable_interrupt(void) {
	PORTD |= (1<<PORTD2);
	EIMSK |= (1<<INT0);
	EICRA &= ~((1<<ISC00)|(1<<ISC01)); // Triggered when low
	//EICRA |= (1<<ISC01); // Triggered when low
	sei(); // Must be called
}

ISR(INT0_vect){ // When PD2 low
	mode++;
	if(mode == 4)
		mode = 1;
	
	lcd_clear();
	lcd_setCursor(0, 0);
	if(mode == 1) {
		lcd_print("Data Transfer:");
		lcd_setCursor(0, 1);
		lcd_print("Slow Mode");
	} else if(mode == 2) {
		lcd_print("Data Transfer:");
		lcd_setCursor(0, 1);
		lcd_print("Normal Mode");
	} else if(mode == 3) {
		lcd_print("Data Transfer:");
		lcd_setCursor(0, 1);
		lcd_print("Fast Mode");
	}

	_delay_ms(1500);
	lcd_clear();
}

