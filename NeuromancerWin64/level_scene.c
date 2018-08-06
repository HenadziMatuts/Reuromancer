#include "globals.h"
#include "scene_control.h"
#include "character_control.h"
#include "bg_animation_control.h"
#include "drawing_control.h"
#include "resource_manager.h"
#include <string.h>
#include <assert.h>
#include <stdarg.h>

typedef enum level_intro_state_t {
	LIS_NEXT_LINE = 0,
	LIS_SCROLLING,
	LIS_WAITING_FOR_INPUT,
} level_intro_state_t;

typedef enum level_state_t {
	LS_INTRO = 0,
	LS_DIALOG_WAIT_FOR_INPUT,
	LS_NORMAL,
} level_state_t;

typedef enum ui_panel_mode_t {
	UI_PM_CASH = 0,
	UI_PM_CON,
	UI_PM_TIME,
	UI_PM_DATE
} ui_panel_mode_t;

static int g_level_n = -1; // dseg:004c
static ui_panel_mode_t g_ui_panel_mode = UI_PM_CASH; // dseg:25b4

static char *g_bih_string_ptr = NULL;
static level_state_t g_state = LS_NORMAL;

/***************************************/
static int sub_147EE(uint16_t opcode, ...);

#pragma pack(push, 1)
typedef struct neuro_vm_state_t {
	uint8_t level;
	uint8_t flag;
	uint8_t mark;
	uint16_t vm_next_op_addr;
	uint8_t var_1;
	uint8_t var_2;
} neuro_vm_state_t;

typedef struct x3f85_t {
	neuro_vm_state_t vm_state[35];
	uint8_t vm_state_end;
	uint8_t x407b[1006];
} x3f85_t;

typedef struct x3f85_wrapper_t {
	uint8_t *vm_next_op_addr[35];
	x3f85_t *x3f85;
} x3f85_wrapper_t;

typedef struct x4bae_t {
	uint8_t x4bae[16];
	uint8_t x4bbe;
	uint8_t x4bbf;
	uint16_t x4bc0;
	uint16_t x4bc2;
	uint16_t x4bc4;
	uint16_t time_m;  // 0x4BC6
	uint8_t time_h;   // 0x4BC8
	uint8_t date_day; // 0x4BC9
	uint8_t x4bca[2]; // padding
	uint8_t x4bcc;
	uint8_t x4bcd[38];
	uint8_t x4bf3;
	uint8_t x4bf4;
	uint8_t x4bf5;
	uint8_t x4bf6[6];
	uint8_t x4bfc;
	uint8_t x4bfd;
	uint8_t x4bfe;
	uint8_t x4bff;
	uint8_t x4c00;
	uint8_t x4c01[3]; // padding
	uint8_t x4c04;
	uint8_t x4c05;
	uint16_t x4c06;
	uint8_t x4c08[8];
	uint8_t x4c10;
	uint8_t x4c11[8];
	uint8_t x4c19;
	uint8_t x4c1a;
	uint16_t x4c1b;
	uint8_t x4c1d[4];
	uint16_t x4c21;
	uint8_t x4c23[2];
	uint16_t x4c25;
	uint8_t x4c27[4];
	uint16_t x4c2b;
	uint8_t x4c2d[2];
	uint16_t x4c2f;
	uint16_t x4c31;
	uint8_t x4c33[4];
	uint8_t x4c37;
	uint16_t x4c38;
	uint8_t x4c3a;
	uint8_t x4c3b;
	uint8_t x4c3c;
	uint16_t x4c3d;
	uint8_t x4c3f; // padding
	uint16_t x4c40;
	uint8_t x4c42[2]; // padding
	uint8_t x4c44;
	uint8_t x4c45;
	uint8_t x4c46;
	uint16_t x4c47;
	uint16_t x4c49;
	uint16_t x4c4b;
	uint16_t x4c4d;
	uint8_t x4c4f[8];
	uint16_t x4c57;
	uint8_t x4c59[3]; // padding
	uint8_t x4c5c;
	uint8_t x4c5d[12];
	uint8_t x4c69;
	uint8_t x4c6a;
	uint16_t x4c6b;
	uint8_t x4c6d[7];
	uint8_t x4c74;
	uint8_t x4c75;
	uint8_t x4c76[2]; // padding
	uint32_t cash; // 0x4C78
	uint16_t x4c7c;
	uint8_t x4c7e[4];
	uint16_t x4c82;
	uint8_t x4c84[3];
	uint16_t x4c87;
	uint16_t x4c89;
	uint16_t x4c8b;
	uint8_t x4c8d;
	uint16_t x4c8e;
	uint16_t x4c90;
	char x4c92[13];
	uint16_t con;     // 0x4C9F
	uint16_t level_n; // 0x4CA1
	uint16_t roompos_spawn_x; // 0x4CA3
	uint16_t roompos_spawn_y; // 0x4CA5
	uint16_t x4ca7;
	uint8_t x4ca9[24];
	uint16_t ui_type; // 0x4CC1
	uint16_t x4cc3;
	uint16_t x4cc5;
	uint16_t x4cc7;
	uint16_t x4cc9;
	uint8_t x4ccb;
	uint8_t x4ccc;
	uint8_t x4ccd;
	uint8_t x4cce;
	uint16_t x4ccf;
} x4bae_t;

typedef struct a59e_t {
	uint8_t a59e[40];
	uint8_t a5c6[40];
	uint8_t a5ee[40];
} a59e_t;

