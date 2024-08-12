/*
 * dht.c
 *
 * Created: 8/12/2024 2:07:20 AM
 *  Author: yigit
 */ 
#include "DHT.h"

volatile uint8_t *ddr;
volatile uint8_t *port;
volatile uint8_t *pins;
uint8_t pin;

void dht11_init(Dht11 device) {
	switch (device.port) {
		case 'B':
		ddr = &DDRB;
		port = &PORTB;
		pins = &PINB;
		break;
		case 'C':
		ddr = &DDRC;
		port = &PORTC;
		pins = &PINC;
		break;
		case 'D':
		ddr = &DDRD;
		port = &PORTD;
		pins = &PIND;
		break;
	}
	pin = device.pin;
	// Enable the internal pull-up resistor on the data pin
	*port |= (1 << pin);
}

void dht11_request_data(void) {
	// Set pin as output and pull it low for at least 18ms
	*ddr |= (1 << pin);
	*port &= ~(1 << pin);
	_delay_ms(20);

	// Pull pin high and wait for 20-40us
	*port |= (1 << pin);
	_delay_us(20);

	// Set pin as input to read data
	*ddr &= ~(1 << pin);
}

uint8_t dht11_read_data(void) {
	uint8_t data = 0;
	uint16_t timeout;

	for (int i = 0; i < 8; i++) {
		timeout = 0;
		// Wait for the pin to go high with timeout
		while (~(*pins) & (1 << pin)) {
			if (++timeout > 10000) return 1;  // Timeout error
		}

		_delay_us(30);  // Wait for 30us

		// If the pin is still high after 30us, it's a '1'
		if (*pins & (1 << pin)) {
			data |= (1 << (7 - i));
		}

		timeout = 0;
		// Wait for the pin to go low again with timeout
		while (*pins & (1 << pin)) {
			if (++timeout > 10000) return 1;  // Timeout error
		}
	}
	return data;
}

uint8_t dht11_read(uint8_t *temperature, uint8_t *humidity) {
	uint8_t checksum, hum_int, hum_dec, temp_int, temp_dec;

	dht11_request_data();

	// Wait for DHT11 response (80us low, 80us high) with timeout
	uint16_t timeout = 0;
	while (~(*pins) & (1 << pin)) {
		if (++timeout > 10000) return 1;  // Timeout error
	}
	timeout = 0;
	while (*pins & (1 << pin)) {
		if (++timeout > 10000) return 1;  // Timeout error
	}
	timeout = 0;
	while (~(*pins) & (1 << pin)) {
		if (++timeout > 10000) return 1;  // Timeout error
	}

	// Read the 40 bits (5 bytes) of data
	hum_int = dht11_read_data();
	hum_dec = dht11_read_data();
	temp_int = dht11_read_data();
	temp_dec = dht11_read_data();
	checksum = dht11_read_data();

	// Verify checksum
	if ((hum_int + hum_dec + temp_int + temp_dec) != checksum) {
		return 1;  // Checksum error
	}

	*humidity = hum_int;
	*temperature = temp_int;
	return 0;  // Success
}
