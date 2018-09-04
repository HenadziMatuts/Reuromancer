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
