#include "data.h"
#include "globals.h"
#include "scene_control.h"
#include "character_control.h"
#include "bg_animation_control.h"
#include "drawing_control.h"
#include "resource_manager.h"
#include "neuro_window_control.h"
#include "inventory_control.h"
#include "dialog_control.h"
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

typedef enum level_intro_state_t {
	LIS_NEXT_LINE = 0,
	LIS_SCROLLING,
	LIS_WAIT_FOR_INPUT,
} level_intro_state_t;

static char *g_bih_string_ptr = NULL;

/***************************************/
static uint64_t sub_105F6(level_state_t *state, uint16_t opcode, ...);

void sub_1342E(char *str, uint16_t mode)
{
	char text[256] = { 0, };
	char *p = text;
	int lines = 0;

	while (!extract_line(&str, p, 38))
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

	neuro_window_setup(mode, lines);
	neuro_window_draw_string(text, 0, 0, 0);
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
	sub_1342E(g_bih_string_ptr, NWM_NPC_DIALOG_REPLY);

	g_4bae.roompos_spawn_x = temp_1;
	g_4bae.roompos_spawn_y = temp_2;

	return;
}

/* just a stub */
static void bih_call(uint16_t offt)
{
	if (g_4bae.level_n != 0)
	{
		return;
	}

	switch (offt) {
	case 0xDB: {
		x4bae_t *s = (x4bae_t*)g_bih_wrapper.ctrl_struct_addr;
		s->cash += s->cash_withdrawal;
		break;
	}

	default:
		break;
	}

}

