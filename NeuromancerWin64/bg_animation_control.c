#include <neuro_routines.h>
#include "globals.h"
#include "resource_manager.h"
#include "data.h"
#include <string.h>

/* 0x005A */
static uint16_t g_animations = 0;

static uint16_t g_005c = 0;

/* 0x6A42 */
static bg_animation_control_table_t g_animation_control[16];

static uint16_t g_a54c[4] = { 0, };

void bg_animation_control_prepare(uint16_t i, uint16_t v)
{
	g_a54c[i] = v;
	g_005c = 8;
}

void bg_animation_control_init_tables(uint8_t *decompd_anh)
{
	for (int i = 0; i < 4; i++)
	{
		g_a54c[i] = 0;
	}

	if (!decompd_anh)
	{
		memset(g_animation_control, 0, 16 * sizeof(bg_animation_control_table_t));
		g_animations = 0;
	}
	else
	{
		g_animations = bg_animation_init_tables(g_animation_control, decompd_anh);
		g_005c = 8;
	}
}

static void _bg_animation_update(bg_animation_control_table_t *tables,
	uint16_t animations, uint8_t *working_area, uint8_t *bg_pixels)
{
	for (uint32_t u = 0; u < animations; u++)
	{
		uint16_t x = g_a54c[u] & 0xC000;
		if (x == 0xC000)
		{
			continue;
		}

		bg_animation_control_table_t *anim = &tables[u];

		if (anim->sleep-- == 0)
		{
			anh_frame_data_t *data = (anh_frame_data_t*)(anim->first_frame_data) + anim->curr_frame;
			uint8_t *frame = anim->first_frame_bytes + data->frame_offset;
			anh_frame_hdr *hdr = (anh_frame_hdr*)frame;

			uint16_t frame_len = hdr->frame_width * hdr->frame_height;
			decode_rle(frame + sizeof(anh_frame_hdr), frame_len, working_area);

			uint16_t bg_offt = (hdr->bg_y_offt * 152) + hdr->bg_x_offt + 0xFB3C; //0xFB4E;
			uint16_t bg_skip = 152 - hdr->frame_width;

			uint8_t *p1 = working_area, *p2 = bg_pixels + bg_offt;

			/* apply frame */
			for (uint16_t i = hdr->frame_height; i != 0; i--)
			{
				for (uint16_t j = hdr->frame_width; j != 0; j--)
				{
					*p2++ ^= *p1++;
				}

				p2 += bg_skip;
			}

			if (++anim->curr_frame == anim->total_frames)
			{
				anim->curr_frame = 0;

				if (x == 0x8000)
				{
					g_a54c[u] = 0xC000;
				}
				if (x == 0x4000)
				{
					anim->curr_frame = g_a54c[u] & 0x3FFF;
					
				}
			}

			data = (anh_frame_data_t*)(anim->first_frame_data) + anim->curr_frame;
			anim->sleep = data->frame_sleep;
		}
	}
}

void bg_animation_control_update()
{
	static int frame_cap_ms = 55;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (g_005c)
	{
		g_005c--;
	}
	else
	{
		if (passed - elapsed <= frame_cap_ms)
		{
			return;
		}
		elapsed = passed;
	}
	
	_bg_animation_update(g_animation_control, g_animations, g_seg011.data, g_seg015.pixels);
}
