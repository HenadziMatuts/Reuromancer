#ifndef _DATA_H
#define _DATA_H

#include <neuro_routines.h>

#pragma pack(push, 1)

/* 0x004C */
extern int g_level_n;

extern uint8_t g_004e[8];

typedef struct neuro_ui_buttons_t {
	neuro_button_t inventory;
	neuro_button_t pax;
	neuro_button_t dialog;
	neuro_button_t skills;
	neuro_button_t chip;
	neuro_button_t disk;
	neuro_button_t date;
	neuro_button_t time;
	neuro_button_t cash;
	neuro_button_t con;
} neuro_ui_buttons_t;

/* 0x1FA2 */
extern neuro_ui_buttons_t g_ui_buttons;

typedef struct neuro_inventory_buttons_t {
	neuro_button_t item_page_exit;
	neuro_button_t exit;
	neuro_button_t more;
} neuro_inventory_buttons_t;

/* 0x21FA */
extern neuro_inventory_buttons_t g_inv_buttons;

typedef struct neuro_inventory_discard_buttons_t {
	neuro_button_t yes;
	neuro_button_t no;
} neuro_inventory_discard_buttons_t;

/* 0x2236 */
extern neuro_inventory_discard_buttons_t g_inv_disc_buttons;

/* 0x25B4 */
extern ui_panel_mode_t g_ui_panel_mode;

typedef struct neuro_vm_state_t {
	uint8_t level;
	uint8_t flag;
	uint8_t mark;
	uint16_t vm_next_op_addr;
	uint8_t var_1;
	uint8_t var_2;
} neuro_vm_state_t;

typedef struct neuro_inventory_t {
	uint8_t items[128];
	uint8_t software[128];
} neuro_inventory_t;

typedef struct x3f85_t {
	neuro_vm_state_t vm_state[35];
	uint8_t vm_state_end;        // 0x407A
	uint8_t x407b[348];
	neuro_inventory_t inventory; // 0x41D7
	uint8_t x42d7[402];
} x3f85_t;

/* wraps 16-bit operation addresses */
typedef struct x3f85_wrapper_t {
	uint8_t *vm_next_op_addr[35];
	x3f85_t *x3f85;
} x3f85_wrapper_t;

extern x3f85_t g_3f85;
extern x3f85_wrapper_t g_3f85_wrapper;

typedef enum jumps_t {
	JE, JNE, JL, JGE
} jumps_t;

/* 0x4469 */
extern char *g_inventory_item_names[];

/* 0x475B */
extern uint8_t g_inventory_item_operations[128];

extern jumps_t g_4b9d[4];

typedef struct x4bae_t {
	uint8_t x4bae[16];
	uint8_t x4bbe;
	uint8_t x4bbf;
	uint16_t active_item;
	uint32_t cash_withdrawal; // 0x4BC2
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

extern x4bae_t g_4bae;

/* Kind of "Window", 0xC91E */
typedef struct neuro_window_t {
	uint16_t left;        // 0xC91E
	uint16_t top;         // 0xC920
	uint16_t right;       // 0xC922
	uint16_t bottom;      // 0xC924
	uint16_t mode;        // 0xC926
	uint16_t c928;
	uint8_t c92a[4];
	uint16_t total_items; // 0xC92E
	uint16_t item[10];    // 0xC930
	uint16_t c944;
} neuro_window_t;

/* Wraps 16-bit addresses of the button area structs */
typedef struct neuro_window_wrapper_t {
	uint8_t *window_item[10];
	neuro_window_t *window;
} neuro_window_wrapper_t;

/* 0xA59E, 0xA5C6, 0xA5EE */
extern neuro_window_t g_a59e[3];
extern neuro_window_wrapper_t g_a59e_wrapper[3];

extern uint8_t g_a61a;

extern uint16_t g_a86a;

typedef struct a8e0_t {
	uint16_t a8e0[4];
	uint8_t bih[12288]; // 0xA8E8
} a8e0_t;

extern a8e0_t g_a8e0;
extern bih_hdr_wrapper_t g_bih_wrapper;

/* 0xC91E */
extern neuro_window_t g_neuro_window;
extern neuro_window_wrapper_t g_neuro_window_wrapper;

extern uint8_t g_c946;

#pragma pack(pop)
#endif
