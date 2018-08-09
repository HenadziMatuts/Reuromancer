#include "globals.h"
#include "scene_control.h"
#include "character_control.h"
#include "bg_animation_control.h"
#include "drawing_control.h"
#include "resource_manager.h"
#include "data.h"
#include <string.h>
#include <assert.h>
#include <stdarg.h>

typedef enum level_intro_state_t {
	LIS_NEXT_LINE = 0,
	LIS_SCROLLING,
	LIS_WAITING_FOR_INPUT,
} level_intro_state_t;

typedef enum inventory_state_t {
	IS_ITEM_LIST = 0,
	IS_ITEM_OPTIONS,
	IS_DISCARD_ITEM,
	IS_ERASE_SOFTWARE_LIST,
	IS_ERASE_SOFTWARE,
} inventory_state_t;

typedef enum level_state_t {
	LS_INTRO = 0,
	LS_WAIT_FOR_INPUT,
	LS_INVENTORY,
	LS_NORMAL,
} level_state_t;

static char *g_bih_string_ptr = NULL;
static level_state_t g_state = LS_NORMAL;
static inventory_state_t g_inventory_state = 0;

/***************************************/
static int neuro_window_setup(uint16_t opcode, ...);

/* sub_14DBA */
static void neuro_window_draw_string(char *text, ...)
{
	switch (g_neuro_window.mode) {
	case 0:
		/* incorrect! */
		g_state = LS_INTRO;
		break;

	case 3: {
		va_list args;
		va_start(args, text);
		uint16_t left = va_arg(args, uint16_t);
		uint16_t top = va_arg(args, uint16_t);
		va_end(args);

		imh_hdr_t *imh = (imh_hdr_t*)g_seg012;
		build_string(text, imh->width * 2, imh->height, left, top, g_seg012 + sizeof(imh_hdr_t));

		break;
	}

	case 8: {
		imh_hdr_t *imh = (imh_hdr_t*)g_seg011;
		build_string(text, imh->width * 2, imh->height, 8, 8, g_seg011 + sizeof(imh_hdr_t));
		drawing_control_add_sprite_to_chain(g_4bae.x4ccf--, 0, g_neuro_window.top, g_seg011, 1);
	}

	default:
		break;
	}
}

static void sub_1342E(uint16_t opcode)
{
	char text[256] = { 0, };
	char *p = text;
	int lines = 0;

	while (!extract_line(&g_bih_string_ptr, p, 38))
	{
		size_t l = strlen(p);

		if (l != 38)
		{
			memset(p + l, 0x20, 38 - l);
		}

		p += 38;
		*p++ = '\n';
		lines++;
	}
	*--p = 0;

	neuro_window_setup(opcode, lines);
	neuro_window_draw_string(text);
}

static void sub_10A5B(uint16_t a, uint16_t b, uint16_t c, uint16_t d)
{
	uint16_t x = 0;

	g_bih_string_ptr = g_a8e0.bih + g_bih_wrapper.bih->text_offset;

	for (int i = 0; i < a; i++)
	{
		while (*g_bih_string_ptr++);
	}

	if (b != 0)
	{
		return;
	}

	uint16_t temp_1 = g_4bae.roompos_spawn_x;
	uint16_t temp_2 = g_4bae.roompos_spawn_y;

	g_4bae.roompos_spawn_x = c * 2;
	g_4bae.roompos_spawn_y = d;

	/* builds dialog bubble with text */
	sub_1342E(8);

	g_4bae.roompos_spawn_x = temp_1;
	g_4bae.roompos_spawn_y = temp_2;

	return;
}

