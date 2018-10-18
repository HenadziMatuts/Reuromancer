#include "data.h"
#include "globals.h"
#include "scene_control.h"
#include "character_control.h"
#include "bg_animation_control.h"
#include "drawing_control.h"
#include "resource_manager.h"
#include "neuro_window_control.h"
#include "scene_real_world.h"
#include "window_animation.h"
#include "address_translator.h"
#include "neuro86.h"
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

static real_world_state_t g_state = RWS_TEXT_OUTPUT;
static int g_wfi_press = 0;

static text_scrolling_data_t g_text_scrolling_data = {
	.frame_cap = 11,
};

static screen_fading_data_t g_screen_fading_data = {
	.direction = FADE_OUT,
	.step = 32,
	.frame_cap = 15,
};

static void level_transition_prepare_anim(int exit)
{
	if ((g_4bae.level_n >= 12 && g_level_n >= 12 && g_4bae.level_n <= 17 && g_level_n <= 17) ||
		(g_4bae.level_n >= 36 && g_level_n >= 36 && g_4bae.level_n <= 38 && g_level_n <= 38) ||
		(g_4bae.level_n >= 53 && g_level_n >= 53 && g_4bae.level_n <= 54 && g_level_n <= 54))
	{
		return;
	}

	g_screen_fading_data.direction = (exit) ? FADE_OUT : FADE_IN;
	window_animation_setup(WA_TYPE_SCREEN_FADING, &g_screen_fading_data);

	g_screen_fading_data.direction = FADE_OUT;
}

/***************************************/

typedef enum sub_105f6_opcodes_t {
	SUB_105F6_OP_PREPARE_BIH = 0,
	SUB_105F6_OP_INIT_LEVEL,
	SUB_105F6_OP_UPDATE_LEVEL,
	SUB_105F6_OP_DEINIT_LEVEL,
	SUB_105F6_OP_GET_VM_PROG_ADDR,
	SUB_105F6_OP_SETUP_LEVEL_INTRO,
	SUB_105F6_OP_NEURO_VM_CYCLE,
} sub_105f6_opcodes_t;

static uint64_t sub_105F6(real_world_state_t *state, uint16_t opcode, ...);

void sub_1342E(char *str, uint16_t mode)
{
	char line[39] = { 0, };
	char text[256] = { 0, };
	char *p = text;
	int lines = 0;

	extract_line_prepare(str, line, 38);

	while (extract_line())
	{
		size_t l = strlen(line);
		if (l != 38)
		{
			memset(line + l, 0x20, 38 - l);
		}

		memmove(p, line, 38);
		p += 38;
		*p++ = '\n';
		lines++;
	}
	*--p = 0;

	neuro_window_setup(mode, lines);
	neuro_window_draw_string(text, 0, 0, 0);
}

static real_world_state_t sub_10A5B(uint16_t line_n, uint16_t mode, uint16_t l, uint16_t t)
{
	char *text = g_a8e0.bih.bytes + g_a8e0.bih.hdr.text_offset;

	for (int i = 0; i < line_n; i++)
	{
		while (*text++);
	}

	if (mode != 0)
	{
		if (g_neuro_window.mode == NWM_NEURO_UI)
		{
			g_text_scrolling_data.text = text;
			window_animation_setup(WA_TYPE_TEXT_SCROLLING, &g_text_scrolling_data);
			return RWS_TEXT_OUTPUT;
		}
		else
		{
			neuro_window_draw_string(text);
		}

		return RWS_NORMAL;
	}

	uint16_t x = g_4bae.roompos_x;
	uint16_t y = g_4bae.roompos_y;

	g_4bae.roompos_x = l * 2;
	g_4bae.roompos_y = t;

	/* builds dialog bubble with text */
	sub_1342E(text, NWM_NPC_DIALOG_REPLY);

	g_4bae.roompos_x = x;
	g_4bae.roompos_y = y;

	return RWS_WAIT_FOR_INPUT;
}

