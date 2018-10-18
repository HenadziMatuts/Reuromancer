#include "data.h"
#include "globals.h"
#include "resource_manager.h"
#include "drawing_control.h"
#include "neuro_window_control.h"
#include "scene_control.h"
#include "address_translator.h"
#include <stdarg.h>
#include <string.h>
#include <assert.h>

/* 0xA59E */
neuro_window_t g_neuro_windows_pool[3] = {
	{ 0, },{ 0, },{ 0, }
};
/* 0xC91E */
neuro_window_t g_neuro_window = {
	{ 0, }
};

int setup_ui_buttons();


void store_window()
{
	memmove(&g_neuro_windows_pool[2], &g_neuro_windows_pool[1], sizeof(neuro_window_t));
	memmove(&g_neuro_windows_pool[1], &g_neuro_windows_pool[0], sizeof(neuro_window_t));
	memmove(&g_neuro_windows_pool[0], &g_neuro_window, sizeof(neuro_window_t));
}

void restore_window()
{
	memmove(&g_neuro_window, &g_neuro_windows_pool[0], sizeof(neuro_window_t));
	memmove(&g_neuro_windows_pool[0], &g_neuro_windows_pool[1], sizeof(neuro_window_t));
	memmove(&g_neuro_windows_pool[1], &g_neuro_windows_pool[2], sizeof(neuro_window_t));
}

/* Setup "Window" - sub_147EE */
int neuro_window_setup(uint16_t mode, ...)
{
	store_window();

	g_neuro_window.mode = mode;
	g_neuro_window.total_items = 0;

	switch (mode) {
	case NWM_NEURO_UI:
		setup_ui_buttons();

		g_neuro_window.left = 0;
		g_neuro_window.top = 0;
		g_neuro_window.right = 319;
		g_neuro_window.bottom = 199;
		g_neuro_window.c944 = 160;
		break;

	case NWM_PAX:
		g_neuro_window.left = 0;
		g_neuro_window.top = 4;
		g_neuro_window.right = 319;
		g_neuro_window.bottom = 107;
		g_neuro_window.c92a = 8;
		g_neuro_window.c92c = 8;
		g_neuro_window.c944 = 160;

		build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
			g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg011.data);
		drawing_control_add_sprite_to_chain(g_4bae.frame_sc_index--,
			g_neuro_window.left, g_neuro_window.top, g_seg011.data, 1);
		break;

	case NWM_INVENTORY:
		g_neuro_window.left = 56;
		g_neuro_window.top = 128;
		g_neuro_window.right = 231;
		g_neuro_window.bottom = 191;
		g_neuro_window.c944 = 88;

		build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
			g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg012.data);
		drawing_control_add_sprite_to_chain(g_4bae.frame_sc_index--,
			g_neuro_window.left, g_neuro_window.top, g_seg012.data, 1);
		break;

	case NWM_PLAYER_DIALOG_CHOICE:
	case NWM_PLAYER_DIALOG_REPLY:
	case NWM_NPC_DIALOG_REPLY: {
		va_list args;
		va_start(args, mode);
		uint16_t lines = va_arg(args, uint16_t);
		va_end(args);

		g_neuro_window.left = 0;
		g_neuro_window.top = 4;
		g_neuro_window.right = 319;
		g_neuro_window.bottom = (lines * 8) + 19;
		g_neuro_window.c928 = lines;
		g_neuro_window.c944 = 160;

		if (g_4bae.ui_type == 0)
		{
			if (g_neuro_window.mode == NWM_PLAYER_DIALOG_CHOICE)
			{
				if (g_4bae.roompos_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_x + 8, g_neuro_window.bottom + 1, g_seg014.dialog_bubbles + 0x1A2, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_x - 8, g_neuro_window.bottom + 1, g_seg014.dialog_bubbles + 0xDC, 0);
				}
			}
			else
			{
				if (g_4bae.roompos_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_x + 8, g_neuro_window.bottom + 1, g_seg014.dialog_bubbles + 0x6E, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_x - 8, g_neuro_window.bottom + 1, g_seg014.dialog_bubbles, 0);
				}
			}
		}

		build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
			g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg011.data);
		break;
	}

	default:
		assert(0);
	}

	return 0;
}

