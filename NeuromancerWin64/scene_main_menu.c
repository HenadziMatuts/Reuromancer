#include "globals.h"
#include "data.h"
#include "scene_control.h"
#include "drawing_control.h"
#include "neuro_menu_control.h"
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

void main_menu_handle_text_enter(int *state, sfTextEvent *event)
{
	static char name[11] = { 0, };

	if (*state == MMS_NEW)
	{
		char input[12] = { 0x00 };
		sfKeyCode key = sfHandleTextInput(event->unicode, name, 11, 0);

		if (key == sfKeyReturn)
		{
			sfSetKeyRepeat(0);
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
			build_neuro_menu_text(&g_menu_dialog, input, 0, 16);
		}
	}
}

void main_menu_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case MMS_INITIAL:
		switch (button->code) {
		case 0: /* new */
			build_neuro_menu_frame(&g_menu_dialog, 32, 152, 128, 40, 6, g_dialog);
			build_neuro_menu_text(&g_menu_dialog, "Your name?", 0, 0);
			build_neuro_menu_text(&g_menu_dialog, "<", 0, 16);
			drawing_control_add_sprite_to_chain(SCI_DIALOG, 32, 152, g_dialog, 1);
			sfSetKeyRepeat(1);
			*state = MMS_NEW;
			break;

		case 1: /* load */
			build_neuro_menu_frame(&g_menu_dialog, 48, 120, 144, 64, 6, g_dialog);
			build_neuro_menu_text(&g_menu_dialog, "Load Game", 32, 0);
			build_neuro_menu_text(&g_menu_dialog, "1  2  3  4", 24, 16);
			build_neuro_menu_text(&g_menu_dialog, "exit", 48, 40);
			build_neuro_menu_item(&g_menu_dialog, 24, 16, 8, 0, '1');
			build_neuro_menu_item(&g_menu_dialog, 48, 16, 8, 1, '2');
			build_neuro_menu_item(&g_menu_dialog, 72, 16, 8, 2, '3');
			build_neuro_menu_item(&g_menu_dialog, 96, 16, 8, 3, '4');
			build_neuro_menu_item(&g_menu_dialog, 48, 40, 32, 10, 'x');
			drawing_control_add_sprite_to_chain(SCI_DIALOG, 48, 120, g_dialog, 1);
			*state = MMS_LOAD;
			break;
		}
		break;

	case MMS_LOAD:
		switch (button->code) {
		case 10: /* exit */
			build_neuro_menu_frame(&g_menu_dialog, 32, 152, 96, 24, 6, g_dialog);
			build_neuro_menu_text(&g_menu_dialog, "New/Load", 8, 0);
			build_neuro_menu_item(&g_menu_dialog, 8, 0, 24, 0, 'n');
			build_neuro_menu_item(&g_menu_dialog, 40, 0, 32, 1, 'l');
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
	build_neuro_menu_frame(&g_menu_dialog, 32, 152, 96, 24, 6, g_dialog);
	build_neuro_menu_text(&g_menu_dialog, "New/Load", 8, 0);
	build_neuro_menu_item(&g_menu_dialog, 8, 0, 24, 0, 'n');
	build_neuro_menu_item(&g_menu_dialog, 40, 0, 32, 1, 'l');
	drawing_control_add_sprite_to_chain(SCI_DIALOG, 32, 152, g_dialog, 1);
}

static void handle_input(sfEvent *event)
{
	neuro_menu_handle_input(NMID_MAIN_MENU, &g_menu_dialog, (int*)&g_state, event);
}

static neuro_scene_id_t update_main_menu_fade_out()
{
	static int frame = 0;

	static int frame_cap_ms = 15;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return NSID_MAIN_MENU;
	}
	elapsed = passed;

	if (frame == 7)
	{
		frame = 0;
		g_fader_alpha = 0xFF;
		return NSID_REAL_WORLD;
	}

	frame++;
	g_fader_alpha += 32;

	return NSID_MAIN_MENU;
}

static neuro_scene_id_t update()
{
	neuro_scene_id_t scene = NSID_MAIN_MENU;

	update_cursor();

	switch (g_state) {
	case MMS_TO_LEVEL_SCENE:
		return update_main_menu_fade_out();

	case MMS_TO_NOT_IMPLEMENTED_SCENE:
		return NSID_NOT_IMPLEMENTED;

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
