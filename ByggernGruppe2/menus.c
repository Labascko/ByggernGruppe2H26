/*
 * menus.c
 *
 * Created: 09.11.2026 14:16:27
 *  Author: eskilod
 */ 
#include <avr/io.h>
#include <stdio.h>

#include "OLED_driver.h"
#include "menus.h"
//#include <stddef.h> // for NULL

// Define the actual global variables here (only once!)
Menu main_menu;
Menu sub_menu1;
Menu sub_menu2;
Menu sub_menu3;
Menu* current_menu;

void init_menus(void) {
	// sub_menu1 initialization
	sub_menu1.title = "Instillinger";
	sub_menu1.num_items = 2;
	sub_menu1.parent_menu = NULL;
	sub_menu1.position = 0;

	MenuItem item11 = {.text = "* Valg 1", .child_menu = &sub_menu1};
	sub_menu1.items[0] = item11;

	MenuItem item12 = {.text = "* Tilbake", .child_menu = &main_menu};
	sub_menu1.items[1] = item12;
	
	// sub_menu2 initialization
	sub_menu2.title = "spill";
	sub_menu2.num_items = 2;
	sub_menu2.parent_menu = NULL;
	sub_menu2.position = 0;

	MenuItem item21 = {.text = "* Valg 1", .child_menu = &sub_menu2};
	sub_menu2.items[0] = item21;

	MenuItem item22 = {.text = "*pyntevalg 1", .child_menu = &sub_menu2};
	sub_menu2.items[1] = item22;

	// sub_menu3 initialization
	sub_menu3.title = " Game over!";
	sub_menu3.num_items = 2;
	sub_menu3.parent_menu = NULL;
	sub_menu3.position = 0;

	MenuItem item31 = {.text = "* Til hovedmeny", .child_menu = &main_menu};
	sub_menu3.items[0] = item31;

	MenuItem item32 = {.text = "* spill igjen", .child_menu = &sub_menu2};
	sub_menu3.items[1] = item32;


	// main_menu initialization
	main_menu.title = "Hovedmeny";
	main_menu.num_items = 2;
	main_menu.parent_menu = NULL;
	main_menu.position = 0;

	MenuItem item1 = {.text = "* Start spill", .child_menu = &sub_menu2};
	main_menu.items[0] = item1;

	MenuItem item2 = {.text = "* Instillinger", .child_menu = &sub_menu1};
	main_menu.items[1] = item2;

	// Set pointer
	current_menu = &main_menu;
}