/*
 * adc.c
 *
 * Created: 14.09.2026 10:14:25
 *  Author: eskilod
 */ 
#include <avr/io.h>
#include <stdio.h>

uint8_t adc_read(uint8_t channel){
	volatile uint8_t * adc = (uint8_t *) 0x1000;
	adc[0] = 0x00; //
	//for(volatile int i= 0; i<100; i++){}
	uint8_t ret_val;
	for(int i=0; i<=channel; i++){
		ret_val = adc[0];
	}
	
	//ret_val = adc[0];
	//printf("ADC-verdi: %02X\n", ret_val);
	return ret_val;
}