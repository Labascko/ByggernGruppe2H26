/*
 * MCP2515_driver.h
 *
 * Created: 28.09.2026 17:07:04
 *  Author: eskilod
 */ 


#ifndef MCP2515_DRIVER_H_
#define MCP2515_DRIVER_H_

char MCP2515_spi_read(uint8_t address);
void MCP2515_spi_write(uint8_t address, uint8_t data);
void MCP2515_RTS(char address); //usikker på hva som må med her
char MCP2515_read_status(void); //^
void MCP2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data); //^ osv...
void MCP2515_reset(void);


#endif /* MCP2515_DRIVER_H_ */