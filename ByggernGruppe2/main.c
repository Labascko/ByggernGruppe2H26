/*
 * ByggernGruppe2.c
 *
 * Created: 31.08.2026 08:40:21
 * Author : emredi
 */ 

/*
COM3 serial port
*/
#define F_CPU 4915200UL
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uartdriver.h"
#include "xmem_driver.h"
#include "sram_test.h"
#include "clock_gen.h"
#include "adc.h"
#include "joystick.h"
#include "spi_driver.h"
#include "OLED_driver.h"
#include "IO_board.h"
#include "CAN_driver.h"
#include "MCP2515_driver.h"

#define FOSC 4915200UL// Clock Speed

#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

ISR(INT0_vect) {
	char status = MCP2515_read_status();
	
}

volatile int8_t knapp_trykket = 0;
ISR(INT2_vect) {
	
	// Kode som kjøres når Timer0 feiler/trigger
	knapp_trykket = 1;
	//Skru av interrupt	
	GICR &= ~(1 << INT2);

	timer0_init();
}

volatile int8_t loops=0;
ISR(TIMER0_COMP_vect){
	if(loops <8){
		loops+=1;
	}else{
		loops=0;
		
		//Reset timer-innstilinger
		TCCR0 = 0;
		//reset flagg
		TIMSK &= ~(1 << OCIE0);
		
		GIFR |= (1<<INTF2);
		GICR |= (1 << INT2);
		//printf("Timer ferdig");
	}
	
	
}

int main(void)
{
	// Initialiseringsfunksjoner:
	uart_init(MYUBRR);
	xmem_init();	
	timer1_init();
	fdevopen(uart_putchar, NULL);
	init_joy_b();

	joy_pos joy_diff = {0,0};
	joy_calibrate_auto(&joy_diff);
	
	spi_masterinit();
	
	OLED_init();
	OLED_reset();
	
	OLED_goto_column(0);
	OLED_goto_line(0);
	//const char test[] = "Det funker!!TESTEST";
	//OLED_print(test, sizeof(test)/sizeof(test[0])-1);
	
	OLED_clear_line(1);
	
	
	 Menu main_menu;
	 
	 Menu sub_menu1;
	 sub_menu1.title = "Hovedmeny";
	 sub_menu1.num_items = 4;
	 sub_menu1.parent_menu = NULL;
	 sub_menu1.position = 0;
	 MenuItem item11 = {.text = "* Valg 1", .child_menu = &sub_menu1};
	 sub_menu1.items[0] = item11;
	 MenuItem item22 = {.text = "* Tilbake", .child_menu = &main_menu};
	 sub_menu1.items[1] = item22;


	 main_menu.title = "Hovedmeny";
	 main_menu.num_items = 4;
	 main_menu.parent_menu = NULL;
	 main_menu.position = 0;
	 MenuItem item1 = {.text = "* Valg 1", .child_menu = &sub_menu1};
	 main_menu.items[0] = item1;
	 MenuItem item2 = {.text = "* Valg 2", .child_menu = &sub_menu1};
	 main_menu.items[1] = item2;
		
		
	Menu* current_menu = &main_menu;
	
	//SRAM_test();
	//uart_transmit(uart_recieve());
	joy_dir direction = NEUTRAL;
	joy_dir new_direction = NEUTRAL;
	sei();
	
	
	MCP2515_reset();
	_delay_ms(10); // Gi MCP2515 litt tid til å starte opp

	
	
	
	
	while(1){
		//OLED_goto_column(0);
		//OLED_goto_line(0);
		//printf("%d ",joy_pos_read(joy_diff).x);	
		////OLED_reset();
		//char str[5];
		//snprintf(str, sizeof(str), "%d", joy_pos_read(joy_diff).x);
		//OLED_print(str, sizeof(str));
		//OLED_goto_line(2);
		//OLED_print("testasdasdsad", 8);
		new_direction = joy_dir_read(joy_diff);
		if(direction!=new_direction){
			if(new_direction == UP && current_menu->position>0){
				current_menu->position -= 1;
			}else if(new_direction == DOWN && current_menu->position<current_menu->num_items){
				current_menu->position += 1;
			}
		}
		direction = new_direction;
		
		
		OLED_print_menu(*current_menu);

		
		if (knapp_trykket) {
			knapp_trykket = 0; // Nullstill flagget
			
			// Kjør den treige koden her i stedet for i ISR-en:
			// (Forutsetter at UART er satt opp)
			OLED_reset();
			current_menu = (current_menu->items[current_menu->position].child_menu);
			//printf("knapp trykket\n");
		}

			
	}
	
	return 0;                                                                                                                                                                                                                                  
}

