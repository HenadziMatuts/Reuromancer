#ifndef _DRAWING_CONTROL
#define _DRAWING_CONTROL

#include <neuro_routines.h>
#include <stdint.h>

extern uint8_t g_vga[320 * 200 * 4];
extern sprite_layer_t g_sprite_chain[SCI_TOTAL];

void drawing_control_add_sprite_to_chain(int n,
	uint32_t left, uint32_t top, uint8_t *sprite, int opaque);
void drawing_control_remove_sprite_from_chain(int n);
void drawing_control_flush_sprite_chain();

void drawing_control_draw_sprite_chain_to_vga();
void drawing_control_flush_vga();

#endif
