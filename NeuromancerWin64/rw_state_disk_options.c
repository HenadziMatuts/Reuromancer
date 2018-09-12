#include "data.h"
#include "globals.h"
#include "save_load.h"
#include "resource_manager.h"
#include "window_animation.h"
#include "neuro_menu_control.h"
#include "scene_real_world.h"

typedef enum disk_options_state_t {
	DOS_OPEN = 0,
	DOS_MAIN_MENU,
	DOS_LOAD,
	  DOS_RELOAD_LEVEL,
	DOS_SAVE,
	DOS_PAUSE_WFI,
	DOS_QUIT,
	DOS_CLOSE,
} disk_options_state_t;

static disk_options_state_t g_state = DOS_OPEN;
uint8_t g_load_game = 0;

static disk_options_quit()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("Quit", 6, 0);
	neuro_menu_draw_text("OK to QUIT Y/N", 0, 3);

	neuro_menu_add_item(11, 3, 1, 1, 'y');
	neuro_menu_add_item(13, 3, 1, 0, 'n');

	return DOS_QUIT;
}

static disk_options_state_t disk_options_pause()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("Game paused.", 0, 2);
	neuro_menu_draw_text("Press a key\nto resume.", 0, 4);

	g_paused = 1;

	return DOS_PAUSE_WFI;
}

static disk_options_state_t disk_options_main_menu()
{
	neuro_menu_flush();

	neuro_menu_draw_text("Disk Options", 2, 0);
	neuro_menu_draw_text("L. Load", 0, 1);
	neuro_menu_draw_text("S. Save", 0, 2);
	neuro_menu_draw_text("P. Pause", 0, 3);
	neuro_menu_draw_text("Q. Quit", 0, 4);
	neuro_menu_draw_text("exit", 6, 5);

	neuro_menu_flush_items();

	neuro_menu_add_item(0, 1, 16, 1, 'l');
	neuro_menu_add_item(0, 2, 16, 2, 's');
	neuro_menu_add_item(0, 3, 16, 3, 'p');
	neuro_menu_add_item(0, 4, 16, 4, 'q');
	neuro_menu_add_item(6, 5, 4, 0x0A, 'x');

	return DOS_MAIN_MENU;
}

static disk_options_state_t on_disk_options_quit_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0: /* no */
		return disk_options_main_menu();

	case 1: /* yes */
		g_exit_game = 1;
		break;
	}

	return DOS_QUIT;
}

static disk_options_state_t on_disk_options_main_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 1: /* load */
		load_menu();
		return DOS_LOAD;

	case 2: /* save */
		save_menu();
		return DOS_SAVE;

	case 3: /* pause */
		return disk_options_pause();

	case 4: /* quit */
		return disk_options_quit();

	case 0x0A: /* exit */
		return DOS_CLOSE;
	}

	return DOS_MAIN_MENU;
}

void disk_menu_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case DOS_MAIN_MENU:
		*state = on_disk_options_main_menu_button(button);
		break;

	case DOS_QUIT:
		*state = on_disk_options_quit_button(button);
		break;

	case DOS_LOAD: {
		int loaded = on_load_menu_button(button);
		if (loaded == 0)
		{
			*state = disk_options_main_menu();
		}
		else if (loaded == 1)
		{
			g_load_game = 1;
			*state = DOS_RELOAD_LEVEL;
		}
		break;
	}

	case DOS_SAVE: {
		int saved = on_save_menu_button(button);
		if (saved == 0)
		{
			*state = disk_options_main_menu();
		}
		else if (saved == 1)
		{
			g_paused = 1;
			*state = DOS_PAUSE_WFI;
		}
		break;
	}

	default:
		break;
	}
}

static disk_options_state_t disk_options_wait_for_input(disk_options_state_t state, sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		switch (state) {
		case DOS_PAUSE_WFI:
			g_paused = 0;
			return disk_options_main_menu();
		}
	}

	return state;
}

void handle_disk_options_input(sfEvent *event)
{
	switch (g_state) {
	case DOS_PAUSE_WFI:
		g_state = disk_options_wait_for_input(g_state, event);
		break;

	case DOS_MAIN_MENU:
	case DOS_QUIT:
	case DOS_LOAD:
	case DOS_SAVE:
		neuro_menu_handle_input(NMID_DISK_OPTIONS_MENU, &g_neuro_menu, (int*)&g_state, event);
		break;
	}
}

static screen_fading_data_t g_screen_fading_data = {
	.direction = FADE_OUT,
	.step = 32,
	.frame_cap = 15,
};

static window_folding_frame_data_t g_close_frame_data[12] = {
	{ 144, 64, 48, 120 }, { 144, 62, 48, 121 }, { 144, 58,  48, 123 },
	{ 144, 50, 48, 127 }, { 144, 34, 48, 135 }, { 144,  2,  48, 151 },
	{ 142,  2, 49, 151 }, { 138,  2, 51, 151 }, { 128,  2,  56, 151 },
	{ 110,  2, 65, 151 }, {  74,  2, 83, 151 }, {   2,  2, 119, 151 }
}, g_open_frame_data[12] = {
	{   2,  2, 119, 151 }, {   4,  2, 118, 151 }, {   8,  2, 116, 151 },
	{  18,  2, 111, 151 }, {  36,  2, 102, 151 }, {  72,  2,  84, 151 },
	{ 144,  2,  48, 151 }, { 144,  4,  48, 150 }, { 144,  8,  48, 148 },
	{ 144, 16,  48, 144 }, { 144, 32,  48, 136 }, { 144, 64,  48, 120 }
};

static window_folding_data_t g_disk_options_anim_data = {
	.total_frames = 12,
	.frame_cap = 28,
	.pixels = g_seg011.data,
};

real_world_state_t update_disk_options()
{
	static int anim = 0;

	switch (g_state) {
	case DOS_OPEN:
	case DOS_CLOSE:
		if (!anim)
		{
			anim = 1;
			g_disk_options_anim_data.frame_data = (g_state == DOS_OPEN) ?
				g_open_frame_data : g_close_frame_data;
			g_disk_options_anim_data.sprite_chain_index = (g_state == DOS_OPEN) ?
				g_4bae.frame_sc_index : g_4bae.frame_sc_index + 1;
			window_animation_setup(WA_TYPE_WINDOW_FOLDING, &g_disk_options_anim_data);
		}
		else if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			anim = 0;
			if (g_state == DOS_OPEN)
			{
				neuro_menu_create(6, 7, 16, 16, 6, NULL);
				g_state = disk_options_main_menu();
			}
			else
			{
				neuro_menu_destroy();
				g_state = DOS_OPEN;
				return RWS_NORMAL;
			}
		}
		break;

	case DOS_RELOAD_LEVEL:
		window_animation_setup(WA_TYPE_SCREEN_FADING, &g_screen_fading_data);
		g_state = DOS_OPEN;
		return RWS_RELOAD_LEVEL;
	}

	return RWS_DISK_OPTIONS;
}
