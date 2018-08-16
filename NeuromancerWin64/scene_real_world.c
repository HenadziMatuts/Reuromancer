#include "data.h"
#include "globals.h"
#include "scene_control.h"
#include "character_control.h"
#include "bg_animation_control.h"
#include "drawing_control.h"
#include "resource_manager.h"
#include "neuro_window_control.h"
#include "scene_real_world.h"
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

static real_world_state_t g_state = RWS_TEXT_OUTPUT;

static char *g_bih_string_ptr = NULL;

/***************************************/
static uint64_t sub_105F6(real_world_state_t *state, uint16_t opcode, ...);

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
		text_output_set_text(g_bih_string_ptr);
		return;
	}

	uint16_t temp_1 = g_4bae.roompos_x;
	uint16_t temp_2 = g_4bae.roompos_y;

	g_4bae.roompos_x = c * 2;
	g_4bae.roompos_y = d;

	/* builds dialog bubble with text */
	sub_1342E(g_bih_string_ptr, NWM_NPC_DIALOG_REPLY);

	g_4bae.roompos_x = temp_1;
	g_4bae.roompos_y = temp_2;

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
static void neuro_vm(real_world_state_t *state)
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

			*state = RWS_WAIT_FOR_INPUT;

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
				*state = RWS_TEXT_OUTPUT;
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

		/* load level */
		case 0x10: {
			g_load_level_vm = 1;
			uint8_t level = *(uint8_t*)(opcode_addr + 1);
			g_4bae.level_n = level;
			*state = RWS_RELOAD_LEVEL;
			break;
		}

		case 0x11:
			g_update_hold++;
			g_3f85_wrapper.vm_next_op_addr[vm_thread]++;
			break;

		case 0x12:
			g_update_hold = 0;
			g_3f85_wrapper.vm_next_op_addr[vm_thread]++;
			break;

		/* set dialog ctrl */
		case 0x13: {
			uint16_t level_n = *(opcode_addr + 1);
			g_3f85.level_info[level_n].first_dialog_reply = *(opcode_addr + 2);
			g_3f85.level_info[level_n].total_dialog_replies = *(opcode_addr + 3);
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
			*state = RWS_DIALOG;
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

	text_output_set_text(g_bih_string_ptr);

	return 0;
}

/* 0xBCF */
void init_deinit_neuro_cb(int cmd)
{
	switch (cmd) {
	case 0: { /* reset vm state */
		for (int i = 0; i < 4; i++)
		{
			uint16_t n = g_a8e0.a8e0[i];
			if (n == 0xffff) {
				continue;
			}

			g_3f85.vm_state[n].mark = 0xFF;
		}
		break;
	}

	default:
		break;
	}
}

void init_deinit_level_2(uint16_t offt)
{
	switch (offt) {
	case 0x16F: {
		x4bae_t *ctl = (x4bae_t*)g_bih_wrapper.ctrl_struct_addr;
		int enough_cash = (ctl->cash - 250 < 0) ? 0 : 1;
		
		ctl->x4c7c = enough_cash;
		uint8_t day = ctl->date_day + 1;

		ctl->x4bcd[33]++;
		if (ctl->x4bcd[33] == 4)
		{
			ctl->x4c59[1] = day;
		}
		else if (ctl->x4bcd[33] == 1)
		{
			ctl->x4c59[0] = day;
		}

		break;
	}

	case 0x1AF: {
		x4bae_t *ctl = (x4bae_t*)g_bih_wrapper.ctrl_struct_addr;
		ctl->x4bcd[30] = 1;

		g_bih_wrapper.init_deinit_cb(0);
		break;
	}

	default:
		break;
	}
}

void init_deinit_level_1(uint16_t offt)
{
	switch (offt) {
	case 0x3F:
		return;

	case 0x40:
		g_bih_wrapper.init_deinit_cb(0);
		return;
	
	default:
		break;
	}
}

void init_deinit_level_0(uint16_t offt)
{
	switch (offt) {
	case 0xFB:
	case 0xFC:
		return;

	default:
		break;
	}
}

void init_deinit_bih_call(uint16_t offt)
{
	switch (g_level_n) {
	case 0:
		init_deinit_level_0(offt);
		break;

	case 1:
		init_deinit_level_1(offt);
		break;

	case 2:
		init_deinit_level_2(offt);
		break;

	default:
		break;
	}
}

typedef enum sub_105f6_opcodes_t {
	SUB_105F6_OP_PLAY_LEVEL_INTRO = 5,
	SUB_105F6_OP_NEURO_VM_STEP,
} sub_105f6_opcodes_t;

