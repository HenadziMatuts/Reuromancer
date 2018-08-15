#ifndef _SCENE_REAL_WORLD_H
#define _SCENE_REAL_WORLD_H

typedef enum real_world_state_t {
	RWS_TEXT_OUTPUT = 0,
	RWS_NORMAL,
	RWS_INVENTORY,
	RWS_DIALOG,
	RWS_WAIT_FOR_INPUT,
} real_world_state_t;

typedef enum text_output_state_t {
	TOS_NEXT_LINE = 0,
	TOS_SCROLLING,
	TOS_WAIT_FOR_INPUT
} text_output_state_t;

void text_output_set_text(char *p);
void handle_text_output_input(sfEvent *event);
real_world_state_t update_text_output();

/*
 * "IS_OPEN_INVENTORY"        -> "IS_ITEM_LIST",           "IS_WFI_AND_CLOSE"
 *
 * "IS_ITEM_LIST"             -> "IS_ITEM_OPTIONS",        "IS_CLOSE_INVENTORY"
 *
 * "IS_ITEM_OPTIONS"          -> "IS_ITEM_LIST",           "IS_DISCARD_ITEM",   "IS_ERASE_SOFTWARE_LIST",
 *                               "IS_GIVE_CREDITS",        "IS_GIVE_ITEM",      "IS_OPERATE_SOFTWARE_LIST",
 *                               "IS_WFI_AND_CONTINUE",    "IS_WFI_AND_CLOSE"
 *
 * "IS_DISCARD_ITEM"          -> "IS_ITEM_LIST"
 *
 * "IS_ERASE_SOFTWARE_LIST"   -> "IS_ERASE_SOFTWARE",      "IS_ITEM_LIST"
 *
 * "IS_ERASE_SOFTWARE"        -> "IS_ERASE_SOFTWARE_LIST", "IS_WFI_AND_CONTINUE"
 *
 * "IS_WFI_AND_CONTINUE"      -> "IS_ITEM_LIST"
 *
 * "IS_GIVE_CREDITS"          -> "IS_CLOSE_INVENTORY"
 *
 * "IS_GIVE_ITEM"             -> "IS_CLOSE_INVENTORY"
 *
 * "IS_OPERATE_SOFTWARE_LIST" -> "IS_WFI_AND_CLOSE",       "IS_WFI_AND_CONTINUE"
 *
 * "IS_WFI_AND_CLOSE"         -> "IS_CLOSE_INVENTORY"
 *
 * "IS_CLOSE_INVENTORY"       -> "IS_OPEN_INVENTORY" (EXIT)
 */

typedef enum inventory_state_t {
	IS_OPEN_INVENTORY = -1,
	IS_ITEM_LIST = 0,
	IS_SOFTWARE_LIST, // <- not a real state
	IS_ITEM_OPTIONS,
	IS_DISCARD_ITEM,
	IS_OPERATE_SOFTWARE_LIST,
	IS_ERASE_SOFTWARE_LIST,
	IS_ERASE_SOFTWARE,
	IS_GIVE_CREDITS,
	IS_GIVE_ITEM,
	IS_WFI_AND_CONTINUE,
	IS_WFI_AND_CLOSE,
	IS_CLOSE_INVENTORY,
} inventory_state_t;

void handle_inventory_input(sfEvent *event);
real_world_state_t update_inventory();

typedef enum dialog_state_t {
	DS_OPEN_DIALOG = 0,
	DS_CHOOSE_REPLY_WFI,
	DS_NEXT_REPLY,
	DS_ACCEPT_REPLY,
	DS_ACCEPT_REPLY_WFI,
	DS_CLOSE_DIALOG
} dialog_state_t;

extern uint8_t g_dialog_escapable;
void handle_dialog_input(sfEvent *event);
real_world_state_t update_dialog();

#endif