/* sub_10735 */
static void neuro_vm(real_world_state_t *state)
{
	static int thread = 3;

	thread = (thread < 0) ? 3 : thread;

	for (thread; thread >= 0; thread--)
	{
		uint16_t n = g_a8e0.a8e0[thread];
		if (n == 0xffff) {
			continue;
		}

		uint8_t *opcode_addr = translate_x16_to_x64(DSEG, g_3f85.vm_state[n].vm_next_op_addr);
		uint8_t opcode = *opcode_addr;

		switch (opcode) {
		case 0x00: {
			uint8_t *op_addr = (uint8_t*)sub_105F6(NULL,
				SUB_105F6_OP_GET_VM_PROG_ADDR, g_3f85.vm_state[n].flag & 3, 0);
			translate_x64_to_x16(op_addr, NULL, &g_3f85.vm_state[n].vm_next_op_addr);
			break;
		}

		/* dialog npc reply */
		case 0x01: {
			uint8_t string_num = *(opcode_addr + 1);

			*state = sub_10A5B(string_num, 0,
				g_3f85.vm_state[n].var_1,
				g_3f85.vm_state[n].var_2);

			sfSetKeyRepeat(0);

			g_3f85.vm_state[n].vm_next_op_addr += 2;
			g_4bae.active_dialog_reply = 0xFF;
			
			thread--;
			return;
		}

		/* text output */
		case 0x02: {
			uint8_t string_num = *(opcode_addr + 1);
			*state = sub_10A5B(string_num, 1, 0, 0);

			g_3f85.vm_state[n].vm_next_op_addr += 2;
			thread--;
			return;
		}

		/* load program */
		case 0x03: {
			uint8_t *op_addr = (uint8_t*)sub_105F6(NULL,
				SUB_105F6_OP_GET_VM_PROG_ADDR, g_3f85.vm_state[n].flag & 3, *(opcode_addr + 1));
			translate_x64_to_x16(op_addr, NULL, &g_3f85.vm_state[n].vm_next_op_addr);
			break;
		}

		/* goto */
		case 0x04: {
			int16_t offset = *(int16_t*)(opcode_addr + 1);
			g_3f85.vm_state[n].vm_next_op_addr += (offset + 1);
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
				g_3f85.vm_state[n].vm_next_op_addr += 6;
			}
			else
			{
				int16_t offset = *(int16_t*)(opcode_addr + 4);
				g_3f85.vm_state[n].vm_next_op_addr += (offset + 4);
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
			g_3f85.vm_state[n].vm_next_op_addr += 5;
			break;
		}

		/* load level */
		case 0x10: {
			g_load_level_vm = 1;
			uint8_t level = *(uint8_t*)(opcode_addr + 1);
			g_4bae.level_n = level;
			level_transition_prepare_anim(1);
			*state = RWS_RELOAD_LEVEL;
			thread--;
			return;
		}

		case 0x11:
			g_update_hold++;
			g_3f85.vm_state[n].vm_next_op_addr++;
			break;

		case 0x12:
			g_update_hold = 0;
			g_3f85.vm_state[n].vm_next_op_addr++;
			break;

		/* set dialog ctrl */
		case 0x13: {
			uint16_t level_n = *(opcode_addr + 1);
			g_3f85.level_info[level_n].first_dialog_reply = *(opcode_addr + 2);
			g_3f85.level_info[level_n].total_dialog_replies = *(opcode_addr + 3);
			g_3f85.vm_state[n].vm_next_op_addr += 4;
			break;
		}

		case 0x15: {
			uint16_t index = *(uint16_t*)(opcode_addr + 1);
			uint16_t value = *(uint16_t*)(opcode_addr + 3);
			uint16_t *p = (uint16_t*)&g_4bae.x4bae[index];
			*p += value;
			g_3f85.vm_state[n].vm_next_op_addr += 5;
			break;
		}

		/* exec */
		case 0x16: {
			uint16_t offt = *(uint16_t*)(opcode_addr + 1);
			
			cpu_reset(g_cpu, offt + 0xA8E8);
			cpu_run(g_cpu);
			
			g_3f85.vm_state[n].vm_next_op_addr += 3;
			break;
		}

		/* dialog */
		case 0x17:
			*state = RWS_DIALOG;
			g_dialog_escapable = 0;
			g_3f85.vm_state[n].vm_next_op_addr++;

			thread--;
			return;

		case 0x18: {
			uint16_t *string_num_addr = &g_4bae.x4c7c;
			string_num_addr += *(opcode_addr + 1);

			*state = sub_10A5B(*string_num_addr, 0,
				g_3f85.vm_state[n].var_1,
				g_3f85.vm_state[n].var_2);

			sfSetKeyRepeat(0);

			g_3f85.vm_state[n].vm_next_op_addr += 2;
			g_4bae.active_dialog_reply = 0xFF;

			thread--;
			return;
		}

		default:
			fprintf(stderr, "neuro_vm: opcode: %d\n", opcode);
			assert(opcode == 0);
		}
	}
}

