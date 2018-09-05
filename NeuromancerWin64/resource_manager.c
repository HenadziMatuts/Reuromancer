#include "globals.h"
#include "data.h"
#include "resource_manager.h"
#include "drawing_control.h"
#include <neuro_routines.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct resource_manager_t {
	FILE *neuro1;
	FILE *neuro2;
	uint8_t compd[12288];
	uint8_t decompd[32008];
} resource_manager_t;
static resource_manager_t g_resource_manager;

void resource_manager_init()
{
	memset(&g_resource_manager, 0, sizeof(resource_manager_t));

	assert(g_resource_manager.neuro1 = fopen("NEURO1.DAT", "rb+"));
	assert(g_resource_manager.neuro2 = fopen("NEURO2.DAT", "rb+"));

	assert(resource_manager_load_resource("BUBBLES.IMH", g_seg014.dialog_bubbles));
	assert(resource_manager_load_resource("ROOMPOS.BIH", g_seg016.roompos));
	assert(resource_manager_load_resource("SPRITES.IMH", g_seg013.spritesheet));
	assert(resource_manager_load_resource("CURSORS.IMH", g_seg009.cursors));
	drawing_control_add_sprite_to_chain(SCI_CURSOR, 160, 100, g_seg009.cursors, 0);
}

void resource_manager_deinit()
{
	fclose(g_resource_manager.neuro1);
	fclose(g_resource_manager.neuro2);
}

int resource_manager_load_resource(char *name, uint8_t *dst)
{
	int i = 0, len = 0;

	if (strstr(name, ".PIC"))
	{
		while (g_res_pic[i].file != -1)
		{
			if (!strcmp(g_res_pic[i].name, name))
			{
				FILE *neuro = g_res_pic[i].file == 0
					? g_resource_manager.neuro1
					: g_resource_manager.neuro2;

				fseek(neuro, g_res_pic[i].offset + 32, SEEK_SET);
				fread(g_resource_manager.compd, g_res_pic[i].size, 1, neuro);
				len = decompress_pic(g_resource_manager.compd,
						g_resource_manager.decompd);
				memmove(dst, g_resource_manager.decompd, len);

				return 1;
			}
			i++;
		}
	}
	else if (strstr(name, ".IMH"))
	{
		while (g_res_imh[i].file != -1)
		{
			if (!strcmp(g_res_imh[i].name, name))
			{
				FILE *neuro = g_res_imh[i].file == 0
					? g_resource_manager.neuro1
					: g_resource_manager.neuro2;

				fseek(neuro, g_res_imh[i].offset + 32, SEEK_SET);
				fread(g_resource_manager.compd, g_res_imh[i].size, 1, neuro);
				len = decompress_imh(g_resource_manager.compd,
					g_resource_manager.decompd);
				memmove(dst, g_resource_manager.decompd, len);

				return 1;
			}
			i++;
		}
	}
	else if (strstr(name, ".BIH"))
	{
		while (g_res_bih[i].file != -1)
		{
			if (!strcmp(g_res_bih[i].name, name))
			{
				FILE *neuro = g_res_bih[i].file == 0
					? g_resource_manager.neuro1
					: g_resource_manager.neuro2;

				fseek(neuro, g_res_bih[i].offset, SEEK_SET);
				fread(g_resource_manager.compd, g_res_bih[i].size, 1, neuro);
				len = decompress_bih(g_resource_manager.compd,
					g_resource_manager.decompd);
				memmove(dst, g_resource_manager.decompd, len);

				return 1;
			}
			i++;
		}
	}
	else if (strstr(name, ".ANH"))
	{
		while (g_res_anh[i].file != -1)
		{
			if (!strcmp(g_res_anh[i].name, name))
			{
				FILE *neuro = g_res_anh[i].file == 0
					? g_resource_manager.neuro1
					: g_resource_manager.neuro2;

				fseek(neuro, g_res_anh[i].offset, SEEK_SET);
				fread(g_resource_manager.compd, g_res_anh[i].size, 1, neuro);
				len = decompress_anh(g_resource_manager.compd,
					g_resource_manager.decompd);
				memmove(dst, g_resource_manager.decompd, len);

				return 1;
			}
			i++;
		}
	}
	else if (strstr(name, ".TXH"))
	{
		while (g_res_txh[i].file != -1)
		{
			if (!strcmp(g_res_txh[i].name, name))
			{
				FILE *neuro = g_res_txh[i].file == 0
					? g_resource_manager.neuro1
					: g_resource_manager.neuro2;

				fseek(neuro, g_res_txh[i].offset, SEEK_SET);
				fread(g_resource_manager.compd, g_res_txh[i].size, 1, neuro);
				len = decompress_anh(g_resource_manager.compd,
					g_resource_manager.decompd);
				memmove(dst, g_resource_manager.decompd, len);

				return 1;
			}
			i++;
		}
	}
	else if (!strcmp(name, "SAVEGAME.SAV"))
	{
		FILE *neuro = g_res_savegame.file == 0
			? g_resource_manager.neuro1
			: g_resource_manager.neuro2;

		fseek(neuro, g_res_savegame.offset, SEEK_SET);
		fread(dst, g_res_savegame.size, 1, neuro);

		return 1;
	}

	return 0;
}

int resource_manager_write_resource(char *name, uint8_t *src)
{
	if (!strcmp(name, "SAVEGAME.SAV"))
	{
		FILE *neuro = g_res_savegame.file == 0
			? g_resource_manager.neuro1
			: g_resource_manager.neuro2;

		fseek(neuro, g_res_savegame.offset, SEEK_SET);
		fwrite(src, g_res_savegame.size, 1, neuro);
		fflush(neuro);

		return 1;
	}
	
	return 0;
}
