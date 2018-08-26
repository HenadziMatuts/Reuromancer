#ifndef _WINDOW_ANIMATION
#define _WINDOW_ANIMATION

#include <stdint.h>
#include <SFML\Graphics.h>

typedef enum window_animation_type_t {
	WA_TYPE_UNKNOWN = -1,
	WA_TYPE_WINDOW_FOLDING = 0,
	WA_TYPE_SCREEN_FADING,
	WA_TYPE_TEXT_SCROLLING,
	WA_TYPE_PAGE_TURNING,
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

typedef enum sreen_fading_direction_t {
	FADE_IN = 0,
	FADE_OUT
} sreen_fading_direction_t;

typedef struct screen_fading_data_t {
	sreen_fading_direction_t direction;
	uint32_t step;
	uint32_t frame_cap;
} screen_fading_data_t;

typedef struct text_scrolling_data_t {
	char *text;
	uint32_t frame_cap;
} text_scrolling_data_t;

typedef void(*page_turning_cb_redraw)(void);
typedef void(*page_turning_cb_end)(void);

typedef struct page_turning_data_t {
	uint32_t step;
	uint32_t frame_cap;
	page_turning_cb_redraw redraw;
	page_turning_cb_end end;
} page_turning_data_t;

typedef enum window_animation_event_t {
	WA_EVENT_NO_EVENT = 0,
	WA_EVENT_WAIT_FOR_INPUT,
	WA_EVENT_COMPLETED
} window_animation_event_t;

typedef void (*fn_window_animation_renderer_hook)(sfRenderWindow*, sfVector2f*);
extern fn_window_animation_renderer_hook g_window_animation_renderer_hook;

void window_animation_setup(window_animation_type_t type, void *data);
window_animation_event_t window_animation_update();

#endif
