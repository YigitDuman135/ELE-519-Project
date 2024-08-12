/*
 * eeprom.c
 *
 * Created: 8/12/2024 4:16:37 AM
 *  Author: yigit
 */ 
#include "eeprom.h"

void eeprom_write(uint16_t address, uint8_t data) {
	while(EECR & (1<<EEPE)); // Wait until previous program completed
	EEARL = (uint8_t)(0x00FF & address); // First, address lowbyte first
	EEARH = (uint8_t)(address>>8); // Then, address highbyte
	EEDR = data; // Recorded data
	EECR |= (1<<EEMPE); // Master program enable
	EECR |= (1<<EEPE); // Program enable
}

uint8_t eeprom_read(uint16_t address) {
	uint8_t data;
	EEARL = (uint8_t)(0x00FF & address); // First, address lowbyte first
	EEARH = (uint8_t)(address>>8); // Then, address highbyte
	EECR |= (1<<EERE); // Read enable
	data = EEDR; // Read recorded data
	return data;
}

void eeprom_clear(void) {
	uint16_t adr;
	for(adr = 0x0000;adr<1024;adr++) {
		eeprom_write(adr, 0x00);
	}
}