/*
 * eeprom.h
 *
 * Created: 8/12/2024 4:16:52 AM
 *  Author: yigit
 */ 
#include <avr/io.h>

#ifndef EEPROM_H_
#define EEPROM_H_

void eeprom_write(uint16_t address, uint8_t data);
uint8_t eeprom_read(uint16_t address);
void eeprom_clear(void);

#endif /* EEPROM_H_ */