/* sub_14DBA */
void neuro_window_draw_string(char *text, ...)
{
	switch (g_neuro_window.mode) {
	case NWM_NEURO_UI:
		build_string(text, 320, 200, 176, 182, g_seg010.background + sizeof(imh_hdr_t));
		break;

	case NWM_PAX: {
		va_list args;
		va_start(args, text);
		uint16_t up = va_arg(args, uint16_t);
		va_end(args);

		imh_hdr_t *imh = (imh_hdr_t*)g_seg011.data;

		if (up)
		{
			build_string(text, imh->width * 2, imh->height,
				g_neuro_window.c92a, g_neuro_window.c92c, g_seg011.data + sizeof(imh_hdr_t));
		}
		else
		{
			uint32_t t = g_neuro_window.bottom -
				g_neuro_window.top - g_neuro_window.c92c - 7;
			build_string(text, imh->width * 2, imh->height,
				g_neuro_window.c92a, t, g_seg011.data + sizeof(imh_hdr_t));
		}

		break;
	}

	case NWM_INVENTORY: {
		va_list args;
		va_start(args, text);
		uint16_t left = va_arg(args, uint16_t);
		uint16_t top = va_arg(args, uint16_t);
		va_end(args);

		imh_hdr_t *imh = (imh_hdr_t*)g_seg012.data;
		build_string(text, imh->width * 2, imh->height, left, top, g_seg012.data + sizeof(imh_hdr_t));

		break;
	}

	case NWM_PLAYER_DIALOG_REPLY: {
		va_list args;
		va_start(args, text);
		uint16_t l = va_arg(args, uint16_t);
		uint16_t t = va_arg(args, uint16_t);
		uint16_t mode = va_arg(args, uint16_t);
		va_end(args);

		if (mode != 0)
		{
			imh_hdr_t *imh = (imh_hdr_t*)g_seg011.data;
			l = (l * 8) + 8;
			t = (t * 8) + 8;
			build_string(text, imh->width * 2, imh->height, l, t, g_seg011.data + sizeof(imh_hdr_t));
			break;
		}
	}
	case NWM_PLAYER_DIALOG_CHOICE:
	case NWM_NPC_DIALOG_REPLY: {
		imh_hdr_t *imh = (imh_hdr_t*)g_seg011.data;
		build_string(text, imh->width * 2, imh->height, 8, 8, g_seg011.data + sizeof(imh_hdr_t));
		drawing_control_add_sprite_to_chain(g_4bae.frame_sc_index--, 0, g_neuro_window.top, g_seg011.data, 1);
		break;
	}

	default:
		assert(0);
	}
}

/* sub_14B1B */
int neuro_window_add_button(neuro_button_t *button)
{
	switch (g_neuro_window.mode) {
	case NWM_NEURO_UI:
	case NWM_PAX:
	case NWM_INVENTORY:
	case 4:
		translate_x64_to_x16((uint8_t*)button, NULL,
			&g_neuro_window.item[g_neuro_window.total_items++]);
		break;

	default:
		assert(0);
	}

	return 0;
}

void neuro_window_clear()
{
	switch (g_neuro_window.mode) {
	case NWM_NEURO_UI:
		break;

	case NWM_PAX:
		build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
			g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg011.data);
		break;

	case NWM_INVENTORY:
		build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
			g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg012.data);
		break;

	case 4:
		break;

	default:
		assert(0);
	}
}

void neuro_window_flush_buttons()
{
	switch (g_neuro_window.mode) {
	case NWM_NEURO_UI:
	case NWM_PAX:
	case NWM_INVENTORY:
	case 4:
		g_neuro_window.total_items = 0;
		break;

	default:
		assert(0);
	}
}

void neuro_window_set_draw_string_offt(uint16_t l, uint16_t t)
{
	assert((g_neuro_window.mode == 2) || (g_neuro_window.mode == 3));

	g_neuro_window.c92a = l;
	g_neuro_window.c92c = t;
}

static void window_handle_kboard(int *state, sfKeyEvent *event)
{
	switch (g_neuro_window.mode) {
	case NWM_PAX:
		rw_pax_handle_kboard(state, event);
		break;

	case NWM_INVENTORY:
		rw_inventory_handle_kboard(state, event);
		break;

	case NWM_PLAYER_DIALOG_REPLY:
		rw_dialog_handle_kboard(state, event);
		break;

	default:
		break;
	}
}

static void window_handle_text_enter(int *state, sfTextEvent *event)
{
	switch (g_neuro_window.mode) {
	case NWM_PAX:
		rw_pax_handle_text_enter(state, event);
		break;

	case NWM_INVENTORY:
		rw_inventory_handle_text_enter(state, event);
		break;

	case NWM_PLAYER_DIALOG_REPLY:
		rw_dialog_handle_text_enter(state, event);
		break;

	default:
		break;
	}
}

static void window_handle_button_press(int *state, neuro_button_t *button)
{
	switch (g_neuro_window.mode) {
	case NWM_NEURO_UI:
		rw_ui_handle_button_press(state, button);
		break;

	case NWM_PAX:
		rw_pax_handle_button_press(state, button);
		break;

	case NWM_INVENTORY:
		rw_inventory_handle_button_press(state, button);
		break;

	default:
		break;
	}
}

