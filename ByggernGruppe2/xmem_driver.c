
#include <avr/io.h>
#include <stdio.h>
#include "xmem_driver.h"

//Kode hentet fra labforelesning 2
#define BASE_ADDRESS 0x1800

void xmem_init(void){
	//Skru på XMEM/eksternt minne
	MCUCR |= (1<<SRE);
	//Bestemmer hvilke pinner som skal maskeres vekk (Lar de to "øverste" pinnene brukes til andre ting)
	SFIOR |= (1<<XMM0); 
}

void xmem_write(uint8_t data, uint16_t addr){
	//Peker som går til "baseaddresse" for Xmem
	volatile char * ext_mem = (char *) BASE_ADDRESS;
	//Legg data ved addresse
	ext_mem[ addr ] = data; 
}

uint8_t xmem_read(uint16_t addr){
	//Peker som går til "baseaddresse" for Xmem
	volatile char * ext_mem = (char *) BASE_ADDRESS;
	//les data ved addresse
	uint8_t ret_val = ext_mem[ addr ];
	return ret_val;
}