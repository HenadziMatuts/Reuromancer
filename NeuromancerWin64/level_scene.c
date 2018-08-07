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

typedef enum level_state_t {
	LS_INTRO = 0,
	LS_DIALOG_WAIT_FOR_INPUT,
	LS_NORMAL,
} level_state_t;

static char *g_bih_string_ptr = NULL;
static level_state_t g_state = LS_NORMAL;

/***************************************/
static int sub_147EE(uint16_t opcode, ...);

static void sub_14DBA(char *text)
{
	switch (g_c91e.mode) {
	case 0: {
		g_state = LS_INTRO;
		break;
	}

	case 8: {
		imh_hdr_t *imh = (imh_hdr_t*)g_seg011;
		build_string(text, imh->width * 2, imh->height, 8, 8, g_seg011 + sizeof(imh_hdr_t));
		drawing_control_add_sprite_to_chain(g_4bae.x4ccf--, 0, g_c91e.top, g_seg011, 1);
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

	sub_147EE(opcode, lines);
	sub_14DBA(text);
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

			g_state = LS_DIALOG_WAIT_FOR_INPUT;

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

	sub_14DBA(g_bih_string_ptr);

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
static int setup_ui_button(void *area)
{
	switch (g_c91e.mode)
	{
	case 0:
		g_c91e_wrapper.window_item[g_c91e.total_items++] = area;
		break;

	default:
		break;
	}

	return 0;
}

/* sub_156D4 */
static int setup_ui_buttons()
{
	assert((g_c91e.mode == 0) || (g_c91e.mode > 2 && g_c91e.mode <= 4));
	g_c91e.total_items = 0;

	setup_ui_button(&g_ui_buttons.inventory);
	setup_ui_button(&g_ui_buttons.dialog);

	if (has_pax())
	{
		setup_ui_button(&g_ui_buttons.pax);
	}

	setup_ui_button(&g_ui_buttons.skills);
	setup_ui_button(&g_ui_buttons.chip);
	setup_ui_button(&g_ui_buttons.disk);
	setup_ui_button(&g_ui_buttons.date);
	setup_ui_button(&g_ui_buttons.time);
	setup_ui_button(&g_ui_buttons.cash);
	setup_ui_button(&g_ui_buttons.con);

	return 0;
}

/* Setup "Window" */
static int sub_147EE(uint16_t opcode, ...)
{
	memmove(g_a59e.a5ee, g_a59e.a5c6, 40);
	memmove(g_a59e.a5c6, g_a59e.a59e, 40);
	memmove(g_a59e.a59e, &g_c91e, 40);

	g_c91e.mode = opcode;
	g_c91e.total_items = 0;

	switch (opcode) {
	/* UI "window" */
	case 0:
		setup_ui_buttons();

		g_c91e.left = 0;
		g_c91e.top = 0;
		g_c91e.width = 319;
		g_c91e.height = 199;
		g_c91e.c944 = 160;
		break;

	case 8: {
		/* Dialog "window" */
		va_list args;
		va_start(args, opcode);
		uint16_t lines = va_arg(args, uint16_t);
		va_end(args);

		g_c91e.left = 0;
		g_c91e.top = 4;
		g_c91e.width = 319;
		g_c91e.height = (lines * 8) + 19;
		g_c91e.c928 = lines;
		g_c91e.c944 = 160;

		if (g_4bae.ui_type == 0)
		{
			if (g_c91e.mode == 1)
			{
				if (g_4bae.roompos_spawn_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_c91e.height + 1, g_dialog_bubbles + 0x1A2, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_c91e.height + 1, g_dialog_bubbles + 0xDC, 0);
				}
			}
			else
			{
				if (g_4bae.roompos_spawn_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_c91e.height + 1, g_dialog_bubbles + 0x6E, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_c91e.height + 1, g_dialog_bubbles, 0);
				}
			}
		}

		build_text_frame(g_c91e.height + 1, g_c91e.width + 1, (imh_hdr_t*)g_seg011);
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

	sub_147EE(0);

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

static void select_ui_button(neuro_ui_button_t *button)
{
	uint8_t *ui_pic = g_background + sizeof(imh_hdr_t);
	uint8_t *p = ui_pic +
		((button->left - g_c91e.left) / 2) +
		((button->top - g_c91e.top) * g_c91e.c944);

	uint16_t lines = button->bottom - button->top + 1;
	uint16_t width = ((button->right | 1) - (button->left & 0xFE) + 1) / 2;
	uint16_t skip = g_c91e.c944 - width;

	for (uint16_t h = 0; h < lines; h++, p += skip)
	{
		for (uint16_t w = 0; w < width; w++)
		{
			*p++ ^= 0xFF;
		}
	}

	return;
}

static on_ui_button(neuro_ui_button_t *button)
{
	switch (button->code) {
	case 0x0A:
		g_ui_panel_mode = UI_PM_DATE;
		break;

	case 0x0B:
		g_ui_panel_mode = UI_PM_TIME;
		break;

	case 0x0C:
		g_ui_panel_mode = UI_PM_CASH;
		break;

	case 0x0D:
		g_ui_panel_mode = UI_PM_CON;
		break;

	default:
		break;
	}
}

static void unselect_ui_button(neuro_ui_button_t *button)
{
	select_ui_button(button);
}

static neuro_ui_button_t* cursor_ui_button_hit_test()
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];
	neuro_ui_button_t *hit =
		(neuro_ui_button_t*)g_c91e_wrapper.window_item[0];

	for (uint16_t u = 0; u < g_c91e.total_items; u++, hit++)
	{
		if (cursor->left > hit->left && cursor->left < hit->right &&
			cursor->top > hit->top && cursor->top < hit->bottom)
		{
			return hit;
		}
	}

	return NULL;
}

static void ui_handle_input(sfEvent *event)
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];
	neuro_ui_button_t *button = NULL;
	static neuro_ui_button_t *selected = NULL; /* selected button */
	static int _selected = 0; /* redrawing flag */

	/* ui area */
	if (cursor->left < 0 || cursor->left > 320 ||
		cursor->top < 140 || cursor->top > 200)
	{
		return;
	}

	if (sfMouse_isButtonPressed(sfMouseLeft))
	{
		if (button = cursor_ui_button_hit_test())
		{
			if ((!selected || (button == selected)) && !_selected)
			{
				select_ui_button(button);
				selected = button;
				_selected = 1;
			}
		}
		else if (selected && _selected)
		{
			unselect_ui_button(selected);
			_selected = 0;
		}
	}
	else if (event->mouseButton.type == sfEvtMouseButtonReleased)
	{
		if (selected)
		{
			if (_selected)
			{
				unselect_ui_button(selected);
				_selected = 0;
			}

			if (selected == cursor_ui_button_hit_test())
			{
				on_ui_button(selected);
			}

			selected = NULL;
		}
	}
}

