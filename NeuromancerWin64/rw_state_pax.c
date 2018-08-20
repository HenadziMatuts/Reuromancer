#include "data.h"
#include "globals.h"
#include "resource_manager.h"
#include "scene_real_world.h"
#include "neuro_window_control.h"
#include "drawing_control.h"
#include <neuro_routines.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef enum pax_state_t {

} pax_state_t;

void handle_pax_input(sfEvent *event)
{

}

real_world_state_t update_pax()
{
	/*
	switch (g_state) {
	case IS_OPEN_INVENTORY:
		g_state = update_inventory_open();
		break;

	case IS_CLOSE_INVENTORY:
		g_state = update_inventory_close();
		if (g_state == IS_OPEN_INVENTORY)
		{
			return RWS_NORMAL;
		}
		break;
	}
	*/
	return RWS_NORMAL;
}
