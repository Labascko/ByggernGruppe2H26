/*
 * IO_board.c
 *
 * Created: 28.09.2026 07:08:41
 *  Author: eskilod
 */ 
#define F_CPU 4915200
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "spi_driver.h"
#include "IO_board.h"

Buttons read_IO_buttons(void){
	Buttons btns;
	slave_select(IO);
	spi_mastertransmit(0x04, IO);
	_delay_us(40);
	
	btns.right = spi_master_read(IO);
	_delay_us(4);
	//printf("0x%02X\n", SPDR);
	btns.left = spi_master_read(IO);
	_delay_us(4);
	btns.nav = spi_master_read(IO);
	return btns; 
}