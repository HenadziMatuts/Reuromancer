#ifndef _SCENE_REAL_WORLD_H
#define _SCENE_REAL_WORLD_H

typedef enum real_world_state_t {
	RWS_FADE_IN = 0,
	RWS_TEXT_OUTPUT,
	RWS_NORMAL,
	RWS_INVENTORY,
	RWS_PAX,
	RWS_DIALOG,
	RWS_WAIT_FOR_INPUT,
	RWS_RELOAD_LEVEL
} real_world_state_t;

/* Text Output state handles */
void text_output_set_text(char *p);
void handle_text_output_input(sfEvent *event);
real_world_state_t update_text_output();

/* Inventory state handles */
void handle_inventory_input(sfEvent *event);
real_world_state_t update_inventory();

/* Dialog state handles */
extern uint8_t g_dialog_escapable;
void handle_dialog_input(sfEvent *event);
real_world_state_t update_dialog();

/* PAX state handles */
void handle_pax_input(sfEvent *event);
real_world_state_t update_pax();

#endif
