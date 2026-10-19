/*
 * timer.c
 *
 * Created: 12.10.2026 15:12:31
 *  Author: eskilod
 */ 

#include <stdio.h>
#include <sam.h>

#define F_CPU 84000000
#define PWM_CHANNEL 4
#define PWM_PIN (1u << 21) //D9 pin
#define FULL_CYCLE 20000 // 20ms (50Hz)
#define MAX_DUTY 2100
#define MIN_DUTY 900

#define ADC_CHANNEL 7

#define LIGHT_ON_LEVEL   2200
#define LIGHT_OFF_LEVEL  1800

void pwm_init(void){
	//Kanal 1:
	//Registeroversikt på side 1003 i datablad
	
	// Periode 20ms=Prescaler*CPRD/F_CPU, Bruk prescaler = 1024, CPRD=1640.625, Eller prescaler = 128, CPRD = 13125
	
	//PWM->PWM_CLK = 
	
	// Enable PCIO clock
	PMC->PMC_PCER0 = (1u << ID_PIOC);
	// Enable PWM peripheral clock
	PMC->PMC_PCER1 = (1u << (ID_PWM - 32));
	// ABSR bit = 1 -> Peripheral B
	PIOC->PIO_ABSR |= PWM_PIN;
	// Disable normal GPIO control
	PIOC->PIO_PDR = PWM_PIN;
	//Configure PWM clock A
	
	PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(F_CPU/1000000);
	
	// Disable channel 4 while configuring
	PWM->PWM_DIS = PWM_DIS_CHID4;
	
	PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CMR = PWM_CMR_CPRE_CLKA;
	
	PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CPRD = FULL_CYCLE;
	
	//sett start duty-cycle
	PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTY = 1500;
	
	//Enable
	PWM->PWM_ENA = PWM_ENA_CHID4;
	
	//PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTYUPD = 2100;
	
}

void adc_init(void){
    /* Enable ADC peripheral clock */
    PMC->PMC_PCER1 = (1u << (ID_ADC - 32));

    /* Reset ADC */
    ADC->ADC_CR = ADC_CR_SWRST;

    ADC->ADC_MR =
          ADC_MR_FREERUN_ON
        | ADC_MR_PRESCAL(3)
        | ADC_MR_STARTUP_SUT64
        | ADC_MR_SETTLING_AST3
        | ADC_MR_TRACKTIM(0)
        | ADC_MR_TRANSFER(1);

    /* Enable A0 = ADC channel 7 */
    ADC->ADC_CHER = ADC_CHER_CH7;

    /*
     * Comparison window:
     *
     * LOW  = 1800
     * HIGH = 2200
     */
    ADC->ADC_CWR =
          ADC_CWR_LOWTHRES(LIGHT_OFF_LEVEL)
        | ADC_CWR_HIGHTHRES(LIGHT_ON_LEVEL);

    /*
     * Initially light is considered OFF.
     *
     * Therefore wait for:
     *
     * ADC > 2200
     */
    ADC->ADC_EMR =
          ADC_EMR_CMPSEL(ADC_CHANNEL)
        | ADC_EMR_CMPMODE_HIGH;

    /* Enable comparison interrupt */
    ADC->ADC_IER = ADC_IER_COMPE;

    /* Enable ADC interrupt in NVIC */
    NVIC_ClearPendingIRQ(ADC_IRQn);
    NVIC_EnableIRQ(ADC_IRQn);

    /* Start conversions */
    ADC->ADC_CR = ADC_CR_START;
	
}

void pwm_set_duty(uint16_t duty){
	if(MIN_DUTY <= duty && duty <= MAX_DUTY){
		PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTYUPD = duty;
		//PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTYUPD = duty;

	}else if(duty < 900){
		PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTYUPD = MIN_DUTY;
	}else{
		PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTYUPD = MAX_DUTY;
	}
}