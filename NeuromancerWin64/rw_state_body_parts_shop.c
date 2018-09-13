#include "data.h"
#include "neuro86.h"
#include "globals.h"
#include "window_animation.h"
#include "neuro_menu_control.h"
#include "scene_real_world.h"
#include <string.h>

typedef enum parts_shop_state_t {
	PSS_OPEN = 0,
	PSS_SELL_MENU,
	PSS_BUY_MENU,
	PSS_WFI,
	PSS_CLOSE,
} parts_shop_state_t;

static parts_shop_state_t g_state = PSS_OPEN;
static int g_first_listed = 0;

/* 1 - sell, 0 - buy */
int g_body_shop_op = 1;
int g_body_shop_discount = 0;

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

uint16_t g_body_parts_buy_prices[20] = { /* 0x2314 */
	12000, 10000, 6000, 3000,
	2500, 2100, 2100, 1000,
	600, 600, 300, 300, 300,
	200, 100, 100, 90, 90, 60, 6
}, g_body_parts_sell_prices[20] = { /* 0x233C */
	6000, 5000, 3000, 1500,
	1250, 1050, 1050, 500,
	300, 300, 150, 150, 150,
	100, 50, 50, 45, 45, 30, 3
}, g_body_parts_discounted_prices[20] = { /* 0x2364 */
	6600, 6500, 3300, 1650,
	1375, 1155, 1155, 550,
	330, 330, 165, 165, 165,
	110, 78, 78, 55, 55, 33, 3
}, g_constitution_damage[20] = { /* 0x238C */
	200, 150, 150, 100,
	75, 75, 75, 75, 50, 50,
	25, 25, 25, 25, 10, 10,
	10, 10, 10, 10
};

static int body_part_is_not_sold(int part)
{
	uint16_t bit = 0x80 >> (part & 7);
	uint16_t index = part >> 3;

	return ((bit & g_4bae.sold_body_parts_bitstring[index]) == 0) ? 1 : 0;
}

typedef enum body_parts_menu_page_t {
	FIRST,
	NEXT,
	CURRENT
} body_parts_menu_page_t;

static parts_shop_state_t body_parts_menu_page(int sell, int page)
{
	char credits[9] = { 0 };
	static int items_listed = 0;

	if (page == FIRST)
	{
		items_listed = 0;
	}
	else if (page == CURRENT)
	{
		items_listed = g_first_listed;
	}

	g_first_listed = items_listed;

	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text(sell ? "SELL PARTS  credits-" :
		"BUY PARTS   credits - ", 0, 0);
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
			body_part_is_not_sold(items_listed) ? ' ' : '-',
			g_body_parts[items_listed],
			sell ? g_body_parts_sell_prices[items_listed] :
				(g_body_shop_discount ?
					g_body_parts_discounted_prices[items_listed] :
					g_body_parts_buy_prices[items_listed])
		);

		neuro_menu_draw_text(item_string, 0, i + 1);
		neuro_menu_add_item(3, i + 1, 25, i, '1' + i);

		items_listed++;
	}

	items_listed %= 20;

	return sell ? PSS_SELL_MENU : PSS_BUY_MENU;
}

static parts_shop_state_t body_parts_menu(int sell)
{
	neuro_menu_create(6, 1, 17, 28, 6, NULL);
	return body_parts_menu_page(sell, FIRST);
}