typedef struct c91e_t {
	uint16_t left;
	uint16_t top;
	uint16_t width;
	uint16_t height;
	uint16_t c926;
	uint16_t c928;
	uint8_t c92a[4];
	uint16_t c92e;
	uint16_t c930[10];
	uint16_t c944;
} c91e_t;

typedef struct c91e_wrapper_t {
	uint8_t *ui_button_areas[10];
	c91e_t *c91e;
} c91e_wrapper_t;

typedef struct a8e0_t {
	uint16_t a8e0[4];
	uint8_t bih[12288]; // 0xA8E8
} a8e0_t;

typedef struct neuro_ui_button_area_t {
	uint16_t left;
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
	uint16_t code;
	uint8_t unknown[2];
} neuro_ui_button_area_t;

typedef struct neuro_ui_button_areas_t {
	neuro_ui_button_area_t inventory;
	neuro_ui_button_area_t pax;
	neuro_ui_button_area_t dialog;
	neuro_ui_button_area_t skills;
	neuro_ui_button_area_t chip;
	neuro_ui_button_area_t disk;
	neuro_ui_button_area_t date;
	neuro_ui_button_area_t time;
	neuro_ui_button_area_t cash;
	neuro_ui_button_area_t con;
} neuro_ui_button_areas_t;

static a59e_t g_a59e = {
	{ 0, }, { 0, }, { 0, }
};

static c91e_t g_c91e = {
	{ 0, }
};

c91e_wrapper_t g_c91e_wrapper = {
	{ NULL, }, &g_c91e
};

static x4bae_t g_4bae = {
	.x4bae = { 0, },
	.x4bbe = 0xff,
	.x4bbf = 0xff,
	.x4bc0 = 0xffff,
	.x4bc2 = 0,
	.x4bc4 = 0,
	.time_m = 0,
	.time_h = 0x0C,
	.date_day = 0,
	.x4bca = { 0, 0 },
	.x4bcc = 0,
	.x4bcd = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0
	},
	.x4bf3 = 0xFF,
	.x4bf4 = 0xFF,
	.x4bf5 = 0xFF,
	.x4bf6 = { 0x4C, 0, },
	.x4bfc = 0,
	.x4bfd = 0,
	.x4bfe = 0,
	.x4bff = 0,
	.x4c00 = 0,
	.x4c01 = { 0, 0, 0 },
	.x4c04 = 0xFF,
	.x4c05 = 0xFF,
	.x4c06 = 0xFFFF,
	.x4c08 = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0 },
	.x4c10 = 0,
	.x4c11 = { 0, 0, 0, 0, 0, 0, 1, 0 },
	.x4c19 = 0,
	.x4c1a = 0,
	.x4c1b = 0,
	.x4c1d = { 0, 0, 0, 0 },
	.x4c21 = 0,
	.x4c23 = { 0, 0 },
	.x4c25 = 0,
	.x4c27 = { 0, 0, 0, 0 },
	.x4c2b = 0,
	.x4c2d = { 0, 0 },
	.x4c2f = 0x3E8,
	.x4c31 = 0,
	.x4c33 = { 1, 2, 1, 0 },
	.x4c37 = 0xFF,
	.x4c38 = 0,
	.x4c3a = 0,
	.x4c3b = 0,
	.x4c3c = 0,
	.x4c3d = 0,
	.x4c3f = 0,
	.x4c40 = 0,
	.x4c42 = { 0, 0 },
	.x4c44 = 0xFF,
	.x4c45 = 0,
	.x4c46 = 0,
	.x4c47 = 0xA120,
	.x4c49 = 7,
	.x4c4b = 0x7530,
	.x4c4d = 0,
	.x4c4f = { 0, },
	.x4c57 = 0,
	.x4c59 = { 0, 0, 0 },
	.x4c5c = 0xFF,
	.x4c5d = { 0, },
	.x4c69 = 0,
	.x4c6a = 0,
	.x4c6b = 0,
	.x4c6d = { 0, },
	.x4c74 = 0,
	.x4c75 = 0,
	.x4c76 = { 0, 0 },
	.cash = 6,
	.x4c7c = 0,
	.x4c7e = { 0, 0, 0, 0 },
	.x4c82 = 0,
	.x4c84 = { 0, 0, 0 },
	.x4c87 = 0x7D0,
	.x4c89 = 0x7D0,
	.x4c8b = 0,
	.x4c8d = 0,
	.x4c8e = 0,
	.x4c90 = 0,
	.x4c92 = { '{', '@', 'C', 'a', 's', 'e', 0, },
	.con = 0x7D0,
	.level_n = 0,
	.roompos_spawn_x = 0xA0,
	.roompos_spawn_y = 0x69,
	.x4ca7 = 0,
	.x4ca9 = {
		0x40, 0, 0x78, 0, 0, 0, 0x40, 0, 0x38, 0, 0, 0,
		0x40, 0, 0x4B, 0, 0, 0, 0xC0, 0, 0xE8, 3, 0, 0,
	},
	.ui_type = 0,
	.x4cc3 = 0,
	.x4cc5 = 0,
	.x4cc7 = 0,
	.x4cc9 = 0,
	.x4ccb = 0,
	.x4ccc = 0,
	.x4ccd = 0,
	.x4cce = 0,
	.x4ccf = 2
};

