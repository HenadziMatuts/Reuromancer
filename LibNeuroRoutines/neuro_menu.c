#include "neuro_routines.h"
#include <string.h>

void build_text_frame(uint32_t _h, uint32_t _w, imh_hdr_t *dst)
{
	uint8_t *p = (uint8_t*)dst + sizeof(imh_hdr_t);
	uint32_t w = _w / 2, h = _h;

	memset(dst, 0, sizeof(imh_hdr_t));
	dst->height = _h;
	dst->width = _w / 2;

	for (uint32_t u = 0; u < h; u++, p += w)
	{
		if (u == 0 || u == h - 1)
		{
			memset(p, 0x00, w);
		}
		else
		{
			memset(p, 0xFF, w);
			p[0] = 0x0F;
			p[w - 1] = 0xF0;
		}
	}
}

void build_neuro_menu_frame(neuro_menu_t *dialog,
		uint16_t left, uint16_t top, uint16_t w, uint16_t h,
		uint16_t flags, uint8_t *pixels)
{
	dialog->flags = flags;

	dialog->left = left;
	dialog->top = top;
	dialog->right = left + w - 1;
	dialog->bottom = top + h - 1;

	dialog->inner_left = dialog->left + 8;
	dialog->inner_top = dialog->top + 8;
	dialog->inner_right = dialog->right - 8;
	dialog->inner_bottom = dialog->bottom - 8;

	memmove(&dialog->_inner_left,
		&dialog->inner_left, sizeof(uint16_t) * 4);

	dialog->items_count = 0;

	dialog->width = w / 2;
	dialog->pixels = pixels;

	build_text_frame(h, w, (imh_hdr_t*)pixels);
}

void build_neuro_menu_text(neuro_menu_t *dialog,
		char *text, uint16_t x_offt, uint16_t y_offt)
{
	uint16_t w = dialog->width * 2;
	uint16_t h = dialog->bottom - dialog->top + 1;
	uint16_t l = dialog->inner_left - dialog->left + x_offt;
	uint16_t t = dialog->inner_top - dialog->top + y_offt;
	uint8_t *p = (uint8_t*)dialog->pixels;

	build_string(text, w, h, l, t, p + sizeof(imh_hdr_t));
}

void build_neuro_menu_item(neuro_menu_t *dialog,
		uint16_t x_offt, uint16_t y_offt, uint16_t w,
		uint16_t item_num, char c)
{
	if (item_num > 15)
	{
		return;
	}

	neuro_button_t *item = &dialog->items[dialog->items_count];

	memset(item, 0, sizeof(neuro_button_t));

	item->left = dialog->inner_left + x_offt;
	item->top = dialog->inner_top + y_offt;
	item->right = item->left + w - 1;
	item->bottom = item->top + 7;
	item->code = item_num;
	item->label = c;

	dialog->items_count++;
}