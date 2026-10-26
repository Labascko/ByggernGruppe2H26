/*
 * byggern_g2_node2.c
 *
 * Created: 05.10.2026 13:17:52
 * Author : eskilod
 */ 

#include <stdio.h>

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

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
	
    uart_init(F_CPU, 9600);
    printf("Starting node 2:\n\r");
	
	adc_init();
	//setup_adc_test();
	
	
	
	// Enable PCIO clock
    //PMC->PMC_PCER0 = (1u << ID_PIOC);

    //PIOC->PIO_PER = (1u << 21);
    //PIOC->PIO_OER = (1u << 21);
	
	//PIOC->PIO_SODR = (1u << 21);  // HIGH
	
	//PIOC->PIO_CODR = (1u << 21);  // LOW
	
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
    while(1) 
    {
		
		//uint32_t val = read_adc_value();
		//printf("ADC: %d \n", val);
		//pwm_set_duty(2000);
		 //can_rx(&message);
		 //printf("Data 1: %d\n", received_msg.byte[1]);
		
		/*for(int i = 0; i < 1000000; i++){
			PIOC->PIO_SODR = (1u << 21);  // HIGH
			
		}
		printf("kjører");
		for(int i = 0; i < 1000000; i++){
			PIOC->PIO_CODR = (1u << 21);  // LOW
		}*/	
		
		if(PIOA->PIO_PDSR & (1u << 28)){
			//printf("Input: HIGH\n");
		}else{
			//printf("Input: LOW\n");
		}
		
		
		
		uint32_t posisjon = TC2->TC_CHANNEL[0].TC_CV;
		printf("Posisjon: %d \n",posisjon);
		
		
		
		if(received_msg_flag){
			//Kode som kjøres når CAN melding mottas, oppdatert meldingsverdi i received_msg
			
			
			//printf("Data 1: %d\n", (int8_t)received_msg.byte[0]);
			
			if(received_msg.id == 0x0005){
				// printf("Servopos: %d\n", servopos);
				//Meldingen har id som hører til joystickposisjon
				servopos = (int16_t)(servopos - 2*(int8_t)received_msg.byte[0]);
				// printf("Data 1: %d\n", (int16_t)received_msg.byte[0]);
				if(servopos > 2100){
					servopos = 2100;
				}else if(servopos < 900){
					servopos = 900;
				}
				servo_pwm_set_duty(servopos);
				
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