static x3f85_t g_3f85 = {
	{
		{ 0x00, 0xC1, 0xFF, 0x0000, 0x24, 0x28 },
		{ 0x01, 0xE9, 0xFF, 0x0000, 0x1E, 0x27 },
		{ 0x02, 0x01, 0xFF, 0x0000, 0x19, 0x3D },
		{ 0x02, 0x02, 0xFF, 0x0000, 0x50, 0x3E },
		{ 0x03, 0xC1, 0xFF, 0x0000, 0x6B, 0x33 },
		{ 0x05, 0x41, 0xFF, 0x0000, 0x8C, 0x4B },
		{ 0x06, 0x01, 0xFF, 0x0000, 0x00, 0x00 },
		{ 0x07, 0xC1, 0xFF, 0x0000, 0x1B, 0x2F },
		{ 0x08, 0x01, 0xFF, 0x0000, 0x77, 0x26 },
		{ 0x09, 0xE1, 0xFF, 0x0000, 0x89, 0x35 },
		{ 0x0A, 0xE1, 0xFF, 0x0000, 0x54, 0x33 },
		{ 0x0B, 0xC1, 0xFF, 0x0000, 0x2C, 0x34 },
		{ 0x12, 0xC1, 0xFF, 0x0000, 0x84, 0x24 },
		{ 0x13, 0xE1, 0xFF, 0x0000, 0x6D, 0x38 },
		{ 0x15, 0x01, 0xFF, 0x0000, 0x50, 0x35 },
		{ 0x16, 0xE1, 0xFF, 0x0000, 0x6C, 0x33 },
		{ 0x17, 0xE1, 0xFF, 0x0000, 0x4B, 0x3D },
		{ 0x18, 0xC1, 0xFF, 0x0000, 0x30, 0x25 },
		{ 0x19, 0xE1, 0xFF, 0x0000, 0x59, 0x2D },
		{ 0x1A, 0xC1, 0xFF, 0x0000, 0x3F, 0x2F },
		{ 0x1B, 0xE1, 0xFF, 0x0000, 0x89, 0x35 },
		{ 0x1C, 0xC1, 0xFF, 0x0000, 0x78, 0x34 },
		{ 0x1F, 0x41, 0xFF, 0x0000, 0x33, 0x21 },
		{ 0x21, 0xC1, 0xFF, 0x0000, 0x60, 0x34 },
		{ 0x23, 0xE1, 0xFF, 0x0000, 0x58, 0x33 },
		{ 0x27, 0xE1, 0xFF, 0x0000, 0x34, 0x31 },
		{ 0x28, 0x01, 0xFF, 0x0000, 0x78, 0x30 },
		{ 0x2B, 0xE1, 0xFF, 0x0000, 0x38, 0x31 },
		{ 0x2C, 0xE9, 0xFF, 0x0000, 0x1E, 0x27 },
		{ 0x2D, 0xC1, 0xFF, 0x0000, 0x25, 0x24 },
		{ 0x2D, 0xC2, 0xFF, 0x0000, 0x0E, 0x24 },
		{ 0x31, 0xE1, 0xFF, 0x0000, 0x71, 0x31 },
		{ 0x33, 0x01, 0xFF, 0x0000, 0x61, 0x32 },
		{ 0x34, 0xE1, 0xFF, 0x0000, 0x5C, 0x32 },
		{ 0x37, 0x41, 0xFF, 0x0000, 0x54, 0x29 },
	},
	0xFF,
	{
		0xFF, 0x00, 0xFF, 0xFF, 0x01, 0xFF,
		0xFF, 0x00, 0xFF, 0x04, 0xFF, 0xFF,
		0xFF, 0x00, 0x01, 0xFF, 0xFF, 0xFF,
		0xFF, 0x00, 0xFF, 0xFF, 0x04, 0xFF,
		0x0FF, 0x0, 0x3, 0x0C, 0x5, 0x1, 0x0FF, 0x0, 0x4, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0, 0x0FF, 0x0E, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0F, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x10, 0x0FF,
		0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF,
		0x0FF, 0x0C, 0x0FF, 0x0FF, 0x0, 0x0B, 0x17, 0x0D, 0x4, 0x0FF, 0x0, 0x0C, 0x18, 0x0E, 0x0FF, 0x0FF, 0x0,
		0x0D, 0x19, 0x0F, 0x6, 0x0FF, 0x0, 0x0E, 0x0FF, 0x10, 0x7, 0x0FF, 0x0, 0x0F, 0x1A, 0x11, 0x0FF, 0x0FF,
		0x0, 0x10, 0x0FF, 0x0FF, 0x12, 0x0FF, 0x0, 0x0FF, 0x11, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0, 0x15, 0x1D, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x14, 0x0FF, 0x0FF, 0x0, 0x0FF,
		0x0FF, 0x0B, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0C, 0x0FF, 0x0, 0x0FF, 0x0D, 0x0FF, 0x0FF, 0x0FF,
		0x0, 0x0FF, 0x1E, 0x0FF, 0x0E, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x10, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x1D, 0x0FF, 0x0FF, 0x0, 0x1C, 0x20, 0x0FF, 0x14, 0x0FF, 0x0, 0x1F,
		0x26, 0x0FF, 0x19, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x1E, 0x0FF, 0x0FF, 0x0, 0x21, 0x29, 0x0FF, 0x1D, 0x0FF,
		0x0, 0x0FF, 0x0FF, 0x20, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x21, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x24,
		0x0FF, 0x0FF, 0x0, 0x23, 0x0FF, 0x25, 0x0FF, 0x0FF, 0x0, 0x24, 0x2B, 0x26, 0x0FF, 0x0FF, 0x0, 0x25, 0x2C,
		0x27, 0x1E, 0x0FF, 0x0, 0x26, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x29, 0x0FF, 0x0FF, 0x0, 0x28,
		0x0FF, 0x0FF, 0x20, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x25, 0x0FF,
		0x0, 0x0FF, 0x30, 0x2D, 0x26, 0x0FF, 0x0, 0x2C, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x28,
		0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x33, 0x0FF, 0x2C, 0x0FF, 0x0, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x35, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x36, 0x0FF, 0x30, 0x0FF, 0x0,
		0x0FF, 0x0FF, 0x35, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x36, 0x0FF, 0x0FF, 0x0, 0x35, 0x0FF, 0x0FF, 0x33,
		0x0FF, 0x0, 0x36, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x35, 0x0FF, 0x0, 0x0FF, 0x0FF,
		0x0FF, 0x36, 0x5F, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0,
		0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0,
		0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0,
		0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF,
		0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0,
		0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0,
		0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0,
		0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x1, 0x0, 0x0, 0x0FF, 0x1, 0x0, 0x0, 0x0FF, 0x1, 0x0, 0x0, 0x0FF, 0x1, 0x0,
		0x0, 0x0FF, 0x1, 0x0, 0x0, 0x0FF, 0x1, 0x0, 0x0, 0x0FF, 0x5, 0x0, 0x0, 0x0FF, 0x1, 0x0, 0x0, 0x0FF, 0x5, 0x0,
		0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0,
		0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0,
		0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0,
		0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0,
		0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0, 0x0, 0x0,
		0x0FF, 0x0, 0x0, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x52, 0x4F, 0x59, 0x20, 0x4D, 0x49, 0x4C, 0x45, 0x53, 0x54, 0x4F, 0x4E, 0x45, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x0, 0x38, 0x31, 0x32, 0x35, 0x34, 0x37, 0x37, 0x32, 0x34, 0x0, 0x0FF, 0x0, 0x0, 0x42, 0x41,
		0x4C, 0x20, 0x34, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x35, 0x35,
		0x31, 0x37, 0x30, 0x33, 0x32, 0x38, 0x38, 0x0, 0x0FF, 0x0, 0x2, 0x4B, 0x52, 0x49, 0x53, 0x54, 0x4F, 0x46, 0x46,
		0x45, 0x52, 0x20, 0x55, 0x4C, 0x4D, 0x20, 0x20, 0x20, 0x20, 0x0, 0x31, 0x38, 0x38, 0x38, 0x36, 0x34, 0x32, 0x30,
		0x32, 0x0, 0x0FF, 0x0, 0x3, 0x4B, 0x49, 0x4D, 0x20, 0x54, 0x59, 0x47, 0x45, 0x52, 0x20, 0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x0, 0x31, 0x35, 0x39, 0x32, 0x31, 0x37, 0x33, 0x32, 0x39, 0x0, 0x0FF, 0x0, 0x1, 0x41,
		0x53, 0x48, 0x4C, 0x45, 0x59, 0x20, 0x52, 0x4F, 0x42, 0x49, 0x4E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0,
		0x30, 0x34, 0x32, 0x33, 0x38, 0x35, 0x30, 0x30, 0x33, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x54, 0x41, 0x4B, 0x4F, 0x44,
		0x41, 0x20, 0x4D, 0x49, 0x53, 0x48, 0x49, 0x4A, 0x49, 0x20, 0x20, 0x20, 0x20, 0x0, 0x38, 0x38, 0x33, 0x38,
		0x34, 0x39, 0x39, 0x33, 0x31, 0x0, 0x0FF, 0x0, 0x0, 0x41, 0x4B, 0x49, 0x52, 0x41, 0x20, 0x4F, 0x27, 0x42,
		0x52, 0x49, 0x45, 0x4E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x33, 0x39, 0x34, 0x38, 0x35, 0x37, 0x32, 0x39, 0x31,
		0x0, 0x0FF, 0x0, 0x1, 0x53, 0x56, 0x45, 0x4E, 0x20, 0x4B, 0x48, 0x41, 0x52, 0x4B, 0x4F, 0x56, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x0, 0x33, 0x38, 0x39, 0x34, 0x35, 0x35, 0x37, 0x35, 0x36, 0x0, 0x0FF, 0x0, 0x2, 0x4D, 0x45,
		0x4C, 0x49, 0x53, 0x53, 0x41, 0x20, 0x42, 0x4F, 0x52, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x31, 0x30,
		0x32, 0x39, 0x33, 0x38, 0x35, 0x36, 0x35, 0x0, 0x0FF, 0x0, 0x3, 0x50, 0x2E, 0x20, 0x52, 0x59, 0x41, 0x4E,
		0x20, 0x57, 0x41, 0x4E, 0x47, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x35, 0x36, 0x34, 0x39, 0x38, 0x37, 0x32,
		0x37, 0x31, 0x0, 0x0FF, 0x0, 0x4, 0x4D, 0x2E, 0x20, 0x43, 0x2E, 0x20, 0x42, 0x45, 0x41, 0x52, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x31, 0x32, 0x39, 0x34, 0x38, 0x35, 0x36, 0x33, 0x36, 0x0, 0x0FF, 0x0, 0x5,
		0x53, 0x2E, 0x20, 0x5A, 0x2E, 0x20, 0x57, 0x41, 0x54, 0x45, 0x52, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0,
		0x30, 0x35, 0x33, 0x36, 0x35, 0x36, 0x35, 0x32, 0x38, 0x0, 0x0FF, 0x0, 0x5, 0x0,
	}
};

