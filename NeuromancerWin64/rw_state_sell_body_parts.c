#include "data.h"
#include "window_animation.h"
#include "neuro_menu_control.h"
#include "scene_real_world.h"
#include <string.h>

typedef enum sell_parts_state_t {
	SPS_OPEN = 0,
	SPS_SELL_MENU,
	SPS_BUY_MENU,
	SPS_WFI,
	SPS_CLOSE,
} sell_parts_state_t;

static sell_parts_state_t g_state = SPS_OPEN;

/* 0x224E */
char *g_body_parts[20] = {
	"Heart",
	"Eyes (2)",
	"Lungs (2)",
	"Stomach",
	"Liver",
	"Kidneys (2)",
	"Gall Bladder",
	"Pancreas",
	"Legs (2)",
	"Arms (2)",
	"Tongue",
	"Larynx",
	"Nose",
	"Ears (2)",
	"Intestine (large)",
	"Intestine (small)",
	"Spleen",
	"Bone Marrow",
	"Spinal Fluid",
	"Appendix"
};

uint16_t g_body_parts_buy_prices[20] = {
	12000, 10000, 6000, 3000,
	2500, 2100, 2100, 1000,
	600, 600, 300, 300, 300,
	200, 100, 100, 90, 90, 60, 6
};

/* 0x233C */
uint16_t g_body_parts_sell_prices[20] = {
	6000, 5000, 3000, 1500,
	1250, 1050, 1050, 500,
	300, 300, 150, 150, 150,
	100, 50, 50, 45, 45, 30, 3
};

static int is_body_part_not_sold(int part)
{
	uint16_t bit = 0x80 >> (g_level_n & 7);
	uint16_t index = part >> 3;

	return ((bit & g_4bae.sold_body_parts_bitstring[index]) == 0) ? 1 : 0;
}

static sell_parts_state_t body_parts_menu_page(int sell, int next)
{
	char credits[9] = { 0 };
	static int items_listed = 0;

	if (next == 0)
	{
		items_listed = 0;
	}

	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("SELL PARTS  credits-", 0, 0);
	neuro_menu_draw_text("more", 14, 5);
	neuro_menu_draw_text("exit", 9, 5);

	neuro_menu_add_item(9, 5, 4, 0x0B, 'x');
	neuro_menu_add_item(14, 5, 4, 0x0A, 'm');

	sprintf(credits, "%-8ld", g_4bae.cash);
	neuro_menu_draw_text(credits, 20, 0);

	for (int i = 0; i < 4; i++)
	{
		char item_string[28] = { 0 };
		
		sprintf(item_string, "%c %c%-19s%5d",
			'1' + i,
			is_body_part_not_sold(items_listed) ? ' ' : '-',
			g_body_parts[items_listed],
			sell ? g_body_parts_sell_prices[items_listed] : g_body_parts_buy_prices[items_listed]
		);

		neuro_menu_draw_text(item_string, 0, i + 1);
		neuro_menu_add_item(3, i + 1, 25, i, '1' + i);

		items_listed++;
	}

	items_listed %= 20;

	return sell ? SPS_SELL_MENU : SPS_BUY_MENU;
}

static sell_parts_state_t body_parts_menu(int sell)
{
	neuro_menu_create(6, 1, 17, 28, 6, NULL);
	return body_parts_menu_page(sell, 0);
}

static sell_parts_state_t on_sell_parts_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* items */
		break;

	case 0x0B: /* exit */
		return SPS_CLOSE;

	case 0x0A: /* more */
		body_parts_menu_page(1, 1);
		break;

	default:
		break;
	}

	return SPS_SELL_MENU;
}

void sell_parts_menu_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case SPS_SELL_MENU:
		*state = on_sell_parts_menu_button(button);
		break;
	}
}

static sell_parts_state_t sell_parts_wfi(sell_parts_state_t state, sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		switch (state) {
		case SPS_WFI:
			return SPS_CLOSE;
		}
	}

	return state;
}

void handle_sell_parts_input(sfEvent *event)
{
	switch (g_state) {
	case SPS_WFI:
		g_state = sell_parts_wfi(g_state, event);
		break;

	case SPS_SELL_MENU:
		neuro_menu_handle_input(NMID_SELL_PARTS_MENU, &g_neuro_menu, (int*)&g_state, event);
		break;
	}
}

static window_folding_frame_data_t g_close_frame_data[12] = {
	{ 240, 64,  0, 128 }, { 240, 62,  0, 129 }, { 240, 58,   0, 131 },
	{ 240, 50,  0, 135 }, { 240, 34,  0, 143 }, { 240,  2,   0, 159 },
	{ 236,  2,  2, 159 }, { 228,  2,  6, 159 }, { 212,  2,  14, 159 },
	{ 182,  2, 29, 159 }, { 122,  2, 59, 159 }, {   2,  2, 119, 159 }
}, g_open_frame_data[12] = {
	{   2,  2, 119, 159 }, {   6,  2, 117, 159 }, {  14,  2, 113, 159 },
	{  30,  2, 105, 159 }, {  60,  2,  90, 159 }, { 120,  2,  60, 159 },
	{ 240,  2,   0, 159 }, { 240,  4,   0, 158 }, { 240,  8,   0, 156 },
	{ 240, 16,   0, 152 }, { 240, 32,   0, 144 }, { 240, 64,   0, 128 }
};

static window_folding_data_t g_sell_parts_anim_data = {
	.total_frames = 12,
	.frame_cap = 28,
	.pixels = g_seg011.data,
};

real_world_state_t update_sell_parts()
{
	static int anim = 0;

	switch (g_state) {
	case SPS_OPEN:
	case SPS_CLOSE:
		if (!anim)
		{
			anim = 1;
			g_sell_parts_anim_data.frame_data = (g_state == SPS_OPEN) ?
				g_open_frame_data : g_close_frame_data;
			g_sell_parts_anim_data.sprite_chain_index = (g_state == SPS_OPEN) ?
				g_4bae.frame_sc_index : g_4bae.frame_sc_index + 1;
			window_animation_setup(WA_TYPE_WINDOW_FOLDING, &g_sell_parts_anim_data);
		}
		else if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			anim = 0;
			if (g_state == SPS_OPEN)
			{
				g_state = body_parts_menu(1);
			}
			else
			{
				g_state = SPS_OPEN;
				neuro_menu_destroy();
				g_4bae.x4c82 = 0;
				return RWS_NORMAL;
			}
		}
		break;
	}

	return RWS_SELL_BODY_PART;
}