/* sub_106BD */
static int setup_intro()
{
	uint16_t bit = 0x80 >> (g_level_n & 7);
	uint16_t index = g_level_n >> 3;
	char *text = g_a8e0.bih.bytes + g_a8e0.bih.hdr.text_offset;

	if ((bit & g_visited_levels_bitstring[index]) == 0) {
		/* setup long intro */
		g_visited_levels_bitstring[index] |= bit;
	}
	else {
		/* setup short intro */
		while (*text++);
	}

	g_text_scrolling_data.text = text;
	window_animation_setup(WA_TYPE_TEXT_SCROLLING, &g_text_scrolling_data);

	return 0;
}

static void reset_vm()
{
	for (int i = 0; i < 4; i++)
	{
		uint16_t n = g_a8e0.a8e0[i];
		if (n == 0xffff) {
			continue;
		}

		g_3f85.vm_state[n].mark = 0xFF;
	}
}

typedef enum neuro_cb_cmd_t {
	CB_CMD_RESET_VM = 0,
	CB_CMD_HAS_ITEM = 2,
	CB_CMD_REMOVE_ITEM = 3,
	CB_CMD_NPC_REPLY = 5,
	CB_CMD_SELL_BODY_PART = 8,
	CB_CMD_BUY_BODY_PART = 9,
	CB_CMD_PREPARE_ANIM = 31,
} neuro_cb_cmd_t;

/* seg000:0BCF */
uint8_t neuro_cb(uint16_t sp)
{
	union {
		uint16_t *ss16;
		uint8_t *ss8;
	} ss;

	ss.ss8 = translate_x16_to_x64(DSEG, sp + 4);
	uint16_t cmd = *ss.ss16;

	switch (cmd) {
	case CB_CMD_RESET_VM:
		reset_vm();
		break;

	case CB_CMD_HAS_ITEM: {
		uint16_t item_code = *(ss.ss16 + 1);
		uint8_t *inventory = (g_inventory_item_operations[item_code] & 0xC0) ?
			g_3f85.inventory.items : g_3f85.inventory.software;

		g_4bae.x4c82 = 0;

		for (int i = 0; i < 32; i++, inventory += 4)
		{
			uint8_t item = inventory[0];
			
			if (item == item_code)
			{
				g_4bae.x4c82 = 1;
				break;
			}
		}

		break;
	}

	case CB_CMD_REMOVE_ITEM: {
		uint16_t item_code = *(ss.ss16 + 1);
		uint8_t *inventory = (g_inventory_item_operations[item_code] & 0xC0) ?
			g_3f85.inventory.items : g_3f85.inventory.software;
		
		for (int i = 0; i < 32; i++, inventory += 4)
		{
			uint8_t item = inventory[0];

			if (*(ss.ss16 + 2) == 0)
			{
				break;
			}

			if (item == item_code)
			{
				*inventory = 0xFF;
			}
		}
		
		break;
	}

	case CB_CMD_NPC_REPLY: {
		char *string = translate_x16_to_x64(DSEG, *(ss.ss16 + 1));
		uint16_t num = *(ss.ss16 + 2), u;

		for (u = 0; u < num; u++)
		{
			if (!_strcmpi(g_dialog_user_input, string))
			{
				break;
			}
			string += strlen(string) + 1;
		}

		g_4bae.x4c82 = u;
		break;
	}

	case CB_CMD_SELL_BODY_PART:
		g_body_shop_op = 1;
		g_state = RWS_BODY_PARTS_SHOP;
		return CPU_STOPPED;

	case CB_CMD_BUY_BODY_PART:
		g_body_shop_discount = *(ss.ss16 + 1);
		g_body_shop_op = 0;
		g_state = RWS_BODY_PARTS_SHOP;
		return CPU_STOPPED;

	case CB_CMD_PREPARE_ANIM:
		bg_animation_control_prepare(*(ss.ss16 + 1), *(ss.ss16 + 2));
		break;

	default:
		fprintf(stderr, "neuro_cb: cmd: %d\n", cmd);
		assert(cmd == 0);
	}

	return CPU_RUNNING;
}

