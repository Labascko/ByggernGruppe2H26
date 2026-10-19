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
#include "CAN_driver.h"


void init_joy_b(void){
	DDRE &= ~(1 << PE0);
	PORTE |= (1 << PE0);
	EMCUCR &= ~(1 << ISC2);
	GICR |= (1 << INT2);
}

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
	
	if((abs(pos.x)<50) && (abs(pos.y)<50)){
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

void joy_send(joy_pos pos){
	CAN_message send_pos;
	send_pos.id = 0x0005;
	send_pos.data[0] = pos.x;
	send_pos.data[1] = pos.y;
	send_pos.length = 2;
	CAN_transmit(send_pos);
}