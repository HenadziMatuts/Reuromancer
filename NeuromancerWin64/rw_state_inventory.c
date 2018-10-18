#include "data.h"
#include "globals.h"
#include "resource_manager.h"
#include "scene_real_world.h"
#include "neuro_window_control.h"
#include "drawing_control.h"
#include "window_animation.h"
#include "items.h"
#include <neuro_routines.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int is_jack_on_level();

/*
* "IS_OPEN_INVENTORY"        -> "IS_ITEM_LIST",           "IS_WFI_AND_CLOSE"
*
* "IS_ITEM_LIST"             -> "IS_ITEM_OPTIONS",        "IS_CLOSE_INVENTORY"
*
* "IS_ITEM_OPTIONS"          -> "IS_ITEM_LIST",           "IS_DISCARD_ITEM",   "IS_ERASE_SOFTWARE_LIST",
*                               "IS_GIVE_CREDITS",        "IS_GIVE_ITEM",      "IS_OPERATE_SOFTWARE_LIST",
*                               "IS_WFI_AND_CONTINUE",    "IS_WFI_AND_CLOSE"
*
* "IS_DISCARD_ITEM"          -> "IS_ITEM_LIST"
*
* "IS_ERASE_SOFTWARE_LIST"   -> "IS_ERASE_SOFTWARE",      "IS_ITEM_LIST"
*
* "IS_ERASE_SOFTWARE"        -> "IS_ERASE_SOFTWARE_LIST", "IS_WFI_AND_CONTINUE"
*
* "IS_WFI_AND_CONTINUE"      -> "IS_ITEM_LIST"
*
* "IS_GIVE_CREDITS"          -> "IS_CLOSE_INVENTORY"
*
* "IS_GIVE_ITEM"             -> "IS_CLOSE_INVENTORY"
*
* "IS_OPERATE_SOFTWARE_LIST" -> "IS_WFI_AND_CLOSE",       "IS_WFI_AND_CONTINUE"
*
* "IS_WFI_AND_CLOSE"         -> "IS_CLOSE_INVENTORY"
*
* "IS_CLOSE_INVENTORY"       -> "IS_OPEN_INVENTORY" (EXIT)
*/
typedef enum inventory_state_t {
	IS_OPEN_INVENTORY = -1,
	IS_ITEM_LIST = 0,
	IS_SOFTWARE_LIST, // <- not a real state
	IS_ITEM_OPTIONS,
	IS_DISCARD_ITEM,
	IS_OPERATE_SOFTWARE_LIST,
	IS_ERASE_SOFTWARE_LIST,
	IS_ERASE_SOFTWARE,
	IS_GIVE_CREDITS,
	IS_GIVE_ITEM,
	IS_WFI_AND_CONTINUE,
	IS_WFI_AND_CLOSE,
	IS_CLOSE_INVENTORY,
} inventory_state_t;

static inventory_state_t g_state = IS_OPEN_INVENTORY;

typedef enum inventory_type_t {
	IT_ITEMS = 0,
	IT_SOFTWARE,
} inventory_type_t;

static neuro_button_t *g_inventory_item_button = (neuro_button_t*)(g_stack + 0x7D0);
static uint8_t g_inventory_item_code[4];
static uint8_t g_inventory_item_index[4];

typedef enum inventory_show_list_page_t {
	ISLP_NEXT = 0,
	ISLP_FIRST,
} inventory_show_list_page_t;

