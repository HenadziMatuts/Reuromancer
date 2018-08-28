#include "data.h"
#include "resource_manager.h"
#include "scene_real_world.h"
#include "drawing_control.h"
#include "neuro_menu_control.h"
#include "window_animation.h"
#include <neuro_routines.h>

typedef enum skills_state_t {
	SS_OPEN_SKILLS = 0,
	SS_NO_SKILLS_WFI,
	SS_CLOSE_SKILLS,

	SS_IDLE,
} skills_state_t;

static skills_state_t g_state = SS_OPEN_SKILLS;
static uint16_t g_skills[16] = { 0x00, };

static skills_state_t handle_pax_wait_for_input(skills_state_t state, sfEvent *event)
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
		g_state = handle_pax_wait_for_input(g_state, event);
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

	if (!skills_prepare())
	{
		/* play track 6 */
		neuro_menu_draw_text("No skills.", 0, 2);
		return SS_NO_SKILLS_WFI;
	}

	return SS_IDLE;
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
				neuro_menu_flush();
				g_state = SS_OPEN_SKILLS;
				return RWS_NORMAL;
			}
		}
		break;
	}

	return RWS_SKILLS;
}