static uint8_t g_004e[8] = {
	0x00,
};

typedef enum jumps_t {
	JE, JNE, JL, JGE
} jumps_t;

static jumps_t g_4b9d[4] = {
	JE, JNE, JL, JGE
};

static x3f85_wrapper_t g_3f85_wrapper = {
	{ NULL, }, &g_3f85,
};

static a8e0_t g_a8e0 = {
	{ 0, }, { 0, }
};

static bih_hdr_wrapper_t g_bih_wrapper = {
	NULL, NULL, NULL
};

static neuro_ui_button_areas_t g_ui_button_areas = {
	.inventory = {
		0x10, 0x93, 0x23, 0xA5, 0x00,
		{ 0x69, 0x00 }
	},
	.pax = {
		0x28, 0x93, 0x3B, 0xA5, 0x01,
		{ 0x70, 0x00 }
	},
	.dialog = {
		0x40, 0x93, 0x53, 0xA5, 0x02,
		{ 0x74, 0x00 }
	},
	.skills = {
		0x10, 0xAB, 0x23, 0xBD, 0x03,
		{ 0x73, 0x00 }
	},
	.chip = {
		0x28, 0xAB, 0x3B, 0xBD, 0x04,
		{ 0x72, 0x00 }
	},
	.disk = {
		0x40, 0xAB, 0x53, 0xBD, 0x05,
		{ 0x64, 0x00 }
	},
	.date = {
		0x70, 0xA8, 0x7D, 0xB2, 0x0A,
		{ 0x31, 0x00 }
	},
	.time = {
		0x80, 0xA8, 0x8F, 0xB2, 0x0B,
		{ 0x32, 0x00 }
	},
	.cash = {
		0x70, 0xB3, 0x7D, 0xBB, 0x0C,
		{ 0x33, 0x00 }
	},
	.con = {
		0x80, 0xB3, 0x8F, 0xBB, 0x0D,
		{ 0x34, 0x00 }
	}
};

