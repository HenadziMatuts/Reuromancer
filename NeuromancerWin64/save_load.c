#include "data.h"
#include "save_load.h"
#include "resource_manager.h"
#include "neuro_menu_control.h"
#include "drawing_control.h"
#include <assert.h>
#include <string.h>

static void add_slots()
{
	neuro_menu_draw_text("1  2  3  4", 3, 2);
	neuro_menu_draw_text("exit", 6, 5);

	neuro_menu_add_item(6, 5, 4, 0x0A, 'x');

	for (int i = 0; i < 4; i++)
	{
		neuro_menu_add_item((3 * i) + 3, 2, 1, i, i + '1');
	}
}

void save_menu()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("Save Game", 4, 0);
	add_slots();
}

int on_save_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* slots */
		return -1;

	case 0x0A: /* exit */
		return 0;
	}

	return -1;
}

void load_menu()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("Load Game", 4, 0);
	add_slots();

	for (int i = 0; i < 4; i++)
	{
		neuro_menu_add_item((3 * i) + 3, 2, 1, i, i + '1');
	}
}

int on_load_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* slots */
		return -1;

	case 0x0A: /* exit */
		return 0;
	}

	return -1;
}