static uint64_t sub_105F6(real_world_state_t *state, uint16_t opcode, ...)
{
	switch (opcode) {
	case SUB_105F6_OP_PREPARE_BIH: {
		va_list args;
		va_start(args, opcode);
		uint16_t level_n = va_arg(args, uint16_t);
		va_end(args);

		g_a642 = &g_3f85.level_info[level_n];
		bih_hdr_t *bih = &g_a8e0.bih.hdr;
		translate_x64_to_x16((uint8_t*)&g_4bae, NULL, &bih->ctrl_struct_addr);
		translate_x64_to_x16((uint8_t*)neuro_cb, &bih->cb_segt, &bih->cb_offt);

		break;
	}

	case SUB_105F6_OP_UPDATE_LEVEL: /* update level bih call */
		bg_animation_control_update();
	case SUB_105F6_OP_INIT_LEVEL: /* enter/exit level bih call */
	case SUB_105F6_OP_DEINIT_LEVEL: {
		uint16_t offt = g_a8e0.bih.hdr.init_obj_code_offt[opcode - 1];
		cpu_reset(g_cpu, offt + 0xA8E8);
		cpu_run(g_cpu);
		break;
	}

	case SUB_105F6_OP_GET_VM_PROG_ADDR: {
		va_list args;
		va_start(args, opcode);
		uint16_t x = va_arg(args, uint16_t);
		uint16_t y = va_arg(args, uint16_t);
		va_end(args);

		uint16_t array_offt = g_a8e0.bih.hdr.bytecode_array_offt[x - 1];
		uint8_t *array = g_a8e0.bih.bytes + array_offt;
		uint8_t *program = g_a8e0.bih.bytes + array[y * 2];
		return (uint64_t)program;
	}

	case SUB_105F6_OP_SETUP_LEVEL_INTRO: {
		setup_intro();
		break;
	}

	case SUB_105F6_OP_NEURO_VM_CYCLE:
		neuro_vm(state);
		break;

	default:
		assert(opcode == 0);
	}

	return 0;
}

int is_pax_on_level()
{
	uint8_t *p = g_a8e0.bih.bytes + sizeof(bih_hdr_t); // 0xA910

	while ((*p != 0) && (*p != 1))
	{
		p++;
	}

	return (*p == 1) ? 1 : 0;
}

int is_jack_on_level()
{
	uint8_t *p = g_a8e0.bih.bytes + sizeof(bih_hdr_t); // 0xA910

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

	if (is_pax_on_level())
	{
		neuro_window_add_button(&g_ui_buttons.pax);
	}

	neuro_window_add_button(&g_ui_buttons.skills);
	neuro_window_add_button(&g_ui_buttons.chip);
	neuro_window_add_button(&g_ui_buttons.disk);
	neuro_window_add_button(&g_ui_buttons.date);
	neuro_window_add_button(&g_ui_buttons.time);
	neuro_window_add_button(&g_ui_buttons.cash);
	neuro_window_add_button(&g_ui_buttons.constitution);

	return 0;
}

/***************************************/

void build_date_string(char *dst, uint8_t date_day)
{
	uint16_t day = 16;
	uint16_t month = 11;
	uint16_t year = 58;

	if (date_day > 14)
	{
		if (day + date_day > 61)
		{
			year = 59;
			month = 1;
			day = day + date_day - 61;
		}
		else
		{
			month = 12;
			day = day + date_day - 30;
		}
	}
	else
	{
		day += date_day;
	}

	sprintf(dst, "%02d/%02d/%02d", month, day, year);
}

