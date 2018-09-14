#include "address_translator.h"
#include "data.h"
#include <stdio.h>
#include <assert.h>

#define SEGMENTS_TOTAL 18

typedef struct adderss_translator_helper_t {
	uint16_t segment;
	uint16_t offset;
	uint16_t size;
	uint8_t *data_ptr;
} adderss_translator_helper_t;

static adderss_translator_helper_t g_helper[SEGMENTS_TOTAL] = {
	{ SEG_000, 0x00, 0xFD0B, NULL },
	{ SEG_001, 0x0B, 0x024F, NULL },
	{ SEG_002, 0x0A, 0x0416, NULL },
	{ SEG_003, 0x00, 0x0320, NULL },
	{ SEG_004, 0x00, 0x06C0, (uint8_t*)&g_seg004 },
	{ SEG_005, 0x00, 0x0400, NULL },
	{ SEG_006, 0x00, 0x5DD0, NULL },
	{ SEG_007, 0x00, 0x1C30, NULL },
	{ SEG_008, 0x00, 0x04B0, NULL },
	{ SEG_009, 0x00, 0x0190, (uint8_t*)&g_seg009 },
	{ SEG_010, 0x00, 0x7D40, (uint8_t*)&g_seg010 },
	{ SEG_011, 0x00, 0xA410, (uint8_t*)&g_seg011 },
	{ SEG_012, 0x00, 0x1610, (uint8_t*)&g_seg012 },
	{ SEG_013, 0x00, 0xF230, (uint8_t*)&g_seg013 },
	{ SEG_014, 0x00, 0x0260, (uint8_t*)&g_seg014 },
	{ SEG_015, 0x00, 0x42A0, (uint8_t*)&g_seg015 },
	{ SEG_016, 0x00, 0x5DA0, (uint8_t*)&g_seg016 },
	{    DSEG, 0x00, 0x0000, NULL }
};

typedef struct seg_map_t {
	uint16_t offt;
	uint16_t size;
	void *obj_ptr;
} seg_map_t;

static seg_map_t g_seg000_map[] = {
	{ 0x0BCF, 0, neuro_cb },
	{ 0xFFFF, 0, NULL }
};

static seg_map_t g_dseg_map[] = {
	/* ui buttons */
	{ 0x1FA2, 12, &g_ui_buttons.inventory },
	{ 0x1FAE, 12, &g_ui_buttons.pax },
	{ 0x1FBA, 12, &g_ui_buttons.dialog },
	{ 0x1FC6, 12, &g_ui_buttons.skills },
	{ 0x1FD2, 12, &g_ui_buttons.chip },
	{ 0x1FDE, 12, &g_ui_buttons.disk },
	{ 0x1FEA, 12, &g_ui_buttons.date },
	{ 0x1FF6, 12, &g_ui_buttons.time },
	{ 0x2002, 12, &g_ui_buttons.cash },
	{ 0x200E, 12, &g_ui_buttons.constitution },
	/* pax buttons */
	{ 0x201A, 12, &g_pax_buttons.exit },
	{ 0x2026, 12, &g_pax_buttons.user_info },
	{ 0x2032, 12, &g_pax_buttons.banking },
	{ 0x203E, 12, &g_pax_buttons.news },
	{ 0x204A, 12, &g_pax_buttons.board },
	/* pax banking buttons */
	{ 0x2176, 12, &g_pax_banking_buttons.exit },
	{ 0x2182, 12, &g_pax_banking_buttons.download },
	{ 0x218E, 12, &g_pax_banking_buttons.upload },
	{ 0x219A, 12, &g_pax_banking_buttons.transactions },
	/* pax info menu buttons */
	{ 0x21A6, 12, &g_pax_info_menu_buttons.exit },
	{ 0x21B2, 12, &g_pax_info_menu_buttons.more },
	/* pax msg board buttons */
	{ 0x21BE, 12, &g_pax_board_menu_buttons.exit },
	{ 0x21CA, 12, &g_pax_board_menu_buttons.view },
	{ 0x21D6, 12, &g_pax_board_menu_buttons.send },
	/* pax send msg buttons */
	{ 0x21E2, 12, &g_pax_board_send_msg_buttons.yes },
	{ 0x21EE, 12, &g_pax_board_send_msg_buttons.no },
	/* inventory buttons */
	{ 0x21FA, 12, &g_inv_buttons.item_page_exit },
	{ 0x2206, 12, &g_inv_buttons.exit },
	{ 0x2212, 12, &g_inv_buttons.more },
	/* inventory discard buttons */
	{ 0x2236, 12, &g_inv_disc_buttons.yes },
	{ 0x2242, 12, &g_inv_disc_buttons.no },

	{ 0x3F85, 1252, &g_3f85 },
	{ 0x4BAE,  291, &g_4bae },
	{ 0xA8E0, 8244, &g_a8e0 },

	/* stack */
	{ STACK_OFFT, 0x800, g_stack },
	{ 0xFFFF, 0, NULL },
};

