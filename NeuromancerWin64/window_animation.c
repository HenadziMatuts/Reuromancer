#include "window_animation.h"
#include "globals.h"
#include "data.h"
#include "drawing_control.h"
#include "neuro_window_control.h"
#include "resource_manager.h"
#include <neuro_routines.h>
#include <string.h>
#include <assert.h>

fn_window_animation_renderer_hook g_window_animation_renderer_hook = NULL;

typedef struct _page_turning_data_t {
	page_turning_data_t user_data;
	uint16_t l, t, w, h;
	int16_t height;
} _page_turning_data_t;

typedef struct _text_scrolling_data_t {
	text_scrolling_data_t user_data;
	char line[64];
	uint16_t line_length;
	uint16_t max_lines;
	uint16_t l, w, t, b;
	uint8_t *pixels;
} _text_scrolling_data_t;

typedef struct _screen_fading_data_t {
	screen_fading_data_t user_data;
	int32_t alpha;
} _screen_fading_data_t;

typedef struct window_animation_instance_t {
	window_animation_type_t type;
	union {
		window_folding_data_t folding;
		_screen_fading_data_t fading;
		_text_scrolling_data_t scrolling;
		_page_turning_data_t turning;
	} data;
} window_animation_instance_t;

static window_animation_instance_t g_animation = {
	.type = WA_TYPE_UNKNOWN,
};

static void page_turning_renderer_hook(sfRenderWindow *window, sfVector2f *scale)
{
	_page_turning_data_t *data = &g_animation.data.turning;

	sfRectangleShape *rect = sfRectangleShape_create();
	sfVector2f pos = { data->l, data->t };
	sfVector2f size = { data->w, data->height };
	sfColor color = { 255, 255, 255, 255 };

	sfRectangleShape_setSize(rect, size);
	sfRectangleShape_setPosition(rect, pos);
	sfRectangleShape_setFillColor(rect, color);
	sfRectangleShape_setScale(rect, *scale);

	sfRenderWindow_drawShape(window, (sfShape*)rect, NULL);
	sfRectangleShape_destroy(rect);
}

static window_animation_event_t update_page_turning(_page_turning_data_t *_data)
{
	static int elapsed = 0;
	static int dir = 0;

	page_turning_data_t *data = &_data->user_data;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= (int)data->frame_cap)
	{
		return WA_EVENT_NO_EVENT;
	}
	elapsed = passed;

	if (dir == 0)
	{
		_data->height += data->step;

		if (_data->height >= _data->h)
		{
			_data->height = _data->h;
			dir = 1;
			data->redraw();
		}
	}
	else
	{
		_data->height -= data->step;
		_data->t += data->step * 2;

		if (_data->height <= 0)
		{
			dir = 0;
			data->end();
			return WA_EVENT_COMPLETED;
		}
	}

	return WA_EVENT_NO_EVENT;
}

static window_animation_event_t update_text_scrolling(_text_scrolling_data_t *_data)
{
	static int lines_on_screen = 0, lines_scrolled = 0;
	static int next_line = 1;
	static int elapsed = 0;

	text_scrolling_data_t *data = &_data->user_data;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= (int)data->frame_cap)
	{
		return WA_EVENT_NO_EVENT;
	}
	elapsed = passed;

	if (next_line)
	{
		char *line = _data->line;
		int has_more = extract_line();

		neuro_window_draw_string(has_more ? line : " ", 0);
		next_line = 0;

		if (!has_more)
		{
			next_line = 1;
			lines_on_screen = 0;
			return WA_EVENT_COMPLETED;
		}
		else if (++lines_on_screen == _data->max_lines)
		{
			lines_on_screen = 0;
			return WA_EVENT_WAIT_FOR_INPUT;
		}
	}
	else
	{
		uint8_t *pix = _data->pixels + sizeof(imh_hdr_t);

		for (int i = _data->t + 1, j = _data->t; i < _data->b; i++, j++)
		{
			memmove(&pix[160 * j + _data->l], &pix[160 * i + _data->l], _data->w);
		}

		if (++lines_scrolled == 8)
		{
			lines_scrolled = 0;
			next_line = 1;
		}
	}

	return WA_EVENT_NO_EVENT;
}

