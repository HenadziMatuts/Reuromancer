#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

#include <stdint.h>

extern uint8_t g_cursors[400];
extern uint8_t g_background[32064];
extern uint8_t g_seg011[42000];
extern uint8_t g_seg012[5648];
extern uint8_t g_sprites[62000];
extern uint8_t g_dialog_bubbles[609];
extern uint8_t g_level_bg[17056];
extern uint8_t g_roompos[23968];
extern uint8_t g_savegame[12000];

void resource_manager_init();
void resource_manager_deinit();
int  resource_manager_load_resource(char *name, uint8_t *dst);

#endif
