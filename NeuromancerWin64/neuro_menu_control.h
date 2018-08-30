#ifndef _NEURO_MENU_CONTROL_H
#define _NEURO_MENU_CONTROL_H

#include <neuro_routines.h>
#include "SFML\Window\Event.h"

typedef enum neuro_menu_id_t {
	NMID_MAIN_MENU = 0,
	NMID_NOT_IMPLEMENTED_MENU,
	NMID_SKILLS_MENU,
} neuro_menu_id_t;

extern neuro_menu_t g_neuro_menu;

void neuro_menu_create(uint16_t mode,
		uint16_t l, uint16_t t, uint16_t w, uint16_t h, uint8_t *pixels);
void neuro_menu_draw_text(char *text, uint16_t l, uint16_t t);
void neuro_menu_add_item(uint16_t l, uint16_t t, uint16_t w,
		uint16_t code, char label);
void neuro_menu_flush_items();
void neuro_menu_flush();
void neuro_menu_destroy();

void neuro_menu_handle_input(neuro_menu_id_t id, neuro_menu_t *menu,
			int *state, sfEvent *event);


/* Main Menu Handlers */
void main_menu_handle_text_enter(int *state, sfTextEvent *event);
void main_menu_handle_button_press(int *state, neuro_button_t *button);

/* Not Implemented Screen Handlers */
void not_implemented_menu_handle_button_press(int *state, neuro_button_t *button);

/* Skills Menu Handlers */
void skills_menu_handle_text_enter(int *state, sfTextEvent *event);
void skills_menu_handle_button_press(int *state, neuro_button_t *button);

#endif
