#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

#include <stdint.h>

void resource_manager_init();
void resource_manager_deinit();
int resource_manager_load_resource(char *name, uint8_t *dst);
int resource_manager_write_resource(char *name, uint8_t *src);

#endif
