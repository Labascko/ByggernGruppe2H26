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
#include "spi_driver.h"
#include "OLED_driver.h"

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
	init_joy_b();

	joy_pos joy_diff = {0,0};
	joy_calibrate_auto(&joy_diff);
	
	spi_masterinit();
	
	OLED_init();
	OLED_reset();
	
	OLED_goto_column(0);
	OLED_goto_line(0);
	const char test[] = "Det funker!!TESTEST";
	OLED_print(test, sizeof(test)/sizeof(test[0])-1);
	
	OLED_clear_line(1);
	
	//SRAM_test();
	//uart_transmit(uart_recieve());
	
	while(1){
		OLED_goto_column(0);
		OLED_goto_line(0);
		printf("%d ",joy_pos_read(joy_diff).x);	
		//OLED_reset();
		char str[5];
		snprintf(str, sizeof(str), "%d", joy_pos_read(joy_diff).x);
		OLED_print(str, sizeof(str));
		
		

			
	}
	
	return 0;                                                                                                                                                                                                                                  
}

