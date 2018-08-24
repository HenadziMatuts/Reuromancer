#include "window_animation.h"
#include "globals.h"
#include "drawing_control.h"
#include <neuro_routines.h>
#include <string.h>

fn_window_animation_renderer_hook g_window_animation_renderer_hook = NULL;

typedef struct _screen_fade_data_t {
	int32_t alpha;
	screen_fade_data_t user_data;
} _screen_fade_data_t;

typedef struct window_animation_instance_t {
	window_animation_type_t type;
	union {
		window_folding_data_t folding;
		_screen_fade_data_t fade;
	} data;
} window_animation_instance_t;

static window_animation_instance_t g_animation = {
	.type = WA_TYPE_UNKNOWN,
};

static void screen_fade_renderer_hook(sfRenderWindow *window, sfVector2f *scale)
{
	sfRectangleShape *fader = sfRectangleShape_create();
	sfVector2f size = { 320, 240 };
	sfColor color = { 0, 0, 0, g_animation.data.fade.alpha };

	sfRectangleShape_setFillColor(fader, color);
	sfRectangleShape_setSize(fader, size);
	sfRectangleShape_setScale(fader, *scale);

	sfRenderWindow_drawShape(window, (sfShape*)fader, NULL);
	sfRectangleShape_destroy(fader);
}

static window_animation_event_t update_screen_fade(_screen_fade_data_t *_data)
{
	static int elapsed = 0;

	screen_fade_data_t *data = &_data->user_data;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= (int)data->frame_cap)
	{
		return WA_EVENT_NO_EVENT;
	}
	elapsed = passed;

	if (data->direction == FADE_IN)
	{
		_data->alpha -= data->step;
		if (_data->alpha <= 0)
		{
			_data->alpha = 0;
			return WA_EVENT_COMPLETED;
		}
	}
	else
	{
		_data->alpha += data->step;
		if (_data->alpha >= 255)
		{
			_data->alpha = 255;
			return WA_EVENT_COMPLETED;
		}
	}

	return WA_EVENT_NO_EVENT;
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

	case WA_TYPE_SCREEN_FADE:
		evt = update_screen_fade(&g_animation.data.fade);
		break;

	default:
		return WA_EVENT_COMPLETED;
	}

	if (evt == WA_EVENT_COMPLETED)
	{
		g_animation.type = WA_TYPE_UNKNOWN;
		g_window_animation_renderer_hook = NULL;
	}

	return evt;
}

void window_animation_setup(window_animation_type_t type, void *data)
{
	switch (type) {
	case WA_TYPE_WINDOW_FOLDING:
		g_animation.type = type;
		memmove(&g_animation.data.folding, data, sizeof(window_folding_data_t));
		break;

	case WA_TYPE_SCREEN_FADE:
		g_animation.type = type;
		memmove(&g_animation.data.fade.user_data, data, sizeof(screen_fade_data_t));
		g_animation.data.fade.alpha =
			(g_animation.data.fade.user_data.direction == FADE_IN) ? 255 : 0;
		g_window_animation_renderer_hook = screen_fade_renderer_hook;
		break;

	default:
		g_animation.type = WA_TYPE_UNKNOWN;
		break;
	}
}
