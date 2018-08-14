#include "globals.h"
#include "scene_control.h"
#include "resource_manager.h"
#include "drawing_control.h"
#include <neuro_routines.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

static uint8_t *g_dialog = NULL;
static neuro_menu_t g_not_implemented_dialog;
static int16_t g_selected_dialog_item = -1;

static void init()
{
	memset(g_vga, 0, 320 * 200 * 4);

	assert(resource_manager_load_resource("TITLE.IMH", g_background));
	drawing_control_add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_background, 1);

	assert(g_dialog = calloc(8192, 1));
	build_menu_dialog_frame(&g_not_implemented_dialog, 64, 100, 192, 40, 6, g_dialog);
	build_menu_dialog_text(&g_not_implemented_dialog, "Not implemented yet :(", 0, 0);
	build_menu_dialog_text(&g_not_implemented_dialog, "Okay", 72, 16);
	build_menu_dialog_item(&g_not_implemented_dialog, 72, 16, 32, 0, 'k');
	drawing_control_add_sprite_to_chain(SCI_DIALOG, 64, 100, g_dialog, 1);
}

static neuro_scene_id_t on_menu_dialog_item(char item)
{
	neuro_scene_id_t scene = NSID_NOT_IMPLEMENTED;

	switch (item)
	{
	case 'k':
		scene = NSID_MAIN_MENU;
		break;

	default:
		break;
	}

	return scene;
}

static neuro_scene_id_t update(sfEvent *event)
{
	neuro_scene_id_t scene = NSID_NOT_IMPLEMENTED;

	update_cursor();

	/* mouse input */
	if (sfMouse_isButtonPressed(sfMouseLeft))
	{
		int selected = 0;

		for (uint16_t i = 0; i < g_not_implemented_dialog.items_count; i++)
		{
			if (cursor_menu_dialog_item_hit_test(i, &g_not_implemented_dialog))
			{
				if (g_selected_dialog_item == -1 || g_selected_dialog_item == i)
				{
					select_menu_dialog_item(&g_not_implemented_dialog,
						&g_not_implemented_dialog.items[i], 1);
					g_selected_dialog_item = i;
					selected = 1;
					break;
				}
			}
		}
		if (!selected)
		{
			unselect_menu_dialog_items(&g_not_implemented_dialog);
		}
	}
	else if (event->mouseButton.type == sfEvtMouseButtonReleased)
	{
		int selected = g_selected_dialog_item;
		unselect_menu_dialog_items(&g_not_implemented_dialog);
		g_selected_dialog_item = -1;

		for (uint16_t i = 0; i < g_not_implemented_dialog.items_count; i++)
		{
			if (cursor_menu_dialog_item_hit_test(i, &g_not_implemented_dialog))
			{
				if (selected == i)
				{
					scene = on_menu_dialog_item(g_not_implemented_dialog.items[i].label);
					break;
				}
			}
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

void setup_not_implemented_scene()
{
	g_scene.id = NSID_NOT_IMPLEMENTED;
	g_scene.init = init;
	g_scene.handle_input = NULL;
	g_scene.update = update;
	g_scene.deinit = deinit;
}
