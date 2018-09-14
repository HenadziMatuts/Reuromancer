#ifndef _DATA_H
#define _DATA_H

#include "drawing_control.h"
#include "neuro86.h"
#include <neuro_routines.h>

#pragma pack(push, 1)

/* seg000:0BCF */
uint8_t neuro_cb(uint16_t sp);

typedef struct pax_news_hdr_t {
	uint16_t addr;
	uint8_t val;
	uint8_t flag;
	char date[9];
	char subject[27];
} pax_news_hdr_t;

typedef struct pax_board_msg_hdr_t {
	uint16_t addr;
	uint8_t val;
	uint8_t flag;
	char date[9];
	char to[13];
	char from[14];
} pax_board_msg_hdr_t;

typedef struct seg004_layout_t {
	pax_news_hdr_t pax_news[21];
	pax_board_msg_hdr_t pax_board_msg[22];
	uint8_t padding[8];
} seg004_layout_t;

typedef struct seg009_layout_t {
	uint8_t padding[2];
	uint8_t cursors[398];
} seg009_layout_t;

typedef struct seg010_layout_t {
	uint8_t padding[2];
	uint8_t background[32062];
} seg010_layout_t;

typedef struct seg011_layout_t {
	uint8_t padding[2];
	uint8_t data[41998];
} seg011_layout_t;

typedef struct seg012_layout_t {
	uint8_t padding[10];
	uint8_t data[5638];
} seg012_layout_t;

typedef struct seg013_layout_t {
	uint8_t padding[2];
	uint8_t spritesheet[61998];
} seg013_layout_t;

typedef struct seg014_layout_t {
	uint8_t padding[2];
	uint8_t dialog_bubbles[607];
} seg014_layout_t;

typedef struct seg015_layout_t {
	uint8_t padding[10];
	imh_hdr_t hdr;
	uint8_t pixels[17038];
} seg015_layout_t;

typedef struct seg016_layout_t {
	uint8_t roompos[1160];
	uint8_t anh[22808];
} seg016_layout_t;

extern seg004_layout_t g_seg004; // seg004: 0000 - 06BF
extern seg009_layout_t g_seg009; // seg009: 0000 - 018F
extern seg010_layout_t g_seg010; // seg010: 0000 - 7D3F
extern seg011_layout_t g_seg011; // seg011: 0000 - A40F
extern seg012_layout_t g_seg012; // seg012: 0000 - 160F
extern seg013_layout_t g_seg013; // seg013: 0000 - F22F
extern seg014_layout_t g_seg014; // seg014: 0000 - 025F
extern seg015_layout_t g_seg015; // seg015: 0000 - 329F
extern seg016_layout_t g_seg016; // seg016: 0000 - 5D9F
extern uint8_t g_stack[2048];       //     ss: 0000 - 2047

/* 0x004C */
extern int g_level_n;

/* 0x004E */
extern uint8_t g_visited_levels_bitstring[8];

/* 0x1516 */
extern uint16_t g_paused;

/* 0x152C */
extern uint16_t g_update_hold;

/* 0x1E46 */
extern int16_t g_exit_point;

/* 0x1E4A */
extern uint16_t g_load_level_vm;

typedef struct neuro_ui_buttons_t {
	neuro_button_t inventory, pax, dialog;
	neuro_button_t skills, chip, disk;
	neuro_button_t date, time, cash, constitution;
} neuro_ui_buttons_t;

/* 0x1FA2 */
extern neuro_ui_buttons_t g_ui_buttons;

typedef struct neuro_pax_buttons_t {
	neuro_button_t exit, user_info, banking;
	neuro_button_t news, board;
} neuro_pax_buttons_t;

/* 0x201A */
extern neuro_pax_buttons_t g_pax_buttons;

typedef struct neuro_pax_banking_buttons {
	neuro_button_t exit, download, upload, transactions;
} neuro_pax_banking_buttons;

/* 0x2176 */
extern neuro_pax_banking_buttons g_pax_banking_buttons;

typedef struct neuro_pax_info_menu_buttons {
	neuro_button_t exit, more;
} neuro_pax_info_menu_buttons;

/* 0x21A6 */
extern neuro_pax_info_menu_buttons g_pax_info_menu_buttons;

typedef struct neuro_pax_msg_board_menu_buttons {
	neuro_button_t exit, view, send;
} neuro_pax_msg_board_menu_buttons;

