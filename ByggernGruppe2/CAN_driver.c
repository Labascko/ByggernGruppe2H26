/*
 * CAN_driver.c
 *
 * Created: 28.09.2026 15:24:41
 *  Author: eskilod
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "CAN_driver.h"
#include "MCP2515_driver.h"

void CAN_init(void){
	MCP2515_reset();
	_delay_ms(10);
	
	//config mode
	MCP2515_bit_modify(0x0F, 0xE0, 0x80);
	
	//Timing som skal gi 250kB/s. 
	//Baud Rate Prescaler. Dele på 2
	MCP2515_bit_modify(0x2A, 0x3F, 2-1);
	//7 T_Q enheter som "gis" til propagation time
	MCP2515_bit_modify(0x29, 0x07, 7-1);
	//BTLMODE settes 1
	MCP2515_bit_modify(0x29, 0x80, 0x80);
	//4 T_Q enheter til PHSEG1
	MCP2515_bit_modify(0x29, 0x38, ((4-1)<<3));
	//4 T_Q enheter til PHSEG2
	MCP2515_bit_modify(0x28, 0x07, 4-1);
	
	//set loopback mode
	MCP2515_bit_modify(0x0F, 0xE0, 0x40);
}