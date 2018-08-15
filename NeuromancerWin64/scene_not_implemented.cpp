#include "globals.h"
#include "scene_control.h"
#include "resource_manager.h"
#include "neuro_menu_control.h"
#include "drawing_control.h"
#include <neuro_routines.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

typedef enum not_implemented_state_t {
	NIS_INITIAL = 0,
	NIS_TO_MAIN_MENU
} not_implemented_state_t;

static not_implemented_state_t g_state = NIS_INITIAL;

static uint8_t *g_dialog = NULL;
static neuro_menu_t g_not_implemented_dialog;

void not_implemented_menu_handle_button_press(int *state, neuro_button_t *button)
{
	switch (button->code) {
	case 0:
		*state = NIS_TO_MAIN_MENU;
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
	build_neuro_menu_frame(&g_not_implemented_dialog, 64, 100, 192, 40, 6, g_dialog);
	build_neuro_menu_text(&g_not_implemented_dialog, "Not implemented yet :(", 0, 0);
	build_neuro_menu_text(&g_not_implemented_dialog, "Okay", 72, 16);
	build_neuro_menu_item(&g_not_implemented_dialog, 72, 16, 32, 0, 'k');
	drawing_control_add_sprite_to_chain(SCI_DIALOG, 64, 100, g_dialog, 1);
}

static void handle_input(sfEvent *event)
{
	neuro_menu_handle_input(NMID_MAIN_MENU, &g_not_implemented_dialog, (int*)&g_state, event);
}

static neuro_scene_id_t update()
{
	neuro_scene_id_t scene = NSID_NOT_IMPLEMENTED;
	update_cursor();

	switch (g_state) {
	case NIS_TO_MAIN_MENU:
		return NSID_MAIN_MENU;
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
	g_scene.handle_input = handle_input;
	g_scene.update = update;
	g_scene.deinit = deinit;

	g_state = NIS_INITIAL;
}
