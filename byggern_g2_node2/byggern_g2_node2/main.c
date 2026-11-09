/*
 * byggern_g2_node2.c
 *
 * Created: 05.10.2026 13:17:52
 * Author : eskilod
 */ 

#include <stdio.h>
#include <unistd.h>
#include <time.h>


#include "sam.h"
#include "uart.h"
#include "time.h"
#include "can.h"

#define F_CPU 84000000


volatile uint8_t IR_flag = 0;
void PIOA_Handler(void)
{
	uint32_t status = PIOA->PIO_ISR;
	//printf("Interruptfuksjon kjører");
	if (status & (1u << 28))
	{
		IR_flag = 1;
	}
}

volatile uint8_t adc_up_flag = 0;
volatile uint8_t adc_down_flag = 0;
void ADC_Handler(void) {
	uint32_t status = ADC->ADC_ISR;
	// Sjekk om det var comparison interrupt som utløste avbruddet
	if (status & ADC_ISR_COMPE) {
		// Hva er nåværende tilstand?
		if ((ADC->ADC_EMR & ADC_EMR_CMPMODE_Msk) == ADC_EMR_CMPMODE_HIGH) {
			// Vi passerte HØY terskel (> 2200)
			// Endre hardware til å vente på at signalet faller under LAV terskel (< 1800)
			ADC->ADC_EMR = (ADC->ADC_EMR & ~ADC_EMR_CMPMODE_Msk) | ADC_EMR_CMPMODE_LOW;
			adc_up_flag = 1;
			
			//uint32_t current_adc_val = ADC->ADC_CDR[7];
			//printf("ADC: %d \n", current_adc_val);
		}
		else {
			// Vi passerte LAV terskel (< 1800) 
			// Endre hardware til å vente på at signalet stiger over HØY terskel (> 2200)
			ADC->ADC_EMR = (ADC->ADC_EMR & ~ADC_EMR_CMPMODE_Msk) | ADC_EMR_CMPMODE_HIGH;
			adc_down_flag = 1;
			
			//uint32_t current_adc_val = ADC->ADC_CDR[7];
			//printf("ADC: %d \n", current_adc_val);
		}
	}
}

