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


//ISR(INT0_vect) {
	//printf("interrupt");
	//uint8_t intflags = MCP2515_spi_read(0x2C);
	//if(intflags & 0x01){
		//CAN_r0 = 1;
		//MCP2515_bit_modify(0x2E, 0x01, 0x00); // Tøm RX0IF
	//}
	//if(intflags & 0x02){
		//CAN_r1 = 1;
		//MCP2515_bit_modify(0x2E, 0x02, 0x00); // Tøm RX1IF
	//}
	////Skru av interrupt midlertidig
	//GICR &= ~(1 << INT0);
//}
volatile int8_t CAN_r0 = 0;
volatile int8_t CAN_r1 = 0;
volatile CAN_message CAN_r0_msg;
volatile CAN_message CAN_r1_msg;
ISR(INT0_vect) {
	// Les interrupt-flaggene fra MCP2515 (0x2E = CANINTF)
	uint8_t intflags = MCP2515_spi_read(0x2C);
	
	if (intflags & 0x01) { // RX0IF
		CAN_r0_msg = CAN_read(0); 
		MCP2515_bit_modify(0x2C, 0x01, 0x00); // Tøm RX0IF
		CAN_r0 = 1;
	}
	if (intflags & 0x02) { // RX1IF
		CAN_r1_msg = CAN_read(1); 
		MCP2515_bit_modify(0x2C, 0x02, 0x00); // Tøm RX1IF
		CAN_r1 = 1;
	}
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
	 sub_menu1.title = "Hovedmenytest";
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
	
	CAN_init();
	_delay_ms(100);

	
	

	
	//CAN_message test_message;
	//test_message.id = 0x0AAA;
	//test_message.length = 2;
	//test_message.data[0] = 0xAB;
	//test_message.data[1] = 0x11;
	//CAN_transmit(test_message.id, test_message.data, test_message.length);
	
	//_delay_ms(2000); //Teste med tid for å sende melding
	//
	//CAN_message received = CAN_read(0);
	//printf("%d",received.data[0]);
	//uint16_t id = 0;
	printf("Starting node 1: \n");
	while(1){
		//id += 1;
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
		
		char data[2] = {1,2};
		//CAN_transmit(id,data,2);
		//printf("test");

		//Behandlig av flagg satt av interrupts:
		//Reaksjon på trykking av knapp
		if (knapp_trykket) {
			knapp_trykket = 0; 
			OLED_reset();
			current_menu = (current_menu->items[current_menu->position].child_menu);
		}
		
		_delay_ms(1000);
		
		if (CAN_r0) {
			CAN_r0 = 0;
			printf("Mottatt melding på kanal 0 med id: %d\n", CAN_r0_msg.id);
		}
		if (CAN_r1) {
			CAN_r1 = 0;
			printf("Mottatt melding på kanal 1 med id: %d\n", CAN_r1_msg.id);
		}
		
	}
	
	
	
	return 0;                                                                                                                                                                                                                                  
}

