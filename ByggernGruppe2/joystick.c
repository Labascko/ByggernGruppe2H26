/*
 * joystick.c
 *
 * Created: 07.09.2026 14:42:00
 *  Author: eskilod
 *
 * Work in progress...
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "joystick.h"
#include "adc.h"






void joy_calibrate_auto(joy_pos *diff){
	int8_t x = adc_read(0);
	int8_t y = adc_read(1);
	
	int8_t diff_x = 0x00 - x;
	int8_t diff_y = 0x00 - y;
	
	diff->x = diff_x;
	diff->y = diff_y;
}


joy_pos joy_pos_read(joy_pos diff){	
	joy_pos pos = {adc_read(0)+diff.x, adc_read(1)+diff.y};   
		
	if(pos.x > 0){
		pos.x = pos.x * 100/84;
	} else{
		pos.x = pos.x * 100/(97);
	}
		
	if(pos.y > 0){
		pos.y = pos.y * 100/86;
	} else{
		pos.y = pos.y * 100/85;
	}
	
	return pos;
}

joy_dir joy_dir_read(joy_pos diff){
	joy_pos pos = joy_pos_read(diff);
	
	if((abs(pos.x)<10) && (abs(pos.y)<10)){
		return NEUTRAL;
	}else if(abs(pos.x)>abs(pos.y)){
		if(pos.x>0){
			return RIGHT;
		} else {
			return LEFT;
		} 
	}else{
		if(pos.y>0){
			return UP;
		} else{
			return DOWN;
		}
	}
}

joy_pos touchpad_read(void){
	joy_pos pos = {(adc_read(2)-0x80)*100/127, (adc_read(3)-0x80)*100/127};  
	return pos;
}