static inventory_state_t inventory_item_list(int inv_type, int max_items, int page)
{
	static uint16_t items_listed = 0;
	static uint8_t *inv = NULL;
	uint16_t items_total = count_items(inv_type);

	items_total = (inv_type) ? items_total : items_total + 1;
	inv = inv ? inv :
		(inv_type ? g_3f85.inventory.software : g_3f85.inventory.items);

	/* clear window */
	neuro_window_clear();
	neuro_window_draw_string(inv_type ? "Software" : "Items", inv_type ? 8 : 56, 8);

	if (items_total)
	{
		if (items_total == items_listed || page)
		{
			items_listed = 0;
			inv = inv_type ? g_3f85.inventory.software : g_3f85.inventory.items;
		}

		uint8_t credists = items_listed ? 0 : (inv_type ? 0 : 1);
		uint16_t listed_on_page = 0;
		char string[32] = { 0, };

		neuro_window_flush_buttons();

		max_items = (items_total - items_listed < 4) ? items_total - items_listed : 4;

		while (listed_on_page != max_items)
		{
			neuro_button_t *button = &g_inventory_item_button[listed_on_page];

			if (credists)
			{
				g_inventory_item_button[listed_on_page].code = listed_on_page;
				g_inventory_item_code[listed_on_page] = 0x7F;

				sprintf(string, "1.  Credits %d", g_4bae.cash);
				credists = 0;
			}
			else
			{
				if (*inv == 0xFF)
				{
					inv += 4;
					continue;
				}

				g_inventory_item_button[listed_on_page].code = listed_on_page;
				g_inventory_item_code[listed_on_page] = *inv;
				g_inventory_item_index[listed_on_page] =
					(uint8_t)((inv - (inv_type ? g_3f85.inventory.software : g_3f85.inventory.items)) / 4);

				char c = (*(inv + 2) == 0) ? ' ' : '-';
				char *item_name = get_item_name(*inv, NULL);;

				if (inv_type == 0)
				{
					sprintf(string, "%d. %c%s", listed_on_page + 1, c, item_name);
				}
				else
				{
					sprintf(string, "%d. %c%-11s %2d.0",
						listed_on_page + 1, c, item_name, *(inv + 1));
				}

				inv += 4;
			}
			items_listed++;

			neuro_window_draw_string(string, 8, ++listed_on_page * 8 + 8);

			button->left = 64;
			button->top = (listed_on_page * 8) + 136;
			button->right = 223;
			button->bottom = (listed_on_page * 8) + 143;
			button->label = listed_on_page + '0';

			neuro_window_add_button(button);
		}

		neuro_window_add_button(&g_inv_buttons.exit);
		neuro_window_draw_string("exit", 40, 48);

		if (items_total > 4)
		{
			neuro_window_add_button(&g_inv_buttons.more);
			neuro_window_draw_string("more", 80, 48);
		}
	}
	else
	{
		neuro_window_draw_string("None", 50, 40);
		return (inv_type == IT_ITEMS) ? IS_WFI_AND_CLOSE : IS_WFI_AND_CONTINUE;
	}

	return (inv_type == IT_ITEMS) ? IS_ITEM_LIST : IS_SOFTWARE_LIST;
}

static void inventory_item_options()
{
	char string[32] = { 0 };
	char *item_name = NULL;
	int i = 0;

	neuro_window_clear();
	neuro_window_flush_buttons();

	item_name = get_item_name(g_c946, (g_c946 == 0x7F) ? string : NULL);
	neuro_window_draw_string(item_name, 8, 8);

	neuro_window_draw_string("X. Exit", 8, 16);
	neuro_window_add_button(&g_inv_buttons.item_page_exit);

	for (i = 0; i < 4; i++)
	{
		g_inventory_item_button[i].left = 64;
		g_inventory_item_button[i].top = i * 8 + 152;
		g_inventory_item_button[i].right = 199;
		g_inventory_item_button[i].bottom = g_inventory_item_button[i].top + 7;
	}

	for (i = 0; i < 4; i++)
	{
		if (g_a8e0.a8e0[i] != 0xFFFF)
		{
			break;
		}
	}

	if (i != 4 || g_4bae.level_n == 55)
	{
		neuro_window_draw_string("G. Give Item", 8, 40);
		g_inventory_item_button[2].code =
			g_inventory_item_button[2].label = 'g';
		neuro_window_add_button(&g_inventory_item_button[2]);
		i = 48;
	}
	else if (i == 4 && g_4bae.level_n != 55)
	{
		i = 40;
	}

	if (g_c946 == 0x53 || (g_c946 >= 0x1D && g_c946 <= 0x34))
	{
		g_4bae.x4bf3 = g_c946;
		neuro_window_draw_string("E. Erase Software", 8, i);

		i = (i == 40) ? 2 : 3;
		g_inventory_item_button[i].code =
			g_inventory_item_button[i].label = 'e';
		neuro_window_add_button(&g_inventory_item_button[i]);
	}

	neuro_window_draw_string("O. Operate Item", 8, 24);
	neuro_window_draw_string("D. Discard Item", 8, 32);

	g_inventory_item_button[0].code =
		g_inventory_item_button[0].label = 'o';
	g_inventory_item_button[1].code =
		g_inventory_item_button[1].label = 'd';

	neuro_window_add_button(&g_inventory_item_button[0]);
	neuro_window_add_button(&g_inventory_item_button[1]);
}

