#ifndef _SCENE_CONTROL_H
#define _SCENE_CONTROL_H

#include <stdint.h>
#include <SFML\Window\Event.h>

typedef enum neuro_scene_id_t {
	NSID_NONE = -1,
	NSID_MAIN_MENU = 0,
	NSID_REAL_WORLD = 1,
	NSID_NOT_IMPLEMENTED = 255,
} neuro_scene_id_t;

typedef void(*pfn_init_scene)();
typedef void(*pfn_handle_input)(sfEvent *event);
typedef neuro_scene_id_t(*pfn_update_scene)();
typedef void(*pfn_deinit_scene)();

typedef struct neuro_scene_t {
	neuro_scene_id_t id;
	pfn_init_scene init;
	pfn_deinit_scene deinit;
	pfn_handle_input handle_input;
	pfn_update_scene update;
} neuro_scene_t;

extern neuro_scene_t g_scene;

void scene_control_setup_scene(neuro_scene_id_t id);

void setup_main_menu_scene();
void setup_not_implemented_scene();
void setup_real_world_scene();

#endif
