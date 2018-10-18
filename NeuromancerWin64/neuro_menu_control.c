#include "data.h"
#include "globals.h"
#include "drawing_control.h"
#include "neuro_menu_control.h"
#include "resource_manager.h"
#include "address_translator.h"
#include <assert.h>
#include <string.h>

/* 0x65FA */
neuro_menu_t g_neuro_menu;
/* 0x66DC */
neuro_menu_t g_neuro_menus_pool[3];

static void build_neuro_menu(neuro_menu_t *menu,
	uint16_t left, uint16_t top, uint16_t w, uint16_t h,
	uint16_t mode, uint8_t *pixels)
{
	menu->mode = mode;

	menu->left = left;
	menu->top = top;
	menu->right = left + w - 1;
	menu->bottom = top + h - 1;

	menu->inner_left = menu->left + 8;
	menu->inner_top = menu->top + 8;
	menu->inner_right = menu->right - 8;
	menu->inner_bottom = menu->bottom - 8;

	memmove(&menu->_inner_left,
		&menu->inner_left, sizeof(uint16_t) * 4);

	menu->items_count = 0;

	menu->width = w / 2;
	translate_x64_to_x16(pixels, &menu->pixels_segt, &menu->pixels_offt);

	build_text_frame(h, w, (imh_hdr_t*)pixels);
}

static void build_neuro_menu_text(neuro_menu_t *menu,
	char *text, uint16_t x_offt, uint16_t y_offt)
{
	uint16_t w = menu->width * 2;
	uint16_t h = menu->bottom - menu->top + 1;
	uint16_t l = menu->inner_left - menu->left + x_offt;
	uint16_t t = menu->inner_top - menu->top + y_offt;
	uint8_t *p = translate_x16_to_x64(menu->pixels_segt, menu->pixels_offt);

	build_string(text, w, h, l, t, p + sizeof(imh_hdr_t));
}

static void build_neuro_menu_item(neuro_menu_t *menu,
	uint16_t x_offt, uint16_t y_offt, uint16_t w,
	uint16_t item_num, char c)
{
	if (item_num > 15)
	{
		return;
	}

	neuro_button_t *item = &menu->items[menu->items_count];

	memset(item, 0, sizeof(neuro_button_t));

	item->left = menu->inner_left + x_offt;
	item->top = menu->inner_top + y_offt;
	item->right = item->left + w - 1;
	item->bottom = item->top + 7;
	item->code = item_num;
	item->label = c;

	menu->items_count++;
}

void neuro_menu_store()
{
	memmove(&g_neuro_menus_pool[2], &g_neuro_menus_pool[1], sizeof(neuro_menu_t));
	memmove(&g_neuro_menus_pool[1], &g_neuro_menus_pool[0], sizeof(neuro_menu_t));
	memmove(&g_neuro_menus_pool[0], &g_neuro_menu, sizeof(neuro_menu_t));
}

void neuro_menu_restore()
{
	memmove(&g_neuro_menu, &g_neuro_menus_pool[0], sizeof(neuro_menu_t));
	memmove(&g_neuro_menus_pool[0], &g_neuro_menus_pool[1], sizeof(neuro_menu_t));
	memmove(&g_neuro_menus_pool[1], &g_neuro_menus_pool[2], sizeof(neuro_menu_t));
}

void neuro_menu_create(uint16_t mode,
	uint16_t l, uint16_t t, uint16_t w, uint16_t h, uint8_t *pixels)
{
	neuro_menu_store();

	switch (mode) {
	case 6:
		if (!pixels)
		{
			pixels = g_seg011.data;
		}

		l = 8 * (l - 1);
		t = 8 * (t - 1);
		w = (w * 8) + 16;
		h = (h * 8) + 16;

		build_neuro_menu(&g_neuro_menu, l, t, w, h, mode, pixels);
		drawing_control_add_sprite_to_chain(g_4bae.frame_sc_index--, l, t, pixels, 1);
		break;

	case 7:
		assert(7 == 0);

	default:
		assert(0);
	}
}

