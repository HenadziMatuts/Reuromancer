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
		g_animation_control[u].first_frame_data = (uint8_t*)entry_hdr + sizeof(anh_entry_hdr_t);
		g_animation_control[u].first_frame_bytes =
			g_animation_control[u].first_frame_data +
			(g_animation_control[u].total_frames * sizeof(anh_frame_data_t));
		g_animation_control[u].sleep = *(uint16_t*)(g_animation_control[u].first_frame_data);
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
			anh_frame_data_t *data = (anh_frame_data_t*)(anim->first_frame_data +
				(anim->curr_frame * sizeof(anh_frame_data_t)));
			uint8_t *frame = anim->first_frame_bytes + data->frame_offset;
			anh_frame_hdr *hdr = (anh_frame_hdr*)frame;

			uint16_t frame_len = hdr->frame_width * hdr->frame_height;
			decode_rle(frame + sizeof(anh_frame_hdr), frame_len, g_seg011);

			uint16_t bg_offt = (hdr->bg_y_offt * 152) + hdr->bg_x_offt + 0xFB4E;
			uint16_t bg_skip = 152 - hdr->frame_width;

			uint8_t *p1 = g_seg011, *p2 = g_level_bg + bg_offt - 10;

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
				data = (anh_frame_data_t*)(anim->first_frame_data);
			}
			else
			{
				data++;
			}

			anim->sleep = data->frame_sleep;
		}
	}
}