static void screen_fade_renderer_hook(sfRenderWindow *window, sfVector2f *scale)
{
	sfRectangleShape *fader = sfRectangleShape_create();
	sfVector2f size = { 320, 240 };
	sfColor color = { 0, 0, 0, g_animation.data.fading.alpha };

	sfRectangleShape_setFillColor(fader, color);
	sfRectangleShape_setSize(fader, size);
	sfRectangleShape_setScale(fader, *scale);

	sfRenderWindow_drawShape(window, (sfShape*)fader, NULL);
	sfRectangleShape_destroy(fader);
}

static window_animation_event_t update_screen_fading(_screen_fading_data_t *_data)
{
	static int elapsed = 0;

	screen_fading_data_t *data = &_data->user_data;
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

	case WA_TYPE_SCREEN_FADING:
		evt = update_screen_fading(&g_animation.data.fading);
		break;

	case WA_TYPE_TEXT_SCROLLING:
		evt = update_text_scrolling(&g_animation.data.scrolling);
		break;

	case WA_TYPE_PAGE_TURNING:
		evt = update_page_turning(&g_animation.data.turning);
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

static void prepare_page_turning()
{
	switch (g_neuro_window.mode) {
	case NWM_PAX:
		g_animation.data.turning.l = 8;
		g_animation.data.turning.t = 12;
		g_animation.data.turning.w = 304;
		g_animation.data.turning.h = 94;
		g_animation.data.turning.height = 0;
		break;

	default:
		assert(0);
	}
}

static void prepare_text_scrolling()
{
	switch (g_neuro_window.mode) {
	case NWM_NEURO_UI:
		g_animation.data.scrolling.line_length = 17;
		g_animation.data.scrolling.max_lines = 7;
		g_animation.data.scrolling.l = 88;
		g_animation.data.scrolling.w = 68;
		g_animation.data.scrolling.t = 134;
		g_animation.data.scrolling.b = 191;
		g_animation.data.scrolling.pixels = g_seg010.background;
		memset(g_animation.data.scrolling.line, 0, 64);
		extract_line_prepare(g_animation.data.scrolling.user_data.text,
			g_animation.data.scrolling.line, 17);
		break;

	case NWM_PAX:
		g_animation.data.scrolling.line_length = 38;
		g_animation.data.scrolling.max_lines = 9;
		g_animation.data.scrolling.l = 8;
		g_animation.data.scrolling.w = 304;
		g_animation.data.scrolling.t = 16;
		g_animation.data.scrolling.b = 97;
		g_animation.data.scrolling.pixels = g_seg011.data;
		memset(g_animation.data.scrolling.line, 0, 64);
		extract_line_prepare(g_animation.data.scrolling.user_data.text,
			g_animation.data.scrolling.line, 38);
		break;

	default:
		assert(0);
	}
}

void window_animation_setup(window_animation_type_t type, void *data)
{
	g_animation.type = type;

	switch (type) {
	case WA_TYPE_WINDOW_FOLDING:
		memmove(&g_animation.data.folding, data, sizeof(window_folding_data_t));
		break;

	case WA_TYPE_SCREEN_FADING:
		memmove(&g_animation.data.fading.user_data, data, sizeof(screen_fading_data_t));
		g_animation.data.fading.alpha =
			(g_animation.data.fading.user_data.direction == FADE_IN) ? 255 : 0;
		g_window_animation_renderer_hook = screen_fade_renderer_hook;
		break;

	case WA_TYPE_TEXT_SCROLLING:
		memmove(&g_animation.data.scrolling.user_data, data, sizeof(text_scrolling_data_t));
		prepare_text_scrolling();
		break;

	case WA_TYPE_PAGE_TURNING:
		memmove(&g_animation.data.turning.user_data, data, sizeof(page_turning_data_t));
		prepare_page_turning();
		g_window_animation_renderer_hook = page_turning_renderer_hook;
		break;

	default:
		g_animation.type = WA_TYPE_UNKNOWN;
		break;
	}
}
