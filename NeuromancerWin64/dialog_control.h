#ifndef _DIALOG_CONTROL_H
#define _DIALOG_CONTROL_H

#include "scene_control.h"
#include <SFML\Window\Event.h>

typedef enum dialog_state_t {
	DS_OPEN_DIALOG = 0,
	DS_CHOOSE_REPLY_WFI,
	DS_NEXT_REPLY,
	DS_ACCEPT_REPLY,
	DS_ACCEPT_REPLY_WFI,
	DS_CLOSE_DIALOG
} dialog_state_t;

extern uint8_t g_dialog_escapable;

real_world_state_t update_dialog(sfEvent *event);

#endif
