/*
 * adc.h
 *
 * Created: 8/11/2024 9:49:54 PM
 *  Author: yigit
 */ 
#include <avr/io.h>

#ifndef ADC_H_
#define ADC_H_

void adc_init(void);
uint16_t adc_read(uint8_t ch);

#endif /* ADC_H_ */