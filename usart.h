/*
 * usart.h
 *
 * Created: 8/12/2024 11:40:03 AM
 *  Author: yigit
 */ 
#include <avr/io.h>

#ifndef USART_H_
#define USART_H_

void usart_init(void);

uint8_t usart_receive(void);
void usart_send(uint8_t data);
void usart_send_string(const char *str);

#endif /* USART_H_ */