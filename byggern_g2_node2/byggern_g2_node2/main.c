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

volatile int IR_flag = 0;


void PIOA_Handler(void)
{
	uint32_t status = PIOA->PIO_ISR;
	printf("Interruptfuksjon kjører");
	if (status & (1u << 28))
	{
		IR_flag = 1;
	}
}

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
	
    uart_init(F_CPU, 9600);
    printf("Starting node 2:\n\r");
	
	
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
	
	pwm_init();
	
	
	
	
	
	//can_tx(test_message);
	
	CanMsg message;
	
	//pwm_set_duty(2100);
	for(int i =0; i<1000000; i++){}
	pwm_set_duty(1500);
	
	
	int16_t servopos = 1500;
    while(1) 
    {
		
		
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
				pwm_set_duty(servopos);
				
			}
			
			received_msg_flag = 0;
		}
		if (IR_flag)
		{
			printf("IR interrupt");
			IR_flag = 0;
		}
			
    }
}