typedef enum inventory_discard_t {
	ID_ITEM = 0,
	ID_SOFTWARE,
	ID_GIVE,
} inventory_discard_t;

static void inventory_discard(int discard)
{
	char string[32] = { 0, };
	char *item_name = 0;

	neuro_window_clear();
	neuro_window_flush_buttons();

	if (discard == ID_GIVE)
	{
		neuro_window_draw_string("GIVE", 80, 8);
	}
	else
	{
		neuro_window_draw_string((discard == ID_SOFTWARE) ? "ERASE" : "Discard", 72, 8);
	}

	item_name = get_item_name(g_c946, NULL);

	if (g_c946 > 0x1C)
	{
		sprintf(string, "%s", item_name);
	}
	else
	{
		sprintf(string, "%s %d.0", item_name, g_3f85.inventory.software[g_a86a * 4 + 1]);
	}

	neuro_window_draw_string(string, 16, 24);
	neuro_window_draw_string("Are you sure (Y/N)", 8, 40);

	neuro_window_add_button(&g_inv_disc_buttons.yes);
	neuro_window_add_button(&g_inv_disc_buttons.no);
}

static inventory_state_t inventory_give_item()
{
	neuro_window_clear();
	neuro_window_flush_buttons();

	if (g_c946 == 0x7F)
	{
		char credits[32] = { 0, };
		neuro_window_draw_string("Give how much?", 8, 16);

		sprintf(credits, "Credits %d", g_4bae.cash);
		neuro_window_draw_string(credits, 8, 8);
		neuro_window_draw_string("<", 8, 24);
		return IS_GIVE_CREDITS;
	}
	else
	{
		inventory_discard(ID_GIVE);
		return IS_GIVE_ITEM;
	}
}