/* sub_10735 */
static void neuro_vm()
{
	for (int i = 3; i >= 0; i--)
	{
		uint16_t op_index = g_a8e0.a8e0[i];
		if (op_index == 0xffff) {
			continue;
		}

		uint8_t *next_opcode_addr = g_3f85_wrapper.vm_next_op_addr[op_index];
		uint8_t opcode = *next_opcode_addr;

		switch (opcode) {
		/* dialog reply, 1 arg (line number) */
		case 1:
			sub_10A5B(*(next_opcode_addr + 1), 0,
				g_3f85.vm_state[op_index].var_1,
				g_3f85.vm_state[op_index].var_2);

			g_state = LS_WAIT_FOR_INPUT;

			g_3f85_wrapper.vm_next_op_addr[op_index] += 2;
			g_4bae.x4bbe = 0xFF;
			break;

		case 5:
		case 6:
		case 7: 
		case 8: {
			jumps_t jmp = g_4b9d[opcode - 5];
			uint8_t arg_1 = *(next_opcode_addr + 1);
			uint8_t arg_3 = *(next_opcode_addr + 3);

			if (((jmp == JE) && (g_4bae.x4bae[arg_1] == arg_3)) ||
				((jmp == JNE) && (g_4bae.x4bae[arg_1] != arg_3)) ||
				((jmp == JL) && (g_4bae.x4bae[arg_1] < arg_3)) ||
				((jmp == JGE) && (g_4bae.x4bae[arg_1] >= arg_3)))
			{
				g_3f85_wrapper.vm_next_op_addr[op_index] += 6;
			}
			else
			{
				int16_t arg_4 = *(int16_t*)(next_opcode_addr + 4);
				g_3f85_wrapper.vm_next_op_addr[op_index] += arg_4;
				g_3f85_wrapper.vm_next_op_addr[op_index] += 4;
			}

			break;
		}

		case 19: {
			uint8_t *p = g_3f85.x407b + *(next_opcode_addr + 1);
			p[0] = *(next_opcode_addr + 2);
			p[1] = *(next_opcode_addr + 3);

			g_3f85_wrapper.vm_next_op_addr[op_index] += 4;
			break;
		}

		default:
			break;
		}
	}
}

/* sub_106BD */
static int setup_intro()
{
	uint16_t x = (0x80 >> (g_level_n & 7)) | 0x80;
	uint16_t y = (g_level_n >> 3) | (g_level_n & 0x80);

	g_bih_string_ptr = g_a8e0.bih + g_bih_wrapper.bih->text_offset;

	if ((g_004e[y] & x) == 0) {
		/* setup long intro */
		g_004e[y] |= x;
	}
	else {
		/* setup short intro */
		while (*g_bih_string_ptr++);
	}

	neuro_window_draw_string(g_bih_string_ptr);

	return 0;
}

typedef enum sub_105f6_opcodes_t {
	SUB_105F6_OP_PLAY_LEVEL_INTRO = 5,
	SUB_105F6_OP_NEURO_VM_STEP,
} sub_105f6_opcodes_t;

static uint64_t sub_105F6(uint16_t opcode, ...)
{
	switch (opcode) {
	case 0: {
		va_list args;
		va_start(args, opcode);
		uint16_t level_n = va_arg(args, uint16_t);
		va_end(args);

		// mov [A642], 0x407B + 6*level_n
		g_bih_wrapper.bih = (bih_hdr_t*)g_a8e0.bih;
		g_bih_wrapper.ctrl_struct_addr = (uint8_t*)&g_4bae;
		// g_bih_wrapper.cb_addr = &cb;
		break;
	}

	case 1:
	case 3: {
		uint16_t offt = g_bih_wrapper.bih->own_cb_offsets[opcode];
		// call (a8e8 + offt)
		break;
	}

	case 4: {
		va_list args;
		va_start(args, opcode);
		uint16_t x = va_arg(args, uint16_t);
		uint16_t y = va_arg(args, uint16_t);
		va_end(args);

		uint8_t offt = *((uint8_t*)&g_bih_wrapper.bih->text_offset + (x * 2));
		uint8_t *p = g_a8e0.bih + offt;
		p = g_a8e0.bih + *(p + (y * 2));
		return (uint64_t)p;
	}

	case SUB_105F6_OP_PLAY_LEVEL_INTRO: {
		setup_intro();
		break;
	}

	case SUB_105F6_OP_NEURO_VM_STEP:
		neuro_vm();
		break;

	default:
		break;
	}

	return 0;
}

