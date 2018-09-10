#include "data.h"
#include "save_load.h"
#include "resource_manager.h"
#include "neuro_menu_control.h"
#include "neuro_window_control.h"
#include "drawing_control.h"
#include <assert.h>
#include <string.h>

static uint16_t g_4c6b = 0;
static uint8_t g_savegames[12000];

#pragma pack(push, 1)
typedef struct neuro_savegame_t {
	x3f85_t x3f85;
	x4bae_t x4bae;
	uint8_t visited_levels[8];
	uint8_t x3b94[374];
	neuro_menu_t neuro_menus[4];
	neuro_window_t neuro_windows[4];
} neuro_savegame_t;

static void add_slots()
{
	neuro_menu_draw_text("1  2  3  4", 3, 2);
	neuro_menu_draw_text("exit", 6, 5);

	neuro_menu_add_item(6, 5, 4, 0x0A, 'x');

	for (int i = 0; i < 4; i++)
	{
		neuro_menu_add_item((3 * i) + 3, 2, 1, i, i + '1');
	}
}

static void save_game(int slot)
{
	char text[17];
	assert(resource_manager_load_resource("SAVEGAME.SAV", g_savegames));
	neuro_savegame_t *savegame = (neuro_savegame_t*)(g_savegames + (slot * 3000));

	memmove(&savegame->x3f85, &g_3f85, sizeof(x3f85_t));
	memmove(&savegame->x4bae, &g_4bae, sizeof(x4bae_t));
	memmove(savegame->visited_levels, g_visited_levels_bitstring, 8);

	memmove(savegame->x3b94, g_3b94, 64);
	memmove(savegame->x3b94 + 64, g_sprite_chain, 310);

	memmove(savegame->neuro_menus, g_neuro_menus_pool, 3 * sizeof(neuro_menu_t));
	memmove(&savegame->neuro_menus[3], &g_neuro_menu, sizeof(neuro_menu_t));

	memmove(savegame->neuro_windows, g_neuro_windows_pool, 3 * sizeof(neuro_window_t));
	memmove(&savegame->neuro_windows[3], &g_neuro_window, sizeof(neuro_window_t));

	assert(resource_manager_write_resource("SAVEGAME.SAV", g_savegames));

	neuro_menu_flush();
	neuro_menu_flush_items();

	sprintf(text, "Saved to slot %d.", slot + 1);
	neuro_menu_draw_text(text, 0, 2);
}

int on_save_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* slots */
		g_4bae.x4c6b = g_4c6b;
		save_game(button->code);
		return 1;

	case 0x0A: /* exit */
		g_4bae.x4c6b = g_4c6b;
		return 0;
	}

	return -1;
}

void save_menu()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("Save Game", 4, 0);
	add_slots();

	g_4c6b = g_4bae.x4c6b;
	g_4bae.x4c6b = 0;
}

static void load_game(int slot)
{
	neuro_menu_destroy();
	assert(resource_manager_load_resource("SAVEGAME.SAV", g_savegames));
	neuro_savegame_t *savegame = (neuro_savegame_t*)(g_savegames + (slot * 3000));

	memmove(&g_3f85, &savegame->x3f85, sizeof(x3f85_t));
	memmove(&g_4bae, &savegame->x4bae, sizeof(x4bae_t));
	memmove(g_visited_levels_bitstring, savegame->visited_levels, 8);

	memmove(g_3b94, savegame->x3b94, 64);
	memmove(g_sprite_chain, savegame->x3b94 + 64, 310);

	memmove(g_neuro_menus_pool, savegame->neuro_menus, 3 * sizeof(neuro_menu_t));
	memmove(&g_neuro_menu, &savegame->neuro_menus[3], sizeof(neuro_menu_t));

	memmove(g_neuro_windows_pool, savegame->neuro_windows, 3 * sizeof(neuro_window_t));
	memmove(&g_neuro_window, &savegame->neuro_windows[3], sizeof(neuro_window_t));

	drawing_control_flush_sprite_chain();
	drawing_control_add_sprite_to_chain(SCI_CURSOR, 160, 100, g_seg009.cursors, 0);
	neuro_menu_destroy();

	// seg005[0] = 0xFF
	g_4bae.x4ccd = 0;
	g_4bae.x4bcc = 1;
}

int on_load_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* slots */
		load_game(button->code);
		return 1;

	case 0x0A: /* exit */
		g_4bae.x4c6b = g_4c6b;
		return 0;
	}

	return -1;
}

void load_menu()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("Load Game", 4, 0);
	add_slots();

	g_4c6b = g_4bae.x4c6b;
	g_4bae.x4c6b = 0;
}
