/*
 * spi_driver.c
 *
 * Created: 21.09.2026 08:56:05
 *  Author: emredi
 */

#include <avr/io.h>
#include <stdio.h>
#include "spi_driver.h"

void spi_masterinit(void)
{
	//Sett MOSI og SCK som output, alle andre porter som input
	DDRB = (1<<DDB5)|(1<<DDB7)|(1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3)|(1<<DDB4);
	//Setter de 4 laveste pinnene på port b til HØY
	PORTB |= (0b1111);

	//Skru på SPI som Master, clock rate fck/16 => 307200
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void spi_mastertransmit(char cData, Slave slave)
{
	//Vel slaven som skal motta melding
	slave_select(slave);	
	//Start overføring
	SPDR = cData;
	//Vent til overføring er ferdig
	while(!(SPSR & (1<<SPIF)));
}

void spi_mastertransmit_bytes(char* cData, int n, Slave slave){
	//Løkke som sender n bytes med spi_mastertransmit()
	for(int i=0; i<n; i++){
		spi_mastertransmit(cData[i], slave);
	}
}

char spi_master_read(Slave slave){
	//Send dummy-byte for å oppdatere registeret
	spi_mastertransmit(0x00, slave);
	return SPDR;
}

void spi_slaveinit(void)
{
	//Funksjon for å initialisere ATmega-162 som spi-slave
	//Sett MISO som output, alle andre som input
	DDRB = (1<<DDB6);
	//Skru på SPI
	SPCR = (1<<SPE);
}

char spi_slavereceive(void)
{
	//Ta imot data som slave
	//Vent på overføring
	while(!(SPSR & (1<<SPIF)));
	//Returner data
	return SPDR;
}

void slave_select(Slave slave){	
	//Skru av pinne for ønsket slave, og andre slaver høy
	uint8_t mask = (1<<PB1)|(1<<PB2)|(1<<PB3);
	if(slave == NONE){
		PORTB |= mask;
	} else{
		PORTB = (PORTB|mask) &~(1<<(slave+1)); 
	}
}