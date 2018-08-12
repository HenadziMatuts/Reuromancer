#ifndef _NEURO_WINDOW_CONTROL_H
#define _NEURO_WINDOW_CONTROL_H

#include <neuro_routines.h>
#include <SFML\Window\Event.h>

void store_window();
void restore_window();

int neuro_window_setup(uint16_t mode, ...);
void neuro_window_draw_string(char *text, ...);
int neuro_window_add_button(neuro_button_t *button);
int setup_ui_buttons();

void window_handle_input(int *state, sfEvent *event);

#endif
