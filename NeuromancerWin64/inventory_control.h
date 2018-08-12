#ifndef _INVENTORY_CONTROL_H
#define _INVENTORY_CONTROL_H

#include "scene_control.h"
#include <SFML\Window\Event.h>

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

level_state_t update_inventory(sfEvent *event);

void inventory_handle_mouse(inventory_state_t *state, neuro_button_t *button);
void inventory_handle_kboard(inventory_state_t *state, sfEvent *event);

#endif