static uint8_t* dseg_x16_to_x64(uint16_t offt)
{
	int i = 0;

	while (g_dseg_map[i].offt != 0xFFFF)
	{
		if (offt >= g_dseg_map[i].offt &&
			offt < g_dseg_map[i].offt + g_dseg_map[i].size)
		{
			return (uint8_t*)g_dseg_map[i].obj_ptr + (offt - g_dseg_map[i].offt);
		}

		i++;
	}

	fprintf(stderr, "offt: %d\n", offt);
	assert(0);
	return NULL;
}

static uint8_t* seg000_x16_to_x64(uint16_t offt)
{
	int i = 0;

	while (g_seg000_map[i].offt != 0xFFFF)
	{
		if (offt == g_seg000_map[i].offt)
		{
			return (uint8_t*)g_seg000_map[i].obj_ptr;
		}

		i++;
	}

	assert(0);
	return NULL;
}

uint8_t* translate_x16_to_x64(uint16_t seg, uint16_t offt)
{
	switch (seg) {
	case SEG_000:
		return seg000_x16_to_x64(offt);
	case SEG_004:
	case SEG_009:
	case SEG_010:
	case SEG_011:
	case SEG_012:
	case SEG_013:
	case SEG_014:
	case SEG_015:
	case SEG_016:
		return (g_helper[seg].data_ptr + offt) - g_helper[seg].offset;
	case DSEG:
		return dseg_x16_to_x64(offt);

	default:
		assert(0);
	}

	return NULL;
}

static uint16_t dseg_x64_to_x16(uint8_t *src)
{
	int i = 0;

	while (g_dseg_map[i].offt != 0xFFFF)
	{
		uint8_t *ptr = (uint8_t*)g_dseg_map[i].obj_ptr;

		if (src >= ptr &&
			src < ptr + g_dseg_map[i].size)
		{
			return g_dseg_map[i].offt + (uint16_t)(src - ptr);
		}

		i++;
	}

	return 0xFFFF;
}

static uint16_t seg000_x64_to_x16(uint8_t *src)
{
	int i = 0;

	while (g_seg000_map[i].offt != 0xFFFF)
	{
		uint8_t *ptr = (uint8_t*)g_seg000_map[i].obj_ptr;

		if (src == ptr)
		{
			return g_seg000_map[i].offt;
		}

		i++;
	}

	return 0xFFFF;
}

void translate_x64_to_x16(uint8_t *src, uint16_t *seg, uint16_t *offt)
{
	int i;

	for (i = 0; i < SEGMENTS_TOTAL; i++)
	{
		if (i == SEG_000)
		{
			uint16_t _offt = seg000_x64_to_x16(src);
			if (_offt == 0xFFFF)
			{
				continue;
			}
			else
			{
				if (seg) {
					*seg = SEG_000;
				}

				if (offt) {
					*offt = _offt;
				}

				return;
			}
		}
		else if (i == DSEG)
		{
			uint16_t _offt = dseg_x64_to_x16(src);
			if (_offt == 0xFFFF)
			{
				continue;
			}
			else
			{
				if (seg) {
					*seg = DSEG;
				}

				if (offt) {
					*offt = _offt;
				}

				return;
			}
		}

		if (!g_helper[i].data_ptr)
		{
			continue;
		}

		if (src >= g_helper[i].data_ptr &&
			src < g_helper[i].data_ptr + g_helper[i].size)
		{
			break;
		}
	}

	assert(i != SEGMENTS_TOTAL);

	if (seg)
	{
		*seg = g_helper[i].segment;
	}

	if (offt)
	{
		*offt = (uint16_t)(src - g_helper[i].data_ptr) + g_helper[i].offset;
	}
}
