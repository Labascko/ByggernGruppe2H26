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
#include "menus.h"
//#include "animation.h"

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
// Kommentar
volatile int8_t CAN_r0 = 0;
volatile int8_t CAN_r1 = 0;

// Kommentar
volatile CAN_message CAN_r0_msg;
volatile CAN_message CAN_r1_msg;
ISR(INT0_vect) {
	// Les interrupt-flaggene fra MCP2515 (0x2E = CANINTF)
	uint8_t intflags = MCP2515_spi_read(0x2C);
	if(intflags == 0xFF){
		intflags = MCP2515_spi_read(0x2C);
	}
	
	//printf("Intflags i hex: 0x%X\n", intflags);
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

// Kommentar
volatile int8_t joystick_pressed = 0;
ISR(INT2_vect) {
	
	// Kode som kjøres når Timer0 feiler/trigger
	joystick_pressed = 1;
	//Skru av interrupt	
	GICR &= ~(1 << INT2);

	timer0_init();
}

// Kommentar
volatile int8_t loops = 0;
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

// game_state: forteller hvilket state spillet er i
// states:
// 0: meny
// 1: spill 1
// 2: animation on oled
// 3: game over
volatile int8_t high_score = 0;
volatile int8_t current_score = 0;
volatile int8_t game_state = 0;

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
	OLED_clear_line(1);
	
	init_menus();
	
	//SRAM_test();
	//uart_transmit(uart_recieve());
	joy_dir direction = NEUTRAL;
	joy_dir new_direction = NEUTRAL;
	sei();
	
	
	MCP2515_reset();
	_delay_ms(10); // Gi MCP2515 litt tid til å starte opp
	
	CAN_init();
	_delay_ms(100);
	printf("Starting node 1: \n");
	
	int8_t goals = 0;
	while(1){
		
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
		
		if(current_menu->title == "spill"){
			// spill logikk -> game over
			controls_send(joy_pos_read(joy_diff), touchpad_read(), joystick_pressed);
			if(joystick_pressed == 1){
				goals = goals - 1;
			}
			
			if(goals >= 1){
				current_menu = &sub_menu3;
				goals = 0;
			}
			
			
		}else if (game_state == 1){
			// meny logikk -> spill/animasjon
		}else if (game_state == 2){
			// animasjonsvindu logikk () -> meny
		}else{
			// game over (tid) -> meny/spill
		}
		
		
		//OLED_goto_column(0);
		//OLED_goto_line(0);
		////OLED_reset();
		//PORTB |= (1<<PB0);
		//for(uint8_t i = 0; i < 128; i++){
			//for(uint8_t u = 0; u < 8; u++){
				//spi_mastertransmit(pgm_read_byte(&frame0[i][u]), OLED);
			//}
		//}
		
		
		//Behandlig av flagg satt av interrupts:
		//Reaksjon på trykking av knapp
		
		if (joystick_pressed) {		
			joystick_pressed = 0; 
			OLED_reset();
			current_menu = (current_menu->items[current_menu->position].child_menu);
		}
	
		
		
		if (CAN_r0) {
			CAN_r0 = 0;
			// Mottatt melding på kanal 0
			if(CAN_r0_msg.id == 4){
				goals += 1;
			}
			printf("Goals: %d \n", goals);
		}
		if (CAN_r1) {
			CAN_r1 = 0;
			//Mottatt melding på kanal 1
			if(CAN_r0_msg.id == 4){
				goals += 1;
			}
			printf("Goals: %d \n", goals);
		}
	}
	return 0;                                                                                                                                                                                                                                  
}

