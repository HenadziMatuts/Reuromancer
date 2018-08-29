#include "data.h"
#include "resource_manager.h"
#include "scene_real_world.h"
#include "drawing_control.h"
#include "neuro_menu_control.h"
#include "window_animation.h"
#include "items.h"
#include <neuro_routines.h>
#include <string.h>

typedef enum skills_state_t {
	SS_OPEN_SKILLS = 0,
	SS_NO_SKILLS_WFI,
	SS_SKILLS_PAGE,
	SS_CLOSE_SKILLS,
} skills_state_t;

static skills_state_t g_state = SS_OPEN_SKILLS;
static uint16_t g_skills_total = 0;
static uint16_t g_skills[16] = { 0x00, };

static skills_state_t skills_page(int next)
{
	static int listed = 0;

	neuro_menu_flush_items();

	if (next == 0)
	{
		listed = 0;
	}

	if (g_skills_total > 4)
	{
		neuro_menu_draw_text("more", 12, 5);
		neuro_menu_add_item(12, 5, 4, 10, 'm');
	}

	neuro_menu_draw_text("exit", 7, 5);
	neuro_menu_add_item(7, 5, 4, 11, 'x');

	for (int i = 0; i < 4; i++)
	{
		char skill_string[25] = { 0, };
		sprintf(skill_string, "%24s", " ");

		if (listed < g_skills_total)
		{
			skill_string[0] = i + '1';
			skill_string[1] = '.';

			char *skill_name = get_item_name(g_skills[listed], NULL);
			memmove(skill_string + 3, skill_name, strlen(skill_name));

			int skill_index = g_skills[listed] - 0x43;
			int skill_level = g_3f85.skills[skill_index];
			char skill_level_string[3] = { 0, };

			sprintf(skill_level_string, "%2d", skill_level + 1);
			memmove(skill_string + 21, skill_level_string, 2);

			neuro_menu_add_item(0, i + 1, 24, i, i + '1');
			listed++;
		}

		neuro_menu_draw_text(skill_string, 0, i + 1);
	}

	listed %= g_skills_total;

	return SS_SKILLS_PAGE;
}

static skills_state_t on_skills_page_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0x0A: /* more */
		return skills_page(1);

	case 0x0B: /* exit */
		return SS_CLOSE_SKILLS;

	default:
		break;
	}

	return SS_SKILLS_PAGE;
}

void skills_menu_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case SS_SKILLS_PAGE:
		*state = on_skills_page_menu_button(button);
		break;

	default:
		break;
	}
}

static skills_state_t handle_skills_wait_for_input(skills_state_t state, sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		switch (state) {
		case SS_NO_SKILLS_WFI:
			return SS_CLOSE_SKILLS;

		default:
			return state;
		}
	}

	return state;
}

void handle_skills_input(sfEvent *event)
{
	switch (g_state) {
	case SS_NO_SKILLS_WFI:
		g_state = handle_skills_wait_for_input(g_state, event);
		break;

	case SS_SKILLS_PAGE:
		neuro_menu_handle_input(NMID_SKILLS_MENU, &g_neuro_menu, (int*)&g_state, event);
		break;

	default:
		break;
	}
}

static int skills_prepare()
{
	int j = 0;

	for (int i = 0; i < 16; i++)
	{
		if (g_3f85.skills[i] == 0xFF)
		{
			continue;
		}

		g_skills[j++] = i + 0x43;
	}

	return j;
}

static skills_state_t skills_show()
{
	neuro_menu_draw_frame(6, 7, 17, 25, 6, NULL);
	neuro_menu_draw_text("SKILLS", 9, 0);
	neuro_menu_flush_items();

	g_skills_total = skills_prepare();
	if (!g_skills_total)
	{
		/* play track 6 */
		neuro_menu_draw_text("No skills.", 0, 2);
		return SS_NO_SKILLS_WFI;
	}

	return skills_page(0);
}

static window_folding_frame_data_t g_close_frame_data[12] = {
	{ 216, 64, 48, 128 }, { 216, 62,  48, 129 }, { 216, 56,  48, 132 },
	{ 216, 48, 48, 136 }, { 216, 36,  48, 142 }, { 216, 20,  48, 150 },
	{ 210,  2, 51, 159 }, { 202,  2,  55, 159 }, { 188,  2,  62, 159 },
	{ 162,  2, 75, 159 }, { 108,  2, 102, 159 }, {   2,  2, 155, 159 },

}, g_open_frame_data[12] = {
	{   2,  2, 155, 159 }, {   6,  2, 153, 159 }, {  14,  2, 149, 159 },
	{  28,  2, 142, 159 }, {  54,  2, 129, 159 }, { 108,  2, 102, 159 },
	{ 216,  3,  48, 159 }, { 216,  5,  48, 157 }, { 216, 11,  48, 154 },
	{ 216, 21,  48, 149 }, { 216, 33,  48, 143 }, { 216, 64,  48, 128 }
};

static window_folding_data_t g_skills_anim_data = {
	.total_frames = 12,
	.frame_cap = 35,
	.pixels = g_seg011,
};

real_world_state_t update_skills()
{
	static int anim = 0;

	switch (g_state) {
	case SS_OPEN_SKILLS:
	case SS_CLOSE_SKILLS:
		if (!anim)
		{
			anim = 1;
			g_skills_anim_data.frame_data = (g_state == SS_OPEN_SKILLS) ?
				g_open_frame_data : g_close_frame_data;
			g_skills_anim_data.sprite_chain_index = (g_state == SS_OPEN_SKILLS) ?
				g_4bae.frame_sc_index : g_4bae.frame_sc_index + 1;
			window_animation_setup(WA_TYPE_WINDOW_FOLDING, &g_skills_anim_data);
		}
		else if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			anim = 0;
			if (g_state == SS_OPEN_SKILLS)
			{
				g_state = skills_show();
			}
			else
			{
				neuro_menu_destroy();
				g_state = SS_OPEN_SKILLS;
				return RWS_NORMAL;
			}
		}
		break;
	}

	return RWS_SKILLS;
}
