/*
 * joystick.h
 *
 * Created: 07.09.2026 14:42:23
 *  Author: eskilod
 *
 * Work in progress...
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

typedef struct JoyPos {
	int8_t x;
	int8_t y;
	};

typedef enum JoyDir {
	LEFT,
	RIGHT,
	UP,
	DOWN,
	NEUTRAL
	};
	
/*
typedef enum JoyDirX {
	LEFT,
	RIGHT,
	NEUTRAL
};

typedef enum JoyDirY {
	UP,
	DOWN,
	NEUTRAL
};

*/

#if 0
JoyPos joy_pos_read(); 
JoyDir joy_dir_read();
#endif

#endif /* JOYSTICK_H_ */