/* sub_1152B */
static int has_pax()
{
	uint8_t *p = g_a8e0.bih + sizeof(bih_hdr_t); // 0xA910

	while ((*p != 0) && (*p != 1))
	{
		p++;
	}

	return *p;
}

/* sub_14B1B */
static int neuro_window_add_button(neuro_button_t *button)
{
	switch (g_neuro_window.mode)
	{
	case 0:
	case 3:
		g_neuro_window_wrapper.window_item[g_neuro_window.total_items++] = (uint8_t*)button;
		break;

	default:
		break;
	}

	return 0;
}

/* sub_156D4 */
static int setup_ui_buttons()
{
	assert((g_neuro_window.mode == 0) || (g_neuro_window.mode > 2 && g_neuro_window.mode <= 4));
	g_neuro_window.total_items = 0;

	neuro_window_add_button(&g_ui_buttons.inventory);
	neuro_window_add_button(&g_ui_buttons.dialog);

	if (has_pax())
	{
		neuro_window_add_button(&g_ui_buttons.pax);
	}

	neuro_window_add_button(&g_ui_buttons.skills);
	neuro_window_add_button(&g_ui_buttons.chip);
	neuro_window_add_button(&g_ui_buttons.disk);
	neuro_window_add_button(&g_ui_buttons.date);
	neuro_window_add_button(&g_ui_buttons.time);
	neuro_window_add_button(&g_ui_buttons.cash);
	neuro_window_add_button(&g_ui_buttons.con);

	return 0;
}

static void store_window()
{
	memmove(&g_a59e[2], &g_a59e[1], sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[2], &g_a59e_wrapper[1], sizeof(neuro_window_wrapper_t));

	memmove(&g_a59e[1], &g_a59e[0], sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[1], &g_a59e_wrapper[0], sizeof(neuro_window_wrapper_t));

	memmove(&g_a59e[0], &g_neuro_window, sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[0], &g_neuro_window_wrapper, sizeof(neuro_window_wrapper_t));
}

static void restore_window()
{
	memmove(&g_neuro_window, &g_a59e[0], sizeof(neuro_window_t));
	memmove(&g_neuro_window_wrapper, &g_a59e_wrapper[0], sizeof(neuro_window_wrapper_t));

	memmove(&g_a59e[0], &g_a59e[1], sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[0], &g_a59e_wrapper[1], sizeof(neuro_window_wrapper_t));

	memmove(&g_a59e[1], &g_a59e[2], sizeof(neuro_window_t));
	memmove(&g_a59e_wrapper[1], &g_a59e_wrapper[2], sizeof(neuro_window_wrapper_t));
}

