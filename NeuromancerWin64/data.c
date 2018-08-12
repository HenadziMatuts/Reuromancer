#include "data.h"

/* 0x004C */
int g_level_n = -1;

uint8_t g_004e[8] = {
	0x00,
};

/* 0x1FA2 */
neuro_ui_buttons_t g_ui_buttons = {
	.inventory = {
		0x10, 0x93, 0x23, 0xA5, 0x00, 'i'
	},
	.pax = {
		0x28, 0x93, 0x3B, 0xA5, 0x01, 'p'
	},
	.dialog = {
		0x40, 0x93, 0x53, 0xA5, 0x02, 't'
	},
	.skills = {
		0x10, 0xAB, 0x23, 0xBD, 0x03, 's',
	},
	.chip = {
		0x28, 0xAB, 0x3B, 0xBD, 0x04, 'r',
	},
	.disk = {
		0x40, 0xAB, 0x53, 0xBD, 0x05, 'd'
	},
	.date = {
		0x70, 0xA8, 0x7D, 0xB2, 0x0A, '1'
	},
	.time = {
		0x80, 0xA8, 0x8F, 0xB2, 0x0B, '2'
	},
	.cash = {
		0x70, 0xB3, 0x7D, 0xBB, 0x0C, '3'
	},
	.con = {
		0x80, 0xB3, 0x8F, 0xBB, 0x0D, '4'
	}
};

/* 0x21FA */
neuro_inventory_buttons_t g_inv_buttons = {
	.item_page_exit = {
		0x40, 0x90, 0xC7, 0x97, 0x0A, 'x'
	},
	.exit = {
		0x60, 0xB0, 0x7F, 0xB7, 0x0A, 'x'
	},
	.more = {
		0x88, 0xB0, 0xA7, 0xB7, 0x0B, 'm'
	}
};

/* 0x2236 */
neuro_inventory_discard_buttons_t g_inv_disc_buttons = {
	.yes = {
		0xB0, 0xA8, 0xB7, 0xAF, 0x01, 'y'
	},
	.no = {
		0xC0, 0xA8, 0xC7, 0xAF, 0x00, 'n'
	}
};

/* 0x25B4 */
ui_panel_mode_t g_ui_panel_mode = UI_PM_CASH;

