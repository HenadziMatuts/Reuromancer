#include "address_translator.h"
#include "data.h"
#include <assert.h>

#define SEGMENTS_TOTAL 18
#define SEG_000 0x020E
#define SEG_001 0x11DE
#define SEG_002 0x1203
#define SEG_003 0x1245
#define SEG_004 0x1277
#define SEG_005 0x12E3
#define SEG_006 0x1323
#define SEG_007 0x1900
#define SEG_008 0x1AC3
#define SEG_009 0x1B0E
#define SEG_010 0x1B27
#define SEG_011 0x22FB
#define SEG_012 0x2D3C
#define SEG_013 0x2E9D
#define SEG_014 0x3DC0
#define SEG_015 0x3DE6
#define SEG_016 0x4210
#define DSEG    0x47EA

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

typedef struct dseg_map_t {
	int16_t offt;
	uint16_t size;
	void *obj_ptr;
} dseg_map_t;

static dseg_map_t g_dseg_map[] = {
	/* ui buttons */
	{ 0x1FA2, 0x0C, &g_ui_buttons.inventory },
	{ 0x1FAE, 0x0C, &g_ui_buttons.pax },
	{ 0x1FBA, 0x0C, &g_ui_buttons.dialog },
	{ 0x1FC6, 0x0C, &g_ui_buttons.skills },
	{ 0x1FD2, 0x0C, &g_ui_buttons.chip },
	{ 0x1FDE, 0x0C, &g_ui_buttons.disk },
	{ 0x1FEA, 0x0C, &g_ui_buttons.date },
	{ 0x1FF6, 0x0C, &g_ui_buttons.time },
	{ 0x2002, 0x0C, &g_ui_buttons.cash },
	{ 0x200E, 0x0C, &g_ui_buttons.constitution },
	/* pax buttons */
	{ 0x201A, 0x0C, &g_pax_buttons.exit },
	{ 0x2026, 0x0C, &g_pax_buttons.user_info },
	{ 0x2032, 0x0C, &g_pax_buttons.banking },
	{ 0x203E, 0x0C, &g_pax_buttons.news },
	{ 0x204A, 0x0C, &g_pax_buttons.board },
	/* pax banking buttons */
	{ 0x2176, 0x0C, &g_pax_banking_buttons.exit },
	{ 0x2182, 0x0C, &g_pax_banking_buttons.download },
	{ 0x218E, 0x0C, &g_pax_banking_buttons.upload },
	{ 0x219A, 0x0C, &g_pax_banking_buttons.transactions },
	/* pax info menu buttons */
	{ 0x21A6, 0x0C, &g_pax_info_menu_buttons.exit },
	{ 0x21B2, 0x0C, &g_pax_info_menu_buttons.more },
	/* pax msg board buttons */
	{ 0x21BE, 0x0C, &g_pax_board_menu_buttons.exit },
	{ 0x21CA, 0x0C, &g_pax_board_menu_buttons.view },
	{ 0x21D6, 0x0C, &g_pax_board_menu_buttons.send },
	/* pax send msg buttons */
	{ 0x21E2, 0x0C, &g_pax_board_send_msg_buttons.yes },
	{ 0x21EE, 0x0C, &g_pax_board_send_msg_buttons.no },
	/* inventory buttons */
	{ 0x21FA, 0x0C, &g_inv_buttons.item_page_exit },
	{ 0x2206, 0x0C, &g_inv_buttons.exit },
	{ 0x2212, 0x0C, &g_inv_buttons.more },
	/* inventory discard buttons */
	{ 0x2236, 0x0C, &g_inv_disc_buttons.yes },
	{ 0x2242, 0x0C, &g_inv_disc_buttons.no },

	{ -1, 0, NULL },
};

uint8_t* translate_x16_to_x64(uint16_t seg, uint16_t offt)
{
	int i;

	switch (seg) {
	case SEG_004: i = 4;  break;
	case SEG_009: i = 9;  break;
	case SEG_010: i = 10; break;
	case SEG_011: i = 11; break;
	case SEG_012: i = 12; break;
	case SEG_013: i = 13; break;
	case SEG_014: i = 14; break;
	case SEG_015: i = 15; break;
	case SEG_016: i = 16; break;

	default:
		assert(0);
	}

	return (g_helper[i].data_ptr + offt) - g_helper[i].offset;
}

void translate_x64_to_x16(uint8_t *src, uint16_t *seg, uint16_t *offt)
{
	int i;

	for (i = 0; i < SEGMENTS_TOTAL; i++)
	{
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

	*seg = g_helper[i].segment;
	*offt = (uint16_t)(src - g_helper[i].data_ptr) + g_helper[i].offset;
}