/* Setup "Window" - sub_147EE */
static int neuro_window_setup(uint16_t mode, ...)
{
	store_window();

	g_neuro_window.mode = mode;
	g_neuro_window.total_items = 0;

	switch (mode) {
	/* UI "window" */
	case 0:
		setup_ui_buttons();

		g_neuro_window.left = 0;
		g_neuro_window.top = 0;
		g_neuro_window.right = 319;
		g_neuro_window.bottom = 199;
		g_neuro_window.c944 = 160;

		break;

	case 3:
		/* inventory "window" */
		g_neuro_window.left = 56;
		g_neuro_window.top = 128;
		g_neuro_window.right = 231;
		g_neuro_window.bottom = 191;
		g_neuro_window.c944 = 88;

		build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
			g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg012);
		drawing_control_add_sprite_to_chain(g_4bae.x4ccf--,
			g_neuro_window.left, g_neuro_window.top, g_seg012, 1);

		break;

	case 8: {
		/* Dialog "window" */
		va_list args;
		va_start(args, mode);
		uint16_t lines = va_arg(args, uint16_t);
		va_end(args);

		g_neuro_window.left = 0;
		g_neuro_window.top = 4;
		g_neuro_window.right = 319;
		g_neuro_window.bottom = (lines * 8) + 19;
		g_neuro_window.c928 = lines;
		g_neuro_window.c944 = 160;

		if (g_4bae.ui_type == 0)
		{
			if (g_neuro_window.mode == 1)
			{
				if (g_4bae.roompos_spawn_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_neuro_window.bottom + 1, g_dialog_bubbles + 0x1A2, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_neuro_window.bottom + 1, g_dialog_bubbles + 0xDC, 0);
				}
			}
			else
			{
				if (g_4bae.roompos_spawn_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_neuro_window.bottom + 1, g_dialog_bubbles + 0x6E, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_neuro_window.bottom + 1, g_dialog_bubbles, 0);
				}
			}
		}

		build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
			g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg011);

		break;
	}

	default:
		break;
	}

	return 0;
}
/***************************************/

static void ui_panel_update()
{
	char panel_string[9];
	uint8_t *bg_pix = g_background + sizeof(imh_hdr_t);
	static int update_cap_ms = 1000;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed > update_cap_ms)
	{
		elapsed = passed;

		if (++g_4bae.time_m == 60)
		{
			g_4bae.time_m = 0;
			if (++g_4bae.time_h == 24)
			{
				g_4bae.time_h = 0;
				g_4bae.date_day++;
				g_4bae.x4c10 ^= 1;
				g_4bae.x4c38 = 0;
				g_4bae.x4c5c = 0xFF;
			}
		}
	}

	if (g_4bae.ui_type == 0)
	{
		if (g_4bae.level_n == 49 || g_4bae.level_n == 21)
		{
			g_ui_panel_mode = 1;
		}
	}

	switch (g_ui_panel_mode) {
	case UI_PM_CASH:
		sprintf(panel_string, "$%7d", g_4bae.cash);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;

	case UI_PM_CON:
		sprintf(panel_string, "%8d", g_4bae.con);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;

	case UI_PM_TIME:
		sprintf(panel_string, "   %02d:%02d", g_4bae.time_h, g_4bae.time_m);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;

	case UI_PM_DATE: {
		uint16_t day = 16;
		uint16_t month = 11;
		uint16_t year = 58;

		if (g_4bae.date_day > 14)
		{
			if (day + g_4bae.date_day > 61)
			{
				year = 59;
				month = 1;
				day = day + g_4bae.date_day - 61;
			}
			else
			{
				month = 12;
				day = day + g_4bae.date_day - 30;
			}
		}
		else
		{
			day += g_4bae.date_day;
		}

		sprintf(panel_string, "%02d/%02d/%02d", month, day, year);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;
	}

	default:
		break;
	}
}

