/*
 * uartdriver.c
 *
 * Created: 31.08.2026 13:01:36
 *  Author: emredi
 */ 

#include <avr/io.h>
#include <avr/iom162.h>
#include <stdio.h>
#include "uartdriver.h"

void uart_init(unsigned int ubrr){
	UBRR0H = (unsigned int) (ubrr>>8);
	UBRR0L = (unsigned int) ubrr;
	
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
	UCSR0C = (1<<URSEL0) | (1<<USBS0) | (3<<UCSZ00);
	
}

int uart_putchar(char c, FILE *stream){
	uart_transmit((unsigned char) c);
	
	return 0;
}

void uart_transmit(unsigned char data){
	while( !(UCSR0A & (1<<UDRE0)) );
	UDR0 = data;
}

unsigned char uart_recieve(void){
	while ( !(UCSR0A & (1<<RXC0)) );
	return UDR0;
}