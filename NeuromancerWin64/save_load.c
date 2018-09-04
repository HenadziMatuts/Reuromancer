#include "data.h"
#include "save_load.h"
#include "resource_manager.h"
#include "neuro_menu_control.h"
#include "drawing_control.h"
#include <assert.h>
#include <string.h>

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
	uint8_t *data;
} adderss_translator_helper_t;

adderss_translator_helper_t addr_translator_helper[SEG_TOTAL] = {
	{ 0x020E, 0x00, NULL },
	{ 0x11DE, 0x0B, NULL },
	{ 0x1203, 0x0A, NULL },
	{ 0x1245, 0x00, NULL },
	{ 0x1277, 0x00, (uint8_t*)&g_seg004 },
	{ 0x12E3, 0x00, NULL },
	{ 0x1323, 0x00, NULL },
	{ 0x1900, 0x00, NULL },
	{ 0x1AC3, 0x00, NULL },
	{ 0x1B0E, 0x00, (uint8_t*)&g_seg009 },
	{ 0x1B27, 0x00, (uint8_t*)&g_seg010 },
	{ 0x22FB, 0x00, (uint8_t*)&g_seg011 },
	{ 0x2D3C, 0x00, (uint8_t*)&g_seg012 },
	{ 0x2E9D, 0x00, (uint8_t*)&g_seg013 },
	{ 0x3DC0, 0x00, (uint8_t*)&g_seg014 },
	{ 0x3DE6, 0x00, (uint8_t*)&g_seg015 },
	{ 0x4210, 0x00, (uint8_t*)&g_seg016 },
	{ 0x47EA, 0x00, NULL }
};

static void add_slots()
{
	neuro_menu_draw_text("1  2  3  4", 3, 2);
	neuro_menu_draw_text("exit", 6, 5);

	neuro_menu_add_item(6, 5, 4, 0x0A, 'x');

	for (int i = 0; i < 4; i++)
	{
		neuro_menu_add_item((3 * i) + 3, 2, 1, i, i + '1');
	}
}

void save_menu()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("Save Game", 4, 0);
	add_slots();
}

int on_save_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* slots */
		return -1;

	case 0x0A: /* exit */
		return 0;
	}

	return -1;
}

void load_menu()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("Load Game", 4, 0);
	add_slots();

	for (int i = 0; i < 4; i++)
	{
		neuro_menu_add_item((3 * i) + 3, 2, 1, i, i + '1');
	}
}

int on_load_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* slots */
		return -1;

	case 0x0A: /* exit */
		return 0;
	}

	return -1;
}

