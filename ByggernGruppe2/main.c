/*
 * ByggernGruppe2.c
 *
 * Created: 31.08.2026 08:40:21
 * Author : emredi
 */ 

/*
COM3 serial port
*/

#include <avr/io.h>
#include <stdio.h>
#include "uartdriver.h"
#include "xmem_driver.h"
#include "sram_test.h"

#define FOSC 4915200UL// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

int main(void)
{
    /* Replace with your application code */
	
    //DDRA = 0x51;
	
	uart_init(MYUBRR);
	xmem_init();	
	
	fdevopen(uart_putchar, NULL);
	/*uart_transmit('a');*/
	
	
	xmem_write(0xFF, 0x0000);
	
	SRAM_test();
	
	while(1){
		
		/*for(int i = 0; i < 10000; i++){
			PORTA = 0x51;
		}
		for(int i = 0; i < 10000; i++){
			PORTA = 0x00;
		}*/
		/*uart_transmit('b');
		unsigned char message = uart_recieve() + 1;*/
		//uart_transmit(uart_recieve());
		
		//printf("test");
		
		
		
		
	}
	/*
	uart_init(MYUBRR);
	while(1){
		uart_transmit('a');
	}
	*/
	return 0;                                                                                                                                                                                                                                  
}

