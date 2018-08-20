#include "data.h"
#include "globals.h"
#include "resource_manager.h"
#include "scene_real_world.h"
#include "neuro_window_control.h"
#include "drawing_control.h"
#include <neuro_routines.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef enum pax_state_t {
	PS_OPEN_PAX = 0,
	PS_MAIN_MENU,
	PS_CLOSE_PAX
} pax_state_t;

static pax_state_t g_state = PS_OPEN_PAX;

static void sub_14BAF(uint16_t a, uint16_t b)
{
	assert((g_neuro_window.mode == 2) || (g_neuro_window.mode == 3));

	g_neuro_window.c92a = a;
	g_neuro_window.c92c = b;
}

static pax_state_t pax_main_menu()
{
	sub_14BAF(8, 8);

	neuro_window_draw_string(""
		"X. Exit System\n"
		"1. First Time PAX User Info.\n"
		"2. Access Banking Interlink\n"
		"3. Night City News\n"
		"4. Bulletin Board\n"
		"\n\n\n"
		"        choose a function", 1);

	g_neuro_window.total_items = 0;
	neuro_window_add_button(&g_pax_buttons.exit);
	neuro_window_add_button(&g_pax_buttons.user_info);
	neuro_window_add_button(&g_pax_buttons.banking);
	neuro_window_add_button(&g_pax_buttons.news);
	neuro_window_add_button(&g_pax_buttons.board);

	return PS_MAIN_MENU;
}

static pax_state_t on_pax_main_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0: /*exit*/
		return PS_CLOSE_PAX;

	default:
		break;
	}

	return PS_MAIN_MENU;
}

void rw_pax_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case PS_MAIN_MENU:
		*state = on_pax_main_menu_button(button);
		break;

	default:
		break;
	}
}

void handle_pax_input(sfEvent *event)
{
	switch (g_state) {
	case PS_MAIN_MENU:
	default:
		neuro_window_handle_input((int*)&g_state, event);
		break;
	}
}

pax_state_t update_pax_close()
{
	uint16_t frames[12][4] = {
		//  w,  h,   l,   t,
		{ 320, 104,  0,  4 },{ 320, 102,  0,  5 },{ 320, 98,   0,  7 },
		{ 320,  86,  0, 13 },{ 320,  64,  0, 24 },{ 320, 30,   0, 41 },
		{ 314,   2,  3, 55 },{ 302,   2,  9, 55 },{ 278,  2,  21, 55 },
		{ 230,   2, 45, 55 },{ 134,   2, 93, 55 },{   4,  2, 158, 55 },
	};

	static int frame = 0;
	static int frame_cap_ms = 35;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return PS_CLOSE_PAX;
	}
	elapsed = passed;

	if (frame == 12)
	{
		frame = 0;
		drawing_control_remove_sprite_from_chain(++g_4bae.window_sc_index);
		restore_window();
		return PS_OPEN_PAX;
	}

	build_text_frame(frames[frame][1], frames[frame][0], (imh_hdr_t*)g_seg011);
	drawing_control_add_sprite_to_chain(g_4bae.window_sc_index + 1,
		frames[frame][2], frames[frame][3], g_seg011, 1);
	frame++;

	return PS_CLOSE_PAX;
}

pax_state_t update_pax_open()
{
	uint16_t frames[12][4] = {
		//  w,  h,   l,   t,
		{   4,  2, 158, 55 },{  10,  2, 155, 55 },{  20,   2, 150, 55 },
		{  40,  2, 140, 55 },{  80,  2, 120, 55 },{ 160,   2,  80, 55 },
		{ 320,  4,   0, 54 },{ 320,  6,   0, 53 },{ 320,  14,   0, 49 },
		{ 320, 26,   0, 43 },{ 320, 52,   0, 30 },{ 320, 104,   0,  4 }
	};

	static int frame = 0;
	static int frame_cap_ms = 35;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return PS_OPEN_PAX;
	}
	elapsed = passed;

	if (frame == 12)
	{
		frame = 0;
		neuro_window_setup(NWM_PAX);
		return pax_main_menu();
	}

	build_text_frame(frames[frame][1], frames[frame][0], (imh_hdr_t*)g_seg011);
	drawing_control_add_sprite_to_chain(g_4bae.window_sc_index,
		frames[frame][2], frames[frame][3], g_seg011, 1);
	frame++;

	return PS_OPEN_PAX;
}

real_world_state_t update_pax()
{
	switch (g_state) {
	case PS_OPEN_PAX:
		g_state = update_pax_open();
		break;

	case PS_CLOSE_PAX:
		g_state = update_pax_close();
		if (g_state == PS_OPEN_PAX)
		{
			return RWS_NORMAL;
		}
		break;
	}
	
	return RWS_PAX;
}