static uint64_t sub_105F6(real_world_state_t *state, uint16_t opcode, ...)
{
	switch (opcode) {
	case 0: {
		va_list args;
		va_start(args, opcode);
		uint16_t level_n = va_arg(args, uint16_t);
		va_end(args);

		g_a642 = &g_3f85.level_info[level_n];
		g_bih_wrapper.bih = (bih_hdr_t*)g_a8e0.bih;
		g_bih_wrapper.ctrl_struct_addr = (uint8_t*)&g_4bae;
		g_bih_wrapper.init_deinit_cb = init_deinit_neuro_cb;
		break;
	}

	case 1: /* enter/exit level bih call */
	case 3: {
		uint16_t offt = g_bih_wrapper.bih->init_obj_code_offt[opcode - 1];
		init_deinit_bih_call(offt);
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
		*state = RWS_TEXT_OUTPUT;
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

static void roompos_init()
{
	roompos_t *roompos = (roompos_t*)g_roompos;
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

	assert(resource_manager_load_resource("NEURO.IMH", g_background));
	drawing_control_add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_background, 1);

	neuro_window_setup(0);

	if (g_level_n >= 0 && g_4bae.x4bcc == 0)
	{
		sub_105F6(NULL, 3);
	}

	g_4bae.x4bcc = 0;

	g_4bae.x4cc3 = 0;
	g_4bae.active_dialog_reply = 0xFF;
	g_4bae.x4bbf = 0xFF;
	g_4bae.active_item = 0xFFFF;
	g_4bae.cash_withdrawal = 0xFFFFFFFF;
	g_update_hold = 0;
	g_4bae.x4bf4 = 0xFF;

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
	sub_105F6(&g_state, SUB_105F6_OP_PLAY_LEVEL_INTRO);

	g_exit_point = -1;

	ui_panel_update();
	return;
}

static void deinit()
{
	drawing_control_remove_sprite_from_chain(SCI_LEVEL_BG);
	drawing_control_remove_sprite_from_chain(SCI_BACKGRND);
	drawing_control_remove_sprite_from_chain(SCI_CHARACTER);
	g_bih_string_ptr = NULL;
}

void rw_ui_handle_button_press(int *state, neuro_button_t *button)
{
	switch (button->code) {
	case 0x00: /* inventory */
		*state = RWS_INVENTORY;
		break;

	case 0x02: /* dialog */
		g_dialog_escapable = 1;
		*state = RWS_DIALOG;
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

static void handle_wait_for_input(sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		switch (g_neuro_window.mode) {
		case NWM_NPC_DIALOG_REPLY:
			drawing_control_remove_sprite_from_chain(++g_4bae.x4ccf);
			drawing_control_remove_sprite_from_chain(SCI_DIALOG_BUBBLE);
			restore_window();
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
	case RWS_TEXT_OUTPUT:
		handle_text_output_input(event);
		break;

	case RWS_NORMAL:
		handle_normal_input(event);
		break;

	case RWS_INVENTORY:
		handle_inventory_input(event);
		break;

	case RWS_DIALOG:
		handle_dialog_input(event);
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
	real_world_state_t state = RWS_NORMAL;
	character_dir_t dir = CD_NULL;

	ui_panel_update();
	dir = character_control_update();
	bg_animation_control_update();

	/* execute the following VM instruction */
	sub_105F6(&state, SUB_105F6_OP_NEURO_VM_STEP);

	if (!g_update_hold)
	{
		if (g_load_level_vm)
		{
			roompos_init();
			g_load_level_vm = 0;
		}
		else if (dir != CD_NULL)
		{
			g_exit_point = (int16_t)dir;
			int level = roompos_hit_exit_zone();
			if (level != -1)
			{
				/* reload level */
				state = RWS_RELOAD_LEVEL;
				g_4bae.level_n = level;
			}
		}
	}

	return state;
}

static neuro_scene_id_t update()
{
	neuro_scene_id_t scene = NSID_REAL_WORLD;

	update_cursor();

	switch (g_state) {
	case RWS_TEXT_OUTPUT:
		g_state = update_text_output();
		break;

	case RWS_NORMAL:
		g_state = update_normal();
		break;

	case RWS_INVENTORY:
		g_state = update_inventory();
		break;

	case RWS_DIALOG:
		g_state = update_dialog();
		break;

	case RWS_RELOAD_LEVEL:
		deinit();
		init();
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

	g_state = RWS_TEXT_OUTPUT;
}
