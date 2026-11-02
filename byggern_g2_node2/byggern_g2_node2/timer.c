/*
 * timer.c
 *
 * Created: 12.10.2026 15:12:31
 *  Author: eskilod
 */ 

#include <stdio.h>
#include <sam.h>

#define F_CPU 84000000
#define SERVO_PWM_CHANNEL 4
#define SERVO_PWM_PIN (1u << 21) //D9 pin
#define SERVO_FULL_CYCLE 20000 // 20ms (50Hz)
#define SERVO_MAX_DUTY 2100
#define SERVO_MIN_DUTY 900

#define PWM_CHANNEL 0
#define PWM_PIN (1u << 12) //D20 pin
#define FULL_CYCLE 20000 // 40ms (25Hz)
#define MAX_DUTY 20000
#define MIN_DUTY 5000




#define ADC_CHANNEL 7 //Må bruke pinne D54

#define LIGHT_ON_LEVEL   2500
#define LIGHT_OFF_LEVEL  1800

void servo_pwm_init(void){
	//Kanal 1:
	//Registeroversikt på side 1003 i datablad
	
	// Periode 20ms=Prescaler*CPRD/F_CPU, Bruk prescaler = 1024, CPRD=1640.625, Eller prescaler = 128, CPRD = 13125
	
	//PWM->PWM_CLK = 
	
	// Enable PCIO clock
	PMC->PMC_PCER0 = (1u << ID_PIOC);
	// Enable PWM peripheral clock
	PMC->PMC_PCER1 = (1u << (ID_PWM - 32));
	// ABSR bit = 1 -> Peripheral B
	PIOC->PIO_ABSR |= SERVO_PWM_PIN;
	// Disable normal GPIO control
	PIOC->PIO_PDR = SERVO_PWM_PIN;
	//Configure PWM clock A
	
	PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(F_CPU/1000000);
	
	// Disable channel 4 while configuring
	PWM->PWM_DIS = PWM_DIS_CHID4;
	
	PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CMR = PWM_CMR_CPRE_CLKA;
	
	PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CPRD = SERVO_FULL_CYCLE;
	
	//sett start duty-cycle
	PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTY = 1500;
	
	//Enable
	PWM->PWM_ENA = PWM_ENA_CHID4;
	
	//PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTYUPD = 2100;
	
}

void servo_pwm_set_duty(uint16_t duty){
	if(SERVO_MIN_DUTY <= duty && duty <= SERVO_MAX_DUTY){
		PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTYUPD = duty;
		//PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTYUPD = duty;

		}else if(duty < 900){
		PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTYUPD = SERVO_MIN_DUTY;
		}else{
		PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTYUPD = SERVO_MAX_DUTY;
	}
}


void pwm_init(void){
	//Kanal 1:
	//Registeroversikt på side 1003 i datablad
	
	// Enable PCIO clock
	PMC->PMC_PCER0 = (1u << ID_PIOB);
	// Enable PWM peripheral clock
	PMC->PMC_PCER1 = (1u << (ID_PWM - 32));
	// ABSR bit = 1 -> Peripheral B
	PIOB->PIO_ABSR |= PWM_PIN;
	// Disable normal GPIO control
	PIOB->PIO_PDR = PWM_PIN;
	//Configure PWM clock A
	
	PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(F_CPU/1000000);
	
	// Disable channel 6 while configuring
	PWM->PWM_DIS = PWM_DIS_CHID0;
	
	PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CMR = PWM_CMR_CPRE_CLKA;
	
	PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CPRD = FULL_CYCLE;
	
	//sett start duty-cycle
	PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTY = 10000; //
	
	//Enable
	PWM->PWM_ENA = PWM_ENA_CHID0;
	
	//PWM->PWM_CH_NUM[PWM_CHANNEL].PWM_CDTYUPD = 2100;
	
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



void timer_encoder_init(void){
	//Power Management Controller skrupå TC2 og TC0
	PMC->PMC_PCER0 |= (1u << ID_TC2);
	PMC->PMC_PCER0 |= (1u << ID_PIOC);
	
	PMC->PMC_PCER1 = (1u << (ID_TC6 - 32u));
	//PMC->PMC_PCER0 |= (1u << ID_TC0);
	
	//clock enable channel 2:
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	
	//Sett pinne pc25 og pc26 som inputs:
	PIOC->PIO_PDR = (1u<<25 | 1u<<26);
	//Velg peripheral B for disse:
	PIOC->PIO_ABSR |= (1u << 25) | (1u << 26);
	
	TC2->TC_CHANNEL[0].TC_CMR = (1<<2 | 1<<0);
	
	// Skru på Quadrature Decoder Enabled og Speed Enabled
	TC2->TC_BMR |= TC_BMR_SPEEDEN | TC_BMR_QDEN;
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
    //ADC->ADC_CR = ADC_CR_START;
	
}



void timer3_init(void) {
	// 2. Enable peripheral clock for TC3 (ID_TC3 = 30, which is TC1 Channel 0)
	PMC->PMC_PCER0 = (1u << ID_TC3);

	// 3. Configure TC1 Channel 0 in Waveform Mode, UP clock with RC compare reset, clock = MCK/2
	TC1->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 |
	TC_CMR_WAVE |
	TC_CMR_WAVSEL_UP_RC;

	// 4. Set RC value for 1 Hz (42 MHz / 1 = 42000000)
	TC1->TC_CHANNEL[0].TC_RC = 4200000;

	// 5. Enable Interrupt on RC Compare
	TC1->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
	
	// 6. Enable TC3 interrupt vector in NVIC
	NVIC_EnableIRQ(TC3_IRQn);

	// 7. Start Timer
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

