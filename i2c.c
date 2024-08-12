/*
 * i2c.c
 *
 * Created: 8/9/2024 1:28:11 PM
 *  Author: yigit
 */ 
#include "i2c.h"

void I2C_Init(I2C_Initialiser Device) {
	device = Device;
	if(device.baud_rate == F_100kHz) {
		TWBR = 0x48; // Baud Rate = 100kHz
	} else if(device.baud_rate == F_400kHz) {
		TWBR = 0x0C; // Baud Rate = 400kHz
	}
	TWCR = (1<<TWEN);	//Enable I2C
	TWSR = 0x00;		//Prescaler set to 1

}
//Start condition
void I2C_Start(void){
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA);	//start condition
	while (!(TWCR & (1<<TWINT)));				//check for start condition

}
//I2C stop condition
void I2C_Stop(void) {
	TWCR = (1<<TWEN)|(1<<TWSTO); // Enable I2C & Stop cond.
}

void I2C_Write(uint8_t data) {
	TWDR = data;						//Move value to I2C
	TWCR = (1<<TWINT) | (1<<TWEN);	//Enable I2C and clear interrupt
	while  (!(TWCR &(1<<TWINT)));
}

uint8_t I2C_Read(void) {
	TWCR  = (1<<TWEN) | (1<<TWINT);	//Enable I2C and clear interrupt
	while (!(TWCR & (1<<TWINT)));	//Read successful with all data received in TWDR
	return TWDR;
}
