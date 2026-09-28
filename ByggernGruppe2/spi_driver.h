/*
 * spi_driver.h
 *
 * Created: 21.09.2026 08:56:44
 *  Author: emredi
 */ 


#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

typedef enum {
	CAN,
	OLED,
	IO,
	NONE
}Slave;

void spi_masterinit(void);

void spi_mastertransmit(char cData, Slave slave);

char spi_master_read(Slave slave);

void spi_mastertransmit_bytes(char* cData, int n, Slave slave);

void spi_slaveinit(void);

char spi_slavereceive(void);



void slave_select(Slave slave);
/*
void spi_write(void);

void spi_read(void);

void spi_write_bytes();

void spi_read_bytes(void);
*/
#endif /* SPI_DRIVER_H_ */