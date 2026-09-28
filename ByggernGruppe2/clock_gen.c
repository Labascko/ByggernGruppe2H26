/*
 * CFile1.c
 *
 * Created: 14.09.2026 09:18:28
 *  Author: eskilod
 */ 

#include <avr/io.h>
#include <stdio.h>

void timer1_init(void){
	DDRD |= (1<<DDD5);
	
	TCCR1A = (1<<COM1A0);
	TCCR1B = (1<<WGM12 | 1<<CS10);
	
	/*unsigned char sreg;
	sreg = SREG;
	_CLI();*/
	
	OCR1A = 0x0000;
	
	//SREG = sreg;
}

void timer0_init(void){
	//Telleverdi 0
	TCNT0 = 0;
	//Velg prescaler (1024 ganger tregere)
	//TCCR0 |= (1<<CS00)|(1<<CS02);
	//TCCR0 &= ~(1<<CS01);
	
	//Velge modus (CTC):
	//TCCR0 |= (1<<WGM01);
	//TCCR0 &= ~(1<<WGM00);
	//compare value
	OCR0 = 255;
	
	//Enable overflow interrupt
	TIMSK |= (1<<OCIE0);
	
	TCCR0 = (1<<CS00)|(1<<CS02)|(1<<WGM01);
	
	
}