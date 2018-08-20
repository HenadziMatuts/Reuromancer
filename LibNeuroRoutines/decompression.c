#include "neuro_routines.h"
#include <string.h>

static void xor_rows(uint8_t *inout, uint32_t w, uint32_t h)
{
	uint8_t *p = inout;

	for (uint32_t i = 0; i < h - 1; i++)
	{
		for (uint32_t j = 0; j < w; j++)
		{
			p[((i + 1)*w) + j] ^= p[(i*w) + j];
		}
	}
}

int decode_rle(uint8_t *_src, uint32_t len, uint8_t *_dst)
{
	uint8_t *src = _src, *dst = _dst, *p = _dst;
	uint32_t processed = 0;

	while (len)
	{
		if (*src > 0x7F)
		{
			int i = 0x100 - *src++;
			processed++;

			while (i--)
			{
				*dst++ = *src++;
				len--;
				processed++;
			}
		}
		else
		{
			int num = *src++, val = *src++;
			processed += 2;

			memset(dst, val, (size_t)++num);
			dst += num;
			len -= num;
		}
	}

	return processed;
}

static int decode_imh(uint8_t *_src, uint32_t len, uint8_t *_dst)
{
	uint32_t total_len = 0;
	uint8_t *src = _src, *dst = _dst;
	imh_hdr_t *imh;

	while (len)
	{
		uint32_t size = 0, processed = 0;

		imh = (imh_hdr_t*)src;
		size = imh->width * imh->height;

		memmove(dst, src, sizeof(imh_hdr_t));

		total_len += sizeof(imh_hdr_t) + size;
		src += sizeof(imh_hdr_t);
		dst += sizeof(imh_hdr_t);
		len -= sizeof(imh_hdr_t);

		processed = decode_rle(src, imh->width * imh->height, dst);
		xor_rows(dst, imh->width, imh->height);
		src += processed;
		len -= processed;
		dst += size;
	}

	return total_len;
}

int decompress_anh(uint8_t *src, uint8_t *dst)
{
	return huffman_decompress(src, dst);
}

int decompress_bih(uint8_t *src, uint8_t *dst)
{
	return huffman_decompress(src, dst);
}

int decompress_imh(uint8_t *src, uint8_t *dst)
{
	uint8_t imh[64000];
	uint32_t len = huffman_decompress(src, imh);

	return decode_imh(imh, len, dst);
}

int decompress_pic(uint8_t *src, uint8_t *dst)
{
	uint8_t pic[64000];

	huffman_decompress(src, pic);
	decode_rle(pic, 152 * 112, dst);
	xor_rows(dst, 152, 112);

	return 152 * 112;
}

int decompress_txh(uint8_t *src, uint8_t *dst)
{
	return huffman_decompress(src, dst);
}
