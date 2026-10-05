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


int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
	
    uart_init(F_CPU, 9600);
    printf("Starting node 2:\n\r");
	
    PMC->PMC_PCER0 = (1u << ID_PIOC);

    PIOC->PIO_PER = (1u << 21);
    PIOC->PIO_OER = (1u << 21);
	
	PIOC->PIO_SODR = (1u << 21);  // HIGH
	
	PIOC->PIO_CODR = (1u << 21);  // LOW
	
	
	
	//init av CAN
	
	
	
	can_init((CanInit){.brp = 21-1, .phase1 = 4-1, .phase2 = 4-1, .propag = 7-1, .sjw = 0}, 0);
	CanMsg test_message;
	test_message.id	= 5;
	test_message.length = 8;
	test_message.dword[0] = 628383;
	test_message.dword[1] = 612383;
	
	
	
	
	can_tx(test_message);
	
    while (1) 
    {
		for(int i = 0; i < 1000000; i++){
			PIOC->PIO_SODR = (1u << 21);  // HIGH
			
		}
		printf("kjører");
		for(int i = 0; i < 1000000; i++){
			PIOC->PIO_CODR = (1u << 21);  // LOW
			
		}
    }
}
