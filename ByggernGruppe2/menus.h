/*
 * menus.h
 *
 * Created: 09.11.2026 14:08:05
 *  Author: eskilod
 */ 



#ifndef MENUS_H_
#define MENUS_H_

// Declare them as extern so other files can see them
extern Menu main_menu;
extern Menu sub_menu1;
extern Menu sub_menu2;
extern Menu sub_menu3;
extern Menu* current_menu;

// Function to initialize the menus at runtime
void init_menus(void);


#endif /* MENUS_H_ */