static void sub_14DBA(char *text)
{
	switch (g_c91e.c926) {
	case 0: {
		g_state = LS_INTRO;
		break;
	}

	case 8: {
		imh_hdr_t *imh = (imh_hdr_t*)g_seg011;
		build_string(text, imh->width * 2, imh->height, 8, 8, g_seg011 + sizeof(imh_hdr_t));
		drawing_control_add_sprite_to_chain(g_4bae.x4ccf--, 0, g_c91e.top, g_seg011, 1);
	}

	default:
		break;
	}
}

static void sub_1342E(uint16_t opcode)
{
	char text[256] = { 0, };
	char *p = text;
	int lines = 0;

	while (!extract_line(&g_bih_string_ptr, p, 38))
	{
		size_t l = strlen(p);

		if (l != 38)
		{
			memset(p + l, 0x20, 38 - l);
		}

		p += 38;
		*p++ = '\n';
		lines++;
	}
	*--p = 0;

	sub_147EE(opcode, lines);
	sub_14DBA(text);
}

static void sub_10A5B(uint16_t a, uint16_t b, uint16_t c, uint16_t d)
{
	uint16_t x = 0;

	g_bih_string_ptr = g_a8e0.bih + g_bih_wrapper.bih->text_offset;

	for (int i = 0; i < a; i++)
	{
		while (*g_bih_string_ptr++);
	}

	if (b != 0)
	{
		return;
	}

	uint16_t temp_1 = g_4bae.roompos_spawn_x;
	uint16_t temp_2 = g_4bae.roompos_spawn_y;

	g_4bae.roompos_spawn_x = c * 2;
	g_4bae.roompos_spawn_y = d;

	/* builds dialog bubble with text */
	sub_1342E(8);

	g_4bae.roompos_spawn_x = temp_1;
	g_4bae.roompos_spawn_y = temp_2;

	return;
}

static void neuro_vm()
{
	for (int i = 3; i >= 0; i--)
	{
		uint16_t op_index = g_a8e0.a8e0[i];
		if (op_index == 0xffff) {
			continue;
		}

		uint8_t *next_opcode_addr = g_3f85_wrapper.vm_next_op_addr[op_index];
		uint8_t opcode = *next_opcode_addr;

		switch (opcode) {
		/* dialog reply, 1 arg (line number) */
		case 1:
			sub_10A5B(*(next_opcode_addr + 1), 0,
				g_3f85.vm_state[op_index].var_1,
				g_3f85.vm_state[op_index].var_2);

			g_state = LS_DIALOG_WAIT_FOR_INPUT;

			g_3f85_wrapper.vm_next_op_addr[op_index] += 2;
			g_4bae.x4bbe = 0xFF;
			break;

		case 5:
		case 6:
		case 7: 
		case 8: {
			jumps_t jmp = g_4b9d[opcode - 5];
			uint8_t arg_1 = *(next_opcode_addr + 1);
			uint8_t arg_3 = *(next_opcode_addr + 3);

			if (((jmp == JE) && (g_4bae.x4bae[arg_1] == arg_3)) ||
				((jmp == JNE) && (g_4bae.x4bae[arg_1] != arg_3)) ||
				((jmp == JL) && (g_4bae.x4bae[arg_1] < arg_3)) ||
				((jmp == JGE) && (g_4bae.x4bae[arg_1] >= arg_3)))
			{
				g_3f85_wrapper.vm_next_op_addr[op_index] += 6;
			}
			else
			{
				int16_t arg_4 = *(int16_t*)(next_opcode_addr + 4);
				g_3f85_wrapper.vm_next_op_addr[op_index] += arg_4;
				g_3f85_wrapper.vm_next_op_addr[op_index] += 4;
			}

			break;
		}

		case 19: {
			uint8_t *p = g_3f85.x407b + *(next_opcode_addr + 1);
			p[0] = *(next_opcode_addr + 2);
			p[1] = *(next_opcode_addr + 3);

			g_3f85_wrapper.vm_next_op_addr[op_index] += 4;
			break;
		}

		default:
			break;
		}
	}
}