x3f85_t g_3f85 = {
	{
		{ 0x00, 0xC1, 0xFF, 0x0000, 0x24, 0x28 },
		{ 0x01, 0xE9, 0xFF, 0x0000, 0x1E, 0x27 },
		{ 0x02, 0x01, 0xFF, 0x0000, 0x19, 0x3D },
		{ 0x02, 0x02, 0xFF, 0x0000, 0x50, 0x3E },
		{ 0x03, 0xC1, 0xFF, 0x0000, 0x6B, 0x33 },
		{ 0x05, 0x41, 0xFF, 0x0000, 0x8C, 0x4B },
		{ 0x06, 0x01, 0xFF, 0x0000, 0x00, 0x00 },
		{ 0x07, 0xC1, 0xFF, 0x0000, 0x1B, 0x2F },
		{ 0x08, 0x01, 0xFF, 0x0000, 0x77, 0x26 },
		{ 0x09, 0xE1, 0xFF, 0x0000, 0x89, 0x35 },
		{ 0x0A, 0xE1, 0xFF, 0x0000, 0x54, 0x33 },
		{ 0x0B, 0xC1, 0xFF, 0x0000, 0x2C, 0x34 },
		{ 0x12, 0xC1, 0xFF, 0x0000, 0x84, 0x24 },
		{ 0x13, 0xE1, 0xFF, 0x0000, 0x6D, 0x38 },
		{ 0x15, 0x01, 0xFF, 0x0000, 0x50, 0x35 },
		{ 0x16, 0xE1, 0xFF, 0x0000, 0x6C, 0x33 },
		{ 0x17, 0xE1, 0xFF, 0x0000, 0x4B, 0x3D },
		{ 0x18, 0xC1, 0xFF, 0x0000, 0x30, 0x25 },
		{ 0x19, 0xE1, 0xFF, 0x0000, 0x59, 0x2D },
		{ 0x1A, 0xC1, 0xFF, 0x0000, 0x3F, 0x2F },
		{ 0x1B, 0xE1, 0xFF, 0x0000, 0x89, 0x35 },
		{ 0x1C, 0xC1, 0xFF, 0x0000, 0x78, 0x34 },
		{ 0x1F, 0x41, 0xFF, 0x0000, 0x33, 0x21 },
		{ 0x21, 0xC1, 0xFF, 0x0000, 0x60, 0x34 },
		{ 0x23, 0xE1, 0xFF, 0x0000, 0x58, 0x33 },
		{ 0x27, 0xE1, 0xFF, 0x0000, 0x34, 0x31 },
		{ 0x28, 0x01, 0xFF, 0x0000, 0x78, 0x30 },
		{ 0x2B, 0xE1, 0xFF, 0x0000, 0x38, 0x31 },
		{ 0x2C, 0xE9, 0xFF, 0x0000, 0x1E, 0x27 },
		{ 0x2D, 0xC1, 0xFF, 0x0000, 0x25, 0x24 },
		{ 0x2D, 0xC2, 0xFF, 0x0000, 0x0E, 0x24 },
		{ 0x31, 0xE1, 0xFF, 0x0000, 0x71, 0x31 },
		{ 0x33, 0x01, 0xFF, 0x0000, 0x61, 0x32 },
		{ 0x34, 0xE1, 0xFF, 0x0000, 0x5C, 0x32 },
		{ 0x37, 0x41, 0xFF, 0x0000, 0x54, 0x29 },
	},
	0xFF,
	{
		0xFF, 0x00, 0xFF, 0xFF, 0x01, 0xFF,
		0xFF, 0x00, 0xFF, 0x04, 0xFF, 0xFF,
		0xFF, 0x00, 0x01, 0xFF, 0xFF, 0xFF,
		0xFF, 0x00, 0xFF, 0xFF, 0x04, 0xFF,
		0x0FF, 0x0, 0x3, 0x0C, 0x5, 0x1, 0x0FF, 0x0, 0x4, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0, 0x0FF, 0x0E, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0F, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x10, 0x0FF,
		0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF,
		0x0FF, 0x0C, 0x0FF, 0x0FF, 0x0, 0x0B, 0x17, 0x0D, 0x4, 0x0FF, 0x0, 0x0C, 0x18, 0x0E, 0x0FF, 0x0FF, 0x0,
		0x0D, 0x19, 0x0F, 0x6, 0x0FF, 0x0, 0x0E, 0x0FF, 0x10, 0x7, 0x0FF, 0x0, 0x0F, 0x1A, 0x11, 0x0FF, 0x0FF,
		0x0, 0x10, 0x0FF, 0x0FF, 0x12, 0x0FF, 0x0, 0x0FF, 0x11, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0, 0x15, 0x1D, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x14, 0x0FF, 0x0FF, 0x0, 0x0FF,
		0x0FF, 0x0B, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0C, 0x0FF, 0x0, 0x0FF, 0x0D, 0x0FF, 0x0FF, 0x0FF,
		0x0, 0x0FF, 0x1E, 0x0FF, 0x0E, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x10, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x1D, 0x0FF, 0x0FF, 0x0, 0x1C, 0x20, 0x0FF, 0x14, 0x0FF, 0x0, 0x1F,
		0x26, 0x0FF, 0x19, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x1E, 0x0FF, 0x0FF, 0x0, 0x21, 0x29, 0x0FF, 0x1D, 0x0FF,
		0x0, 0x0FF, 0x0FF, 0x20, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x21, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x24,
		0x0FF, 0x0FF, 0x0, 0x23, 0x0FF, 0x25, 0x0FF, 0x0FF, 0x0, 0x24, 0x2B, 0x26, 0x0FF, 0x0FF, 0x0, 0x25, 0x2C,
		0x27, 0x1E, 0x0FF, 0x0, 0x26, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x29, 0x0FF, 0x0FF, 0x0, 0x28,
		0x0FF, 0x0FF, 0x20, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x25, 0x0FF,
		0x0, 0x0FF, 0x30, 0x2D, 0x26, 0x0FF, 0x0, 0x2C, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x28,
		0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x33, 0x0FF, 0x2C, 0x0FF, 0x0, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x35, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x36, 0x0FF, 0x30, 0x0FF, 0x0,
		0x0FF, 0x0FF, 0x35, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x36, 0x0FF, 0x0FF, 0x0, 0x35, 0x0FF, 0x0FF, 0x33,
		0x0FF, 0x0, 0x36, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0, 0x0FF, 0x0FF, 0x0FF, 0x35, 0x0FF, 0x0, 0x0FF, 0x0FF,
		0x0FF, 0x36
	},
	.inventory = {
		.items = {
			0x5F, 0x00, 0x00, 0x00, 0x28, 0x01, 0x32, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00
		},
		.software = {
			0x0A, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,
			0x03, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00,
			0x05, 0x01, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00,
			0xFF, 0x05, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00,
			0xFF, 0x05, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
			0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
		}
	},
	{
		0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
		0x0FF, 0x0FF, 0x0FF, 0x52, 0x4F, 0x59, 0x20, 0x4D, 0x49, 0x4C, 0x45, 0x53, 0x54, 0x4F, 0x4E, 0x45, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x0, 0x38, 0x31, 0x32, 0x35, 0x34, 0x37, 0x37, 0x32, 0x34, 0x0, 0x0FF, 0x0, 0x0, 0x42, 0x41,
		0x4C, 0x20, 0x34, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x35, 0x35,
		0x31, 0x37, 0x30, 0x33, 0x32, 0x38, 0x38, 0x0, 0x0FF, 0x0, 0x2, 0x4B, 0x52, 0x49, 0x53, 0x54, 0x4F, 0x46, 0x46,
		0x45, 0x52, 0x20, 0x55, 0x4C, 0x4D, 0x20, 0x20, 0x20, 0x20, 0x0, 0x31, 0x38, 0x38, 0x38, 0x36, 0x34, 0x32, 0x30,
		0x32, 0x0, 0x0FF, 0x0, 0x3, 0x4B, 0x49, 0x4D, 0x20, 0x54, 0x59, 0x47, 0x45, 0x52, 0x20, 0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x0, 0x31, 0x35, 0x39, 0x32, 0x31, 0x37, 0x33, 0x32, 0x39, 0x0, 0x0FF, 0x0, 0x1, 0x41,
		0x53, 0x48, 0x4C, 0x45, 0x59, 0x20, 0x52, 0x4F, 0x42, 0x49, 0x4E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0,
		0x30, 0x34, 0x32, 0x33, 0x38, 0x35, 0x30, 0x30, 0x33, 0x0, 0x0FF, 0x0, 0x0, 0x0, 0x54, 0x41, 0x4B, 0x4F, 0x44,
		0x41, 0x20, 0x4D, 0x49, 0x53, 0x48, 0x49, 0x4A, 0x49, 0x20, 0x20, 0x20, 0x20, 0x0, 0x38, 0x38, 0x33, 0x38,
		0x34, 0x39, 0x39, 0x33, 0x31, 0x0, 0x0FF, 0x0, 0x0, 0x41, 0x4B, 0x49, 0x52, 0x41, 0x20, 0x4F, 0x27, 0x42,
		0x52, 0x49, 0x45, 0x4E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x33, 0x39, 0x34, 0x38, 0x35, 0x37, 0x32, 0x39, 0x31,
		0x0, 0x0FF, 0x0, 0x1, 0x53, 0x56, 0x45, 0x4E, 0x20, 0x4B, 0x48, 0x41, 0x52, 0x4B, 0x4F, 0x56, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x0, 0x33, 0x38, 0x39, 0x34, 0x35, 0x35, 0x37, 0x35, 0x36, 0x0, 0x0FF, 0x0, 0x2, 0x4D, 0x45,
		0x4C, 0x49, 0x53, 0x53, 0x41, 0x20, 0x42, 0x4F, 0x52, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x31, 0x30,
		0x32, 0x39, 0x33, 0x38, 0x35, 0x36, 0x35, 0x0, 0x0FF, 0x0, 0x3, 0x50, 0x2E, 0x20, 0x52, 0x59, 0x41, 0x4E,
		0x20, 0x57, 0x41, 0x4E, 0x47, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x35, 0x36, 0x34, 0x39, 0x38, 0x37, 0x32,
		0x37, 0x31, 0x0, 0x0FF, 0x0, 0x4, 0x4D, 0x2E, 0x20, 0x43, 0x2E, 0x20, 0x42, 0x45, 0x41, 0x52, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x0, 0x31, 0x32, 0x39, 0x34, 0x38, 0x35, 0x36, 0x33, 0x36, 0x0, 0x0FF, 0x0, 0x5,
		0x53, 0x2E, 0x20, 0x5A, 0x2E, 0x20, 0x57, 0x41, 0x54, 0x45, 0x52, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0,
		0x30, 0x35, 0x33, 0x36, 0x35, 0x36, 0x35, 0x32, 0x38, 0x0, 0x0FF, 0x0, 0x5, 0x0,
	}
};

