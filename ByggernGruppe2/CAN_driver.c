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
#include "spi_driver.h"

void CAN_init(void){
	//Reset can-kontroller ved oppstart
	MCP2515_reset();
	_delay_ms(10);
	
	//config mode
	MCP2515_bit_modify(0x0F, 0xE0, 0x80);
	
	//Timing som skal gi 250kB/s. 
	//Baud Rate Prescaler. Dele p� 2
	MCP2515_bit_modify(0x2A, 0x3F, 2-1);
	//7 T_Q enheter som "gis" til propagation time
	MCP2515_bit_modify(0x29, 0x07, 7-1);
	//BTLMODE settes 1
	MCP2515_bit_modify(0x29, 0x80, 0x80);
	//4 T_Q enheter til PHSEG1
	MCP2515_bit_modify(0x29, 0x38, ((4-1)<<3));
	//4 T_Q enheter til PHSEG2
	MCP2515_bit_modify(0x28, 0x07, 4-1);
	
	
	// Sett RXB0 til � ta imot ALLE meldinger (skru av filtere) + aktiver Rollover (BUKT=1)
	//MCP2515_bit_modify(0x60, 0x64, 0x64);
	// Sett RXB0 til � ta imot ALLE meldinger (skru av filtere) + UTEN Rollover (BUKT=0)
	MCP2515_bit_modify(0x60, 0x60, 0x60);

	// Sett RXB1 til � ta imot ALLE meldinger
	MCP2515_bit_modify(0x70, 0x60, 0x60);
	
	//skru p� interrupts p� RX0 og RX1
	MCP2515_bit_modify(0x2B, 0x03, 0x03);
	
	//Skru p� AVR sin INT0
	GICR |= (1 << INT0);
	
	//sett i loopback mode (Ble brukt for å teste)
	//MCP2515_bit_modify(0x0F, 0xE0, 0x40);
	//_delay_ms(10);

	//sett i Normal mode
	MCP2515_bit_modify(0x0F, 0xE0, 0x00);
	
}

uint8_t CAN_transmit(CAN_message message){
	uint16_t tx_base = 0x00;
	
	//high inneholder 8 mest signifikante, low inneholder 3 minst signifikante �verst, fordi CAN bruker 11-bits ID
	uint8_t id_low = (uint8_t)((message.id&0x07)<<5);
	uint8_t id_high = (uint8_t)(message.id >> 3);
	
	if(!(MCP2515_spi_read(0x30)&(1<<3))){
		//Buffer 0 klar for ny melding
		tx_base = 0x30;
	}else if(!(MCP2515_spi_read(0x40)&(1<<3))){
		//Buffer 1 klar for ny melding
		tx_base = 0x40;
	}else if(!(MCP2515_spi_read(0x50)&(1<<3))){
		//Buffer 2 klar for ny melding
		tx_base = 0x50;
	}else{
		//Ingen kanaler ledige, returner 0 for å indikere at funksjonen feilet
		return 0;
	}
	
	//Sett id
	MCP2515_spi_write(tx_base+1, id_high);
	MCP2515_spi_write(tx_base+2, id_low);
	
	//sett lengde
	MCP2515_spi_write(tx_base+5, message.length);
	
	//sett data
	for(uint16_t i=0; i<message.length; i++){
		if(i>7){
			break;
		}
		MCP2515_spi_write(tx_base+6+i, message.data[i]);
	}
	
	//Skru på request to send på fitt kanal
	if(tx_base == 0x30) {
		MCP2515_RTS(0x01);
	} else if(tx_base == 0x40) {
		MCP2515_RTS(0x02);
	} else if(tx_base == 0x50) {
		MCP2515_RTS(0x04);
	}
	
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
		//Ugyldig verdi for buffer sendti inn, returnerer tom melding
		CAN_message empty = {0,0};
		return empty;
	}
	
	CAN_message message;
	//Les id (med shift fordi id er 11 bit lang)
	message.id = (MCP2515_spi_read(rx_base+1)<<3);	
	message.id |= (MCP2515_spi_read(rx_base+2)>>5);
	
	message.length = MCP2515_spi_read(rx_base+5);
	for(uint8_t i=0; i<message.length; i++){
		if(i>7){
			// kutt loopen om message.length feilaktig har fått en verdi større en CAN-meldingene kan inneholde
			break;
		}
		//legg til data i melding
		message.data[i] = MCP2515_spi_read(rx_base+6+i);
	}

	return message;
}