static void ui_panel_update()
{
	char panel_string[9];
	uint8_t *bg_pix = g_seg010.background + sizeof(imh_hdr_t);
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
				/* play music */
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
		sprintf(panel_string, "%8d", g_4bae.constitution);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;

	case UI_PM_TIME:
		sprintf(panel_string, "   %02d:%02d", g_4bae.time_h, g_4bae.time_m);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;

	case UI_PM_DATE: {
		build_date_string(panel_string, g_4bae.date_day);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;
	}

	default:
		break;
	}
}

static void roompos_init()
{
	roompos_t *roompos = (roompos_t*)g_seg016.roompos;
	roompos_level_t *roompos_level = &roompos->roompos_level[g_4bae.level_n];
	uint16_t transition = 0;
	character_dir_t dir = CD_NULL;

	memmove(g_a8ae, roompos_level->roompos[0], 4);
	memmove(g_8cee, roompos_level->roompos[1], 16);

	if (g_load_level_vm != 0)
	{
		g_exit_point = -1;
	}

	transition = (g_exit_point == -1) ? 2 :
		(g_exit_point + 2) & 3;

	if (g_a642->level_transitions[transition] == 0xFF)
	{
		g_4bae.roompos_x = g_a8ae[1] + g_a8ae[3];
		g_4bae.roompos_y = (g_a8ae[0] + g_a8ae[2]) / 2 ;
	}
	else
	{
		g_4bae.roompos_y = (g_8cee[transition][3] / 2) + g_8cee[transition][1];

		uint16_t r = (g_8cee[transition][0] + g_8cee[transition][2]) << 1;
		uint16_t l = g_8cee[transition][0] << 1;
		uint16_t x3 = g_a8ae[3] << 1;
		uint16_t x4 = g_a8ae[1] << 1;

		if (g_4bae.roompos_x <= l || g_4bae.roompos_x >= r ||
			x3 >= g_4bae.roompos_x || x4 <= g_4bae.roompos_x)
		{
			g_4bae.roompos_x = (g_8cee[transition][0] << 1) + g_8cee[transition][2];
		}
	}

	dir = (transition + 2) & 3;
	character_control_add_sprite_to_chain(g_4bae.roompos_x, g_4bae.roompos_y, dir);
}

static void init()
{
	char resource[32] = { 0, };
	memset(g_vga, 0, 320 * 200 * 4);

	assert(resource_manager_load_resource("NEURO.IMH", g_seg010.background));
	drawing_control_add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_seg010.background, 1);

	neuro_window_setup(0);

	if (g_level_n >= 0 && g_4bae.x4bcc == 0)
	{
		sub_105F6(NULL, SUB_105F6_OP_DEINIT_LEVEL);
	}

	g_4bae.x4bcc = 0;

	g_4bae.x4cc3 = 0;
	g_4bae.active_dialog_reply = 0xFF;
	g_4bae.x4bbf = 0xFF;
	g_4bae.active_item = 0xFFFF;
	g_4bae.cash_withdrawal = 0xFFFFFFFF;
	g_update_hold = 0;
	g_4bae.active_skill = 0xFF;

	memset(g_a8e0.a8e0, 0xFFFF, 8);
	g_level_n = g_4bae.level_n;

	sprintf(resource, "R%d.BIH", g_level_n + 1);
	assert(resource_manager_load_resource(resource, g_a8e0.bih.bytes));

	sprintf(resource, "R%d.PIC", g_level_n + 1);
	assert(resource_manager_load_resource(resource, g_seg015.pixels));
	drawing_control_add_sprite_to_chain(SCI_LEVEL_BG, 8, 8, (uint8_t*)&g_seg015.hdr, 1);

	sprintf(resource, "R%d.ANH", g_level_n + 1);
	resource_manager_load_resource(resource, g_seg016.anh + 0x488) ?
		bg_animation_control_init_tables(g_seg016.anh + 0x488) :
		bg_animation_control_init_tables(NULL);

	sub_105F6(NULL, SUB_105F6_OP_PREPARE_BIH, g_level_n);

	uint16_t u = 0;
	neuro_vm_state_t *p = g_3f85.vm_state;

	/* Sets VM state for the level */
	while (1)
	{
		if (p->level == 0xFF)
		{
			if (g_4bae.level_n != 9 && g_4bae.level_n != 27)
			{
				roompos_init();
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
			uint8_t *op_addr = (uint8_t*)sub_105F6(NULL,
				SUB_105F6_OP_GET_VM_PROG_ADDR, p->flag & 3, 0);
			translate_x64_to_x16(op_addr, NULL, &g_3f85.vm_state[u].vm_next_op_addr);
		}

		uint16_t offt = p->flag & 3;
		g_a8e0.a8e0[offt] = u;

	next:
		p++;
		u++;
	}

	sub_105F6(NULL, SUB_105F6_OP_INIT_LEVEL);
	setup_ui_buttons();

	g_exit_point = -1;
	g_wfi_press = 0;

	ui_panel_update();
	return;
}