void neuro_menu_draw_text(char *text, uint16_t l, uint16_t t)
{
	switch (g_neuro_menu.mode) {
	case 6:
		l = l * 8;
		t = t * 8;
		build_neuro_menu_text(&g_neuro_menu, text, l, t);
		break;

	case 7:
		assert(7 == 0);

	default:
		assert(0);
	}
}

void neuro_menu_add_item(uint16_t l, uint16_t t, uint16_t w,
		uint16_t code, char label)
{
	switch (g_neuro_menu.mode) {
	case 6:
		l = l * 8;
		t = t * 8;
		w = w * 8;
		build_neuro_menu_item(&g_neuro_menu, l, t, w, code, label);
		break;

	case 7:
		assert(7 == 0);

	default:
		assert(0);
	}
}

void neuro_menu_flush_items()
{
	g_neuro_menu.items_count = 0;
}

void neuro_menu_flush()
{
	assert(g_neuro_menu.mode == 6);

	uint16_t h = g_neuro_menu.bottom - g_neuro_menu.top + 1;
	uint16_t w = g_neuro_menu.width * 2;
	imh_hdr_t *pixels = (imh_hdr_t*)translate_x16_to_x64(g_neuro_menu.pixels_segt,
							g_neuro_menu.pixels_offt);
	build_text_frame(h, w, pixels);
}

void neuro_menu_destroy()
{
	switch (g_neuro_menu.mode) {
	case 6:
		drawing_control_remove_sprite_from_chain(++g_4bae.frame_sc_index);
	case 7:
		neuro_menu_restore();
		break;

	default:
		assert(0);
	}
}

static void menu_handle_kboard(neuro_menu_id_t id, int *state, sfKeyEvent *event)
{
	switch (id) {
	case NMID_MAIN_MENU:
		main_menu_handle_kboard(state, event);
		break;
	}
}

static void menu_handle_text_enter(neuro_menu_id_t id, int *state, sfTextEvent *event)
{
	switch (id) {
	case NMID_MAIN_MENU:
		main_menu_handle_text_enter(state, event);
		break;

	case NMID_SKILLS_MENU:
		skills_menu_handle_text_enter(state, event);
		break;
	}
}

static void menu_handle_button_press(neuro_menu_id_t id, int *state, neuro_button_t *button)
{
	switch (id) {
	case NMID_MAIN_MENU:
		main_menu_handle_button_press(state, button);
		break;

	case NMID_NOT_IMPLEMENTED_MENU:
		not_implemented_menu_handle_button_press(state, button);
		break;

	case NMID_SKILLS_MENU:
		skills_menu_handle_button_press(state, button);
		break;

	case NMID_DISK_OPTIONS_MENU:
		disk_menu_handle_button_press(state, button);
		break;

	case NMID_PARTS_SHOP_MENU:
		parts_shop_menu_handle_button_press(state, button);
		break;
	}
}

static void select_menu_button(neuro_menu_t *_menu, neuro_button_t *button)
{
	imh_hdr_t *menu = (imh_hdr_t*)translate_x16_to_x64(_menu->pixels_segt, _menu->pixels_offt);
	uint8_t *pixels = (uint8_t*)menu + sizeof(imh_hdr_t);
	uint32_t item_left = (button->left - _menu->left) / 2;
	uint32_t item_top = button->top - _menu->top;
	uint32_t item_right = (button->right - _menu->left) / 2;
	uint32_t item_bottom = button->bottom - _menu->top;

	for (uint16_t h = item_top; h <= item_bottom; h++)
	{
		for (uint16_t w = item_left; w <= item_right; w++)
		{
			pixels[h * menu->width + w] ^= 0xFF;
		}
	}

	return;
}

static void unselect_menu_button(neuro_menu_t *menu, neuro_button_t *button)
{
	select_menu_button(menu, button);
}

static neuro_button_t* menu_button_kboard_hit_test(neuro_menu_t *menu)
{
	for (uint16_t u = 0; u < menu->items_count; u++)
	{
		neuro_button_t *hit = &menu->items[u];
		sfKeyCode key = ascii_toSfKeyCode(hit->label);

		if (key == sfKeyUnknown)
		{
			continue;
		}

		if (sfKeyboard_isKeyPressed(key))
		{
			return hit;
		}
	}

	return NULL;
}

