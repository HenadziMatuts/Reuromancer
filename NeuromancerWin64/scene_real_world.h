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
	RWS_DISK_OPTIONS,
	RWS_BODY_PARTS_SHOP,
	RWS_WAIT_FOR_INPUT,
	RWS_RELOAD_LEVEL
} real_world_state_t;

void build_date_string(char *dst, uint8_t date_day);

/* Inventory state handles */
void handle_inventory_input(sfEvent *event);
real_world_state_t update_inventory();

/* Dialog state handles */
extern uint8_t g_dialog_escapable;
extern char g_dialog_user_input[17];
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

/* Disk Options handlers */
extern uint8_t g_load_game;
void handle_disk_options_input(sfEvent *event);
real_world_state_t update_disk_options();

/* Sell Body Parts handlers */
extern int g_body_shop_op, g_body_shop_discount;
void handle_parts_shop_input(sfEvent *event);
real_world_state_t update_parts_shop();

#endif