static void deinit()
{
	drawing_control_remove_sprite_from_chain(SCI_LEVEL_BG);
	drawing_control_remove_sprite_from_chain(SCI_BACKGRND);
	drawing_control_remove_sprite_from_chain(SCI_CHARACTER);
}

void rw_ui_handle_button_press(int *state, neuro_button_t *button)
{
	switch (button->code) {
	case 0x00: /* inventory */
		*state = RWS_INVENTORY;
		break;

	case 0x01: /* pax */
		*state = RWS_PAX;
		break;

	case 0x02: /* dialog */
		g_dialog_escapable = 1;
		*state = RWS_DIALOG;
		break;

	case 0x03: /* skills */
		*state = RWS_SKILLS;
		break;

	case 0x04: /* rom */
		if (g_4bae.x4c25 == 0)
		{
			/* play track 6 */
			break;
		}
		*state = RWS_ROM;
		break;

	case 0x05: /* disk options */
		*state = RWS_DISK_OPTIONS;
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

	case 0x0D: /* panel constitution */
		g_ui_panel_mode = UI_PM_CON;
		break;

	default:
		break;
	}
}

static void handle_wait_for_input(sfEvent *event)
{
	if (event->type == sfEvtMouseButtonPressed ||
		event->type == sfEvtKeyPressed)
	{
		g_wfi_press = 1;
	}

	if (g_wfi_press &&
		(event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased))
	{
		switch (g_neuro_window.mode) {
		case NWM_NEURO_UI:
			g_state = RWS_TEXT_OUTPUT;
			break;

		case NWM_NPC_DIALOG_REPLY:
			drawing_control_remove_sprite_from_chain(++g_4bae.frame_sc_index);
			drawing_control_remove_sprite_from_chain(SCI_DIALOG_BUBBLE);
			restore_window();
			sfSetKeyRepeat(1);
			g_state = RWS_NORMAL;

		default:
			break;
		}
	}
}

static void handle_normal_input(sfEvent *event)
{
	character_control_handle_input(event);
	neuro_window_handle_input((int*)&g_state, event);
}

static void handle_input(sfEvent *event)
{
	switch (g_state) {
	case RWS_NORMAL:
		handle_normal_input(event);
		break;

	case RWS_INVENTORY:
		handle_inventory_input(event);
		break;

	case RWS_PAX:
		handle_pax_input(event);
		break;

	case RWS_DIALOG:
		handle_dialog_input(event);
		break;

	case RWS_SKILLS:
		handle_skills_input(event);
		break;

	case RWS_ROM:
		handle_rom_input(event);
		break;

	case RWS_DISK_OPTIONS:
		handle_disk_options_input(event);
		break;

	case RWS_BODY_PARTS_SHOP:
		handle_parts_shop_input(event);
		break;

	case RWS_WAIT_FOR_INPUT:
		handle_wait_for_input(event);
		break;
	}
}

