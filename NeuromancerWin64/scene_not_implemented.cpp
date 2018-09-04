#include "globals.h"
#include "data.h"
#include "scene_control.h"
#include "resource_manager.h"
#include "neuro_menu_control.h"
#include "drawing_control.h"
#include "neuro_menu_control.h"
#include <neuro_routines.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

typedef enum not_implemented_state_t {
	NIS_INITIAL = 0,
	NIS_TO_MAIN_MENU
} not_implemented_state_t;

static not_implemented_state_t g_state = NIS_INITIAL;

void not_implemented_menu_handle_button_press(int *state, neuro_button_t *button)
{
	switch (button->code) {
	case 0:
		neuro_menu_destroy();
		*state = NIS_TO_MAIN_MENU;
		break;

	default:
		break;
	}
}

static void init()
{
	memset(g_vga, 0, 320 * 200 * 4);

	assert(resource_manager_load_resource("TITLE.IMH", g_seg010.background));
	drawing_control_add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_seg010.background, 1);

	neuro_menu_create(6, 9, 13, 22, 3, g_seg011.data);
	neuro_menu_draw_text("Not implemented yet :(", 0, 0);
	neuro_menu_draw_text("oKay", 9, 2);
	neuro_menu_add_item(9, 2, 4, 0, 'k');
}

static void handle_input(sfEvent *event)
{
	neuro_menu_handle_input(NMID_NOT_IMPLEMENTED_MENU, &g_neuro_menu, (int*)&g_state, event);
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
	drawing_control_remove_sprite_from_chain(SCI_BACKGRND);
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
