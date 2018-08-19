#include "neuro_routines.h"
#include <string.h>

int bg_animation_init_tables(bg_animation_control_table_t *tables, uint8_t *decompd_anh)
{
	uint8_t *p = decompd_anh;
	anh_hdr_t *hdr = (anh_hdr_t*)p;
	anh_entry_hdr_t *entry_hdr;

	p += sizeof(anh_hdr_t);
	entry_hdr = (anh_entry_hdr_t*)p;

	for (uint32_t u = 0; u < hdr->anh_entries; u++)
	{
		bg_animation_control_table_t *table = &tables[u];
		memset(table, 0, sizeof(bg_animation_control_table_t));

		table->total_frames = entry_hdr->total_frames;
		table->first_frame_data = (uint8_t*)entry_hdr + sizeof(anh_entry_hdr_t);
		table->first_frame_bytes = table->first_frame_data +
			(table->total_frames * sizeof(anh_frame_data_t));
		table->sleep = *(uint16_t*)(table->first_frame_data);
		table->curr_frame = 0;

		p += (entry_hdr->entry_size + 2);
		entry_hdr = (anh_entry_hdr_t*)p;
	}

	return hdr->anh_entries;
}

void bg_animation_update(bg_animation_control_table_t *tables,
		uint16_t animations, uint8_t *working_area, uint8_t *bg_pixels)
{
	for (uint32_t u = 0; u < animations; u++)
	{
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
