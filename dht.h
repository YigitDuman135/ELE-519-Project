/*
 * dht.h
 *
 * Created: 8/12/2024 2:06:49 AM
 *  Author: yigit
 */ 


#ifndef DHT11_H_
#define DHT11_H_

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

typedef struct DHT11 {
	char port;
	char pin;
} Dht11;

void dht11_init(Dht11 device);
uint8_t dht11_read(uint8_t *temperature, uint8_t *humidity);

#endif /* DHT11_H_ */