static void neuro_menu_handle_kboard_events(neuro_menu_id_t id, neuro_menu_t *menu,
				int *state, sfEvent *event, int *kboard_lock)
{
	neuro_button_t *hit = NULL;
	static neuro_button_t *selected = NULL; /* selected button */
	static sfKeyCode _selected = sfKeyUnknown;

	switch (event->type) {
	case sfEvtKeyPressed:
		if (hit = menu_button_kboard_hit_test(menu))
		{
			if (!selected)
			{
				select_menu_button(menu, hit);
				selected = hit;
				_selected = event->key.code;
				*kboard_lock = 1;
			}
		}
		else
		{
			menu_handle_kboard(id, state, &event->key);
		}
		break;

	case sfEvtKeyReleased:
		if (selected && _selected == event->key.code)
		{
			unselect_menu_button(menu, selected);
			menu_handle_button_press(id, state, selected);
			selected = NULL;
			_selected = sfKeyUnknown;
			*kboard_lock = 0;
		}
		else
		{
			menu_handle_kboard(id, state, &event->key);
		}
		break;

	case sfEvtTextEntered:
		menu_handle_text_enter(id, state, &event->text);
		break;

	default:
		break;
	}
}

static neuro_button_t* menu_button_mouse_hit_test(neuro_menu_t *menu)
{
	sprite_layer_t *cursor = &g_sprite_chain[SCI_CURSOR];

	for (uint16_t u = 0; u < menu->items_count; u++)
	{
		neuro_button_t *hit = &menu->items[u];

		if (cursor->left > hit->left && cursor->left < hit->right &&
			cursor->top > hit->top && cursor->top < hit->bottom)
		{
			return hit;
		}
	}

	return NULL;
}

static void neuro_menu_handle_mouse_events(neuro_menu_id_t id, neuro_menu_t *menu,
				int *state, sfEvent *event, int *mouse_lock)
{
	neuro_button_t *hit = NULL;
	static neuro_button_t *selected = NULL; /* selected button */
	static int _selected = 0;

	switch (event->type) {
	case sfEvtMouseButtonPressed: {
		if (sfMouse_isButtonPressed(sfMouseLeft))
		{
			if (hit = menu_button_mouse_hit_test(menu))
			{
				if (!selected)
				{
					select_menu_button(menu, hit);
					selected = hit;
					_selected = 1;
					*mouse_lock = 1;
				}
			}
		}

		break;
	}

	case sfEvtMouseMoved: {
		if (sfMouse_isButtonPressed(sfMouseLeft))
		{
			hit = menu_button_mouse_hit_test(menu);

			if (selected)
			{
				if (hit != selected)
				{
					if (_selected)
					{
						unselect_menu_button(menu, selected);
						_selected = 0;
					}
				}
				else
				{
					if (_selected == 0)
					{
						select_menu_button(menu, hit);
						_selected = 1;
					}
				}
			}
			else if (hit)
			{
				select_menu_button(menu, hit);
				selected = hit;
				_selected = 1;
				*mouse_lock = 1;
			}

		}
		break;
	}

	case sfEvtMouseButtonReleased: {
		if (selected)
		{
			if (_selected)
			{
				unselect_menu_button(menu, selected);
				_selected = 0;
			}

			if (selected == menu_button_mouse_hit_test(menu))
			{
				menu_handle_button_press(id, state, selected);
			}

			selected = NULL;
			*mouse_lock = 0;
		}
		break;
	}

	default:
		break;
	}
}

void neuro_menu_handle_input(neuro_menu_id_t id, neuro_menu_t *menu,
			int *state, sfEvent *event)
{
	static int mouse_lock = 0;
	static int kboard_lock = 0;

	switch (event->type) {
	case sfEvtMouseButtonPressed:
	case sfEvtMouseButtonReleased:
	case sfEvtMouseMoved:
		if (!kboard_lock)
		{
			neuro_menu_handle_mouse_events(id, menu, state, event, &mouse_lock);
		}
		break;

	case sfEvtKeyPressed:
	case sfEvtKeyReleased:
	case sfEvtTextEntered:
		if (!mouse_lock)
		{
			neuro_menu_handle_kboard_events(id, menu, state, event, &kboard_lock);
		}
		break;

	default:
		break;
	}
}
