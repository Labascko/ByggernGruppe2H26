/*
 * CFile1.c
 *
 * Created: 14.09.2026 09:18:28
 *  Author: eskilod
 */ 

#include <avr/io.h>
#include <stdio.h>

void timer1_init(void){
	//Sett PD5 som output
	DDRD |= (1<<DDD5);
	
	//Automatiser "pin-toggle" ved sammenligningsverdi OCR1A
	TCCR1A = (1<<COM1A0);
	//Velg prescaler og modus. WGM12 => CTC-modus, CS10 => prescaler 1 (ingen deling)
	TCCR1B = (1<<WGM12 | 1<<CS10);
	
	//Sett sammenligningsverdi
	OCR1A = 0x0000;
}

void timer0_init(void){
	//Telleverdi 0
	TCNT0 = 0;

	//Sett sammenligningsverdi i "Output Compare Register 0"
	OCR0 = 255;
	
	//Aktiver overflow interrupt
	TIMSK |= (1<<OCIE0);
	
	//Velg prescaler og modus. CS00 og CS02 => prescaler 1024,  WGM01 => CTC-modus
	TCCR0 = (1<<CS00)|(1<<CS02)|(1<<WGM01);
}