x3f85_wrapper_t g_3f85_wrapper = {
	{ NULL, }, &g_3f85,
};

/* 0x4469 */
char *g_inventory_item_names[104] = {
	{ "Mimic" },
	{ "Jammies" },
	{ "ThunderHead" },
	{ "Vaccine" },
	{ "Blammo" },
	{ "DoorStop" },
	{ "Decoder" },
	{ "Sequencer" },
	{ "ArmorAll" },
	{ "KGB" },
	{ "Comlink" },
	{ "BlowTorch" },
	{ "Probe" },
	{ "Drill" },
	{ "Hammer" },
	{ "Python" },
	{ "Acid" },
	{ "Injector" },
	{ "DepthCharge" },
	{ "Concrete" },
	{ "EasyRider" },
	{ "LogicBomb" },
	{ "Cyberspace" },
	{ "Slow" },
	{ "BattleChess" },
	{ "BattleChess" },
	{ "Scout" },
	{ "Hemlock" },
	{ "KuangEleven" },
	{ "Hiki Gaeru" },
	{ "Gaijin" },
	{ "Bushido" },
	{ "Edokko" },
	{ "Katana" },
	{ "Tofu" },
	{ "Shogun" },
	{ "188BJB" },
	{ "350SL" },
	{ "" },
	{ "" },
	{ "UXB" },
	{ "" },
	{ "ZXB" },
	{ "Cyberspace II" },
	{ "Cyberspace III" },
	{ "" },
	{ "Cyberspace VII" },
	{ "Ninja 2000" },
	{ "Ninja 3000" },
	{ "Ninja 4000" },
	{ "Ninja 5000" },
	{ "Blue Light Spec." },
	{ "Samurai Seven" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "Bargaining" },
	{ "CopTalk" },
	{ "Warez Analysis" },
	{ "Debug" },
	{ "Hardware Repair" },
	{ "ICE Breaking" },
	{ "Evasion" },
	{ "Cryptology" },
	{ "Japanese" },
	{ "Logic" },
	{ "Psychoanalysis" },
	{ "Phenomenology" },
	{ "Philosophy" },
	{ "Sophistry" },
	{ "Zen" },
	{ "Musicianship" },
	{ "CyberEyes" },
	{ "" },
	{ "" },
	{ "guest pass" },
	{ "" },
	{ "" },
	{ "joystick" },
	{ "" },
	{ "" },
	{ "" },
	{ "" },
	{ "caviar" },
	{ "pawn ticket" },
	{ "Security Pass" },
	{ "Zion ticket" },
	{ "Freeside ticket" },
	{ "" },
	{ "Chiba ticket" },
	{ "gas mask" },
	{ "" },
	{ "sake" },
};

