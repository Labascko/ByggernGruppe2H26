/*
 * OLED_driver.c
 *
 * Created: 21.09.2026 12:26:06
 *  Author: emredi
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "spi_driver.h"
#include "fonts.h"
#include "OLED_driver.h"

void OLED_init(void){
	//PB0 brukes til � styre datatype
	//PB0 h�y => pixel data, PB0 lav => kommando
	PORTB &= ~(1<<PB0);
	// N�dvedige kommandoer:
	//skru p� display
	spi_mastertransmit(0xAF, OLED);
	
	//Velg Addressing mode:
	spi_mastertransmit(0x20, OLED);
	spi_mastertransmit(0x00, OLED);
	
	//endre retning:
	spi_mastertransmit(0xA1, OLED);
	spi_mastertransmit(0xC8, OLED);
}

void OLED_goto_line(int line){
	//Kommando
	PORTB &= ~(1<<PB0);
	spi_mastertransmit(0x22, OLED);
	spi_mastertransmit(line, OLED);
	spi_mastertransmit(7, OLED);
}

void OLED_goto_column(int column){
	//kommando
	PORTB &= ~(1<<PB0);
	spi_mastertransmit(0x21, OLED);
	spi_mastertransmit(column, OLED);
	spi_mastertransmit(127, OLED);
}

void OLED_clear_line(int line){
	//kommando
	PORTB &= ~(1<<PB0);
	OLED_goto_line(line);
	OLED_goto_column(0);
	//pixeldata
	PORTB |= (1<<PB0);
	for(int u=0; u<128; u++){
		spi_mastertransmit(pgm_read_byte(&font8[0][0]), OLED);
	}
}

void OLED_reset(void){
	//Fjern alle linjer
	for(int i=0; i<8; i++){
		OLED_clear_line(i);
	}
}

void OLED_print(char* string, int n){
	//pixeldata
	PORTB |= (1<<PB0);
	for(int i=0; i<n; i++){	
		for(int u=0; u<8; u++){
			if((int)string[i]-32 < sizeof(font8)){
				spi_mastertransmit(pgm_read_byte(&font8[(int)string[i]-32][u]), OLED);
			}else{
				spi_mastertransmit(~pgm_read_byte(&font8[(int)string[i]-32][u]), OLED);
			}
		}
	}
}

void OLED_print_inverted(char* string, int n){
	//pixeldata (lik som funksjonen over, men invertert for bokstavene)
	PORTB |= (1<<PB0);
	for(int i=0; i<n; i++){
		for(int u=0; u<8; u++){
			if((int)string[i]-32 < sizeof(font8)){
				spi_mastertransmit(~pgm_read_byte(&font8[(int)string[i]-32][u]), OLED);
				}else{
				spi_mastertransmit(~pgm_read_byte(&font8[(int)string[i]-32][u]), OLED);
			}
		}
	}
}

void OLED_print_menu(Menu menu){
	OLED_goto_column(0);
	OLED_goto_line(0);
	//Print menytittel
	OLED_print(menu.title, strlen(menu.title));
	//for-løkke som printer innholdet i menyen
	for(int i=0; i<menu.num_items; i++){
		OLED_goto_column(10);
		OLED_goto_line(i+1);
		if(i==menu.position){
			OLED_print_inverted(menu.items[i].text, strlen(menu.items[i].text));
		}else{
			OLED_print(menu.items[i].text, strlen(menu.items[i].text));
		}
	}
}