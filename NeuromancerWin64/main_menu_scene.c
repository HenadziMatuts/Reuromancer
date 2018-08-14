#include "globals.h"
#include "data.h"
#include "scene_control.h"
#include "drawing_control.h"
#include "resource_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef enum main_menu_state_t {
	MMS_INITIAL = 0,
	MMS_NEW,
	MMS_LOAD,
	MMS_TO_LEVEL_SCENE,
	MMS_TO_NOT_IMPLEMENTED_SCENE,
} main_menu_state_t;

static main_menu_state_t g_state = MMS_INITIAL;

static uint8_t *g_dialog = NULL;
static neuro_menu_t g_menu_dialog;

static void main_menu_handle_text_enter(main_menu_state_t *state, sfTextEvent *event)
{
	static char name[11] = { 0, };

	if (*state == MMS_NEW)
	{
		char input[12] = { 0x00 };
		sfKeyCode key = handle_sfml_text_input(event->unicode, name, 11, 0);

		if (key == sfKeyReturn)
		{
			*state = MMS_TO_LEVEL_SCENE;
			if (strlen(name))
			{
				sprintf(g_4bae.name + 2, "%s", name);
			}
			memset(name, 0, 11);
		}
		else
		{
			sprintf(input, "%s%s", name, "<");
			memset(input + strlen(input), 0x20, 11 - strlen(input));
			build_menu_dialog_text(&g_menu_dialog, input, 0, 16);
		}
	}
}

static void main_menu_handle_button_press(main_menu_state_t *state, neuro_button_t *button)
{
	switch (*state) {
	case MMS_INITIAL:
		switch (button->code) {
		case 0: /* new */
			build_menu_dialog_frame(&g_menu_dialog, 32, 152, 128, 40, 6, g_dialog);
			build_menu_dialog_text(&g_menu_dialog, "Your name?", 0, 0);
			build_menu_dialog_text(&g_menu_dialog, "<", 0, 16);
			drawing_control_add_sprite_to_chain(SCI_DIALOG, 32, 152, g_dialog, 1);
			*state = MMS_NEW;
			break;

		case 1: /* load */
			build_menu_dialog_frame(&g_menu_dialog, 48, 120, 144, 64, 6, g_dialog);
			build_menu_dialog_text(&g_menu_dialog, "Load Game", 32, 0);
			build_menu_dialog_text(&g_menu_dialog, "1  2  3  4", 24, 16);
			build_menu_dialog_text(&g_menu_dialog, "exit", 48, 40);
			build_menu_dialog_item(&g_menu_dialog, 24, 16, 8, 0, '1');
			build_menu_dialog_item(&g_menu_dialog, 48, 16, 8, 1, '2');
			build_menu_dialog_item(&g_menu_dialog, 72, 16, 8, 2, '3');
			build_menu_dialog_item(&g_menu_dialog, 96, 16, 8, 3, '4');
			build_menu_dialog_item(&g_menu_dialog, 48, 40, 32, 10, 'x');
			drawing_control_add_sprite_to_chain(SCI_DIALOG, 48, 120, g_dialog, 1);
			*state = MMS_LOAD;
			break;
		}
		break;

	case MMS_LOAD:
		switch (button->code) {
		case 10: /* exit */
			build_menu_dialog_frame(&g_menu_dialog, 32, 152, 96, 24, 6, g_dialog);
			build_menu_dialog_text(&g_menu_dialog, "New/Load", 8, 0);
			build_menu_dialog_item(&g_menu_dialog, 8, 0, 24, 0, 'n');
			build_menu_dialog_item(&g_menu_dialog, 40, 0, 32, 1, 'l');
			drawing_control_add_sprite_to_chain(SCI_DIALOG, 32, 152, g_dialog, 1);
			*state = MMS_INITIAL;
			break;

		case 0: /* savegame slots */
		case 1:
		case 2:
		case 3:
			*state = MMS_TO_NOT_IMPLEMENTED_SCENE;
			break;
		}
		break;

	default:
		break;
	}
}

static void init()
{
	memset(g_vga, 0, 320 * 200 * 4);

	assert(resource_manager_load_resource("TITLE.IMH", g_background));
	drawing_control_add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_background, 1);

	assert(g_dialog = calloc(8192, 1));
	build_menu_dialog_frame(&g_menu_dialog, 32, 152, 96, 24, 6, g_dialog);
	build_menu_dialog_text(&g_menu_dialog, "New/Load", 8, 0);
	build_menu_dialog_item(&g_menu_dialog, 8, 0, 24, 0, 'n');
	build_menu_dialog_item(&g_menu_dialog, 40, 0, 32, 1, 'l');
	drawing_control_add_sprite_to_chain(SCI_DIALOG, 32, 152, g_dialog, 1);
}

typedef enum neuro_menu_id_t {
	NMID_MAIN_MENU = 0,
} neuro_menu_id_t;

static void menu_handle_text_enter(neuro_menu_id_t id, int *state, sfTextEvent *event)
{
	switch (id) {
	case NMID_MAIN_MENU:
		main_menu_handle_text_enter((main_menu_state_t*)state, event);
		break;
	}
}

static void menu_handle_button_press(neuro_menu_id_t id, int *state, neuro_button_t *button)
{
	switch (id) {
	case NMID_MAIN_MENU:
		main_menu_handle_button_press((main_menu_state_t*)state, button);
		break;
	}
}

