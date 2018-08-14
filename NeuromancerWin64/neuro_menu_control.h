#ifndef _NEURO_MENU_CONTROL_H
#define _NEURO_MENU_CONTROL_H

#include <neuro_routines.h>
#include "SFML\Window\Event.h"

typedef enum neuro_menu_id_t {
	NMID_MAIN_MENU = 0,
	NMID_NOT_IMPLEMENTED_MENU,
} neuro_menu_id_t;

void neuro_menu_handle_input(neuro_menu_id_t id, neuro_menu_t *menu,
			int *state, sfEvent *event);


/* Main Menu Handlers */
void main_menu_handle_text_enter(int *state, sfTextEvent *event);
void main_menu_handle_button_press(int *state, neuro_button_t *button);

/* Not Implemented Screen Handlers */
void not_implemented_menu_handle_button_press(int *state, neuro_button_t *button);

#endif
