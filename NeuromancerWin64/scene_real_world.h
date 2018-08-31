#ifndef _SCENE_REAL_WORLD_H
#define _SCENE_REAL_WORLD_H

#include <SFML\Window\Event.h>
#include <stdint.h>

typedef enum real_world_state_t {
	RWS_FADE_IN = 0,
	RWS_TEXT_OUTPUT,
	RWS_NORMAL,
	RWS_INVENTORY,
	RWS_PAX,
	RWS_DIALOG,
	RWS_SKILLS,
	RWS_ROM,
	RWS_WAIT_FOR_INPUT,
	RWS_RELOAD_LEVEL
} real_world_state_t;

void build_date_string(char *dst, uint8_t date_day);

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

/* Skills state handles */
void handle_skills_input(sfEvent *event);
real_world_state_t update_skills();

/* ROM state handlers */
void handle_rom_input(sfEvent *event);
real_world_state_t update_rom();

#endif
