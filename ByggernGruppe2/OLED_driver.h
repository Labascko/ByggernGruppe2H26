/*
 * OLED_driver.h
 *
 * Created: 21.09.2026 12:26:23
 *  Author: emredi
 */ 


#ifndef OLED_DRIVER_H_
#define OLED_DRIVER_H_

void OLED_init(void);
void OLED_reset(void);
void OLED_goto_line(int line);
void OLED_clear_line(int line);
void OLED_goto_column(int column);
void OLED_print(char* string, int n);




#endif /* OLED_DRIVER_H_ */