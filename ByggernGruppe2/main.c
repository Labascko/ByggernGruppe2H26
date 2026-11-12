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
		//MCP2515_bit_modify(0x2E, 0x01, 0x00); // T�m RX0IF
	//}
	//if(intflags & 0x02){
		//CAN_r1 = 1;
		//MCP2515_bit_modify(0x2E, 0x02, 0x00); // T�m RX1IF
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
		MCP2515_bit_modify(0x2C, 0x01, 0x00); // T�m RX0IF
		CAN_r0 = 1;
	}
	if (intflags & 0x02) { // RX1IF
		CAN_r1_msg = CAN_read(1); 
		MCP2515_bit_modify(0x2C, 0x02, 0x00); // T�m RX1IF
		CAN_r1 = 1;
	}
}

// Kommentar
volatile int8_t joystick_pressed = 0;
ISR(INT2_vect) {
	
	// Kode som kj�res n�r Timer0 feiler/trigger
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
int main(void)
{
	// Initialiseringsfunksjoner:

	//Initialiser UART
	uart_init(MYUBRR);
	//Link UART til printf()
	fdevopen(uart_putchar, NULL);

	//Initialiser SRAM
	xmem_init();	

	//Initialiser timer 1
	timer1_init();

	//Initialiser joystick knapp
	init_joy_b();

	//Kalibrer joystick ved å måle posisjon ved oppstart og trekke fra ved senere målinger
	joy_pos joy_diff = {0,0};
	joy_calibrate_auto(&joy_diff);
	//Variabler for å bestemme joystick retning
	joy_dir direction = NEUTRAL;
	joy_dir new_direction = NEUTRAL;

	//Variabel for å holde oversikt over mål
	int8_t goals = 0;
	
	//Initialiser SPI
	spi_masterinit();
	
	//Initialiser OLED, og gå til (0,0)
	OLED_init();
	OLED_reset();
	OLED_goto_column(0);
	OLED_goto_line(0);
	OLED_clear_line(1);
	
	//Initialiser menyene slik at de inneholder ønsket informasjon
	init_menus();

	//set enable interrupt (Skru på global interrupts) (cli() kan brukes for å skru av)
	sei();
	
	//Initialiser CAN-kontroller 
	CAN_init();
	_delay_ms(100);


	printf("Starting node 1: \n");
	while(1){
		//Mål retning på joystick
		new_direction = joy_dir_read(joy_diff);
		if(direction!=new_direction){
			//FLytt posisjon i menyen ved endring i joystick-retning
			if(new_direction == UP && current_menu->position>0){
				current_menu->position -= 1;
				}else if(new_direction == DOWN && current_menu->position<current_menu->num_items){
				current_menu->position += 1;
			}
		}
		direction = new_direction;

		//Skriv valgt meny til skjermen
		OLED_print_menu(*current_menu);
		
		if(current_menu->title == "spill"){
			//Send informasjon til node 2 for å styre spillet
			controls_send(joy_pos_read(joy_diff), touchpad_read(), joystick_pressed);

			if(joystick_pressed == 1){
				//fjern et mål dersom joystick knapp trykkes. (Støy fra solenoide gir konsekvent en scoring hver gang den fyrer)
				goals = goals - 1;
			}
			
			if(goals >= 1){
				current_menu = &sub_menu3;
				goals = 0;
			}	
		}
		

		//Utkast for å printe bilde til skjermen.
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
		if (joystick_pressed) {	
			//Behandle interrupt flagg fra joystick knapp
			joystick_pressed = 0; 
			OLED_reset();
			current_menu = (current_menu->items[current_menu->position].child_menu);
		}
		
		if (CAN_r0) {
			CAN_r0 = 0;
			//Behandle interrupt flagg for mottatt melding på kanal 0
			if(CAN_r0_msg.id == 4){
				goals += 1;
			}
			printf("Goals: %d \n", goals);
		}
		if (CAN_r1) {
			CAN_r1 = 0;
			//Behandle interrupt flagg for mottatt melding på kanal 1
			if(CAN_r0_msg.id == 4){
				goals += 1;
			}
			printf("Goals: %d \n", goals);
		}
	}
	return 0;                                                                                                                                                                                                                                  
}

