#include "globals.h"
#include "neuro_window_control.h"
#include "scene_real_world.h"
#include "resource_manager.h"
#include <neuro_routines.h>
#include <string.h>

typedef enum text_output_state_t {
	TOS_NEXT_LINE = 0,
	TOS_SCROLLING,
	TOS_WAIT_FOR_INPUT
} text_output_state_t;

static text_output_state_t g_state = TOS_NEXT_LINE;
static char *g_text = NULL;

void text_output_set_text(char *p)
{
	g_text = p;
}

void handle_text_output_input(sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		if (g_state == TOS_WAIT_FOR_INPUT)
		{
			g_state = TOS_SCROLLING;
		}
	}
}

real_world_state_t update_text_output()
{
	static int lines_on_screen = 0, lines_scrolled = 0;
	static int frame_cap_ms = 13;
	static int elapsed = 0;

	char line[18] = { 0, };
	uint8_t *pix = g_background + sizeof(imh_hdr_t);
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return RWS_TEXT_OUTPUT;
	}
	elapsed = passed;

	if (g_state == TOS_NEXT_LINE)
	{
		int last = extract_line(&g_text, line, 17);

		neuro_window_draw_string(line);

		if (last)
		{
			g_state = TOS_NEXT_LINE;
			lines_on_screen = 0;
			return RWS_NORMAL;
		}

		if (++lines_on_screen == 7)
		{
			g_state = TOS_WAIT_FOR_INPUT;
			lines_on_screen = 0;
		}
		else
		{
			g_state = TOS_SCROLLING;
		}
	}
	else if (g_state == TOS_SCROLLING)
	{
		for (int i = 135, j = 134; i < 191; i++, j++)
		{
			memmove(&pix[160 * j + 88], &pix[160 * i + 88], 68);
		}

		if (++lines_scrolled == 8)
		{
			lines_scrolled = 0;
			g_state = TOS_NEXT_LINE;
		}
	}

	return RWS_TEXT_OUTPUT;
}