/* 0x475B */
uint8_t g_inventory_item_operations[128] = {
	0x25, 0x27, 0x23, 0x01, 0x29, 0x22, 0x22, 0x10,
	0x24, 0x2B, 0x00, 0x22, 0x21, 0x22, 0x22, 0x23,
	0x23, 0x23, 0x22, 0x22, 0x26, 0x22, 0x01, 0x27,
	0x12, 0x20, 0x11, 0x20, 0x20, 0x80, 0x80, 0x80,
	0x80, 0xC0, 0xC0, 0xC0, 0x80, 0x80, 0xC0, 0xC0,
	0x80, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x80,
	0x80, 0xC0, 0xC0, 0x80, 0xC0, 0xFF, 0xFF, 0xFF,
	0x84, 0x85, 0x80, 0x80, 0x80, 0x80, 0x2A, 0x00,
	0x00, 0x00, 0x00, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x83, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00
};

jumps_t g_4b9d[4] = {
	JE, JNE, JL, JGE
};

x4bae_t g_4bae = {
	.x4bae = { 0, },
	.x4bbe = 0xff,
	.x4bbf = 0xff,
	.active_item = 0xffff,
	.cash_withdrawal = 0,
	.time_m = 0,
	.time_h = 0x0C,
	.date_day = 0,
	.x4bca = { 0, 0 },
	.x4bcc = 0,
	.x4bcd = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0
	},
	.x4bf3 = 0xFF,
	.x4bf4 = 0xFF,
	.x4bf5 = 0xFF,
	.x4bf6 = { 0x4C, 0, },
	.x4bfc = 0,
	.x4bfd = 0,
	.x4bfe = 0,
	.x4bff = 0,
	.x4c00 = 0,
	.x4c01 = { 0, 0, 0 },
	.x4c04 = 0xFF,
	.x4c05 = 0xFF,
	.x4c06 = 0xFFFF,
	.x4c08 = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0 },
	.x4c10 = 0,
	.x4c11 = { 0, 0, 0, 0, 0, 0, 1, 0 },
	.x4c19 = 0,
	.x4c1a = 0,
	.x4c1b = 0,
	.x4c1d = { 0, 0, 0, 0 },
	.x4c21 = 0,
	.x4c23 = { 0, 0 },
	.x4c25 = 0,
	.x4c27 = { 0, 0, 0, 0 },
	.x4c2b = 0,
	.x4c2d = { 0, 0 },
	.x4c2f = 0x3E8,
	.x4c31 = 0,
	.x4c33 = { 1, 2, 1, 0 },
	.x4c37 = 0xFF,
	.x4c38 = 0,
	.x4c3a = 0,
	.x4c3b = 0,
	.x4c3c = 0,
	.x4c3d = 0,
	.x4c3f = 0,
	.x4c40 = 0,
	.x4c42 = { 0, 0 },
	.x4c44 = 0xFF,
	.x4c45 = 0,
	.x4c46 = 0,
	.x4c47 = 0xA120,
	.x4c49 = 7,
	.x4c4b = 0x7530,
	.x4c4d = 0,
	.x4c4f = { 0, },
	.x4c57 = 0,
	.x4c59 = { 0, 0, 0 },
	.x4c5c = 0xFF,
	.x4c5d = { 0, },
	.x4c69 = 0,
	.x4c6a = 0,
	.x4c6b = 0,
	.x4c6d = { 0, },
	.x4c74 = 0,
	.x4c75 = 0,
	.x4c76 = { 0, 0 },
	.cash = 50, //6,
	.x4c7c = 0,
	.x4c7e = { 0, 0, 0, 0 },
	.x4c82 = 0,
	.x4c84 = { 0, 0, 0 },
	.x4c87 = 0x7D0,
	.x4c89 = 0x7D0,
	.x4c8b = 0,
	.x4c8d = 0,
	.x4c8e = 0,
	.x4c90 = 0,
	.x4c92 = { '{', '@', 'C', 'a', 's', 'e', 0, },
	.con = 0x7D0,
	.level_n = 0,
	.roompos_spawn_x = 0xA0,
	.roompos_spawn_y = 0x69,
	.x4ca7 = 0,
	.x4ca9 = {
		0x40, 0, 0x78, 0, 0, 0, 0x40, 0, 0x38, 0, 0, 0,
		0x40, 0, 0x4B, 0, 0, 0, 0xC0, 0, 0xE8, 3, 0, 0,
	},
	.ui_type = 0,
	.x4cc3 = 0,
	.x4cc5 = 0,
	.x4cc7 = 0,
	.x4cc9 = 0,
	.x4ccb = 0,
	.x4ccc = 0,
	.x4ccd = 0,
	.x4cce = 0,
	.x4ccf = 2
};

neuro_window_t g_a59e[3] = {
	{ 0, }, { 0, }, { 0, }
};

neuro_window_wrapper_t g_a59e_wrapper[3] = {
	{
		{ NULL, }, &g_a59e[0]
	},
	{
		{ NULL, }, &g_a59e[1]
	},
	{
		{ NULL, }, &g_a59e[2]
	}
};

uint8_t g_a61a = 0;

uint16_t g_a86a = 0;

a8e0_t g_a8e0 = {
	{ 0, }, { 0, }
};

bih_hdr_wrapper_t g_bih_wrapper = {
	NULL, NULL, (bih_hdr_t*)g_a8e0.bih
};

/* 0xC91E */
neuro_window_t g_neuro_window = {
	{ 0, }
};

neuro_window_wrapper_t g_neuro_window_wrapper = {
	{ NULL, }, &g_neuro_window
};

uint8_t g_c946 = 0;