static inventory_state_t inventory_operate_item(uint8_t *item)
{
	uint8_t item_code = item[0];
	uint8_t item_op = g_inventory_item_operations[item_code];

	neuro_window_clear();
	neuro_window_flush_buttons();

	/* credits or no operaton */
	if (g_c946 == 0x7F || item_op == 0xFF)
	{
		neuro_window_draw_string("Nothing happens.", 8, 16);
		return IS_WFI_AND_CONTINUE;
	}

	/* "skill" check */
	if ((rand() & 0xFF) < item[2])
	{
		if ((item_op & 0xC0) != 0)
		{
			neuro_window_draw_string("Hardware failure.", 8, 16);
			return IS_WFI_AND_CONTINUE;

		}
		else
		{
			neuro_window_draw_string("Program crashed.", 8, 16);
			return IS_WFI_AND_CLOSE;
		}
	}

	g_4bae.x4ccb = item_op & 0x0F;
	g_4bae.x4ccc = item_op & 0x30;

	inventory_state_t state;

	/* hardware */
	if (item_op & 0x80)
	{
		switch (g_4bae.x4ccb) {
		case 0: /* deck */
			g_4bae.x4c74 = item_op << 1;
			state = inventory_item_list(IT_SOFTWARE, 4, ISLP_FIRST);
			return (state == IS_SOFTWARE_LIST) ? IS_OPERATE_SOFTWARE_LIST : IS_WFI_AND_CLOSE;

		case 1: /* skill chip */
			g_3f85.skills[item_code - 0x43] = 0;
			item[0] = 0xFF;
			neuro_window_draw_string("Skill chip implanted", 8, 16);
			return IS_WFI_AND_CONTINUE;

		case 2: /* gas mask */
			g_4bae.gas_mask_is_on ^= 1;
			neuro_window_draw_string(g_4bae.gas_mask_is_on ?
				"Gas mask is on." : "Gas mask is off", 8, 16);
			return IS_WFI_AND_CONTINUE;

		case 3: /* some specific software */
		case 4:
		case 5:
		default:
			neuro_window_draw_string("Not implemented Yet.", 8, 16);
			return IS_WFI_AND_CLOSE;
		}
	}

	if (g_4bae.x4ccc != 0x10)
	{
		if (g_4bae.x4ccc == 0x20)
		{   /* cyberspace only items */
			if (g_a61a == 1)
			{
				uint8_t cb = 0;
				uint8_t x = g_4bae.x4ccb - 1;

				while (!(x-- & 0x80))
				{
					cb++;
				}

				switch (cb) {
				default:
					neuro_window_draw_string("Some callback...", 8, 16);
					return IS_WFI_AND_CLOSE;
				}
			}
			else
			{
				neuro_window_draw_string("Cyberspace only.", 8, 16);
				return IS_WFI_AND_CLOSE;
			}
		}
		else if ((g_4bae.x4ccb == 0) || g_4bae.x4c74 & 0x80)
		{   /* jackable items */
			if (is_jack_on_level())
			{
				if (g_4bae.x4ccd != 0)
				{
					return inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
				}
				else if (g_4bae.x4ccb == 0)
				{
					g_a61a = 2;
					g_4bae.x4ccd++;
					/* sub_189AE */
					neuro_window_draw_string("call sub_189AE", 8, 16);
					g_4bae.x4ccd--;
					return IS_WFI_AND_CLOSE;
				}
				else
				{
					/* sub_19E32 */
					neuro_window_draw_string("call sub_19E32", 8, 16);
					return IS_WFI_AND_CLOSE;
				}
			}
			else
			{
				neuro_window_draw_string("No jack here.", 8, 16);
				return IS_WFI_AND_CLOSE;
			}
		}
		else
		{
			neuro_window_draw_string("Nothing happens.", 8, 16);
			return IS_WFI_AND_CLOSE;
		}
	}
	else
	{   /* database only items */
		if (item_code == 0x18)
		{
			neuro_window_draw_string("Nothing happens.", 8, 16);
			return IS_WFI_AND_CLOSE;
		}
		else if (g_4bae.x4cc5 == 0)
		{
			neuro_window_draw_string("Database only.", 8, 16);
			return IS_WFI_AND_CLOSE;
		}
		else
		{
			uint8_t cb = 0;
			uint8_t x = g_4bae.x4ccb - 1;

			while (!(x-- & 0x80))
			{
				cb++;
			}

			switch (cb) {
			default:
				neuro_window_draw_string("Some callback...", 8, 16);
				return IS_WFI_AND_CLOSE;
			}
		}
	}

	return inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
}

static inventory_state_t on_inventory_operate_software_button(neuro_button_t *button)
{
	inventory_state_t state;

	switch (button->code) {
	case 0x00: /* softwares */
	case 0x01:
	case 0x02:
	case 0x03:
		g_a86a = g_inventory_item_index[button->code];
		g_c946 = g_inventory_item_code[button->code];
		return inventory_operate_item(&g_3f85.inventory.software[g_a86a * 4]);

	case 0x0A: /* exit */
		return IS_CLOSE_INVENTORY;

	case 0x0B: /* more */
		state = inventory_item_list(IT_SOFTWARE, 4, ISLP_NEXT);
		return (state == IS_SOFTWARE_LIST) ? IS_OPERATE_SOFTWARE_LIST : IS_CLOSE_INVENTORY;
	}

	return IS_OPERATE_SOFTWARE_LIST;
}

static inventory_state_t on_inventory_erase_software_list_button(neuro_button_t *button)
{
	inventory_state_t state;

	switch (button->code) {
	case 0x00: /* softwares */
	case 0x01:
	case 0x02:
	case 0x03:
		g_a86a = g_inventory_item_index[button->code];
		g_c946 = g_inventory_item_code[button->code];
		inventory_discard(ID_SOFTWARE);
		return IS_ERASE_SOFTWARE;

	case 0x0A: /* exit */
		return inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);

	case 0x0B: /* more */
		state = inventory_item_list(IT_SOFTWARE, 4, ISLP_NEXT);
		return (state == IS_SOFTWARE_LIST) ? IS_ERASE_SOFTWARE_LIST : state;
	}

	return IS_ERASE_SOFTWARE_LIST;
}

