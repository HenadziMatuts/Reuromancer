#include "drawing_control.h"
#include "address_translator.h"
#include <string.h>

/*
* MS-DOS palette.
*/
static uint8_t g_palette[64] = {
	0x00, 0x00, 0x00, 0xFF, // black
	0x00, 0x00, 0xAA, 0xFF, // blue
	0x00, 0xAA, 0x00, 0xFF, // green
	0x00, 0xAA, 0xAA, 0xFF, // cyan
	0xAA, 0x00, 0x00, 0xFF, // red
	0xAA, 0x00, 0xAA, 0xFF, // magnetta
	0xAA, 0x55, 0x00, 0xFF, // brown
	0xAA, 0xAA, 0xAA, 0xFF, // light gray
	0x55, 0x55, 0x55, 0xFF, // dark gray
	0x55, 0x55, 0xFF, 0xFF, // light blue
	0x00, 0xFF, 0x55, 0xFF, // light green
	0x55, 0xFF, 0xFF, 0xFF, // light cyan
	0xFF, 0x55, 0x55, 0xFF, // light red
	0xFF, 0x55, 0xFF, 0xFF, // light magnetta
	0xFF, 0xFF, 0x55, 0xFF, // yellow
	0xFF, 0xFF, 0xFF, 0xFF, // white
};

/*
* Kind of VGA frame buffer.
*/
uint8_t g_vga[320 * 200 * 4];

/*
* Sprite chain (renering queue)
* 0x3BD4
*/
sprite_layer_t g_sprite_chain[SCI_TOTAL];

void drawing_control_add_sprite_to_chain(int n,
		uint32_t left, uint32_t top,
		uint8_t *sprite, int opaque)
{
	sprite_layer_t *layer = NULL;

	if (n > 10)
	{
		return;
	}

	layer = &g_sprite_chain[n];
	memset(layer, 0, sizeof(sprite_layer_t));

	layer->left = left;
	layer->top = top;

	memmove(&layer->sprite_hdr, sprite, sizeof(imh_hdr_t));
	translate_x64_to_x16(sprite + sizeof(imh_hdr_t),
		&layer->pixels_segt, &layer->pixels_offt);

	memmove(&layer->_sprite_hdr, &layer->sprite_hdr,
		sizeof(imh_hdr_t) + (2 * sizeof(uint16_t)));

	layer->update = 1;
	layer->flags = ((opaque << 4) & 16) | 1;
}

void drawing_control_remove_sprite_from_chain(int n)
{
	sprite_layer_t *layer = NULL;
	layer = &g_sprite_chain[n];
	memset(layer, 0, sizeof(sprite_layer_t));
}

void drawing_control_flush_sprite_chain()
{
	memset(g_sprite_chain, 0, sizeof(sprite_layer_t) * SCI_TOTAL);
}

static void draw_to_vga(int left, int top,
	uint32_t w, uint32_t h, uint8_t *pixels, int bg_transparency)
{
	uint32_t cut_x = 0, cut_y = 0;
	uint32_t w_orig = w, h_orig = h;
	uint32_t origin = (top * 320) + left;

	/* invisible */
	if (left >= 320 || top >= 200 ||
		left + w <= 0 || top + h <= 0)
	{
		return;
	}

	if (left + w > 320)
	{
		w = 320 - left;
	}
	if (top + h > 200)
	{
		h = 200 - top;
	}

	if (left < 0)
	{
		cut_x = left * (-1);
	}
	if (top < 0)
	{
		cut_y = top * (-1);
	}

	for (uint32_t i = cut_y; i < h; i++)
	{
		for (uint32_t j = cut_x, k = j; j < w / 2; j++)
		{
			if (!(bg_transparency && (pixels[i * (w_orig / 2) + j] >> 4) == 0))
			{
				memmove(&g_vga[(origin + (i * 320) + k) * 4],
					&g_palette[(pixels[i * (w_orig / 2) + j] >> 4) * 4], 4);
			}
			k++;

			if (!(bg_transparency && (pixels[i * (w_orig / 2) + j] & 0x0F) == 0))
			{
				memmove(&g_vga[(origin + (i * 320) + k) * 4],
					&g_palette[(pixels[i * (w_orig / 2) + j] & 0x0F) * 4], 4);
			}
			k++;
		}
	}
}

static void draw_sprite_to_vga(sprite_layer_t *sprite)
{
	int32_t top = sprite->top - sprite->sprite_hdr.dy;
	int32_t left = sprite->left - sprite->sprite_hdr.dx;
	uint32_t w = sprite->sprite_hdr.width * 2;
	uint32_t h = sprite->sprite_hdr.height;
	uint32_t bg_transparency = ((sprite->flags >> 4) == 0);
	uint8_t *pixels = translate_x16_to_x64(sprite->pixels_segt, sprite->pixels_offt);

	draw_to_vga(left, top, w, h, pixels, bg_transparency);
}

void drawing_control_draw_sprite_chain_to_vga()
{
	for (int i = SCI_TOTAL - 1; i >= 0; i--)
	{
		if (!(g_sprite_chain[i].flags & 1))
		{
			continue;
		}

		draw_sprite_to_vga(&g_sprite_chain[i]);
	}
}

void drawing_control_flush_vga()
{
	memset(g_vga, 0, 320 * 200 * 4);
}
