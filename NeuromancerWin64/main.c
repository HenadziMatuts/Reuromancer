#include "globals.h"
#include "resource_manager.h"
#include "scene_control.h"
#include "drawing_control.h"
#include <neuro_routines.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <SFML\Graphics.h>

/*
 * Video mode.
 */
static sfVideoMode g_mode = {
	640, 400, 32
};

/*
 * Main window.
 */
static sfRenderWindow *g_window = NULL;

/*
 * Texture to be mapped on screen;
 */
static sfTexture *g_texture = NULL;

/*
 * Texture scale factors.
 */
static float g_scale_x = 0;
static float g_scale_y = 0;

/*
 * Timer.
 */
sfClock *g_timer = NULL;

int cursor_menu_dialog_item_hit_test(int item, neuro_menu_t *dialog)
{
	return (g_sprite_chain[SCI_CURSOR].left > dialog->items[item].left &&
		g_sprite_chain[SCI_CURSOR].left < dialog->items[item].right &&
		g_sprite_chain[SCI_CURSOR].top > dialog->items[item].top &&
		g_sprite_chain[SCI_CURSOR].top < dialog->items[item].bottom) ? 1 : 0;
}

void update_cursor()
{
	sfVector2i mouse_pos = sfMouse_getPositionRenderWindow(g_window);

	if (mouse_pos.x < 0)
	{
		mouse_pos.x = 0;
	}
	else if (mouse_pos.x >(int)g_mode.width)
	{
		mouse_pos.x = g_mode.width;
	}
	if (mouse_pos.y < 0)
	{
		mouse_pos.y = 0;
	}
	else if (mouse_pos.y >(int)g_mode.height)
	{
		mouse_pos.y = g_mode.height;
	}

	float mouse_pos_x = (mouse_pos.x / g_scale_x);
	float mouse_pos_y = (mouse_pos.y / g_scale_y);

	g_sprite_chain[SCI_CURSOR].left = (uint16_t)mouse_pos_x;
	g_sprite_chain[SCI_CURSOR].top = (uint16_t)mouse_pos_y;
}

sfKeyCode handle_sfml_text_input(uint32_t u32_char, char *string, uint32_t size, int digits_only)
{
	size_t l = strlen(string);

	if ((digits_only && (u32_char >= 0x30 && u32_char <= 0x39)) ||
		(!digits_only && (u32_char >= 0x20 && u32_char <= 0x7e)))
	{
		/* printable ascii */
		if (l + 1 >= size)
		{
			return sfKeyUnknown;
		}

		string[l] = (char)u32_char;
		string[l + 1] = 0;
	}
	else if (u32_char == 0x08)
	{
		/* backspace */
		if (l == 0)
		{
			return sfKeyUnknown;
		}

		string[l - 1] = 0;

		return sfKeyBack;
	}
	else if (u32_char == 0x0d)
	{
		/* return */
		return sfKeyReturn;
	}

	return sfKeyCount;
}

static void render()
{
	drawing_control_draw_sprite_chain_to_vga();

	sfTexture_updateFromPixels(g_texture, g_vga, 320, 200, 0, 0);

	sfSprite *sprite = sfSprite_create();
	sfVector2f scale = { g_scale_x, g_scale_y };

	sfSprite_setTexture(sprite, g_texture, 1);
	sfSprite_setScale(sprite, scale);

	sfRenderWindow_clear(g_window, sfBlack);

	sfRenderWindow_drawSprite(g_window, sprite, NULL);

	sfRenderWindow_display(g_window);

	sfSprite_destroy(sprite);
}

static void reset()
{
	drawing_control_flush_vga();
	drawing_control_flush_sprite_chain();

	g_texture = sfTexture_create(320, 200);

	g_scale_x = (float)g_mode.width / 320;
	g_scale_y = (float)g_mode.height / 200;
}

int main(int argc, char *argv[])
{
	sfEvent event;

	g_window = sfRenderWindow_create(g_mode,
		"NeuromancerWin32", sfClose, NULL);
	if (!g_window)
	{
		return -1;
	}
	sfRenderWindow_setMouseCursorVisible(g_window, sfFalse);

	g_timer = sfClock_create();

	reset();
	resource_manager_init();
	scene_control_setup_scene(NSID_MAIN_MENU);

	while (sfRenderWindow_isOpen(g_window))
	{
		sfBool was_event = sfFalse;

		while (sfRenderWindow_pollEvent(g_window, &event))
		{
			was_event = sfTrue;
			if (event.type == sfEvtClosed)
			{
				sfRenderWindow_close(g_window);
			}
			else if (g_scene.handle_input)
			{
				g_scene.handle_input(&event);
			}
		}

		if (!was_event)
		{
			/* no event */
			event.type = sfEvtCount;
		}

		neuro_scene_id_t scene = g_scene.update(&event);
		if (scene != g_scene.id)
		{
			scene_control_setup_scene(scene);
		}

		render();
	}

	g_scene.deinit();
	resource_manager_deinit();
	
	sfClock_destroy(g_timer);
	sfRenderWindow_destroy(g_window);
	sfTexture_destroy(g_texture);
	return 0;
}
