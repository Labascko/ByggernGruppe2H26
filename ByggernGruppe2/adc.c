/*
 * adc.c
 *
 * Created: 14.09.2026 10:14:25
 *  Author: eskilod
 */ 
#include <avr/io.h>
#include <stdio.h>

uint8_t adc_read(uint8_t channel){
	//Sett startadresse for å ADC
	volatile uint8_t * adc = (uint8_t *) 0x1000; 
	adc[0] = 0x00; // Start konvertering
	
	uint8_t ret_val;
	//ADC-en er laget slik at man leser n ganger for å få verdi fra kanal n
	for(int i=0; i<=channel; i++){
		ret_val = adc[0];
	}
	
	return ret_val;
}