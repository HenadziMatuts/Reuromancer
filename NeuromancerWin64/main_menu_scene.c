#include "globals.h"
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
} main_menu_state_t;

static main_menu_state_t g_state = MMS_INITIAL;

static uint8_t *g_dialog = NULL;
static neuro_menu_dialog_t g_menu_dialog;
static int16_t g_selected_dialog_item = -1;

static char g_name[11] = { 0, };

static neuro_scene_id_t on_menu_dialog_item(char item)
{
	neuro_scene_id_t scene = NSID_MAIN_MENU;

	switch (g_state)
	{
	case MMS_INITIAL:
		switch (item)
		{
		case 'n':
			build_menu_dialog_frame(&g_menu_dialog, 32, 152, 128, 40, 6, g_dialog);
			build_menu_dialog_text(&g_menu_dialog, "Your name?", 0, 0);
			build_menu_dialog_text(&g_menu_dialog, "<", 0, 16);
			drawing_control_add_sprite_to_chain(SCI_DIALOG, 32, 152, g_dialog, 1);
			g_state = MMS_NEW;
			break;

		case 'l':
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
			g_state = MMS_LOAD;
			break;
		}
		break;

	case MMS_LOAD:
		switch (item)
		{
		case 'x':
			build_menu_dialog_frame(&g_menu_dialog, 32, 152, 96, 24, 6, g_dialog);
			build_menu_dialog_text(&g_menu_dialog, "New/Load", 8, 0);
			build_menu_dialog_item(&g_menu_dialog, 8, 0, 24, 0, 'n');
			build_menu_dialog_item(&g_menu_dialog, 40, 0, 32, 1, 'l');
			drawing_control_add_sprite_to_chain(SCI_DIALOG, 32, 152, g_dialog, 1);
			g_state = MMS_INITIAL;
			break;

		case '1':
		case '2':
		case '3':
		case '4':
			scene = NSID_NOT_IMPLEMENTED;
			break;
		}
		break;

	case MMS_NEW:
		break;

	default:
		break;
	}

	return scene;
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

static neuro_scene_id_t update(sfEvent *event)
{
	neuro_scene_id_t scene = NSID_MAIN_MENU;

	update_cursor();

	/* mouse input */
	if (sfMouse_isButtonPressed(sfMouseLeft))
	{
		int selected = 0;

		for (uint16_t i = 0; i < g_menu_dialog.items_count; i++)
		{
			if (cursor_menu_dialog_item_hit_test(i, &g_menu_dialog))
			{
				if (g_selected_dialog_item == -1 || g_selected_dialog_item == i)
				{
					select_menu_dialog_item(&g_menu_dialog, &g_menu_dialog.items[i], 1);
					g_selected_dialog_item = i;
					selected = 1;
					break;
				}
			}
		}
		if (!selected)
		{
			unselect_menu_dialog_items(&g_menu_dialog);
		}
	}
	else if (event->mouseButton.type == sfEvtMouseButtonReleased)
	{
		int selected = g_selected_dialog_item;
		unselect_menu_dialog_items(&g_menu_dialog);
		g_selected_dialog_item = -1;

		for (uint16_t i = 0; i < g_menu_dialog.items_count; i++)
		{
			if (cursor_menu_dialog_item_hit_test(i, &g_menu_dialog))
			{
				if (selected == i)
				{
					scene = on_menu_dialog_item(g_menu_dialog.items[i].letter);
					break;
				}
			}
		}
	}
	/* keyboard input */
	else if (event->text.type == sfEvtTextEntered && g_state == MMS_NEW)
	{
		char input[12] = { 0x00 };
		sfKeyCode key = handle_sfml_text_input(event->text.unicode, g_name, 11);

		if (key == sfKeyReturn)
		{
			scene = NSID_LEVEL;
			memset(g_name, 0, 11);
		}
		else
		{
			sprintf(input, "%s%s", g_name, "<");
			memset(input + strlen(input), 0x20, 11 - strlen(input));
			build_menu_dialog_text(&g_menu_dialog, input, 0, 16);
		}
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
	g_scene.update = update;
	g_scene.deinit = deinit;

	g_state = MMS_INITIAL;
}
