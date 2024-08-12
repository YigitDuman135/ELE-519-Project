/*
 * usart.c
 *
 * Created: 8/12/2024 11:40:20 AM
 *  Author: yigit
 */ 
#include "usart.h"

void usart_init(void) {
	UCSR0A = 0x00; // UDRE reset
	UBRR0H = 0x00;
	UBRR0L = 0x67; // Baud rate = 9600bps
	UCSR0C &= ~((1<<UMSEL00)|(1<<UMSEL01)); // Async
	UCSR0C &= ~((1<<UPM00)|(1<<UPM01)); // No parity
	UCSR0C &= ~(1<<USBS0); // 1 Stop bit
	UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01); // 8-bit data frame
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0); // Rx & Tx enable
}

uint8_t usart_receive(void) {
	uint8_t data;
	while(~UCSR0A & (1<<RXC0)); // Wait until receiving complete
	data = UDR0; // Receive character
	return data;
}

void usart_send(uint8_t data) {
	while(~UCSR0A & (1<<UDRE0)); // Wait until UDR empty
	UDR0 = data; // Transmit character
}

void usart_send_string(const char *str) {
	while (*str) {
		usart_send(*str);  // Send each character
		str++;             // Move to the next character
	}
}