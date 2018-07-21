#include "globals.h"
#include "resource_manager.h"
#include <string.h>

static uint32_t g_animations = 0;
static bg_animation_control_table_t g_animation_control[16];

void bg_animation_control_init_tables(uint8_t *decompd_anh)
{
	if (!decompd_anh)
	{
		g_animations = 0;
		return;
	}

	uint8_t *p = decompd_anh;
	anh_hdr_t *hdr = (anh_hdr_t*)p;
	anh_entry_hdr_t *entry_hdr;

	p += sizeof(anh_hdr_t);
	entry_hdr = (anh_entry_hdr_t*)p;

	memset(&g_animation_control, 0, sizeof(bg_animation_control_table_t) * 16);
	g_animations = hdr->anh_entries;

	for (uint32_t u = 0; u < g_animations; u++)
	{
		g_animation_control[u].total_frames = entry_hdr->total_frames;
		g_animation_control[u].first_sleep_offt = (uint8_t*)entry_hdr + sizeof(anh_entry_hdr_t);
		g_animation_control[u].first_frame_offt =
			g_animation_control[u].first_sleep_offt + (g_animation_control[u].total_frames << 2);
		g_animation_control[u].sleep = *(uint16_t*)(g_animation_control[u].first_sleep_offt);
		g_animation_control[u].curr_frame = 0;

		p += (entry_hdr->entry_size + 2);
		entry_hdr = (anh_entry_hdr_t*)p;
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

	for (uint32_t u = 0; u < g_animations; u++)
	{
		bg_animation_control_table_t *anim = &g_animation_control[u];

		if (anim->sleep-- == 0)
		{
			uint16_t frame_offt = *(uint16_t*)(anim->first_sleep_offt + (anim->curr_frame * 4) + 2);
			uint8_t *frame = anim->first_frame_offt + frame_offt;

			uint16_t frame_len = frame[2] * frame[3];
			decode_rle(frame + 4, frame_len, g_seg011);

			uint16_t bg_offt = (frame[1] * 152) + frame[0] + 0xFB4E;
			uint16_t bg_skip = -1 * (frame[2] - 152);

			uint8_t *p1 = g_seg011, *p2 = g_level_bg + bg_offt - 10;

			for (uint16_t i = frame[3]; i != 0; i--)
			{
				for (uint16_t j = frame[2]; j != 0; j--)
				{
					*p2++ ^= *p1++;
				}

				p2 += bg_skip;
			}

			if (++anim->curr_frame == anim->total_frames)
			{
				anim->curr_frame = 0;
			}

			anim->sleep = *(uint16_t*)(anim->first_sleep_offt + (anim->curr_frame * 4));
		}
	}
}