/* sub_10735 */
static void neuro_vm(level_state_t *state)
{
	for (int i = 3; i >= 0; i--)
	{
		uint16_t vm_thread = g_a8e0.a8e0[i];
		if (vm_thread == 0xffff) {
			continue;
		}

		uint8_t *opcode_addr = g_3f85_wrapper.vm_next_op_addr[vm_thread];
		uint8_t opcode = *opcode_addr;

		switch (opcode) {
		/* dialog npc reply */
		case 0x01: {
			uint8_t string_num = *(opcode_addr + 1);

			sub_10A5B(string_num, 0,
				g_3f85.vm_state[vm_thread].var_1,
				g_3f85.vm_state[vm_thread].var_2);

			*state = LS_WAIT_FOR_INPUT;

			g_3f85_wrapper.vm_next_op_addr[vm_thread] += 2;
			g_4bae.active_dialog_reply = 0xFF;
			break;
		}

		/* text output */
		case 0x02: {
			uint8_t string_num = *(opcode_addr + 1);

			/* bad */
			sub_10A5B(string_num, 1, 0, 0);
			if (g_neuro_window.mode == NWM_NEURO_UI)
			{
				*state = LS_TEXT_OUTPUT;
			}

			g_3f85_wrapper.vm_next_op_addr[vm_thread] += 2;
			break;
		}

		/* call */
		case 0x03:
			g_3f85_wrapper.vm_next_op_addr[vm_thread] =
				(uint8_t*)sub_105F6(NULL, 4, g_3f85.vm_state[vm_thread].flag & 3,
					*(opcode_addr + 1));
			break;

		/* goto */
		case 0x04: {
			int16_t offset = *(int16_t*)(opcode_addr + 1);
			g_3f85_wrapper.vm_next_op_addr[vm_thread] += offset;
			g_3f85_wrapper.vm_next_op_addr[vm_thread] += 1;
			break;
		}

		/* conditional jump */
		case 0x05:
		case 0x06:
		case 0x07: 
		case 0x08: {
			jumps_t jmp = g_4b9d[opcode - 5];
			uint8_t x4bae_index = *(opcode_addr + 1);
			uint8_t x4bae_value = *(opcode_addr + 3);

			if (((jmp == JE) && (g_4bae.x4bae[x4bae_index] == x4bae_value)) ||
				((jmp == JNE) && (g_4bae.x4bae[x4bae_index] != x4bae_value)) ||
				((jmp == JL) && (g_4bae.x4bae[x4bae_index] < x4bae_value)) ||
				((jmp == JGE) && (g_4bae.x4bae[x4bae_index] >= x4bae_value)))
			{
				g_3f85_wrapper.vm_next_op_addr[vm_thread] += 6;
			}
			else
			{
				int16_t offset = *(int16_t*)(opcode_addr + 4);
				g_3f85_wrapper.vm_next_op_addr[vm_thread] += offset;
				g_3f85_wrapper.vm_next_op_addr[vm_thread] += 4;
			}

			break;
		}

		/* set memory */
		case 0x0E:
		case 0x0F: {
			uint16_t index = *(uint16_t*)(opcode_addr + 1);
			uint16_t value = *(uint16_t*)(opcode_addr + 3);
			uint16_t *p = (uint16_t*)&g_4bae.x4bae[index];
			*p = value;
			g_3f85_wrapper.vm_next_op_addr[vm_thread] += 5;

			break;
		}

		case 0x11:
			// inc [0x152C]
			g_3f85_wrapper.vm_next_op_addr[vm_thread]++;
			break;

		case 0x12:
			// mov [0x152C], 0
			g_3f85_wrapper.vm_next_op_addr[vm_thread]++;
			break;

		/* set dialog ctrl */
		case 0x13: {
			uint8_t *p = g_3f85.x407b + *(opcode_addr + 1);
			p[0] = *(opcode_addr + 2);
			p[1] = *(opcode_addr + 3);

			g_3f85_wrapper.vm_next_op_addr[vm_thread] += 4;
			break;
		}

		/* exec */
		case 0x16:
			bih_call(*(uint16_t*)(opcode_addr + 1));
			g_3f85_wrapper.vm_next_op_addr[vm_thread] += 3;
			break;

		/* dialog */
		case 0x17:
			*state = LS_DIALOG;
			g_dialog_escapable = 0;
			g_3f85_wrapper.vm_next_op_addr[vm_thread]++;
			break;

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

	return 0;
}

typedef enum sub_105f6_opcodes_t {
	SUB_105F6_OP_PLAY_LEVEL_INTRO = 5,
	SUB_105F6_OP_NEURO_VM_STEP,
} sub_105f6_opcodes_t;

static uint64_t sub_105F6(level_state_t *state, uint16_t opcode, ...)
{
	switch (opcode) {
	case 0: {
		va_list args;
		va_start(args, opcode);
		uint16_t level_n = va_arg(args, uint16_t);
		va_end(args);

		g_a642 = g_3f85.x407b + (6 * level_n);
		g_bih_wrapper.bih = (bih_hdr_t*)g_a8e0.bih;
		g_bih_wrapper.ctrl_struct_addr = (uint8_t*)&g_4bae;
		// g_bih_wrapper.cb_addr = &cb;
		break;
	}

	case 1:
	case 3: {
		uint16_t offt = g_bih_wrapper.bih->init_obj_code_offt[opcode - 1];
		// call (a8e8 + offt)
		break;
	}

	case 4: {
		va_list args;
		va_start(args, opcode);
		uint16_t x = va_arg(args, uint16_t);
		uint16_t y = va_arg(args, uint16_t);
		va_end(args);

		uint16_t array_offt = g_bih_wrapper.bih->bytecode_array_offt[x - 1];
		uint8_t *array = g_a8e0.bih + array_offt;
		uint8_t *program = g_a8e0.bih + array[y * 2];
		return (uint64_t)program;
	}

	case SUB_105F6_OP_PLAY_LEVEL_INTRO: {
		setup_intro();
		break;
	}

	case SUB_105F6_OP_NEURO_VM_STEP:
		neuro_vm(state);
		break;

	default:
		break;
	}

	return 0;
}

int sub_1152B()
{
	uint8_t *p = g_a8e0.bih + sizeof(bih_hdr_t); // 0xA910

	while ((*p != 0) && (*p != 1))
	{
		p++;
	}

	return (*p == 1) ? 1 : 0;
}

int sub_1155A()
{
	uint8_t *p = g_a8e0.bih + sizeof(bih_hdr_t); // 0xA910

	while ((*p != 0) && (*p != 2))
	{
		p++;
	}

	return (*p == 2) ? 1 : 0;
}

/* sub_156D4 */
int setup_ui_buttons()
{
	assert((g_neuro_window.mode == NWM_NEURO_UI) ||
		(g_neuro_window.mode > 2 && g_neuro_window.mode <= 4));
	g_neuro_window.total_items = 0;

	neuro_window_add_button(&g_ui_buttons.inventory);
	neuro_window_add_button(&g_ui_buttons.dialog);

	if (sub_1152B())
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

	assert(resource_manager_load_resource("NEURO.IMH", g_background));
	drawing_control_add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_background, 1);

	neuro_window_setup(0);

	if (g_level_n >= 0 && g_4bae.x4bcc == 0)
	{
		/* sub_105F6(3) */
	}

	g_4bae.x4bcc = 0;

	g_4bae.x4cc3 = 0;
	g_4bae.active_dialog_reply = 0xFF;
	g_4bae.x4bbf = 0xFF;
	g_4bae.active_item = 0xFFFF;
	g_4bae.cash_withdrawal = 0xFFFFFFFF;
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

	sub_105F6(NULL, 0, g_level_n);

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
			g_3f85_wrapper.vm_next_op_addr[u] = (uint8_t*)sub_105F6(NULL, 4, p->flag & 3, 0);
		}

		uint16_t offt = p->flag & 3;
		g_a8e0.a8e0[offt] = u;

	next:
		p++;
		u++;
	}

	sub_105F6(NULL, 1);
	setup_ui_buttons();
	sub_105F6(NULL, SUB_105F6_OP_PLAY_LEVEL_INTRO);

	ui_panel_update();
	return;
}