static void init()
{
	char resource[32] = { 0, };
	memset(g_vga, 0, 320 * 200 * 4);
	g_state = LS_NORMAL;

	assert(resource_manager_load_resource("NEURO.IMH", g_background));
	drawing_control_add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_background, 1);

	neuro_window_setup(0);

	if (g_level_n >= 0 && g_4bae.x4bcc == 0)
	{
		/* sub_105F6(3) */
	}

	g_4bae.x4bcc = 0;

	g_4bae.x4cc3 = 0;
	g_4bae.x4bbe = 0xFF;
	g_4bae.x4bbf = 0xFF;
	g_4bae.x4bc0 = 0xFFFF;
	g_4bae.x4bc2 = 0xFFFF;
	g_4bae.x4bc4 = 0xFFFF;
	// mov [152C], 0
	g_4bae.x4bf4 = 0xFF;
	// mov [1E34], 0
	// mov [1E44], 0

	memset(g_a8e0.a8e0, 0xFFFF, 8);
	g_level_n = g_4bae.level_n;

	sprintf(resource, "R%d.BIH", g_level_n + 1);
	assert(resource_manager_load_resource(resource, g_a8e0.bih));

	sprintf(resource, "R%d.PIC", g_level_n + 1);
	assert(resource_manager_load_resource(resource, g_level_bg + sizeof(imh_hdr_t)));
	drawing_control_add_sprite_to_chain(SCI_LEVEL_BG, 8, 8, g_level_bg, 1);

	sprintf(resource, "R%d.ANH", g_level_n + 1);
	resource_manager_load_resource(resource, g_roompos + 0x488) ?
		bg_animation_control_init_tables(g_roompos + 0x488) :
		bg_animation_control_init_tables(NULL);

	sub_105F6(0, g_level_n);

	uint16_t u = 0;
	neuro_vm_state_t *p = g_3f85.vm_state;

	/* Sets VM state for the level */
	while (1)
	{
		if (p->level == 0xFF)
		{
			if (g_4bae.level_n != 9 && g_4bae.level_n != 27)
			{
				character_control_add_sprite_to_chain(156, 110, CD_DOWN);
			}
			break;
		}

		if (p->level != g_4bae.level_n)
		{
			goto next;
		}

		if (p->mark == 0xFF)
		{
			p->mark = 0;
			g_3f85_wrapper.vm_next_op_addr[u] = (uint8_t*)sub_105F6(4, p->flag & 3, 0);
		}

		uint16_t offt = p->flag & 3;
		g_a8e0.a8e0[offt] = u;

	next:
		p++;
		u++;
	}

	sub_105F6(1);
	setup_ui_buttons();
	sub_105F6(SUB_105F6_OP_PLAY_LEVEL_INTRO);

	ui_panel_update();
	return;
}

typedef enum inventory_type_t {
	IT_ITEMS = 0,
	IT_SOFTWARE,
} inventory_type_t;

static neuro_button_t g_inventory_item_button[4];
static uint8_t g_inventory_item_code[4];
static uint8_t g_inventory_item_index[4];

static uint16_t inventory_count_items(int inv_type)
{
	uint8_t *inv = (inv_type) ? g_3f85.inventory.software : g_3f85.inventory.items;
	uint16_t items = 0;

	for (int i = 0; i < 32; i++, inv += 4)
	{
		if (*inv != 0xFF)
		{
			items++;
		}
	}

	return items;
}

static char* inventoty_get_item_name(uint16_t item_code, char *credits)
{
	if (item_code == 0x7F)
	{
		sprintf(credits, "Credits %d", g_4bae.cash);
		return credits;
	}

	return g_inventory_item_names[item_code];
}

typedef enum inventory_show_list_page_t {
	ISLP_NEXT = 0,
	ISLP_FIRST,
} inventory_show_list_page_t;

static void inventory_item_list(int inv_type, int max_items, int page)
{
	static uint16_t items_listed = 0;
	static uint8_t *inv = NULL;
	uint16_t items_total = inventory_count_items(inv_type);

	items_total = (inv_type) ? items_total : items_total + 1;
	inv = inv ? inv :
		(inv_type ? g_3f85.inventory.software : g_3f85.inventory.items);

	/* clear window */
	build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
		g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg012);

	neuro_window_draw_string(inv_type ? "Software" : "Items", inv_type ? 8 : 56, 8);

	if (items_total)
	{
		if (items_total == items_listed || page)
		{
			items_listed = 0;
			inv = inv_type ? g_3f85.inventory.software : g_3f85.inventory.items;
		}

		uint8_t credists = items_listed ? 0 : (inv_type ? 0 : 1);
		uint8_t inv_index = 0;
		uint16_t listed_on_page = 0;
		char string[32] = { 0, };

		/* sub_14AF2 */
		assert((g_neuro_window.mode == 0) || (g_neuro_window.mode > 2 && g_neuro_window.mode <= 4));
		g_neuro_window.total_items = 0;

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
					inv_index++;
					continue;
				}

				g_inventory_item_button[listed_on_page].code = listed_on_page;
				g_inventory_item_code[listed_on_page] = *inv;
				g_inventory_item_index[listed_on_page] = inv_index;

				char c = (*(inv + 2) == 0) ? ' ' : '-';
				char *item_name = inventoty_get_item_name(*inv, NULL);;

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
				inv_index++;
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
		g_state = LS_WAIT_FOR_INPUT;
	}
}

