/*
 * uartdriver.h
 *
 * Created: 31.08.2026 13:01:14
 *  Author: emredi
 */ 


#ifndef UARTDRIVER_H_
#define UARTDRIVER_H_

void uart_init(unsigned int ubrr);

int uart_putchar(char c, FILE *stream);

void uart_transmit(unsigned char data);

unsigned char uart_recieve(void);


#endif /* UARTDRIVER_H_ */