static int setup_intro()
{
	uint16_t x = (0x80 >> (g_level_n & 7)) | 0x80;
	uint16_t y = (g_level_n >> 3) | (g_level_n & 0x80);

	g_bih_string_ptr = g_a8e0.bih + g_bih_wrapper.bih->text_offset;

	if ((g_004e[y] & x) == 0) {
		/* setup long intro */
		g_004e[y] |= x;
	}
	else {
		/* setup short intro */
		while (*g_bih_string_ptr++);
	}

	sub_14DBA(g_bih_string_ptr);

	return 0;
}

typedef enum sub_105f6_opcodes_t {
	SUB_105F6_OP_PLAY_LEVEL_INTRO = 5,
	SUB_105F6_OP_NEURO_VM_STEP,
} sub_105f6_opcodes_t;

static uint64_t sub_105F6(uint16_t opcode, ...)
{
	switch (opcode) {
	case 0: {
		va_list args;
		va_start(args, opcode);
		uint16_t level_n = va_arg(args, uint16_t);
		va_end(args);

		// mov [A642], 0x407B + 6*level_n
		g_bih_wrapper.bih = (bih_hdr_t*)g_a8e0.bih;
		g_bih_wrapper.ctrl_struct_addr = (uint8_t*)&g_4bae;
		// g_bih_wrapper.cb_addr = &cb;
		break;
	}

	case 1:
	case 3: {
		uint16_t offt = g_bih_wrapper.bih->own_cb_offsets[opcode];
		// call (a8e8 + offt)
		break;
	}

	case 4: {
		va_list args;
		va_start(args, opcode);
		uint16_t x = va_arg(args, uint16_t);
		uint16_t y = va_arg(args, uint16_t);
		va_end(args);

		uint8_t offt = *((uint8_t*)&g_bih_wrapper.bih->text_offset + (x * 2));
		uint8_t *p = g_a8e0.bih + offt;
		p = g_a8e0.bih + *(p + (y * 2));
		return (uint64_t)p;
	}

	case SUB_105F6_OP_PLAY_LEVEL_INTRO: {
		setup_intro();
		break;
	}

	case SUB_105F6_OP_NEURO_VM_STEP:
		neuro_vm();
		break;

	default:
		break;
	}

	return 0;
}

static int has_pax()
{
	uint8_t *p = g_a8e0.bih + sizeof(bih_hdr_t); // 0xA910

	while ((*p != 0) && (*p != 1))
	{
		p++;
	}

	return *p;
}

static int setup_ui_button_area(void *area)
{
	switch (g_c91e.c926)
	{
	case 0:
		g_c91e_wrapper.ui_button_areas[g_c91e.c92e++] = area;
		break;

	default:
		break;
	}

	return 0;
}

static int setup_ui_button_areas()
{
	assert((g_c91e.c926 == 0) || (g_c91e.c926 > 2 && g_c91e.c926 <= 4));
	g_c91e.c92e = 0;

	setup_ui_button_area(&g_ui_button_areas.inventory);
	setup_ui_button_area(&g_ui_button_areas.dialog);

	if (has_pax())
	{
		setup_ui_button_area(&g_ui_button_areas.pax);
	}

	setup_ui_button_area(&g_ui_button_areas.skills);
	setup_ui_button_area(&g_ui_button_areas.chip);
	setup_ui_button_area(&g_ui_button_areas.disk);
	setup_ui_button_area(&g_ui_button_areas.date);
	setup_ui_button_area(&g_ui_button_areas.time);
	setup_ui_button_area(&g_ui_button_areas.cash);
	setup_ui_button_area(&g_ui_button_areas.con);

	return 0;
}

static int sub_147EE(uint16_t opcode, ...)
{
	memmove(g_a59e.a5ee, g_a59e.a5c6, 40);
	memmove(g_a59e.a5c6, g_a59e.a59e, 40);
	memmove(g_a59e.a59e, &g_c91e, 40);

	g_c91e.c926 = opcode;
	g_c91e.c92e = 0;

	switch (opcode) {
	case 0:
		setup_ui_button_areas();

		g_c91e.left = 0;
		g_c91e.top = 0;
		g_c91e.width = 319;
		g_c91e.height = 199;
		g_c91e.c944 = 160;
		break;

	case 8: {
		va_list args;
		va_start(args, opcode);
		uint16_t lines = va_arg(args, uint16_t);
		va_end(args);

		g_c91e.left = 0;
		g_c91e.top = 4;
		g_c91e.width = 319;
		g_c91e.height = (lines * 8) + 19;
		g_c91e.c928 = lines;
		g_c91e.c944 = 160;

		if (g_4bae.ui_type == 0)
		{
			if (g_c91e.c926 == 1)
			{
				if (g_4bae.roompos_spawn_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_c91e.height + 1, g_dialog_bubbles + 0x1A2, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_c91e.height + 1, g_dialog_bubbles + 0xDC, 0);
				}
			}
			else
			{
				if (g_4bae.roompos_spawn_x < 0xA0)
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_c91e.height + 1, g_dialog_bubbles + 0x6E, 0);
				}
				else
				{
					drawing_control_add_sprite_to_chain(SCI_DIALOG_BUBBLE,
						g_4bae.roompos_spawn_x + 8, g_c91e.height + 1, g_dialog_bubbles, 0);
				}
			}
		}

		build_text_frame(g_c91e.height + 1, g_c91e.width + 1, (imh_hdr_t*)g_seg011);
		break;
	}

	default:
		break;
	}

	return 0;
}
/***************************************/