static void inventory_item_options()
{
	char string[32] = { 0 };
	char *item_name = NULL;
	int i = 0;

	/* clear window */
	build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
		g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg012);

	/* sub_14AF2 */
	assert((g_neuro_window.mode == 0) ||
		(g_neuro_window.mode > 2 && g_neuro_window.mode <= 4));
	g_neuro_window.total_items = 0;

	item_name = inventoty_get_item_name(g_c946, (g_c946 == 0x7F) ? string : NULL);
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
} inventory_discard_t;

static void inventory_discard(int discard)
{
	char string[32] = { 0, };
	char *item_name = 0;

	/* clear window */
	build_text_frame(g_neuro_window.bottom - g_neuro_window.top + 1,
		g_neuro_window.right - g_neuro_window.left + 1, (imh_hdr_t*)g_seg012);

	/* sub_14AF2 */
	assert((g_neuro_window.mode == 0) ||
		(g_neuro_window.mode > 2 && g_neuro_window.mode <= 4));
	g_neuro_window.total_items = 0;

	neuro_window_draw_string(discard ? "ERASE" : "Discard", 72, 8);

	item_name = inventoty_get_item_name(g_c946, NULL);

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

static void ui_open_inventory()
{
	/* setup inventory window */
	neuro_window_setup(3);

	g_state = LS_INVENTORY;
	inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
}

static inventory_state_t on_inventory_erase_software_list_button(neuro_button_t *button)
{
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
		inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
		return IS_ITEM_LIST;

	case 0x0B: /* more */
		inventory_item_list(IT_SOFTWARE, 4, ISLP_NEXT);
		break;
	}

	return IS_ERASE_SOFTWARE_LIST;
}

static inventory_state_t on_inventory_discard_button(neuro_button_t *button, int discard)
{
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
		inventory_item_list((discard == ID_ITEM) ? IT_ITEMS : IT_SOFTWARE, 4, ISLP_FIRST);
		return (discard == ID_ITEM) ? IS_ITEM_LIST : IS_ERASE_SOFTWARE_LIST;
	}

	return IS_DISCARD_ITEM;
}

static inventory_state_t on_inventory_item_options_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0x64: /* discard */
		if (g_c946 == 0x53 || g_c946 == 0x7F)
		{
			inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
			return IS_ITEM_LIST;
		}
		inventory_discard(ID_ITEM);
		return IS_DISCARD_ITEM;

	case 0x65: /* erase */
		inventory_item_list(IT_SOFTWARE, 4, ISLP_FIRST);
		return IS_ERASE_SOFTWARE_LIST;

	case 0x67:
		break;

	case 0x6F:
		break;

	case 0x0A: /* exit */
		inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
		return IS_ITEM_LIST;
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
		g_state = LS_NORMAL;
		drawing_control_remove_sprite_from_chain(++g_4bae.x4ccf);
		/* restoring "window" state */
		restore_window();
		break;

	case 0x0B: /* more */
		inventory_item_list(IT_ITEMS, 4, ISLP_NEXT);
		break;
	}

	return IS_ITEM_LIST;
}

