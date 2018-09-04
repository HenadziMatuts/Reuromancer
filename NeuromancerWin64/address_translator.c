#include "address_translator.h"
#include "data.h"
#include <assert.h>

typedef enum segments_t {
	SEG_000 = 0,
	SEG_001, SEG_002, SEG_003, SEG_004,
	SEG_005, SEG_006, SEG_007, SEG_008,
	SEG_009, SEG_010, SEG_011, SEG_012,
	SEG_013, SEG_014, SEG_015, SEG_016,
	DSEG, SEG_TOTAL
} segments_t;

typedef struct adderss_translator_helper_t {
	uint16_t segment;
	uint16_t offset;
	uint16_t size;
	uint8_t *data_ptr;
} adderss_translator_helper_t;

static adderss_translator_helper_t g_helper[SEG_TOTAL] = {
	{ 0x020E, 0x00, 0xFD0B, NULL },
	{ 0x11DE, 0x0B, 0x024F, NULL },
	{ 0x1203, 0x0A, 0x0416, NULL },
	{ 0x1245, 0x00, 0x0320, NULL },
	{ 0x1277, 0x00, 0x06C0, (uint8_t*)&g_seg004 },
	{ 0x12E3, 0x00, 0x0400, NULL },
	{ 0x1323, 0x00, 0x5DD0, NULL },
	{ 0x1900, 0x00, 0x1C30, NULL },
	{ 0x1AC3, 0x00, 0x04B0, NULL },
	{ 0x1B0E, 0x00, 0x0190, (uint8_t*)&g_seg009 },
	{ 0x1B27, 0x00, 0x7D40, (uint8_t*)&g_seg010 },
	{ 0x22FB, 0x00, 0xA410, (uint8_t*)&g_seg011 },
	{ 0x2D3C, 0x00, 0x1610, (uint8_t*)&g_seg012 },
	{ 0x2E9D, 0x00, 0xF230, (uint8_t*)&g_seg013 },
	{ 0x3DC0, 0x00, 0x0260, (uint8_t*)&g_seg014 },
	{ 0x3DE6, 0x00, 0x42A0, (uint8_t*)&g_seg015 },
	{ 0x4210, 0x00, 0x5DA0, (uint8_t*)&g_seg016 },
	{ 0x47EA, 0x00, 0x0000, NULL }
};

uint8_t* translate_x16_to_x64(uint16_t seg, uint16_t offt)
{
	segments_t sgt;

	switch (seg) {
	case 0x1277: /* seg004 */
		sgt = SEG_004;
		break;
	case 0x1B0E: /* seg009 */
		sgt = SEG_009;
		break;
	case 0x1B27: /* seg010 */
		sgt = SEG_010;
		break;
	case 0x22FB: /* seg011 */
		sgt = SEG_011;
		break;
	case 0x2D3C: /* seg012 */
		sgt = SEG_012;
		break;
	case 0x2E9D: /* seg013 */
		sgt = SEG_013;
		break;
	case 0x3DC0: /* seg014 */
		sgt = SEG_014;
		break;
	case 0x3DE6: /* seg015 */
		sgt = SEG_015;
		break;
	case 0x4210: /* seg016 */
		sgt = SEG_016;
		break;

	default:
		assert(0);
	}

	return (g_helper[sgt].data_ptr + offt) - g_helper[sgt].offset;
}

void translate_x64_to_x16(uint8_t *src, uint16_t *seg, uint16_t *offt)
{
	int sgt;

	for (sgt = SEG_000; sgt < SEG_TOTAL; sgt++)
	{
		if (!g_helper[sgt].data_ptr)
		{
			continue;
		}

		if (src >= g_helper[sgt].data_ptr &&
			src < g_helper[sgt].data_ptr + g_helper[sgt].size)
		{
			break;
		}
	}

	assert(sgt != SEG_TOTAL);

	*seg = g_helper[sgt].segment;
	*offt = (uint16_t)(src - g_helper[sgt].data_ptr) + g_helper[sgt].offset;
}