static void ui_panel_update()
{
	char panel_string[9];
	uint8_t *bg_pix = g_background + sizeof(imh_hdr_t);
	static int update_cap_ms = 1000;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed > update_cap_ms)
	{
		elapsed = passed;

		if (++g_4bae.time_m == 60)
		{
			g_4bae.time_m = 0;
			if (++g_4bae.time_h == 24)
			{
				g_4bae.time_h = 0;
				g_4bae.date_day++;
				g_4bae.x4c10 ^= 1;
				g_4bae.x4c38 = 0;
				g_4bae.x4c5c = 0xFF;
			}
		}
	}

	if (g_4bae.ui_type == 0)
	{
		if (g_4bae.level_n == 49 || g_4bae.level_n == 21)
		{
			g_ui_panel_mode = 1;
		}
	}

	switch (g_ui_panel_mode) {
	case UI_PM_CASH:
		sprintf(panel_string, "$%7d", g_4bae.cash);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;

	case UI_PM_CON:
		sprintf(panel_string, "%8d", g_4bae.con);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;

	case UI_PM_TIME:
		sprintf(panel_string, "   %02d:%02d", g_4bae.time_h, g_4bae.time_m);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;

	case UI_PM_DATE: {
		uint16_t day = 16;
		uint16_t month = 11;
		uint16_t year = 58;

		if (g_4bae.date_day > 14)
		{
			if (day + g_4bae.date_day > 61)
			{
				year = 59;
				month = 1;
				day = day + g_4bae.date_day - 61;
			}
			else
			{
				month = 12;
				day = day + g_4bae.date_day - 30;
			}
		}
		else
		{
			day += g_4bae.date_day;
		}

		sprintf(panel_string, "%02d/%02d/%02d", month, day, year);
		build_string(panel_string, 320, 200, 96, 149, bg_pix);
		break;
	}

	default:
		break;
	}
}

static void init()
{
	char resource[32] = { 0, };
	memset(g_vga, 0, 320 * 200 * 4);
	g_state = LS_NORMAL;

	assert(resource_manager_load_resource("NEURO.IMH", g_background));
	drawing_control_add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_background, 1);

	sub_147EE(0);

	if (g_level_n >= 0 && g_4bae.x4bcc == 0)
	{
		/* sub_105F6(3) */
	}

	g_4bae.x4bcc = 0;

	g_4bae.x4cc3 = 0;
	g_4bae.x4bbe = 0xFF;
	g_4bae.x4bbf = 0xFF;
	g_4bae.x4bc0 = 0xFFFF;
	g_4bae.x4bc2 = 0xFFFF;
	g_4bae.x4bc4 = 0xFFFF;
	// mov [152C], 0
	g_4bae.x4bf4 = 0xFF;
	// mov [1E34], 0
	// mov [1E44], 0

	memset(g_a8e0.a8e0, 0xFFFF, 8);
	g_level_n = g_4bae.level_n;

	sprintf(resource, "R%d.BIH", g_level_n + 1);
	assert(resource_manager_load_resource(resource, g_a8e0.bih));

	sprintf(resource, "R%d.PIC", g_level_n + 1);
	assert(resource_manager_load_resource(resource, g_level_bg + sizeof(imh_hdr_t)));
	drawing_control_add_sprite_to_chain(SCI_LEVEL_BG, 8, 8, g_level_bg, 1);

	sprintf(resource, "R%d.ANH", g_level_n + 1);
	resource_manager_load_resource(resource, g_roompos + 0x488) ?
		bg_animation_control_init_tables(g_roompos + 0x488) :
		bg_animation_control_init_tables(NULL);

	sub_105F6(0, g_level_n);

	uint16_t u = 0;
	neuro_vm_state_t *p = g_3f85.vm_state;

	while (1)
	{
		if (p->level == 0xFF)
		{
			if (g_4bae.level_n != 9 && g_4bae.level_n != 27)
			{
				character_control_add_sprite_to_chain(156, 110, CD_DOWN);
			}
			break;
		}

		if (p->level != g_4bae.level_n)
		{
			goto next;
		}

		if (p->mark == 0xFF)
		{
			p->mark = 0;
			g_3f85_wrapper.vm_next_op_addr[u] = (uint8_t*)sub_105F6(4, p->flag & 3, 0);
		}

		uint16_t offt = p->flag & 3;
		g_a8e0.a8e0[offt] = u;

	next:
		p++;
		u++;
	}

	sub_105F6(1);
	setup_ui_button_areas();
	sub_105F6(SUB_105F6_OP_PLAY_LEVEL_INTRO);

	ui_panel_update();
	return;
}

static void select_ui_button(neuro_ui_button_area_t *button)
{
	uint8_t *ui_pic = g_background + sizeof(imh_hdr_t);
	uint8_t *p = ui_pic +
		((button->left - g_c91e.left) / 2) +
		((button->top - g_c91e.top) * g_c91e.c944);

	uint16_t lines = button->bottom - button->top + 1;
	uint16_t width = ((button->right | 1) - (button->left & 0xFE) + 1) / 2;
	uint16_t skip = g_c91e.c944 - width;

	for (uint16_t h = 0; h < lines; h++, p += skip)
	{
		for (uint16_t w = 0; w < width; w++)
		{
			*p++ ^= 0xFF;
		}
	}

	return;
}

