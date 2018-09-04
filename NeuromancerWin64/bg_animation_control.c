#include <neuro_routines.h>
#include "globals.h"
#include "resource_manager.h"
#include "data.h"
#include <string.h>

static uint32_t g_animations = 0;
static bg_animation_control_table_t g_animation_control[16];

void bg_animation_control_init_tables(uint8_t *decompd_anh)
{
	if (!decompd_anh)
	{
		memset(g_animation_control, 0, 16 * sizeof(bg_animation_control_table_t));
		g_animations = 0;
	}
	else
	{
		g_animations = bg_animation_init_tables(g_animation_control, decompd_anh);
	}
}

void bg_animation_control_update()
{
	static int frame_cap_ms = 55;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return;
	}
	elapsed = passed;

	bg_animation_update(g_animation_control, g_animations, g_seg011.data, g_seg015.pixels);
}