/* 0x21BE */
extern neuro_pax_msg_board_menu_buttons g_pax_board_menu_buttons;

typedef struct neuro_pax_board_send_msg_buttons {
	neuro_button_t yes, no;
} neuro_pax_board_send_msg_buttons;

/* 0x21E2 */
extern neuro_pax_board_send_msg_buttons g_pax_board_send_msg_buttons;

typedef struct neuro_inventory_buttons_t {
	neuro_button_t item_page_exit, exit, more;
} neuro_inventory_buttons_t;

/* 0x21FA */
extern neuro_inventory_buttons_t g_inv_buttons;

typedef struct neuro_inventory_discard_buttons_t {
	neuro_button_t yes, no;
} neuro_inventory_discard_buttons_t;

/* 0x2236 */
extern neuro_inventory_discard_buttons_t g_inv_disc_buttons;

/* 0x25B4 */
extern ui_panel_mode_t g_ui_panel_mode;

extern uint8_t g_3b94[64];

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

typedef struct level_info_t {
	uint8_t first_dialog_reply;
	uint8_t total_dialog_replies;
	uint8_t level_transitions[4];
} level_info_t;

typedef struct x3f85_t {
	neuro_vm_state_t vm_state[35];
	uint8_t vm_state_end;        // 0x407A
	level_info_t level_info[58]; // 0x407B
	neuro_inventory_t inventory; // 0x41D7
	uint8_t skills[16]; // 0x42D7
	uint8_t x42e7[386]; 
} x3f85_t;

extern x3f85_t g_3f85;

typedef enum jumps_t {
	JE, JNE, JL, JGE
} jumps_t;

/* 0x475B */
extern uint8_t g_inventory_item_operations[128];

extern jumps_t g_4b9d[4];

typedef struct transaction_record_t {
	uint16_t op;
	uint32_t amount;
} transaction_record_t;

typedef struct x4bae_t {
	uint8_t x4bae[16];
	uint8_t active_dialog_reply; // 0x4BBE
	uint8_t x4bbf;
	uint16_t active_item;     // 0x4BC0
	uint32_t cash_withdrawal; // 0x4BC2
	uint16_t time_m;  // 0x4BC6
	uint8_t time_h;   // 0x4BC8
	uint8_t date_day; // 0x4BC9
	uint8_t x4bca[2]; // padding
	uint8_t x4bcc;
	uint8_t x4bcd[38];
	uint8_t x4bf3;
	uint8_t active_skill; // 0x4BF4
	uint8_t active_skill_level; // 0x4BF5
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
	uint8_t gas_mask_is_on;
	uint8_t x4c1a;
	uint16_t x4c1b;
	uint8_t x4c1d[4];
	uint16_t msg_to_armitage_sent;
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
	uint8_t sold_body_parts_bitstring[3];
	uint16_t x4c87;
	uint32_t bank_account; // 0x4C89
	uint8_t bank_last_transacton_record_index; // 0x4C8D
	uint16_t x4c8e;
	uint16_t x4c90;
	char name[13];    // 0x4C92
	uint16_t constitution; // 0x4C9F
	uint16_t level_n; // 0x4CA1
	uint16_t roompos_x; // 0x4CA3
	uint16_t roompos_y; // 0x4CA5
	uint16_t x4ca7;
	transaction_record_t bank_transaction_record[4]; // 0x4CA9
	uint16_t ui_type; // 0x4CC1
	uint16_t x4cc3;
	uint16_t x4cc5;
	uint16_t x4cc7;
	uint16_t x4cc9;
	uint8_t x4ccb;
	uint8_t x4ccc;
	uint8_t x4ccd;
	uint8_t x4cce;
	uint16_t frame_sc_index; // 0x4CCF
} x4bae_t;

extern x4bae_t g_4bae;

extern uint16_t g_6a40;
extern uint16_t g_6a7a;

/* 0x8CDC */
extern uint16_t g_dlg_with_user_input;

extern uint8_t g_8cee[4][4];

extern uint8_t g_a61a;
extern level_info_t *g_a642;
extern uint16_t g_a86a;
extern uint8_t g_a8ae[4];

typedef struct a8e0_t {
	uint16_t a8e0[4];
	union {
		bih_hdr_t hdr;
		uint8_t bytes[8240];
	} bih; // 0xA8E8
} a8e0_t;

extern a8e0_t g_a8e0;

extern uint8_t g_c946;

#pragma pack(pop)
#endif
