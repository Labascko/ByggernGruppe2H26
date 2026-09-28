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
	/* Set MOSI and SCK output, all others input */
	DDRB = (1<<DDB5)|(1<<DDB7)|(1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3)|(1<<DDB4);
	PORTB |= (0b1111);
	/* Enable SPI, Master, set clock rate fck/16,  307200 Hz */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void spi_mastertransmit(char cData, Slave slave)
{
	slave_select(slave);
	
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

void spi_mastertransmit_bytes(char* cData, int n, Slave slave){
	for(int i=0; i<n; i++){
		spi_mastertransmit(cData[i], slave);
	}
}

char spi_master_read(Slave slave){
	spi_mastertransmit(0x00, slave);
	return SPDR;
}

void spi_slaveinit(void)
{
	/* Set MISO output, all others input */
	DDRB = (1<<DDB6);
	/* Enable SPI */
	SPCR = (1<<SPE);
}

char spi_slavereceive(void)
{
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)))
	;
	/* Return data register */
	return SPDR;
}

void slave_select(Slave slave){	
	uint8_t mask = (1<<PB1)|(1<<PB2)|(1<<PB3);
	if(slave == NONE){
		PORTB |= mask;
	} else{
		PORTB = (PORTB|mask) & ~(1<<(slave+1)); 
	}
}