static inventory_state_t on_inventory_give_item_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0x01: /* yes */
		g_3f85.inventory.items[g_a86a * 4] = 0xFF;
		g_4bae.active_item = g_c946;
		g_4bae.x4bbf = 1;
	case 0x00: /* no */
		return IS_CLOSE_INVENTORY;
	}

	return IS_GIVE_ITEM;
}

static inventory_state_t on_inventory_discard_button(neuro_button_t *button, int discard)
{
	inventory_state_t state;

	switch (button->code) {
	case 0x01: /* yes */
		if (discard == ID_ITEM)
		{
			g_3f85.inventory.items[g_a86a * 4] = 0xFF;
		}
		else
		{
			g_3f85.inventory.software[g_a86a * 4] = 0xFF;
		}
	case 0x00: /* no */
		if (discard == ID_ITEM)
		{
			return inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
		}
		else
		{
			state = inventory_item_list(IT_SOFTWARE, 4, ISLP_FIRST);
			return (state == IS_SOFTWARE_LIST) ? IS_ERASE_SOFTWARE_LIST : state;
		}
	}

	return IS_DISCARD_ITEM;
}

static inventory_state_t on_inventory_item_options_button(neuro_button_t *button)
{
	inventory_state_t state;

	switch (button->code) {
	case 0x64: /* discard */
		if (g_c946 == 0x53 || g_c946 == 0x7F)
		{
			return inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
		}
		inventory_discard(ID_ITEM);
		return IS_DISCARD_ITEM;

	case 0x65: /* erase */
		state = inventory_item_list(IT_SOFTWARE, 4, ISLP_FIRST);
		return (state == IS_SOFTWARE_LIST) ? IS_ERASE_SOFTWARE_LIST : state;

	case 0x67: /* give */
		return inventory_give_item();

	case 0x6F: /* operate */
		return inventory_operate_item(&g_3f85.inventory.items[g_a86a * 4]);

	case 0x0A: /* exit */
		return inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
	}

	return IS_ITEM_OPTIONS;
}

static inventory_state_t on_inventory_item_list_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0x00: /*items */
	case 0x01:
	case 0x02:
	case 0x03:
		g_a86a = g_inventory_item_index[button->code];
		g_c946 = g_inventory_item_code[button->code];
		inventory_item_options();
		return IS_ITEM_OPTIONS;

	case 0x0A: /* exit */
		return IS_CLOSE_INVENTORY;

	case 0x0B: /* more */
		return inventory_item_list(IT_ITEMS, 4, ISLP_NEXT);
	}

	return IS_ITEM_LIST;
}

static inventory_state_t on_inventory_give_credits_text_input(sfTextEvent *event, int ret)
{
	static char input[9] = { 0 };
	char printable[10] = { 0 };

	if (ret)
	{
		uint32_t val = strlen(input) ? (uint32_t)atoi(input) : 0;
		memset(input, 0, 9);

		if (val <= g_4bae.cash)
		{
			g_4bae.cash -= val;
			g_4bae.x4bbf = 1;
			g_4bae.active_item = g_c946;
			g_4bae.cash_withdrawal = val;

			return IS_CLOSE_INVENTORY;
		}

		neuro_window_draw_string("<        ", 8, 24);

		return IS_GIVE_CREDITS;
	}

	sfHandleTextInput(event->unicode, input, 8, 1, 0);
	sprintf(printable, "%s<", input);
	memset(printable + strlen(printable), 0x20, 8 - strlen(printable));
	neuro_window_draw_string(printable, 8, 24);

	return IS_GIVE_CREDITS;
}

void rw_inventory_handle_text_enter(int *state, sfTextEvent *event)
{
	switch (*state) {
	case IS_GIVE_CREDITS:
		*state = on_inventory_give_credits_text_input(event, 0);
		break;
	}
}

static inventory_state_t on_inventory_give_credits_kboard(sfKeyEvent *event)
{
	if (event->type == sfEvtKeyReleased &&
		event->code == sfKeyReturn)
	{
		return on_inventory_give_credits_text_input(NULL, 1);
	}

	return IS_GIVE_CREDITS;
}