int roompos_hit_exit_zone()
{
	if (g_a642->level_transitions[g_exit_point] == 0xFF)
	{
		return -1;
	}

	uint8_t *exit_zone = g_8cee[g_exit_point];
	uint16_t l = exit_zone[0] * 2;
	uint16_t t = exit_zone[1];
	uint16_t r = l + (exit_zone[2] * 2);
	uint16_t b = t + exit_zone[3];

	if (g_exit_point & 1)
	{
		if (g_4bae.roompos_y < t || g_4bae.roompos_y > b)
		{
			return -1;
		}
		
		if (g_exit_point == 1)
		{
			if (g_4bae.roompos_x < l)
			{
				return -1;
			}
		}
		else
		{
			if (g_4bae.roompos_x > r)
			{
				return -1;
			}
		}
	}
	else
	{
		if (g_4bae.roompos_x < l || g_4bae.roompos_x > r)
		{
			return -1;
		}

		if (g_exit_point == 2)
		{
			if (g_4bae.roompos_y < t)
			{
				return -1;
			}
		}
		else
		{
			if (g_4bae.roompos_y > b)
			{
				return -1;
			}
		}
	}

	return g_a642->level_transitions[g_exit_point];
}

static real_world_state_t update_normal()
{
	character_dir_t dir = CD_NULL;

	dir = character_control_update();
	
	sub_105F6(NULL, SUB_105F6_OP_UPDATE_LEVEL);
	sub_105F6(&g_state, SUB_105F6_OP_NEURO_VM_CYCLE);

	if (!g_update_hold)
	{
		if (dir != CD_NULL)
		{
			g_exit_point = (int16_t)dir;
			int level = roompos_hit_exit_zone();
			if (level != -1)
			{
				/* reload level */
				g_4bae.level_n = level;
				level_transition_prepare_anim(1);
				g_state = RWS_RELOAD_LEVEL;
			}
		}
	}

	return g_state;
}

static real_world_state_t update_text_output()
{
	window_animation_event_t evt = window_animation_update();

	if (evt == WA_EVENT_WAIT_FOR_INPUT)
	{
		return RWS_WAIT_FOR_INPUT;
	}
	else if (evt == WA_EVENT_COMPLETED)
	{
		return RWS_NORMAL;
	}

	return RWS_TEXT_OUTPUT;
}

static neuro_scene_id_t update()
{
	neuro_scene_id_t scene = NSID_REAL_WORLD;

	update_cursor();

	if (!g_paused)
	{
		ui_panel_update();
	}

	switch (g_state) {
	case RWS_FADE_IN:
		if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			sub_105F6(&g_state, SUB_105F6_OP_SETUP_LEVEL_INTRO);
			g_state = RWS_TEXT_OUTPUT;
		}
		break;

	case RWS_TEXT_OUTPUT:
		g_state = update_text_output();
		break;

	case RWS_NORMAL:
		g_state = update_normal();
		break;

	case RWS_PAX:
		g_state = update_pax();
		break;

	case RWS_INVENTORY:
		g_state = update_inventory();
		break;

	case RWS_DIALOG:
		g_state = update_dialog();
		break;

	case RWS_SKILLS:
		g_state = update_skills();
		break;

	case RWS_ROM:
		g_state = update_rom();
		break;

	case RWS_DISK_OPTIONS:
		g_state = update_disk_options();
		break;

	case RWS_BODY_PARTS_SHOP:
		g_state = update_parts_shop();
		break;

	case RWS_RELOAD_LEVEL:
		if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			if (g_load_level_vm)
			{
				roompos_init();
				g_load_level_vm = 0;
			}

			deinit();

			if (g_load_game)
			{
				g_load_game = 0;
				g_screen_fading_data.direction = FADE_IN;
				window_animation_setup(WA_TYPE_SCREEN_FADING, &g_screen_fading_data);
				g_screen_fading_data.direction = FADE_OUT;
			}
			else
			{
				level_transition_prepare_anim(0);
			}

			init();
			g_state = RWS_FADE_IN;
		}
		break;

	default:
		break;
	}

	return scene;
}

void setup_real_world_scene()
{
	g_scene.id = NSID_REAL_WORLD;
	g_scene.init = init;
	g_scene.deinit = deinit;
	g_scene.handle_input = handle_input;
	g_scene.update = update;

	g_state = RWS_FADE_IN;
}
