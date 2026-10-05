/*
 * CAN_driver.h
 *
 * Created: 28.09.2026 15:24:54
 *  Author: eskilod
 */ 


#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_

typedef struct {
	uint16_t id;
	uint8_t length;
	char data[8];
}CAN_message;

void CAN_init(void);
uint8_t CAN_transmit(uint16_t id, char* data, uint8_t length);
CAN_message CAN_read(uint8_t buffer);


#endif /* CAN_DRIVER_H_ */