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
	PS_USER_INFO,
	PS_USER_INFO_WFI,
	PS_USER_INFO_END_WFI,
	PS_CLOSE_PAX
} pax_state_t;

static pax_state_t g_state = PS_OPEN_PAX;
static uint8_t g_text_data[2048] = { 0, };
static uint8_t *g_text_ptr = g_text_data;

static void sub_14BAF(uint16_t a, uint16_t b)
{
	assert((g_neuro_window.mode == 2) || (g_neuro_window.mode == 3));

	g_neuro_window.c92a = a;
	g_neuro_window.c92c = b;
}

static pax_state_t pax_main_menu()
{
	neuro_window_clear();
	sub_14BAF(8, 8);

	neuro_window_draw_string(""
		"X. Exit System\n"
		"1. First Time PAX User Info.\n"
		"2. Access Banking Interlink\n"
		"3. Night City News\n"
		"4. Bulletin Board\n"
		"\n\n\n"
		"        choose a function", 1);

	neuro_window_flush_buttons();
	neuro_window_add_button(&g_pax_buttons.exit);
	neuro_window_add_button(&g_pax_buttons.user_info);
	neuro_window_add_button(&g_pax_buttons.banking);
	neuro_window_add_button(&g_pax_buttons.news);
	neuro_window_add_button(&g_pax_buttons.board);

	return PS_MAIN_MENU;
}

static pax_state_t on_pax_user_info()
{
	assert(resource_manager_load_resource("FTUSER.TXH", g_text_data));

	neuro_window_clear();
	neuro_window_flush_buttons();
	sub_14BAF(8, 8);

	neuro_window_draw_string(g_text_data, 1);
	g_text_ptr = g_text_data + strlen(g_text_data) + 1;

	return PS_USER_INFO;
}

static pax_state_t on_pax_main_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0: /*exit*/
		return PS_CLOSE_PAX;

	case 1: /* user_info */
		return on_pax_user_info();

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

static pax_state_t handle_pax_wait_for_input(pax_state_t state, sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		switch (state) {
		case PS_USER_INFO_WFI:
			return PS_USER_INFO;

		case PS_USER_INFO_END_WFI:
			return pax_main_menu();

		default:
			return state;
		}
	}

	return state;
}

void handle_pax_input(sfEvent *event)
{
	switch (g_state) {
	case PS_MAIN_MENU:
		neuro_window_handle_input((int*)&g_state, event);
		break;

	case PS_USER_INFO_WFI:
	case PS_USER_INFO_END_WFI:
		g_state = handle_pax_wait_for_input(g_state, event);
		break;

	default:
		break;
	}
}

pax_state_t update_pax_user_info()
{
	static int lines_on_screen = 0, lines_scrolled = 0;
	static int next_line = 1;
	static int frame_cap_ms = 20;
	static int elapsed = 0;
	
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return PS_USER_INFO;
	}
	elapsed = passed;

	if (next_line)
	{
		char line[39] = { 0, };
		int last = extract_line(&g_text_ptr, line, 38);

		neuro_window_draw_string(line, 0);
		next_line = 0;

		if (last)
		{
			next_line = 1;
			lines_on_screen = 0;
			neuro_window_draw_string("[press any key to get back to main menu]", 0);
			return PS_USER_INFO_END_WFI;
		}
		else if (++lines_on_screen == 9)
		{
			lines_on_screen = 0;
			return PS_USER_INFO_WFI;
		}

		return PS_USER_INFO;
	}
	else
	{
		uint8_t *pix = g_seg011 + sizeof(imh_hdr_t);

		for (int i = 17, j = 16; i < 96; i++, j++)
		{
			memmove(&pix[160 * j + 8], &pix[160 * i + 8], 304);
		}

		if (++lines_scrolled == 8)
		{
			lines_scrolled = 0;
			next_line = 1;
		}
	}

	return PS_USER_INFO;
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

	case PS_USER_INFO:
		g_state = update_pax_user_info();
		break;
	}
	
	return RWS_PAX;
}
