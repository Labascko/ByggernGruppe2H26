/*
 * MCP2515_driver.c
 *
 * Created: 28.09.2026 17:06:51
 *  Author: eskilod
 */ 
#include <avr/io.h>
#include <stdio.h>
#include "MCP2515_driver.h"
#include "spi_driver.h"




char MCP2515_spi_read(uint8_t address){
	spi_mastertransmit(0x03, CAN); //read kommando
	spi_mastertransmit(address, CAN);
	char recieved = spi_master_read(CAN);//dummybyte
	slave_select(NONE);
	return recieved;
}

void MCP2515_spi_write(uint8_t address, uint8_t data) {
	spi_mastertransmit(0x02, CAN); // WRITE-kommando
	spi_mastertransmit(address, CAN);
	spi_mastertransmit(data, CAN);
	slave_select(NONE);
}

void MCP2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data){
	spi_mastertransmit(0x05, CAN); //0000 0101, bit modify
	spi_mastertransmit(address, CAN);
	spi_mastertransmit(mask, CAN);
	spi_mastertransmit(data, CAN);
	slave_select(NONE);
}

void MCP2515_reset(void){
	spi_mastertransmit(0xC0,CAN); //1100 0000, reset
	slave_select(NONE);
}

char MCP2515_read_status(void){
	spi_mastertransmit(0xA0, CAN);
	char result = spi_master_read(CAN);
	slave_select(NONE);
	return result;
}

void MCP2515_RTS(char address){
	if(address <= 7){
		spi_mastertransmit(0x80+address, CAN);
	}
}