static on_ui_button(neuro_ui_button_area_t *button)
{
	switch (button->code) {
	case 0x0A:
		g_ui_panel_mode = UI_PM_DATE;
		break;

	case 0x0B:
		g_ui_panel_mode = UI_PM_TIME;
		break;

	case 0x0C:
		g_ui_panel_mode = UI_PM_CASH;
		break;

	case 0x0D:
		g_ui_panel_mode = UI_PM_CON;
		break;

	default:
		break;
	}
}

static void unselect_ui_button(neuro_ui_button_area_t *button)
{
	select_ui_button(button);
}

static neuro_ui_button_area_t* cursor_ui_button_hit_test()
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];
	neuro_ui_button_area_t *hit =
		(neuro_ui_button_area_t*)g_c91e_wrapper.ui_button_areas[0];

	for (uint16_t u = 0; u < g_c91e.c92e; u++, hit++)
	{
		if (cursor->left > hit->left && cursor->left < hit->right &&
			cursor->top > hit->top && cursor->top < hit->bottom)
		{
			return hit;
		}
	}

	return NULL;
}

static void ui_handle_input(sfEvent *event)
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];
	neuro_ui_button_area_t *button = NULL;
	static neuro_ui_button_area_t *selected = NULL;
	static int _selected = 0;

	/* ui area */
	if (cursor->left < 0 || cursor->left > 320 ||
		cursor->top < 140 || cursor->top > 200)
	{
		return;
	}

	if (sfMouse_isButtonPressed(sfMouseLeft))
	{
		if (button = cursor_ui_button_hit_test())
		{
			if ((!selected || (button == selected)) && !_selected)
			{
				select_ui_button(button);
				selected = button;
				_selected = 1;
			}
		}
		else if (selected && _selected)
		{
			unselect_ui_button(selected);
			_selected = 0;
		}
	}
	else if (event->mouseButton.type == sfEvtMouseButtonReleased)
	{
		if (selected)
		{
			if (_selected)
			{
				unselect_ui_button(selected);
				_selected = 0;
			}

			if (selected == cursor_ui_button_hit_test())
			{
				on_ui_button(selected);
			}

			selected = NULL;
		}
	}
}

static void update_normal(sfEvent *event)
{
	character_control_handle_input();
	character_control_update();

	sub_105F6(SUB_105F6_OP_NEURO_VM_STEP);

	ui_handle_input(event);
	ui_panel_update();

	bg_animation_control_update();
}

static void wait_for_input()
{
	switch (g_state) {
	case LS_DIALOG_WAIT_FOR_INPUT:
		if (sfMouse_isButtonClicked(sfMouseLeft))
		{
			g_state = LS_NORMAL;
			drawing_control_remove_sprite_from_chain(++g_4bae.x4ccf);
			drawing_control_remove_sprite_from_chain(SCI_DIALOG_BUBBLE);

			memmove(&g_c91e, g_a59e.a59e, 40);
			memmove(g_a59e.a59e, g_a59e.a5c6, 40);
			memmove(g_a59e.a5c6, g_a59e.a5ee, 40);
		}
		break;

	default:
		break;
	}
}

static void update_intro()
{
	static level_intro_state_t state = LIS_NEXT_LINE;
	static int lines_on_screen = 0, lines_scrolled = 0;
	static int frame_cap_ms = 10;
	static int elapsed = 0;

	char line[18] = { 0, };
	uint8_t *pix = g_background + sizeof(imh_hdr_t);
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return;
	}
	elapsed = passed;

	if (state == LIS_NEXT_LINE)
	{
		int last = extract_line(&g_bih_string_ptr, line, 17);

		build_string(line, 320, 200, 176, 182, pix);
		
		if (last)
		{
			g_state = LS_NORMAL;
			state = LIS_NEXT_LINE;
			lines_on_screen = 0;
			return;
		}

		state = (++lines_on_screen == 7)
			? LIS_WAITING_FOR_INPUT : LIS_SCROLLING;

	}
	else if (state == LIS_SCROLLING)
	{
		for (int i = 135, j = 134; i < 191; i++, j++)
		{
			memmove(&pix[160 * j + 88], &pix[160 * i + 88], 68);
		}

		if (++lines_scrolled == 8)
		{
			lines_scrolled = 0;
			state = LIS_NEXT_LINE;
		}
	}
	else if (state == LIS_WAITING_FOR_INPUT)
	{
		if (sfMouse_isButtonClicked(sfMouseLeft))
		{
			state = LIS_SCROLLING;
			lines_on_screen = 0;
		}
	}
	
	return;
}

static neuro_scene_id_t update(sfEvent *event)
{
	neuro_scene_id_t scene = NSID_LEVEL;

	update_cursor();

	switch (g_state)
	{
	case LS_INTRO:
		update_intro();
		break;

	case LS_DIALOG_WAIT_FOR_INPUT:
		wait_for_input();
		break;

	case LS_NORMAL:
		update_normal(event);
		break;

	default:
		break;
	}

	return scene;
}

static void deinit()
{
	drawing_control_remove_sprite_from_chain(SCI_LEVEL_BG);
	drawing_control_remove_sprite_from_chain(SCI_BACKGRND);
	drawing_control_remove_sprite_from_chain(SCI_CHARACTER);
	g_bih_string_ptr = NULL;
}

void setup_level_scene()
{
	g_scene.id = NSID_LEVEL;
	g_scene.init = init;
	g_scene.update = update;
	g_scene.deinit = deinit;
}
