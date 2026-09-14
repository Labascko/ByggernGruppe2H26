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

typedef struct {
	int8_t x;
	int8_t y;
}joy_pos;

typedef enum {
	LEFT,
	RIGHT,
	UP,
	DOWN,
	NEUTRAL
}joy_dir;
	
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

joy_pos joy_pos_read(joy_pos diff); 
void joy_calibrate_auto(joy_pos *diff);

joy_pos touchpad_read(void);
joy_dir joy_dir_read(joy_pos diff);


#endif /* JOYSTICK_H_ */