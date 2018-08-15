/**
 * Copyright (c) 2018 Henadzi Matuts
 */

#ifndef _NEURO_ROUTINES_H
#define _NEURO_ROUTINES_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LIBNEURO
#define LIBNEUROAPI __declspec(dllexport)
#else
#define LIBNEUROAPI __declspec(dllimport)
#endif

#pragma pack(push, 2)

typedef struct imh_hdr_t {
	uint16_t dx;
	uint16_t dy;
	uint16_t width;
	uint16_t height;
} imh_hdr_t;

typedef struct resource_t {
	int file;
	char *name;
	long offset;
	size_t size;
} resource_t;

typedef enum spite_chain_index_t {
	SCI_CURSOR = 0,
	SCI_DIALOG = 2,
	SCI_DIALOG_BUBBLE = 3,
	SCI_CHARACTER = 4,
	SCI_LEVEL_BG = 9,
	SCI_BACKGRND = 10,
	SCI_TOTAL = 11
} spite_chain_index_t;

typedef struct sprite_layer_t {
	uint8_t flags; /* 1st bit - active, 5th bit - opaque */
	uint8_t update; /* =1 - update; =2 - update, then delete */
	uint16_t left;
	uint16_t top;
	uint16_t new_left;
	uint16_t new_top;
	imh_hdr_t sprite_hdr;
	uint8_t *sprite_pixels;
	imh_hdr_t _sprite_hdr;
	uint8_t *_sprite_pixels;
} sprite_layer_t;

typedef struct neuro_button_t {
	uint16_t left;
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
	uint16_t code; /* index? */
	char label;
	uint8_t padding;
} neuro_button_t;

typedef struct neuro_menu_t {
	uint16_t left;
	uint16_t top;
	uint16_t right;
	uint16_t bottom;

	uint16_t inner_left;
	uint16_t inner_top;
	uint16_t inner_right;
	uint16_t inner_bottom;

	uint16_t _inner_left;
	uint16_t _inner_top;
	uint16_t _inner_right;
	uint16_t _inner_bottom;

	uint16_t flags;

	uint16_t items_count;
	neuro_button_t items[16];

	uint16_t width;
	uint8_t *pixels;
} neuro_menu_t;

typedef struct bih_hdr_t {
	uint16_t cb_offt;                 // a8e8
	uint16_t cb_segt;                 // a8ea
	uint16_t ctrl_struct_addr;        // a8ec
	uint16_t text_offset;             // a8ee
	uint16_t bytecode_array_offt[3];  // a8f0
	uint16_t init_obj_code_offt[3];   // a8f6
	uint16_t unknown[10];             // a8fc
	/* the rest of bih file */
} bih_hdr_t;

/* wraps bih_hdr_t addresses */
typedef struct bih_hdr_wrapper_t {
	uint8_t *cb_addr;
	uint8_t *ctrl_struct_addr;
	bih_hdr_t *bih;
} bih_hdr_wrapper_t;

typedef struct bg_animation_control_table_t {
	uint16_t total_frames;
	uint8_t *first_frame_data;
	uint8_t *first_frame_bytes;
	uint16_t sleep;
	uint16_t curr_frame;
} bg_animation_control_table_t;

typedef struct anh_hdr_t {
	uint16_t anh_entries;
	/* first entry hdr */
} anh_hdr_t;

typedef struct anh_entry_hdr_t {
	uint16_t entry_size;
	uint16_t total_frames;
	/* anh_frame_data_t first_frame_data */
	/* another frames data */
	/* anh_frame_hdr first_frame_hdr */
	/* another frames */
} anh_entry_hdr_t;

typedef struct anh_frame_data_t {
	uint16_t frame_sleep;
	uint16_t frame_offset;
} anh_frame_data_t;

typedef struct anh_frame_hdr {
	uint8_t bg_x_offt;
	uint8_t bg_y_offt;
	uint8_t frame_width;
	uint8_t frame_height;
	/* rle encoded frame bytes */
} anh_frame_hdr;

typedef struct neuro_savegame_t {
	uint8_t unknown_area_1[1252];
	uint8_t unknown_area_2[291];
	uint8_t unknown_area_3[8];
	uint8_t spritechain_area[374];
	uint8_t unknown_area_4[678];
	uint8_t dialog_area[226];
	uint8_t unknown_area_5[120];
	uint8_t unknown_area_6[40];
} neuro_savegame_t;

typedef enum ui_panel_mode_t {
	UI_PM_CASH = 0,
	UI_PM_CON,
	UI_PM_TIME,
	UI_PM_DATE
} ui_panel_mode_t;

/*
 * Resource tables.
 */
LIBNEUROAPI extern resource_t g_res_imh[29];
LIBNEUROAPI extern resource_t g_res_pic[56];
LIBNEUROAPI extern resource_t g_res_bih[10];
LIBNEUROAPI extern resource_t g_res_anh[22];

/*
 * Font table.
 */
LIBNEUROAPI extern uint8_t cp437_font[1024];

/*
 * Decompression routines.
 */
LIBNEUROAPI int decompress_imh(uint8_t *src, uint8_t *dst);
LIBNEUROAPI int decompress_pic(uint8_t *src, uint8_t *dst);
LIBNEUROAPI int decompress_bih(uint8_t *src, uint8_t *dst);
LIBNEUROAPI int decompress_anh(uint8_t *src, uint8_t *dst);

LIBNEUROAPI int huffman_decompress(uint8_t *src, uint8_t *dst);
LIBNEUROAPI int decode_rle(uint8_t *src, uint32_t len, uint8_t *dst);

/*
 * Text routines.
 */
LIBNEUROAPI void build_character(char c, uint8_t *dst);
LIBNEUROAPI void build_string(char *string, uint32_t w, uint32_t h,
		uint32_t l, uint32_t t, uint8_t *dst);


LIBNEUROAPI void build_text_frame(uint32_t h, uint32_t w, imh_hdr_t *dst);

/*
 * Dialog box control.
 */
LIBNEUROAPI void build_neuro_menu_frame(neuro_menu_t *dialog,
					uint16_t left, uint16_t top, uint16_t w, uint16_t h,
					uint16_t flags, uint8_t *pixels);

LIBNEUROAPI void build_neuro_menu_text(neuro_menu_t *dialog,
					char *text, uint16_t x_offt, uint16_t y_offt);

LIBNEUROAPI void build_neuro_menu_item(neuro_menu_t *dialog,
					uint16_t x_offt, uint16_t y_offt, uint16_t w,
					uint16_t item_num, char c);

/*
 * Sound stuff.
 */
LIBNEUROAPI int build_track_waveform(int track_num, uint8_t *waveform, int len);

/*
 * Background animation.
 */
LIBNEUROAPI int bg_animation_init_tables(bg_animation_control_table_t *tables,
					uint8_t *decompd_anh);
LIBNEUROAPI void bg_animation_update(bg_animation_control_table_t *tables,
					uint16_t animations, uint8_t *working_area, uint8_t *bg_pixels);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif
