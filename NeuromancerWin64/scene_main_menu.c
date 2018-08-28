#include "globals.h"
#include "data.h"
#include "scene_control.h"
#include "drawing_control.h"
#include "neuro_menu_control.h"
#include "resource_manager.h"
#include "window_animation.h"
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

static screen_fading_data_t g_screen_fading_data = {
	.direction = FADE_OUT,
	.step = 32,
	.frame_cap = 15,
};

void main_menu_handle_text_enter(int *state, sfTextEvent *event)
{
	static char name[11] = { 0, };

	if (*state == MMS_NEW)
	{
		char input[12] = { 0x00 };
		sfKeyCode key = sfHandleTextInput(event->unicode, name, 11, 0, 0);

		if (key == sfKeyReturn)
		{
			if (strlen(name))
			{
				sprintf(g_4bae.name + 2, "%s", name);
			}
			memset(name, 0, 11);

			neuro_menu_flush();
			window_animation_setup(WA_TYPE_SCREEN_FADING, &g_screen_fading_data);
			*state = MMS_TO_LEVEL_SCENE;
		}
		else
		{
			sprintf(input, "%s%s", name, "<");
			memset(input + strlen(input), 0x20, 11 - strlen(input));
			neuro_menu_draw_text(input, 0, 2);
		}
	}
}

void main_menu_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case MMS_INITIAL:
		switch (button->code) {
		case 0: /* new */
			neuro_menu_flush();
			neuro_menu_draw_frame(6, 5, 20, 14, 3, NULL);
			neuro_menu_draw_text("Your name?", 0, 0);
			neuro_menu_draw_text("<", 0, 2);
			*state = MMS_NEW;
			break;

		case 1: /* load */
			neuro_menu_flush();
			neuro_menu_draw_frame(6, 7, 16, 16, 6, NULL);
			neuro_menu_draw_text("Load Game", 4, 0);
			neuro_menu_draw_text("1  2  3  4", 3, 2);
			neuro_menu_draw_text("exit", 6, 5);
			neuro_menu_add_item(3, 2, 1, 0, '1');
			neuro_menu_add_item(6, 2, 1, 1, '2');
			neuro_menu_add_item(9, 2, 1, 2, '3');
			neuro_menu_add_item(12, 2, 1, 3, '4');
			neuro_menu_add_item(6, 5, 4, 10, 'x');
			*state = MMS_LOAD;
			break;
		}
		break;

	case MMS_LOAD:
		switch (button->code) {
		case 10: /* exit */
			neuro_menu_flush();
			neuro_menu_draw_frame(6, 5, 20, 10, 1, NULL);
			neuro_menu_draw_text("New/Load", 1, 0);
			neuro_menu_add_item(1, 0, 3, 0, 'n');
			neuro_menu_add_item(5, 0, 4, 1, 'l');
			*state = MMS_INITIAL;
			break;

		case 0: /* savegame slots */
		case 1:
		case 2:
		case 3:
			neuro_menu_flush();
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

	neuro_menu_draw_frame(6, 5, 20, 10, 1, NULL);
	neuro_menu_draw_text("New/Load", 1, 0);
	neuro_menu_add_item(1, 0, 3, 0, 'n');
	neuro_menu_add_item(5, 0, 4, 1, 'l');
}

static void handle_input(sfEvent *event)
{
	neuro_menu_handle_input(NMID_MAIN_MENU, &g_neuro_menu, (int*)&g_state, event);
}

static neuro_scene_id_t update()
{
	neuro_scene_id_t scene = NSID_MAIN_MENU;

	update_cursor();

	switch (g_state) {
	case MMS_TO_LEVEL_SCENE:
		if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			g_screen_fading_data.direction = FADE_IN;
			window_animation_setup(WA_TYPE_SCREEN_FADING, &g_screen_fading_data);
			return NSID_REAL_WORLD;
		}
		break;

	case MMS_TO_NOT_IMPLEMENTED_SCENE:
		return NSID_NOT_IMPLEMENTED;

	default:
		break;
	}

	return scene;
}

static void deinit()
{
	drawing_control_remove_sprite_from_chain(SCI_BACKGRND);
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