void ui_handle_mouse(level_state_t *state, neuro_button_t *button)
{
	switch (button->code) {
	case 0x00: /* inventory */
		*state = LS_INVENTORY;
		break;

	case 0x02: /* dialog */
		g_dialog_escapable = 1;
		*state = LS_DIALOG;
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

static level_state_t wait_for_input()
{
	if (sfMouse_isLeftMouseButtonClicked())
	{
		switch (g_neuro_window.mode) {
		case NWM_NPC_DIALOG_REPLY:
			drawing_control_remove_sprite_from_chain(++g_4bae.x4ccf);
			drawing_control_remove_sprite_from_chain(SCI_DIALOG_BUBBLE);
			/* restoring "window" state */
			restore_window();
			return LS_NORMAL;

		default:
			break;
		}
	}

	return LS_WAIT_FOR_INPUT;
}

static level_state_t update_normal(sfEvent *event)
{
	level_state_t state = LS_NORMAL;
	level_state_t new_state = state;

	window_handle_input((int*)&new_state, event);
	if (new_state != state)
	{
		return new_state;
	}

	ui_panel_update();

	character_control_handle_input();
	character_control_update();

	/* execute the following VM instruction */
	sub_105F6(&state, SUB_105F6_OP_NEURO_VM_STEP);

	bg_animation_control_update();

	return state;
}

static level_state_t update_text_output()
{
	static level_intro_state_t state = LIS_NEXT_LINE;
	static int lines_on_screen = 0, lines_scrolled = 0;
	static int frame_cap_ms = 11;
	static int elapsed = 0;

	char line[18] = { 0, };
	uint8_t *pix = g_background + sizeof(imh_hdr_t);
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return LS_TEXT_OUTPUT;
	}
	elapsed = passed;

	if (state == LIS_NEXT_LINE)
	{
		int last = extract_line(&g_bih_string_ptr, line, 17);

		neuro_window_draw_string(line);

		if (last)
		{
			state = LIS_NEXT_LINE;
			lines_on_screen = 0;
			return LS_NORMAL;
		}

		state = (++lines_on_screen == 7)
			? LIS_WAIT_FOR_INPUT : LIS_SCROLLING;

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
	else if (state == LIS_WAIT_FOR_INPUT)
	{
		if (sfMouse_isLeftMouseButtonClicked())
		{
			state = LIS_SCROLLING;
			lines_on_screen = 0;
		}
	}
	
	return LS_TEXT_OUTPUT;
}

static neuro_scene_id_t update(sfEvent *event)
{
	static level_state_t state = LS_TEXT_OUTPUT;
	neuro_scene_id_t scene = NSID_LEVEL;

	update_cursor();

	switch (state) {
	case LS_TEXT_OUTPUT:
		state = update_text_output();
		break;

	case LS_NORMAL:
		state = update_normal(event);
		break;

	case LS_INVENTORY:
		state = update_inventory(event);
		break;

	case LS_DIALOG:
		state = update_dialog(event);
		break;

	case LS_WAIT_FOR_INPUT:
		state = wait_for_input();
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
	g_scene.handle_input = NULL;
	g_scene.update = update;
	g_scene.deinit = deinit;
}
