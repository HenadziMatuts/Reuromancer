#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "SFML\Window\Event.h"

typedef enum character_dir_t {
	CD_NULL = -1,
	CD_UP = 0,
	CD_RIGHT,
	CD_DOWN,
	CD_LEFT,
} character_dir_t;

void character_control_add_sprite_to_chain(int left, int top, character_dir_t dir);
character_dir_t character_control_update();
character_dir_t character_control_handle_input(sfEvent *event);

#endif
