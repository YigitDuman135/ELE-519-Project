/*
 * adc.c
 *
 * Created: 8/11/2024 9:49:39 PM
 *  Author: yigit
 */ 
#include "adc.h"

void adc_init(void) {
	// Set reference voltage to AVcc, left adjust the result, select ADC0 channel
	ADMUX = (1<<REFS0);
	
	// Enable ADC, set prescaler to 128 (for 16MHz, 125kHz ADC clock)
	ADCSRA = (1<<ADEN) | (7<<ADPS0);
}

uint16_t adc_read(uint8_t ch) {
	// Select ADC channel, clear MUX bits and set channel
	ch &= 0b00000111;  // ADC0 to ADC7 (channels)
	ADMUX = (ADMUX & 0xF8) | ch;
	
	// Start single conversion
	ADCSRA |= (1<<ADSC);
	
	// Wait for conversion to complete
	while(ADCSRA & (1<<ADSC));
	
	// Return the ADC value
	return (ADC);
}