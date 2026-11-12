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
	//Sett baudrate til 9600
	UBRR0H = (unsigned int) (ubrr>>8);
	UBRR0L = (unsigned int) ubrr;
	
	//Skru på mottaker og sender
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
	//URSEL0 velger resgister som skal skrives til, USBS0 velger antall stoppbit, UCSZ00 setter tegnstørrelse 
	UCSR0C = (1<<URSEL0) | (1<<USBS0) | (3<<UCSZ00);
}

int uart_putchar(char c, FILE *stream){
	//funksjon som er tilpasset formatet for å knytte til printf
	uart_transmit((unsigned char)c);
	
	return 0;
}

void uart_transmit(unsigned char data){
	//Vent til klar til å sende neste byte
	while( !(UCSR0A & (1<<UDRE0)) );
	//Send data
	UDR0 = data;
}

unsigned char uart_recieve(void){
	//Vent på å motta en byte
	while ( !(UCSR0A & (1<<RXC0)) );
	//Returner mottatt verdi
	return UDR0;
}