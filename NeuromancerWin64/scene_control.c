#include "scene_control.h"

/*
* Current scene.
*/
neuro_scene_t g_scene = { -1, };

void scene_control_setup_scene(neuro_scene_id_t id)
{
	if (g_scene.id != NSID_NONE && g_scene.deinit)
	{
		g_scene.deinit();
	}

	switch (id)
	{
	case NSID_MAIN_MENU:
		setup_main_menu_scene();
		break;

	case NSID_REAL_WORLD:
		setup_real_world_scene();
		break;

	case NSID_NOT_IMPLEMENTED:
		setup_not_implemented_scene();
		break;

	default:
		break;
	}

	if (g_scene.id != NSID_NONE && g_scene.init)
	{
		g_scene.init();
	}
}