static void on_inventory_button(neuro_button_t *button)
{
	static inventory_state_t state = IS_ITEM_LIST;
	state = (state == g_inventory_state) ? state : g_inventory_state;

	switch (state) {
	case IS_ITEM_LIST:
		state = on_inventory_item_list_button(button);
		break;

	case IS_ITEM_OPTIONS:
		state = on_inventory_item_options_button(button);
		break;

	case IS_DISCARD_ITEM:
		state = on_inventory_discard_button(button, IT_ITEMS);
		break;

	case IS_ERASE_SOFTWARE_LIST:
		state = on_inventory_erase_software_list_button(button);
		break;

	case IS_ERASE_SOFTWARE:
		state = on_inventory_discard_button(button, IT_SOFTWARE);
		break;
	}

	g_inventory_state = state;
}

static void on_ui_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0x00: /* inventory */
		ui_open_inventory();
		break;

	case 0x0A: /* panel date */
		g_ui_panel_mode = UI_PM_DATE;
		break;

	case 0x0B: /* panel time */
		g_ui_panel_mode = UI_PM_TIME;
		break;

	case 0x0C: /* panel cash */
		g_ui_panel_mode = UI_PM_CASH;
		break;

	case 0x0D: /* panel con */
		g_ui_panel_mode = UI_PM_CON;
		break;

	default:
		break;
	}
}

static void on_window_button(neuro_button_t *button)
{
	switch (g_neuro_window.mode) {
	case 0:
		on_ui_button(button);
		break;

	case 3:
		on_inventory_button(button);
		break;

	default:
		break;
	}
}

static void select_window_button(neuro_button_t *button)
{
	uint8_t *pic = NULL;

	switch (g_neuro_window.mode) {
	case 0:
		pic = g_background;
		break;
	case 3:
		pic = g_seg012;
		break;
	default:
		return;
	}

	uint8_t *p = pic + sizeof(imh_hdr_t) +
		((button->left - g_neuro_window.left) / 2) +
		((button->top - g_neuro_window.top) * g_neuro_window.c944);

	uint16_t lines = button->bottom - button->top + 1;
	uint16_t width = ((button->right | 1) - (button->left & 0xFE) + 1) / 2;
	uint16_t skip = g_neuro_window.c944 - width;

	for (uint16_t h = 0; h < lines; h++, p += skip)
	{
		for (uint16_t w = 0; w < width; w++)
		{
			*p++ ^= 0xFF;
		}
	}

	return;
}

static void unselect_window_button(neuro_button_t *button)
{
	select_window_button(button);
}

static neuro_button_t* window_button_hit_test()
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];

	for (uint16_t u = 0; u < g_neuro_window.total_items; u++)
	{
		neuro_button_t *hit =
			(neuro_button_t*)g_neuro_window_wrapper.window_item[u];

		if (cursor->left > hit->left && cursor->left < hit->right &&
			cursor->top > hit->top && cursor->top < hit->bottom)
		{
			return hit;
		}
	}

	return NULL;
}

static void window_handle_input(sfEvent *event)
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];
	neuro_button_t *button = NULL;
	static neuro_button_t *selected = NULL; /* selected button */
	static int _selected = 0; /* redrawing flag */

	switch (g_neuro_window.mode) {
	case 0:
		/* ui area */
		if (cursor->left < 0 || cursor->left > 320 ||
			cursor->top < 140 || cursor->top > 200)
		{
			return;
		}
		break;
	case 3:
		/* inventory area */
		if (cursor->left < g_neuro_window.left || cursor->left > g_neuro_window.right ||
			cursor->top < g_neuro_window.top || cursor->top > g_neuro_window.bottom)
		{
			return;
		}
		break;

	default:
		break;
	}
	
	if (sfMouse_isButtonPressed(sfMouseLeft))
	{
		if (button = window_button_hit_test())
		{
			if ((!selected || (button == selected)) && !_selected)
			{
				select_window_button(button);
				selected = button;
				_selected = 1;
			}
		}
		else if (selected && _selected)
		{
			unselect_window_button(selected);
			_selected = 0;
		}
	}
	else if (event->mouseButton.type == sfEvtMouseButtonReleased)
	{
		if (selected)
		{
			if (_selected)
			{
				unselect_window_button(selected);
				_selected = 0;
			}

			if (selected == window_button_hit_test())
			{
				on_window_button(selected);
			}

			selected = NULL;
		}
	}
}