static void select_window_button(neuro_button_t *button)
{
	uint8_t *pic = NULL;

	switch (g_neuro_window.mode) {
	case NWM_NEURO_UI:
		pic = g_seg010.background;
		break;
	case NWM_PAX:
		pic = g_seg011.data;
		break;
	case NWM_INVENTORY:
		pic = g_seg012.data;
		break;
	default:
		return;
	}

	uint8_t *p = pic + sizeof(imh_hdr_t) +
		((button->left - g_neuro_window.left) / 2) +
		((button->top - g_neuro_window.top) * g_neuro_window.c944);

	uint16_t lines = button->bottom - button->top + 1;
	uint16_t width = ((button->right | 1) - (button->left & 0xFE) + 1) / 2;
	uint16_t skip = g_neuro_window.c944 - width;

	for (uint16_t h = 0; h < lines; h++, p += skip)
	{
		for (uint16_t w = 0; w < width; w++)
		{
			*p++ ^= 0xFF;
		}
	}

	return;
}

static void unselect_window_button(neuro_button_t *button)
{
	select_window_button(button);
}

static neuro_button_t* window_button_kboard_hit_test()
{
	for (uint16_t u = 0; u < g_neuro_window.total_items; u++)
	{
		neuro_button_t *hit =
			(neuro_button_t*)translate_x16_to_x64(DSEG, g_neuro_window.item[u]);

		sfKeyCode key = ascii_toSfKeyCode(hit->label);

		if (key == sfKeyUnknown)
		{
			continue;
		}

		if (sfKeyboard_isKeyPressed(key))
		{
			return hit;
		}
	}

	return NULL;
}

static void neuro_window_handle_kboard_events(int *state, sfEvent *event, int *kboard_lock)
{
	neuro_button_t *hit = NULL;
	static neuro_button_t *selected = NULL; /* selected button */
	static sfKeyCode _selected = sfKeyUnknown;

	switch (event->type) {
	case sfEvtKeyPressed:
		if (hit = window_button_kboard_hit_test())
		{
			if (!selected)
			{
				select_window_button(hit);
				selected = hit;
				_selected = event->key.code;
				*kboard_lock = 1;
			}
		}
		else
		{
			window_handle_kboard(state, &event->key);
		}
		break;

	case sfEvtKeyReleased:
		if (selected && _selected == event->key.code)
		{
			unselect_window_button(selected);
			window_handle_button_press(state, selected);
			selected = NULL;
			_selected = sfKeyUnknown;
			*kboard_lock = 0;
		}
		else
		{
			window_handle_kboard(state, &event->key);
		}
		break;

	case sfEvtTextEntered:
		window_handle_text_enter(state, &event->text);
		break;

	default:
		break;
	}
}

static neuro_button_t* window_button_mouse_hit_test()
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];

	for (uint16_t u = 0; u < g_neuro_window.total_items; u++)
	{
		neuro_button_t *hit =
			(neuro_button_t*)translate_x16_to_x64(DSEG, g_neuro_window.item[u]);

		if (cursor->left > hit->left && cursor->left < hit->right &&
			cursor->top > hit->top && cursor->top < hit->bottom)
		{
			return hit;
		}
	}

	return NULL;
}

static void neuro_window_handle_mouse_events(int *state, sfEvent *event, int *mouse_lock)
{
	neuro_button_t *hit = NULL;
	static neuro_button_t *selected = NULL; /* selected button */
	static int _selected = 0;

	switch (event->type) {
	case sfEvtMouseButtonPressed: {
		if (sfMouse_isButtonPressed(sfMouseLeft))
		{
			if (hit = window_button_mouse_hit_test())
			{
				if (!selected)
				{
					select_window_button(hit);
					selected = hit;
					_selected = 1;
					*mouse_lock = 1;
				}
			}
		}

		break;
	}

	case sfEvtMouseMoved: {
		if (sfMouse_isButtonPressed(sfMouseLeft))
		{
			hit = window_button_mouse_hit_test();

			if (selected)
			{
				if (hit != selected)
				{
					if (_selected)
					{
						unselect_window_button(selected);
						_selected = 0;
					}
				}
				else
				{
					if (_selected == 0)
					{
						select_window_button(hit);
						_selected = 1;
					}
				}
			}
			else if (hit)
			{
				select_window_button(hit);
				selected = hit;
				_selected = 1;
				*mouse_lock = 1;
			}

		}
		break;
	}

	case sfEvtMouseButtonReleased: {
		if (selected)
		{
			if (_selected)
			{
				unselect_window_button(selected);
				_selected = 0;
			}

			if (selected == window_button_mouse_hit_test())
			{
				window_handle_button_press(state, selected);
			}

			selected = NULL;
			*mouse_lock = 0;
		}
		break;
	}

	default:
		break;
	}
}

void neuro_window_handle_input(int *state, sfEvent *event)
{
	static int mouse_lock = 0;
	static int kboard_lock = 0;

	switch (event->type) {
	case sfEvtMouseButtonPressed:
	case sfEvtMouseButtonReleased:
	case sfEvtMouseMoved:
		if (!kboard_lock)
		{
			neuro_window_handle_mouse_events(state, event, &mouse_lock);
		}
		break;

	case sfEvtKeyPressed:
	case sfEvtKeyReleased:
	case sfEvtTextEntered:
		if (!mouse_lock)
		{
			neuro_window_handle_kboard_events(state, event, &kboard_lock);
		}
		break;

	default:
		break;
	}
}
