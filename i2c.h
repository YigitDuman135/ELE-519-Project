/*
 * i2c.h
 *
 * Created: 8/9/2024 1:28:25 PM
 *  Author: yigit
 */ 
#include <avr/io.h>
#include <stdbool.h>

#ifndef I2C_H_
#define I2C_H_

enum frequencies {
	F_100kHz,
	F_400kHz
};

typedef struct I2C {
	uint8_t slave_address;
	enum frequencies baud_rate;
} I2C_Initialiser;

I2C_Initialiser device;

void I2C_Init(I2C_Initialiser Device);

//Start condition
void I2C_Start(void);

//I2C stop condition
void I2C_Stop(void);

void I2C_Write(uint8_t data);

uint8_t I2C_Read(void);

#endif /* I2C_H_ */