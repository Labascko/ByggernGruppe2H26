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
#include "clock_gen.h"
#include "adc.h"
#include "joystick.h"

#define FOSC 4915200UL// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1



int main(void)
{
	
	// Initialiseringsfunksjoner:
	uart_init(MYUBRR);
	xmem_init();	
	clock_gen_init();
	fdevopen(uart_putchar, NULL);
	

	joy_pos joy_diff = {0,0};
	joy_calibrate_auto(&joy_diff);

	
	//SRAM_test();
	//uart_transmit(uart_recieve());
	
	while(1){
		
		
		printf("%d ",joy_dir_read(joy_diff));	
		
	}
	
	return 0;                                                                                                                                                                                                                                  
}

