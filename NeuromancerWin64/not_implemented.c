#include "globals.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static uint8_t *g_dialog = NULL;
static neuro_dialog_t g_menu_dialog;
static int16_t g_selected_dialog_item = -1;

static neuro_scene_id_t exec_item(char item)
{
	neuro_scene_id_t ret = NSID_NOT_IMPLEMENTED;

	switch (item)
	{
	case 'x':
		ret = NSID_MAIN_MENU;
		break;

	default:
		break;
	}

	return ret;
}

static void init()
{
	memset(g_vga, 0x00, 320 * 200 * 4);

	assert(resource_manager_load("TITLE.IMH", g_background));
	add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_background, 1);

	assert(g_dialog = calloc(64000, 1));
	build_dialog_frame(&g_menu_dialog, 64, 80, 192, 40, 6, g_dialog);
	build_dialog_text(&g_menu_dialog, "Not implemented Yet :(", 0, 0);
	build_dialog_text(&g_menu_dialog, "okay", 72, 16);
	build_dialog_item(&g_menu_dialog, 72, 16, 32, 0, 'x');
	add_sprite_to_chain(SCI_DIALOG, 64, 80, g_dialog, 1);
}

static neuro_scene_id_t update(sfEvent *event)
{
	int selected = 0;
	neuro_scene_id_t ret = NSID_NOT_IMPLEMENTED;

	update_cursor();

	if (sfMouse_isButtonPressed(sfMouseLeft))
	{
		for (uint16_t i = 0; i < g_menu_dialog.items_count; i++)
		{
			if (g_sprite_chain[SCI_CURSOR].left > g_menu_dialog.items[i].left &&
				g_sprite_chain[SCI_CURSOR].left < g_menu_dialog.items[i].right &&
				g_sprite_chain[SCI_CURSOR].top > g_menu_dialog.items[i].top &&
				g_sprite_chain[SCI_CURSOR].top < g_menu_dialog.items[i].bottom)
			{
				if (g_selected_dialog_item == -1 || g_selected_dialog_item == i)
				{
					select_dialog_item(&g_menu_dialog, &g_menu_dialog.items[i], 1);
					g_selected_dialog_item = i;
					selected = 1;
					break;
				}
			}
		}
		if (!selected)
		{
			unselect_dialog_items(&g_menu_dialog);
		}
	}
	else if (event->mouseButton.type == sfEvtMouseButtonReleased)
	{
		int sel = g_selected_dialog_item;
		unselect_dialog_items(&g_menu_dialog);
		g_selected_dialog_item = -1;

		for (uint16_t i = 0; i < g_menu_dialog.items_count; i++)
		{
			if (g_sprite_chain[SCI_CURSOR].left > g_menu_dialog.items[i].left &&
				g_sprite_chain[SCI_CURSOR].left < g_menu_dialog.items[i].right &&
				g_sprite_chain[SCI_CURSOR].top > g_menu_dialog.items[i].top &&
				g_sprite_chain[SCI_CURSOR].top < g_menu_dialog.items[i].bottom)
			{
				if (sel == i)
				{
					ret = exec_item(g_menu_dialog.items[i].letter);
					break;
				}
			}
		}
	}

	return ret;
}

static void deinit()
{
	free(g_dialog);
	remove_sprite_from_chain(SCI_DIALOG);
}


void setup_not_implemented_scene()
{
	g_scene.id = NSID_NOT_IMPLEMENTED;
	g_scene.init = init;
	g_scene.update = update;
	g_scene.deinit = deinit;
}