static void select_menu_button(neuro_menu_t *_menu, neuro_button_t *button)
{
	imh_hdr_t *menu = (imh_hdr_t*)_menu->pixels;
	uint8_t *pixels = (uint8_t*)menu + sizeof(imh_hdr_t);
	uint32_t item_left = (button->left - _menu->left) / 2;
	uint32_t item_top = button->top - _menu->top;
	uint32_t item_right = (button->right - _menu->left) / 2;
	uint32_t item_bottom = button->bottom - _menu->top;

	for (uint16_t h = item_top; h <= item_bottom; h++)
	{
		for (uint16_t w = item_left; w <= item_right; w++)
		{
			pixels[h * menu->width + w] ^= 0xFF;
		}
	}

	return;
}

static void unselect_menu_button(neuro_menu_t *menu, neuro_button_t *button)
{
	select_menu_button(menu, button);
}

static neuro_button_t* menu_button_kboard_hit_test(neuro_menu_t *menu)
{
	for (uint16_t u = 0; u < menu->items_count; u++)
	{
		neuro_button_t *hit = &menu->items[u];
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

static void neuro_menu_handle_kboard_events(neuro_menu_id_t id, neuro_menu_t *menu,
				int *state, sfEvent *event, int *kboard_lock)
{
	neuro_button_t *hit = NULL;
	static neuro_button_t *selected = NULL; /* selected button */

	switch (event->type) {
	case sfEvtKeyPressed:
		if (hit = menu_button_kboard_hit_test(menu))
		{
			if (!selected)
			{
				select_menu_button(menu, hit);
				selected = hit;
				*kboard_lock = 1;
			}
		}
		break;

	case sfEvtKeyReleased:
		if (selected)
		{
			unselect_menu_button(menu, selected);
			menu_handle_button_press(id, state, selected);
			selected = NULL;
			*kboard_lock = 0;
		}
		break;

	case sfEvtTextEntered:
		menu_handle_text_enter(id, state, &event->text);
		break;

	default:
		break;
	}
}

static neuro_button_t* menu_button_mouse_hit_test(neuro_menu_t *menu)
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];

	for (uint16_t u = 0; u < menu->items_count; u++)
	{
		neuro_button_t *hit = &menu->items[u];

		if (cursor->left > hit->left && cursor->left < hit->right &&
			cursor->top > hit->top && cursor->top < hit->bottom)
		{
			return hit;
		}
	}

	return NULL;
}

static void neuro_menu_handle_mouse_events(neuro_menu_id_t id, neuro_menu_t *menu,
				int *state, sfEvent *event, int *mouse_lock)
{
	neuro_button_t *hit = NULL;
	static neuro_button_t *selected = NULL; /* selected button */
	static int _selected = 0;

	switch (event->type) {
	case sfEvtMouseButtonPressed: {
		if (sfMouse_isButtonPressed(sfMouseLeft))
		{
			if (hit = menu_button_mouse_hit_test(menu))
			{
				if (!selected)
				{
					select_menu_button(menu, hit);
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
			hit = menu_button_mouse_hit_test(menu);

			if (selected)
			{
				if (hit != selected)
				{
					if (_selected)
					{
						unselect_menu_button(menu, selected);
						_selected = 0;
					}
				}
				else
				{
					if (_selected == 0)
					{
						select_menu_button(menu, hit);
						_selected = 1;
					}
				}
			}
			else if (hit)
			{
				select_menu_button(menu, hit);
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
				unselect_menu_button(menu, selected);
				_selected = 0;
			}

			if (selected == menu_button_mouse_hit_test(menu))
			{
				menu_handle_button_press(id, state, selected);
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

static void neuro_menu_handle_input(neuro_menu_id_t id, neuro_menu_t *menu,
				int *state, sfEvent *event)
{
	static int mouse_lock = 0;
	static int kboard_lock = 0;

	switch (event->type) {
	case sfEvtMouseButtonPressed:
	case sfEvtMouseButtonReleased:
	case sfEvtMouseMoved:
		if (!kboard_lock)
		{
			 neuro_menu_handle_mouse_events(id, menu, state, event, &mouse_lock);
		}
		break;

	case sfEvtKeyPressed:
	case sfEvtKeyReleased:
	case sfEvtTextEntered:
		if (!mouse_lock)
		{
			neuro_menu_handle_kboard_events(id, menu, state, event, &kboard_lock);
		}
		break;

	default:
		break;
	}
}

static void handle_input(sfEvent *event)
{
	neuro_menu_handle_input(NMID_MAIN_MENU, &g_menu_dialog, (int*)&g_state, event);
}

static neuro_scene_id_t update(sfEvent *event)
{
	neuro_scene_id_t scene = NSID_MAIN_MENU;

	update_cursor();

	switch (g_state) {
	case MMS_TO_LEVEL_SCENE:
	case MMS_TO_NOT_IMPLEMENTED_SCENE:
		return (g_state == MMS_TO_LEVEL_SCENE) ? NSID_LEVEL : NSID_NOT_IMPLEMENTED;

	default:
		break;
	}

	return scene;
}

static void deinit()
{
	free(g_dialog);
	drawing_control_remove_sprite_from_chain(SCI_BACKGRND);
	drawing_control_remove_sprite_from_chain(SCI_DIALOG);
}

void setup_main_menu_scene()
{
	g_scene.id = NSID_MAIN_MENU;
	g_scene.init = init;
	g_scene.handle_input = handle_input;
	g_scene.update = update;
	g_scene.deinit = deinit;

	g_state = MMS_INITIAL;
}