static parts_shop_state_t on_buy_parts_menu_button(neuro_button_t *button)
{
	static int bought_something = 0;

	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: { /* items */
		uint16_t body_part = g_first_listed + button->code;

		if (body_part_is_not_sold(body_part))
		{
			/* play track 6 */
		}
		else
		{
			uint16_t *prices = g_body_shop_discount ?
				g_body_parts_discounted_prices :
				g_body_parts_buy_prices;
			uint16_t index = body_part >> 3;
			uint8_t bit = 0x80 >> (body_part & 7);
		
			if (g_4bae.cash < prices[body_part])
			{
				/* play track 6 */
			}
			else
			{
				bit = ~bit;
				g_4bae.sold_body_parts_bitstring[index] &= bit;
				g_4bae.constitution += g_constitution_damage[body_part];
				g_4bae.cash -= prices[body_part];

				bought_something = 1;
				/* play track 11 */
			}
		}

		body_parts_menu_page(0, CURRENT);
		break;
	}

	case 0x0B: /* exit */
		g_4bae.x4c82 = bought_something;
		bought_something = 0;
		return PSS_CLOSE;

	case 0x0A: /* more */
		body_parts_menu_page(0, NEXT);
		break;

	default:
		break;
	}

	return PSS_BUY_MENU;
}

static parts_shop_state_t on_sell_parts_menu_button(neuro_button_t *button)
{
	static int sold_something = 0;

	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: { /* items */
		uint16_t body_part = g_first_listed + button->code;

		if (body_part_is_not_sold(body_part) == 0)
		{
			/* play track 6 */
		}
		else
		{
			uint16_t index = body_part >> 3;
			uint8_t bit = 0x80 >> (body_part & 7);

			g_4bae.sold_body_parts_bitstring[index] |= bit;
			g_4bae.constitution -= g_constitution_damage[body_part];
			g_4bae.cash += g_body_parts_sell_prices[body_part];

			sold_something = 1;
			/* play track 11 */
		}

		body_parts_menu_page(1, CURRENT);
		break;
	}

	case 0x0B: /* exit */
		g_4bae.x4c82 = sold_something;
		sold_something = 0;
		return PSS_CLOSE;

	case 0x0A: /* more */
		body_parts_menu_page(1, NEXT);
		break;

	default:
		break;
	}

	return PSS_SELL_MENU;
}

void parts_shop_menu_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case PSS_SELL_MENU:
		*state = on_sell_parts_menu_button(button);
		break;

	case PSS_BUY_MENU:
		*state = on_buy_parts_menu_button(button);
		break;
	}
}

static parts_shop_state_t parts_shop_wfi(parts_shop_state_t state, sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		switch (state) {
		case PSS_WFI:
			return PSS_CLOSE;
		}
	}

	return state;
}

void handle_parts_shop_input(sfEvent *event)
{
	switch (g_state) {
	case PSS_WFI:
		g_state = parts_shop_wfi(g_state, event);
		break;

	case PSS_SELL_MENU:
	case PSS_BUY_MENU:
		neuro_menu_handle_input(NMID_PARTS_SHOP_MENU, &g_neuro_menu, (int*)&g_state, event);
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

static window_folding_data_t g_parts_shop_anim_data = {
	.total_frames = 12,
	.frame_cap = 28,
	.pixels = g_seg011.data,
};

real_world_state_t update_parts_shop()
{
	static int anim = 0;

	switch (g_state) {
	case PSS_OPEN:
	case PSS_CLOSE:
		if (!anim)
		{
			anim = 1;
			g_parts_shop_anim_data.frame_data = (g_state == PSS_OPEN) ?
				g_open_frame_data : g_close_frame_data;
			g_parts_shop_anim_data.sprite_chain_index = (g_state == PSS_OPEN) ?
				g_4bae.frame_sc_index : g_4bae.frame_sc_index + 1;
			window_animation_setup(WA_TYPE_WINDOW_FOLDING, &g_parts_shop_anim_data);
		}
		else if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			anim = 0;
			if (g_state == PSS_OPEN)
			{
				g_state = body_parts_menu(g_body_shop_op);
			}
			else
			{
				g_state = PSS_OPEN;
				neuro_menu_destroy();
				cpu_set_state(g_cpu, CPU_RUNNING);
				cpu_run(g_cpu);
				return RWS_NORMAL;
			}
		}
		break;
	}

	return RWS_BODY_PARTS_SHOP;
}
