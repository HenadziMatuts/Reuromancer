#ifndef _NEURO_WINDOW_CONTROL_H
#define _NEURO_WINDOW_CONTROL_H

#include <neuro_routines.h>
#include <SFML\Window\Event.h>

typedef enum neuro_window_mode_t {
	NWM_NEURO_UI = 0,
	NWM_PLAYER_DIALOG_CHOICE = 1,
	NWM_INVENTORY = 3,
	NWM_PLAYER_DIALOG_REPLY = 5,
	NWM_NPC_DIALOG_REPLY = 8,
} neuro_window_mode_t;

/* Kind of "Window", 0xC91E */
typedef struct neuro_window_t {
	uint16_t left;        // 0xC91E
	uint16_t top;         // 0xC920
	uint16_t right;       // 0xC922
	uint16_t bottom;      // 0xC924
	uint16_t mode;        // 0xC926
	uint16_t c928;
	uint8_t c92a[4];
	uint16_t total_items; // 0xC92E
	uint16_t item[10];    // 0xC930
	uint16_t c944;
} neuro_window_t;

/* Wraps 16-bit addresses of the button area structs */
typedef struct neuro_window_wrapper_t {
	uint8_t *window_item[10];
	neuro_window_t *window;
} neuro_window_wrapper_t;

/* 0xC91E */
extern neuro_window_t g_neuro_window;
extern neuro_window_wrapper_t g_neuro_window_wrapper;

void store_window();
void restore_window();

int neuro_window_setup(uint16_t mode, ...);
void neuro_window_draw_string(char *text, ...);
int neuro_window_add_button(neuro_button_t *button);
int setup_ui_buttons();

void window_handle_input(int *state, sfEvent *event);

#endif
