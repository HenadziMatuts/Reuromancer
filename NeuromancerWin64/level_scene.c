#include "globals.h"
#include "scene_control.h"
#include "character_control.h"
#include "bg_animation_control.h"
#include "drawing_control.h"
#include "resource_manager.h"
#include <string.h>
#include <assert.h>

typedef enum level_intro_state_t {
	LIS_NEXT_LINE = 0,
	LIS_SCROLLING,
	LIS_WAITING_FOR_INPUT,
} level_intro_state_t;

typedef enum level_state_t {
	LS_INTRO = 0,
	LS_NORMAL,
} level_state_t;

static int g_level_n = 1;
static uint8_t g_strings[12288];
static uint8_t *g_string_ptr = NULL;
static level_state_t g_state = LS_NORMAL;

static void init()
{
	char resource[32] = { 0, };
	memset(g_vga, 0, 320 * 200 * 4);
	g_state = LS_NORMAL;

	assert(resource_manager_load_resource("NEURO.IMH", g_background));
	drawing_control_add_sprite_to_chain(SCI_BACKGRND, 0, 0, g_background, 1);
	character_control_add_sprite_to_chain(156, 110, CD_DOWN);

	sprintf(resource, "R%d.PIC", g_level_n + 1);
	assert(resource_manager_load_resource(resource, g_level_bg + sizeof(imh_hdr_t)));
	drawing_control_add_sprite_to_chain(SCI_LEVEL_BG, 8, 8, g_level_bg, 1);

	sprintf(resource, "R%d.ANH", g_level_n + 1);
	if (resource_manager_load_resource(resource, g_roompos + 0x488))
	{
		bg_animation_control_init_tables(g_roompos + 0x488);
	}
	else
	{
		bg_animation_control_init_tables(NULL);
	}

	sprintf(resource, "R%d.BIH", g_level_n + 1);
	if (resource_manager_load_resource(resource, g_strings))
	{
		bih_hdr_t *hdr = (bih_hdr_t*)g_strings;
		g_string_ptr = g_strings + hdr->text_offset;

		g_state = LS_INTRO;
	}
}

static void update_normal()
{
	character_control_handle_input();
	character_control_update();

	/* UI */

	bg_animation_control_update();
}

static int extract_line(char *line)
{
	char *ps = g_string_ptr, *pl = line;
	int word_len = 0;

	while (1)
	{
		switch (*ps)
		{
		case ' ':
			if (word_len == 0)
			{
				g_string_ptr = ++ps;

				if (strlen(line) == 17)
				{
					break;
				}
				else
				{
					*pl++ = ' ';
				}
			}
			else if (strlen(line) + word_len <= 17)
			{
				memmove(pl, g_string_ptr, word_len);
				pl += word_len;
				g_string_ptr = ps;
				word_len = 0;
			}
			else
			{
				break;
			}
			continue;

		case '\r':
			if (word_len == 0)
			{
				g_string_ptr = ++ps;
				memset(line, 0x20, 17);
			}
			else if (strlen(line) + word_len <= 17)
			{
				memmove(pl, g_string_ptr, word_len);
				g_string_ptr = ++ps;
			}
			break;

		case '\0':
			if (word_len == 0)
			{
				g_string_ptr = ++ps;
				memset(line, 0x20, 17);
				return 1;
			}
			else if (strlen(line) + word_len <= 17)
			{
				memmove(pl, g_string_ptr, word_len);
				g_string_ptr = ps;
			}
			break;

		default:
			word_len++;
			ps++;
			continue;
		}

		break;
	}

	return 0;
}

static void update_intro()
{
	static level_intro_state_t state = LIS_NEXT_LINE;
	static int lines_on_screen = 0, lines_scrolled = 0;
	static int frame_cap_ms = 10;
	static int elapsed = 0;

	char line[18] = { 0, };
	uint8_t *pix = g_background + sizeof(imh_hdr_t);
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return;
	}
	elapsed = passed;

	if (state == LIS_NEXT_LINE)
	{
		int last = extract_line(line);

		build_string(line, 320, 200, 176, 182, pix);
		
		if (last)
		{
			g_state = LS_NORMAL;
			state = LIS_NEXT_LINE;
			lines_on_screen = 0;
			return;
		}

		state = (++lines_on_screen == 7)
			? LIS_WAITING_FOR_INPUT : LIS_SCROLLING;

	}
	else if (state == LIS_SCROLLING)
	{
		for (int i = 135, j = 134; i < 191; i++, j++)
		{
			memmove(&pix[160 * j + 88], &pix[160 * i + 88], 68);
		}

		if (++lines_scrolled == 8)
		{
			lines_scrolled = 0;
			state = LIS_NEXT_LINE;
		}
	}
	else if (state == LIS_WAITING_FOR_INPUT)
	{
		if (sfMouse_isButtonPressed(sfMouseLeft))
		{
			state = LIS_SCROLLING;
			lines_on_screen = 0;
		}
	}

	return;
}

static neuro_scene_id_t update(sfEvent *event)
{
	neuro_scene_id_t scene = NSID_LEVEL;

	update_cursor();

	switch (g_state)
	{
	case LS_INTRO:
		update_intro();
		break;

	case LS_NORMAL:
		update_normal();
		break;

	default:
		break;
	}

	return scene;
}

static void deinit()
{
	drawing_control_remove_sprite_from_chain(SCI_LEVEL_BG);
	drawing_control_remove_sprite_from_chain(SCI_BACKGRND);
	drawing_control_remove_sprite_from_chain(SCI_CHARACTER);
	g_string_ptr = NULL;
}

void setup_level_scene()
{
	g_scene.id = NSID_LEVEL;
	g_scene.init = init;
	g_scene.update = update;
	g_scene.deinit = deinit;
}