void rw_inventory_handle_kboard(int *state, sfKeyEvent *event)
{
	switch (*state) {
	case IS_GIVE_CREDITS:
		*state = on_inventory_give_credits_kboard(event);
		break;
	}
}

void rw_inventory_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case IS_ITEM_LIST:
		*state = on_inventory_item_list_button(button);
		break;

	case IS_ITEM_OPTIONS:
		*state = on_inventory_item_options_button(button);
		break;

	case IS_DISCARD_ITEM:
		*state = on_inventory_discard_button(button, IT_ITEMS);
		break;

	case IS_ERASE_SOFTWARE_LIST:
		*state = on_inventory_erase_software_list_button(button);
		break;

	case IS_ERASE_SOFTWARE:
		*state = on_inventory_discard_button(button, IT_SOFTWARE);
		break;

	case IS_GIVE_ITEM:
		*state = on_inventory_give_item_button(button);
		break;

	case IS_OPERATE_SOFTWARE_LIST:
		*state = on_inventory_operate_software_button(button);
		break;
	}
}

static inventory_state_t handle_inventory_wait_for_input(inventory_state_t state, sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		switch (state) {
		case IS_WFI_AND_CLOSE:
			return IS_CLOSE_INVENTORY;

		case IS_WFI_AND_CONTINUE:
			return inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);

		default:
			return state;
		}
	}

	return state;
}

void handle_inventory_input(sfEvent *event)
{
	switch (g_state) {
	case IS_OPEN_INVENTORY:
	case IS_CLOSE_INVENTORY:
		break;

	case IS_WFI_AND_CLOSE:
	case IS_WFI_AND_CONTINUE:
		g_state = handle_inventory_wait_for_input(g_state, event);
		break;

	default:
		neuro_window_handle_input((int*)&g_state, event);
		break;
	}
}

static window_folding_frame_data_t g_close_frame_data[12] = {
	{ 176, 64, 56, 128 }, { 176, 62,  56, 129 }, { 176, 56,  56, 132 },
	{ 176, 48, 56, 136 }, { 176, 36,  56, 142 }, { 176, 20,  56, 150 },
	{ 172,  2, 58, 159 }, { 164,  2,  62, 159 }, { 148,  2,  70, 159 },
	{ 114,  2, 86, 159 }, { 50,   2, 118, 159 }, {   3,  2, 143, 159 },
}, g_open_frame_data[12] = {
	{   3,  2, 143, 159 }, {   7,  2, 141, 159 }, {  15,  2, 136, 159 },
	{  23,  2, 132, 159 }, {  45,  2, 121, 159 }, {  87,  2, 101, 159 },
	{ 176,  3,  56, 159 }, { 176,  5,  56, 157 }, { 176, 11,  56, 154 },
	{ 176, 21,  56, 149 }, { 176, 33,  56, 143 }, { 176, 64,  56, 128 }
};

static window_folding_data_t g_inv_anim_data = {
	.total_frames = 12,
	.frame_cap = 30,
	.pixels = g_seg012.data,
};

real_world_state_t update_inventory()
{
	static int anim = 0;

	switch (g_state) {
	case IS_OPEN_INVENTORY:
	case IS_CLOSE_INVENTORY:
		if (!anim)
		{
			anim = 1;
			g_inv_anim_data.frame_data = (g_state == IS_OPEN_INVENTORY) ?
				g_open_frame_data : g_close_frame_data;
			g_inv_anim_data.sprite_chain_index = (g_state == IS_OPEN_INVENTORY) ?
				g_4bae.frame_sc_index : g_4bae.frame_sc_index + 1;
			window_animation_setup(WA_TYPE_WINDOW_FOLDING, &g_inv_anim_data);
		}
		else if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			anim = 0;
			if (g_state == IS_OPEN_INVENTORY)
			{
				neuro_window_setup(NWM_INVENTORY);
				g_state = inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
			}
			else
			{
				g_state = IS_OPEN_INVENTORY;
				drawing_control_remove_sprite_from_chain(++g_4bae.frame_sc_index);
				restore_window();
				return RWS_NORMAL;
			}
		}
		break;
	}

	return RWS_INVENTORY;
}
