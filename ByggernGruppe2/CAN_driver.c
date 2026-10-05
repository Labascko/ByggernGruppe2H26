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
	
	//skru på interrupts på RX0 og RX1
	MCP2515_bit_modify(0x2B, 0x03, 0x03);
	
	//Skru på AVR sin INT0
	GICR |= (1 << INT0);
	
	//set loopback mode
	MCP2515_bit_modify(0x0F, 0xE0, 0x40);
}

uint8_t CAN_transmit(uint16_t id, char* data, uint8_t length){
	//Prior to sending the message, the MCU must initialize the CANINTE.TXInE bit to enable or disable the generation of an interrupt when the message is sent.
	
	//The TXBnCTRL.TXREQ bit must be clear (indicating the transmit buffer is not pending transmission) before writing to the transmit buffer.
	uint16_t tx_base = 0x00;
	
	//high inneholder 8 mest signifikante, low inneholder 3 minst signifikante øverst
	uint8_t id_low = (uint8_t)((id&0x07)<<5);
	uint8_t id_high = (uint8_t)(id >> 3);
	
	if(!((0x30)&(1<<3))){
		//Buffer 0 klar for ny melding
		tx_base = 0x30;
	}else if(!(MCP2515_spi_read(0x40)&(1<<3))){
		//Buffer 1 klar for ny melding
		tx_base = 0x40;
	}else if(!(MCP2515_spi_read(0x50)&(1<<3))){
		//Buffer 2 klar for ny melding
		tx_base = 0x50;
	}else{
		return 0;
	}
	
	//Sett id
	MCP2515_spi_write(tx_base+1, id_high);
	MCP2515_spi_write(tx_base+2, id_low);
	
	//sett lengde
	MCP2515_spi_write(tx_base+5, length);
	
	//sett data
	for(uint16_t i=0; i<length; i++){
		if(i>7){
			break;
		}
		MCP2515_spi_write(tx_base+6+i, data[i]);
	}
	
	//Skru på request to send
	MCP2515_bit_modify(tx_base,0x08,0x08);
	
	return 1;
}

CAN_message CAN_read(uint8_t buffer){
	
	uint16_t rx_base = 0x00;
	if(buffer == 0){
		//Les fra buffer 0 
		rx_base = 0x60;
	}else if(buffer == 1){
		//Les fra buffer 1 
		rx_base = 0x70;
	}else{
		CAN_message empty = {0,0};
		return empty;
	}
	
	CAN_message message;
	//Les id
	message.id = (MCP2515_spi_read(rx_base+1)<<3);	
	message.id |= (MCP2515_spi_read(rx_base+2)>>5);
	
	message.length = MCP2515_spi_read(rx_base+5);
	for(uint8_t i=0; i<message.length; i++){
		if(i>7){
			break;
		}
		message.data[i] = MCP2515_spi_read(rx_base+6+i);
	}
	
	
	
	return message;
}
