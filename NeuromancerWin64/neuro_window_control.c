#include "data.h"
#include "globals.h"
#include "resource_manager.h"
#include "drawing_control.h"
#include "neuro_window_control.h"
#include "inventory_control.h"
#include "scene_control.h"
#include <stdarg.h>
#include <string.h>
#include <assert.h>

int setup_ui_buttons();

void store_window()
{
	memmove(&g_a59e[2], &g_a59e[1], sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[2], &g_a59e_wrapper[1], sizeof(neuro_window_wrapper_t));

	memmove(&g_a59e[1], &g_a59e[0], sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[1], &g_a59e_wrapper[0], sizeof(neuro_window_wrapper_t));

	memmove(&g_a59e[0], &g_neuro_window, sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[0], &g_neuro_window_wrapper, sizeof(neuro_window_wrapper_t));
}

void restore_window()
{
	memmove(&g_neuro_window, &g_a59e[0], sizeof(neuro_window_t));
	memmove(&g_neuro_window_wrapper, &g_a59e_wrapper[0], sizeof(neuro_window_wrapper_t));

	memmove(&g_a59e[0], &g_a59e[1], sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[0], &g_a59e_wrapper[1], sizeof(neuro_window_wrapper_t));

	memmove(&g_a59e[1], &g_a59e[2], sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[1], &g_a59e_wrapper[2], sizeof(neuro_window_wrapper_t));
}

/* Setup "Window" - sub_147EE */
int neuro_window_setup(uint16_t mode, ...)
{
	store_window();

	g_neuro_window.mode = mode;
	g_neuro_window.total_items = 0;

	switch (mode) {
	case 0:
		/* UI "window" */
		setup_ui_buttons();

		g_neuro_window.left = 0;
		g_neuro_window.top = 0;
		g_neuro_window.right = 319;
		g_neuro_window.bottom = 199;
		g_neuro_window.c944 = 160;

		break;

	case 3:
		/* inventory "window" */
		g_neuro_window.left = 56;
		g_neuro_window.top = 128;
		g_neuro_window.right = 231;
		g_neuro_window.bottom = 191;
		g_neuro_window.c944 = 88;

		build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
			g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg012);
		drawing_control_add_sprite_to_chain(g_4bae.x4ccf--,
			g_neuro_window.left, g_neuro_window.top, g_seg012, 1);

		break;

	case 1:
	case 5:
	case 8: {
		/* Dialog "window", 1 - choose reply, 5 - acceped reply, 8 - npc reply */
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
			if (g_neuro_window.mode == 1)
			{
				if (g_4bae.roompos_spawn_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_neuro_window.bottom + 1, g_dialog_bubbles + 0x1A2, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x - 8, g_neuro_window.bottom + 1, g_dialog_bubbles + 0xDC, 0);
				}
			}
			else
			{
				if (g_4bae.roompos_spawn_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_neuro_window.bottom + 1, g_dialog_bubbles + 0x6E, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x - 8, g_neuro_window.bottom + 1, g_dialog_bubbles, 0);
				}
			}
		}

		build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
			g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg011);

		break;
	}

	default:
		break;
	}

	return 0;
}

/* sub_14DBA */
void neuro_window_draw_string(char *text, ...)
{
	switch (g_neuro_window.mode) {
	case 0:
		build_string(text, 320, 200, 176, 182, g_background + sizeof(imh_hdr_t));
		break;

	case 3: {
		va_list args;
		va_start(args, text);
		uint16_t left = va_arg(args, uint16_t);
		uint16_t top = va_arg(args, uint16_t);
		va_end(args);

		imh_hdr_t *imh = (imh_hdr_t*)g_seg012;
		build_string(text, imh->width * 2, imh->height, left, top, g_seg012 + sizeof(imh_hdr_t));

		break;
	}

	case 5: {
		va_list args;
		va_start(args, text);
		uint16_t arg_1 = va_arg(args, uint16_t);
		uint16_t arg_2 = va_arg(args, uint16_t);
		uint16_t arg_3 = va_arg(args, uint16_t);
		va_end(args);

		if (arg_3 != 0)
		{
			/* loc_14D11 */
			break;
		}
	}
	case 1:
	case 8: {
		imh_hdr_t *imh = (imh_hdr_t*)g_seg011;
		build_string(text, imh->width * 2, imh->height, 8, 8, g_seg011 + sizeof(imh_hdr_t));
		drawing_control_add_sprite_to_chain(g_4bae.x4ccf--, 0, g_neuro_window.top, g_seg011, 1);
	}

	default:
		break;
	}
}

/* sub_14B1B */
int neuro_window_add_button(neuro_button_t *button)
{
	switch (g_neuro_window.mode)
	{
	case 0:
	case 3:
		g_neuro_window_wrapper.window_item[g_neuro_window.total_items++] = (uint8_t*)button;
		break;

	default:
		break;
	}

	return 0;
}

static void window_handle_kboard(int *state, sfEvent *event)
{
	switch (g_neuro_window.mode) {
	case 0:
		break;

	case 3:
		inventory_handle_kboard((inventory_state_t*)state, event);
		break;

	default:
		break;
	}
}

void ui_handle_mouse(level_state_t *state, neuro_button_t *button);

static void window_handle_mouse(int *state, neuro_button_t *button)
{
	switch (g_neuro_window.mode) {
	case 0:
		ui_handle_mouse((level_state_t*)state, button);
		break;

	case 3:
		inventory_handle_mouse((inventory_state_t*)state, button);
		break;

	default:
		break;
	}
}

static void select_window_button(neuro_button_t *button)
{
	uint8_t *pic = NULL;

	switch (g_neuro_window.mode) {
	case 0:
		pic = g_background;
		break;
	case 3:
		pic = g_seg012;
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

static neuro_button_t* window_button_hit_test()
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];

	for (uint16_t u = 0; u < g_neuro_window.total_items; u++)
	{
		neuro_button_t *hit =
			(neuro_button_t*)g_neuro_window_wrapper.window_item[u];

		if (cursor->left > hit->left && cursor->left < hit->right &&
			cursor->top > hit->top && cursor->top < hit->bottom)
		{
			return hit;
		}
	}

	return NULL;
}

void window_handle_input(int *state, sfEvent *event)
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];
	neuro_button_t *button = NULL;
	static neuro_button_t *selected = NULL; /* selected button */
	static int _selected = 0; /* redrawing flag */

	if (sfMouse_isButtonPressed(sfMouseLeft))
	{
		if (button = window_button_hit_test())
		{
			if ((!selected || (button == selected)) && !_selected)
			{
				select_window_button(button);
				selected = button;
				_selected = 1;
			}
		}
		else if (selected && _selected)
		{
			unselect_window_button(selected);
			_selected = 0;
		}
	}
	else if (event->mouseButton.type == sfEvtMouseButtonReleased)
	{
		if (selected)
		{
			if (_selected)
			{
				unselect_window_button(selected);
				_selected = 0;
			}

			if (selected == window_button_hit_test())
			{
				window_handle_mouse(state, selected);
			}

			selected = NULL;
		}
	}
	else
	{
		window_handle_kboard(state, event);
	}
}
