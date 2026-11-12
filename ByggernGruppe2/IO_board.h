/*
 * IO_board.h
 *
 * Created: 28.09.2026 07:08:59
 *  Author: eskilod
 */ 


#ifndef IO_BOARD_H_
#define IO_BOARD_H_

//Hentet fra "TTK4155 User IO-board"
typedef struct __attribute__((packed)) {
	union {
		uint8_t right;
		struct {
			uint8_t R1:1;
			uint8_t R2:1;
			uint8_t R3:1;
			uint8_t R4:1;
			uint8_t R5:1;
			uint8_t R6:1;
		};
	};
	union {
		uint8_t left;
		struct {
			uint8_t L1:1;
			uint8_t L2:1;
			uint8_t L3:1;
			uint8_t L4:1;
			uint8_t L5:1;
			uint8_t L6:1;
			uint8_t L7:1;
		};
	};
	union {
		uint8_t nav;
		struct {
			uint8_t NB:1;
			uint8_t NR:1;
			uint8_t ND:1;
			uint8_t NL:1;
			uint8_t NU:1;
		};
	};
} Buttons;

Buttons read_IO_buttons(void);



#endif /* IO_BOARD_H_ */