static void update_inventory(sfEvent *event)
{
	window_handle_input(event);
}

static void update_normal(sfEvent *event)
{
	character_control_handle_input();
	character_control_update();

	/* execute the following VM instruction */
	sub_105F6(SUB_105F6_OP_NEURO_VM_STEP);

	window_handle_input(event);
	ui_panel_update();

	bg_animation_control_update();
}

static void update_intro()
{
	static level_intro_state_t state = LIS_NEXT_LINE;
	static int lines_on_screen = 0, lines_scrolled = 0;
	static int frame_cap_ms = 10;
	static int elapsed = 0;

	char line[18] = { 0, };
	uint8_t *pix = g_background + sizeof(imh_hdr_t);
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return;
	}
	elapsed = passed;

	if (state == LIS_NEXT_LINE)
	{
		int last = extract_line(&g_bih_string_ptr, line, 17);

		build_string(line, 320, 200, 176, 182, pix);
		
		if (last)
		{
			g_state = LS_NORMAL;
			state = LIS_NEXT_LINE;
			lines_on_screen = 0;
			return;
		}

		state = (++lines_on_screen == 7)
			? LIS_WAITING_FOR_INPUT : LIS_SCROLLING;

	}
	else if (state == LIS_SCROLLING)
	{
		for (int i = 135, j = 134; i < 191; i++, j++)
		{
			memmove(&pix[160 * j + 88], &pix[160 * i + 88], 68);
		}

		if (++lines_scrolled == 8)
		{
			lines_scrolled = 0;
			state = LIS_NEXT_LINE;
		}
	}
	else if (state == LIS_WAITING_FOR_INPUT)
	{
		if (sfMouse_isButtonClicked(sfMouseLeft))
		{
			state = LIS_SCROLLING;
			lines_on_screen = 0;
		}
	}
	
	return;
}

static void wait_for_input()
{
	if (sfMouse_isButtonClicked(sfMouseLeft))
	{
		switch (g_neuro_window.mode) {
		case 3:
			if (g_inventory_state == IS_ITEM_LIST)
			{
				g_state = LS_NORMAL;
				drawing_control_remove_sprite_from_chain(++g_4bae.x4ccf);
				/* restoring "window" state */
				restore_window();
			}
			else
			{
				g_state = LS_INVENTORY;
				g_inventory_state = IS_ITEM_LIST;
				inventory_item_list(IT_ITEMS, 4, ISLP_FIRST);
			}
			break;

		case 8:
			g_state = LS_NORMAL;
			drawing_control_remove_sprite_from_chain(++g_4bae.x4ccf);
			drawing_control_remove_sprite_from_chain(SCI_DIALOG_BUBBLE);
			/* restoring "window" state */
			restore_window();
			break;

		default:
			break;
		}
	}
}

static neuro_scene_id_t update(sfEvent *event)
{
	neuro_scene_id_t scene = NSID_LEVEL;

	update_cursor();

	switch (g_state)
	{
	case LS_INTRO:
		update_intro();
		break;

	case LS_NORMAL:
		update_normal(event);
		break;

	case LS_INVENTORY:
		update_inventory(event);
		break;

	case LS_WAIT_FOR_INPUT:
		wait_for_input();
		break;

	default:
		break;
	}

	return scene;
}

static void deinit()
{
	drawing_control_remove_sprite_from_chain(SCI_LEVEL_BG);
	drawing_control_remove_sprite_from_chain(SCI_BACKGRND);
	drawing_control_remove_sprite_from_chain(SCI_CHARACTER);
	g_bih_string_ptr = NULL;
}

void setup_level_scene()
{
	g_scene.id = NSID_LEVEL;
	g_scene.init = init;
	g_scene.update = update;
	g_scene.deinit = deinit;
}
