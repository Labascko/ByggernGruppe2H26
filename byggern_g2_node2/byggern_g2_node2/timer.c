/*
 * timer.c
 *
 * Created: 12.10.2026 15:12:31
 *  Author: eskilod
 */ 

#include <stdio.h>


#define F_CPU 84000000

void pwm_init(void){
	//Kanal 1:
	//Registeroversikt på side 1003 i datablad
	
	// Periode 20ms=Prescaler*CPRD/F_CPU, Bruk prescaler = 1024, CPRD=1640.625, Eller prescaler = 128, CPRD = 13125
	
	//PWM->PWM_CLK = 
	
	//Enable
	PWM->PWM_ENA = PWM_ENA_CHID1;
}