static void update_normal(sfEvent *event)
{
	character_control_handle_input();
	character_control_update();

	/* execute the following VM instruction */
	sub_105F6(SUB_105F6_OP_NEURO_VM_STEP);

	ui_handle_input(event);
	ui_panel_update();

	bg_animation_control_update();
}

static void wait_for_input()
{
	switch (g_state) {
	case LS_DIALOG_WAIT_FOR_INPUT:
		if (sfMouse_isButtonClicked(sfMouseLeft))
		{
			g_state = LS_NORMAL;
			drawing_control_remove_sprite_from_chain(++g_4bae.x4ccf);
			drawing_control_remove_sprite_from_chain(SCI_DIALOG_BUBBLE);

			/* restoring "window" state */
			memmove(&g_c91e, g_a59e.a59e, 40);
			memmove(g_a59e.a59e, g_a59e.a5c6, 40);
			memmove(g_a59e.a5c6, g_a59e.a5ee, 40);
		}
		break;

	default:
		break;
	}
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

static neuro_scene_id_t update(sfEvent *event)
{
	neuro_scene_id_t scene = NSID_LEVEL;

	update_cursor();

	switch (g_state)
	{
	case LS_INTRO:
		update_intro();
		break;

	case LS_DIALOG_WAIT_FOR_INPUT:
		wait_for_input();
		break;

	case LS_NORMAL:
		update_normal(event);
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
