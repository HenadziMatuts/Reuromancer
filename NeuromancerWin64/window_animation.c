#include "window_animation.h"
#include "globals.h"
#include "drawing_control.h"
#include <neuro_routines.h>
#include <string.h>

typedef struct window_animation_instance_t {
	window_animation_type_t type;
	union {
		window_folding_data_t folding;
	} data;
} window_animation_instance_t;

static window_animation_instance_t g_animation = {
	.type = WA_TYPE_UNKNOWN,
};

void window_animation_setup(window_animation_type_t type, void *data)
{
	switch (type) {
	case WA_TYPE_WINDOW_FOLDING:
		g_animation.type = type;
		memmove(&g_animation.data.folding, data, sizeof(window_folding_data_t));
		break;

	default:
		g_animation.type = WA_TYPE_UNKNOWN;
		break;
	}
}

static window_animation_event_t update_window_folding(window_folding_data_t *data)
{
	static int frame = 0;
	static int elapsed = 0;

	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= (int)data->frame_cap)
	{
		return WA_EVENT_NO_EVENT;
	}
	elapsed = passed;

	if (frame == data->total_frames)
	{
		frame = 0;
		return WA_EVENT_COMPLETED;
	}

	window_folding_frame_data_t *frame_data = &data->frame_data[frame++];

	build_text_frame(frame_data->h, frame_data->w, (imh_hdr_t*)data->pixels);
	drawing_control_add_sprite_to_chain(data->sprite_chain_index,
		frame_data->l, frame_data->t, data->pixels, 1);

	return WA_EVENT_NO_EVENT;
}

window_animation_event_t window_animation_update()
{
	window_animation_event_t evt;

	switch (g_animation.type) {
	case WA_TYPE_WINDOW_FOLDING:
		evt = update_window_folding(&g_animation.data.folding);
		break;

	default:
		return WA_EVENT_COMPLETED;
	}

	if (evt == WA_EVENT_COMPLETED)
	{
		g_animation.type = WA_TYPE_UNKNOWN;
	}

	return evt;
}
