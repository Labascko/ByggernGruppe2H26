/*
 * CFile1.c
 *
 * Created: 14.09.2026 09:18:28
 *  Author: eskilod
 */ 

#include <avr/io.h>
#include <stdio.h>

void clock_gen_init(void){
	DDRD |= (1<<DDD5);
	
	TCCR1A = (1<<COM1A0);
	TCCR1B = (1<<WGM12 | 1<<CS10);
	
	/*unsigned char sreg;
	sreg = SREG;
	_CLI();*/
	
	OCR1A = 0x0000;
	
	//SREG = sreg;
}