#include "data.h"
#include "globals.h"
#include "character_control.h"
#include "drawing_control.h"
#include "resource_manager.h"

typedef enum character_state_t {
	CS_IDLE = 0,
	CS_MOVING,
} character_state_t;

typedef struct character_t {
	character_state_t state;
	character_dir_t dir;
	int frame;
} character_t;

static uint16_t g_up_frames[8] = {
	0x0000, 0x037A, 0x06F4, 0x0A7C, 0x0DE8, 0x1162, 0x14DC, 0x1856
}, g_right_frames[8] = {
	0x1B46, 0x1EA4, 0x20A4, 0x2394, 0x277C, 0x2AE8, 0x2CE8, 0x2FD8
}, g_down_frames[8] = {
	0x33C0, 0x36B0, 0x3A2A, 0x3DA4, 0x411E, 0x448A, 0x4812, 0x4B8C
}, g_left_frames[8] = {
	0x4F06, 0x5272, 0x5472, 0x5762, 0x5B4A, 0x5EB6, 0x6134, 0x6424
};

static character_t g_character = {
	CS_IDLE, CD_DOWN, 0
};

void character_control_add_sprite_to_chain(int left, int top, character_dir_t dir)
{
	uint8_t *frame = g_sprites;

	switch (dir)
	{
	case CD_UP:
		frame += g_up_frames[0];
		break;

	case CD_RIGHT:
		frame += g_right_frames[0];
		break;

	case CD_DOWN:
		frame += g_down_frames[0];
		break;

	case CD_LEFT:
		frame += g_left_frames[0];
		break;

	default:
		return;
	}

	drawing_control_add_sprite_to_chain(SCI_CHARACTER, left, top, frame, 0);

	g_character.state = CS_IDLE;
	g_character.dir = dir;
	g_character.frame = 0;
}

void character_control_update()
{
	static int frame_cap_ms = 100;
	static int speed_hort_pix = 5;
	static int speed_vert_pix = 2;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	sprite_layer_t *ch_sprite = &g_sprite_chain[SCI_CHARACTER];

	if (passed - elapsed > frame_cap_ms)
	{
		elapsed = passed;

		g_4bae.roompos_spawn_x = ch_sprite->left;
		g_4bae.roompos_spawn_y = ch_sprite->top;

		if (g_character.state == CS_IDLE)
		{
			return;
		}

		int left = 0, top = 0;

		switch (g_character.dir)
		{
		case CD_LEFT:
			left = (int)ch_sprite->left -
				ch_sprite->sprite_hdr.dx - speed_hort_pix;
			if (left > 20)
			{
				ch_sprite->left -= speed_hort_pix;
			}
			g_character.frame = (g_character.frame + 1) % 8;
			drawing_control_add_sprite_to_chain(SCI_CHARACTER, ch_sprite->left, ch_sprite->top,
				g_sprites + g_left_frames[g_character.frame], 0);
			break;

		case CD_RIGHT:
			left = ch_sprite->left - ch_sprite->sprite_hdr.dx +
				(ch_sprite->sprite_hdr.width * 2) + speed_hort_pix;
			if (left < 300)
			{
				ch_sprite->left += speed_hort_pix;
			}
			g_character.frame = (g_character.frame + 1) % 8;
			drawing_control_add_sprite_to_chain(SCI_CHARACTER, ch_sprite->left, ch_sprite->top,
				g_sprites + g_right_frames[g_character.frame], 0);
			break;

		case CD_UP:
			top = ch_sprite->top - speed_vert_pix;
			if (top > 94)
			{
				ch_sprite->top -= speed_vert_pix;
			}
			g_character.frame = (g_character.frame + 1) % 8;
			drawing_control_add_sprite_to_chain(SCI_CHARACTER, ch_sprite->left, ch_sprite->top,
				g_sprites + g_up_frames[g_character.frame], 0);
			break;

		case CD_DOWN:
			top = ch_sprite->top + speed_vert_pix;
			if (top < 116)
			{
				ch_sprite->top += speed_vert_pix;
			}
			g_character.frame = (g_character.frame + 1) % 8;
			drawing_control_add_sprite_to_chain(SCI_CHARACTER, ch_sprite->left, ch_sprite->top,
				g_sprites + g_down_frames[g_character.frame], 0);
			break;

		}
	}

	return;
}

static character_dir_t character_control_handle_mouse()
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];
	sprite_layer_t *character = &g_sprite_chain[SCI_CHARACTER];

	if (sfMouse_isButtonPressed(sfMouseLeft))
	{
		/* mouse input */
		g_character.state = CS_MOVING;

		if (cursor->left < character->left - character->sprite_hdr.dx)
		{
			/* go left*/
			return CD_LEFT;
		}
		else if (cursor->left >
			character->left - character->sprite_hdr.dx + character->sprite_hdr.width * 2)
		{
			/*go right*/
			return CD_RIGHT;
		}
		else
		{
			if (cursor->top < character->top - character->sprite_hdr.dy)
			{
				/* go up */
				return CD_UP;
			}
			else if (cursor->top > character->top)
			{
				/* go down */
				return CD_DOWN;
			}
			else
			{
				g_character.state = CS_IDLE;
				return CD_NULL;
			}
		}
	}

	return CD_NULL;
}

static character_dir_t character_control_handle_kboard()
{
	g_character.state = CS_MOVING;

	if (sfKeyboard_isKeyPressed(sfKeyLeft))
	{
		return CD_LEFT;
	}
	else if (sfKeyboard_isKeyPressed(sfKeyRight))
	{
		return CD_RIGHT;
	}
	else if (sfKeyboard_isKeyPressed(sfKeyUp))
	{
		return CD_UP;
	}
	else if (sfKeyboard_isKeyPressed(sfKeyDown))
	{
		return CD_DOWN;
	}

	g_character.state = CS_IDLE;
	return CD_NULL;
}

void character_control_handle_input()
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];
	sprite_layer_t *character = &g_sprite_chain[SCI_CHARACTER];
	character_dir_t new_dir = CD_NULL;
	g_character.state = CS_IDLE;

	/* scene area */
	if (cursor->left > 0 && cursor->left < 320 &&
		cursor->top > 0 && cursor->top < 140)
	{
		/* mouse input */
		new_dir = character_control_handle_mouse();
	}
	if (new_dir == CD_NULL)
	{
		/* keyboard input */
		new_dir = character_control_handle_kboard();
	}

	if (new_dir != CD_NULL && new_dir != g_character.dir)
	{
		switch (new_dir)
		{
		case CD_LEFT:
			g_character.dir = CD_LEFT;
			g_character.frame = 0;
			drawing_control_add_sprite_to_chain(SCI_CHARACTER, character->left, character->top,
				g_sprites + g_left_frames[0], 0);
			break;
		case CD_RIGHT:
			g_character.dir = CD_RIGHT;
			g_character.frame = 0;
			drawing_control_add_sprite_to_chain(SCI_CHARACTER, character->left, character->top,
				g_sprites + g_right_frames[0], 0);
			break;
		case CD_UP:
			g_character.dir = CD_UP;
			g_character.frame = 0;
			drawing_control_add_sprite_to_chain(SCI_CHARACTER, character->left, character->top,
				g_sprites + g_up_frames[0], 0);
			break;
		case CD_DOWN:
			g_character.dir = CD_DOWN;
			g_character.frame = 0;
			drawing_control_add_sprite_to_chain(SCI_CHARACTER, character->left, character->top,
				g_sprites + g_down_frames[0], 0);
			break;
		}
	}
}
