#include "globals.h"
#include "drawing_control.h"
#include "neuro_menu_control.h"

static void menu_handle_text_enter(neuro_menu_id_t id, int *state, sfTextEvent *event)
{
	switch (id) {
	case NMID_MAIN_MENU:
		main_menu_handle_text_enter(state, event);
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
		break;
	}
}

static void select_menu_button(neuro_menu_t *_menu, neuro_button_t *button)
{
	imh_hdr_t *menu = (imh_hdr_t*)_menu->pixels;
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

	switch (event->type) {
	case sfEvtKeyPressed:
		if (hit = menu_button_kboard_hit_test(menu))
		{
			if (!selected)
			{
				select_menu_button(menu, hit);
				selected = hit;
				*kboard_lock = 1;
			}
		}
		break;

	case sfEvtKeyReleased:
		if (selected)
		{
			unselect_menu_button(menu, selected);
			menu_handle_button_press(id, state, selected);
			selected = NULL;
			*kboard_lock = 0;
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
