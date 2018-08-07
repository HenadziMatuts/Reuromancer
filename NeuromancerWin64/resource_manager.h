#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

#include <stdint.h>

extern uint8_t g_cursors[399];
extern uint8_t g_background[32063];
extern uint8_t g_seg011[41999];
extern uint8_t g_seg012[5647];
extern uint8_t g_sprites[61999];
extern uint8_t g_dialog_bubbles[607];
extern uint8_t g_level_bg[17055];
extern uint8_t g_roompos[23967];

void resource_manager_init();
void resource_manager_deinit();
int  resource_manager_load_resource(char *name, uint8_t *dst);

#endif
