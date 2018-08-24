#ifndef _WINDOW_ANIMATION
#define _WINDOW_ANIMATION

#include <stdint.h>

typedef enum window_animation_type_t {
	WA_TYPE_UNKNOWN = -1,
	WA_TYPE_WINDOW_FOLDING = 0,
} window_animation_type_t;

typedef struct window_folding_frame_data_t {
	uint16_t w, h, l, t;
} window_folding_frame_data_t;

typedef struct window_folding_data_t {
	uint32_t total_frames;
	window_folding_frame_data_t *frame_data;
	uint32_t frame_cap;
	uint8_t *pixels;
	uint16_t sprite_chain_index;
} window_folding_data_t;

typedef enum window_animation_event_t {
	WA_EVENT_NO_EVENT = 0,
	WA_EVENT_COMPLETED
} window_animation_event_t;

void window_animation_setup(window_animation_type_t type, void *data);
window_animation_event_t window_animation_update();

#endif