volatile int16_t error_integral = 0;
int16_t error = 0;
int16_t posref = 0;
int16_t k_p = 3;
int16_t k_i = 1;
// T = 100ms
volatile uint8_t solenoid_on = 0;
volatile uint8_t solenoid_loopcounter = 0;
void TC3_Handler(void) {
	
	
	if(abs(error)>150){
		error_integral += error;
		if (error_integral>2000){
			error_integral =2000;
		}else if(error_integral<-2000){
			error_integral = -2000;
		}
		
		int16_t input = k_p  * error + k_i * error_integral/5;
		if(input<0){
			PIOC->PIO_SODR = (1u << 23);  // HIGH motorretning
			pwm_set_duty(20000+input);
		} else{
			PIOC->PIO_CODR = (1u << 23);  // LOW motorretning
			pwm_set_duty(20000-input);
		}
	}else{
		pwm_set_duty(20000);
	}
	
	
	if(solenoid_on == 1){
		solenoid_loopcounter += 1;
	}
	if((solenoid_on == 1) && (solenoid_loopcounter > 5)){
		solenoid_on = 0;
		solenoid_loopcounter = 0;
		PIOB->PIO_CODR = (1u << 14);  // LOW
	}
	
	//printf("Referanse: %d\n", posref);
	//printf("Error: %d \n", error);
	//printf("Pådrag: %d \n", input);
	//printf("Feilintegral: %d\n", error_integral/ 10);
	// Reading the Status Register (TC_SR) for TC1 Channel 0 clears the interrupt flag
	volatile uint32_t status = TC1->TC_CHANNEL[0].TC_SR;
}

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
	
    uart_init(F_CPU, 9600);
    printf("Starting node 2:\n\r");
	
	adc_init();
	//setup_adc_test();
	
	timer3_init();
	
	
	//Ordne pinne B14 (D53) som styringssignal til solenoide:
	// Enable PCIO clock
	PMC->PMC_PCER0 = (1u << ID_PIOB);

	PIOB->PIO_PER = (1u << 14);
	PIOB->PIO_OER = (1u << 14);
	
	//Styre solenoide
	//PIOB->PIO_SODR = (1u << 14);  // HIGH (solenoide på)
	//PIOB->PIO_CODR = (1u << 14);  // LOW (solenoide av)
	
	
	
	// Enable PCIO clock
    PMC->PMC_PCER0 = (1u << ID_PIOC);

    PIOC->PIO_PER = (1u << 23);
    PIOC->PIO_OER = (1u << 23);
	
	//PIOC->PIO_SODR = (1u << 23);  // HIGH
	PIOC->PIO_CODR = (1u << 23);  // LOW
	
	
	
	PMC->PMC_PCER0 = (1u << ID_PIOA);
	
	PIOA->PIO_PER = (1u << 28);
	
	PIOA->PIO_ODR = (1u << 28);
	// Disable Interrupt while setup
	PIOA->PIO_IDR = (1u << 28);
	// Enable Additional interrupt mode
	PIOA->PIO_AIMER = (1u << 28);
	// Enable Edge detection 
	PIOA->PIO_ESR = (1u << 28);
	// Enable Falling Edge detection
	PIOA->PIO_FELLSR = (1u << 28);
	// Clear any old pending interrupts
	(void)PIOA->PIO_ISR;
	// Enable interrupt
	PIOA->PIO_IER = (1u << 28);
	
	NVIC_ClearPendingIRQ(PIOA_IRQn);
	// Enable interrupt in NVIC (port A)
	NVIC_EnableIRQ(PIOA_IRQn);
	
	
	
	//PIOA->PIO_PUER = (1u << 28);
	
	//init av CAN
	can_init((CanInit){.brp = 21-1, .phase1 = 4-1, .phase2 = 4-1, .propag = 7-1, .sjw = 0}, 1);
	received_msg_flag = 0;
	
	CanMsg test_message;
	test_message.id	= 1;
	test_message.length = 1;
	test_message.byte[0] = 1;
	
	servo_pwm_init();
	timer_encoder_init();
	
	pwm_init();
	
	
	
	
	//can_tx(test_message);
	
	CanMsg message;
	
	servo_pwm_set_duty(2100);
	for(int i =0; i<1000000; i++){}
	servo_pwm_set_duty(1500);
	
	
	int16_t servopos = 1500;
	//int16_t error_integral = 0;
	//int16_t k_p = 3;
	//int16_t k_i = 1;
	
	int8_t right = 0;
	pwm_set_duty(15000);
    while(1) 
    {
		int16_t position = TC2->TC_CHANNEL[0].TC_CV;
		error = posref - position;
		
		
		//printf("Posisjon: %d \n",position);
		
		//int16_t error = posref - position;
		//error_integral += error/10;
		//
		//int16_t input = k_p * error; //+ k_i * error_integral;
		
		//if(input<0){
			//PIOC->PIO_SODR = (1u << 23);  // HIGH motorretning
			//pwm_set_duty(20000+input);
		//}else{
			//PIOC->PIO_CODR = (1u << 23);  // LOW motorretning
			//pwm_set_duty(20000-input);
		//}
		//printf("Posref: %d \n", posref);
		//printf("Error: %d\n",error);
		//printf("Pådrag: %d\n", input);
		
		
		//if (position > 1500 && !right){
			//pwm_set_duty(20000);
			////_delay_ms(10);
			//PIOC->PIO_SODR = (1u << 23);  // HIGH motorretning
			//
			//right = 1;
			//pwm_set_duty(15000);
		//}
		//if (position < 500 && right){
			//pwm_set_duty(20000);
			//
			//PIOC->PIO_CODR = (1u << 23);  // LOW motorretning
			//right = 0;
			//pwm_set_duty(15000);
		//}
		
		
		
		
		
		
		if(received_msg_flag){
			//Kode som kjøres når CAN melding mottas, oppdatert meldingsverdi i received_msg
			
			
			//printf("Data 1: %d\n", (int8_t)received_msg.byte[0]);
			if(received_msg.id == 0x0003){
				//Joystick knapp trykket
				servopos = (int16_t)(servopos - 2*(int8_t)received_msg.byte[0]);
				// printf("Data 1: %d\n", (int16_t)received_msg.byte[0]);
				if(servopos > 2100){
					servopos = 2100;
					}else if(servopos < 900){
					servopos = 900;
				}
				servo_pwm_set_duty(servopos);
				
				//touchpad
				posref = (int16_t)(((int8_t)received_msg.byte[4])*(-1)+100)*10;
				
				//printf("%d",received_msg.byte[2]);
				if(received_msg.byte[2]==1){
					PIOB->PIO_SODR = (1u << 14);
					solenoid_on = 1;
				}
				
				//PIOB->PIO_SODR = (1u << 14);  // HIGH (solenoide på)
			}	
			if(received_msg.id == 0x0005){
				//Joystick posisjon fra node 1
				
				// printf("Servopos: %d\n", servopos);
				servopos = (int16_t)(servopos - 2*(int8_t)received_msg.byte[0]);
				// printf("Data 1: %d\n", (int16_t)received_msg.byte[0]);
				if(servopos > 2100){
					servopos = 2100;
				}else if(servopos < 900){
					servopos = 900;
				}
				servo_pwm_set_duty(servopos);
			}
			if(received_msg.id == 0x0006){
				//Touchpad posisjon fra node 1
				
				//printf("Touchpad posisjon x: %d \n", (int8_t)received_msg.byte[1]);
				posref = (int16_t)(((int8_t)received_msg.byte[1])*(-1)+100)*10;
				//posref = (int8_t)received_msg.byte[1];
			}
			
			received_msg_flag = 0;
		}
		if(adc_down_flag){
			CanMsg goal = {.id = 4, .length = 1, .byte[0]=1};
			can_tx(goal);
			adc_down_flag = 0;
		}
		if(adc_up_flag){
			adc_up_flag = 0;
		}
			
    }
}

