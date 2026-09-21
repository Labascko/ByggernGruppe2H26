/*
 * OLED_driver.c
 *
 * Created: 21.09.2026 12:26:06
 *  Author: emredi
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "spi_driver.h"
#include "fonts.h"

void OLED_init(void){
	//PB0 brukes til å styre datatype
	//PB0 høy => pixel data, PB0 lav => kommando
	
	PORTB &= ~(1<<PB0);
	// Nødvedige kommandoer:
	//skru på display
	spi_mastertransmit(0xAF, OLED);
	
	//Velg Addressing mode:
	spi_mastertransmit(0x20, OLED);
	spi_mastertransmit(0x00, OLED);
	
	//endre retning:
	spi_mastertransmit(0xA1, OLED);
	spi_mastertransmit(0xC8, OLED);
}

void OLED_goto_line(int line){
	PORTB &= ~(1<<PB0);
	spi_mastertransmit(0x22, OLED);
	spi_mastertransmit(line, OLED);
	spi_mastertransmit(7, OLED);
}

void OLED_goto_column(int column){
	PORTB &= ~(1<<PB0);
	spi_mastertransmit(0x21, OLED);
	spi_mastertransmit(column, OLED);
	spi_mastertransmit(127, OLED);
}

void OLED_clear_line(int line){
	PORTB &= ~(1<<PB0);
	OLED_goto_line(line);
	OLED_goto_column(0);
	PORTB |= (1<<PB0);
	for(int u=0; u<128; u++){
		spi_mastertransmit(pgm_read_byte(&font8[0][0]), OLED);
	}
}

void OLED_reset(void){
	for(int i=0; i<8; i++){
		OLED_clear_line(i);
	}
}

void OLED_print(char* string, int n){
	PORTB |= (1<<PB0);
	//printf(&string[2]);
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