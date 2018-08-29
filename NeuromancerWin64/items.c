#include "data.h"
#include "items.h"

/* 0x4469 */
static char *g_item_names[104] = {
	"Mimic",
	"Jammies",
	"ThunderHead",
	"Vaccine",
	"Blammo",
	"DoorStop",
	"Decoder",
	"Sequencer",
	"ArmorAll",
	"KGB",
	"Comlink",
	"BlowTorch",
	"Probe",
	"Drill",
	"Hammer",
	"Python",
	"Acid",
	"Injector",
	"DepthCharge",
	"Concrete",
	"EasyRider",
	"LogicBomb",
	"Cyberspace",
	"Slow",
	"BattleChess",
	"BattleChess",
	"Scout",
	"Hemlock",
	"KuangEleven",
	"Hiki Gaeru",
	"Gaijin",
	"Bushido",
	"Edokko",
	"Katana",
	"Tofu",
	"Shogun",
	"188BJB",
	"350SL",
	"", "",
	"UXB",
	"",
	"ZXB",
	"Cyberspace II",
	"Cyberspace III",
	"",
	"Cyberspace VII",
	"Ninja 2000",
	"Ninja 3000",
	"Ninja 4000",
	"Ninja 5000",
	"Blue Light Spec.",
	"Samurai Seven",
	"", "", "", "", "", "", "",
	"", "", "", "", "", "", "",
	"Bargaining",
	"CopTalk",
	"Warez Analysis",
	"Debug",
	"Hardware Repair",
	"ICE Breaking",
	"Evasion",
	"Cryptology",
	"Japanese",
	"Logic",
	"Psychoanalysis",
	"Phenomenology",
	"Philosophy",
	"Sophistry",
	"Zen",
	"Musicianship",
	"CyberEyes",
	"", "",
	"guest pass",
	"", "",
	"joystick",
	"", "", "", "",
	"caviar",
	"pawn ticket",
	"Security Pass",
	"Zion ticket",
	"Freeside ticket",
	"",
	"Chiba ticket",
	"gas mask",
	"",
	"sake",
};

char* get_item_name(uint16_t item_code, char *credits)
{
	if (item_code == 0x7F)
	{
		sprintf(credits, "Credits %d", g_4bae.cash);
		return credits;
	}

	return g_item_names[item_code];
}

uint16_t count_items(int sortware)
{
	uint8_t *inv = (sortware) ? g_3f85.inventory.software : g_3f85.inventory.items;
	uint16_t items = 0;

	for (int i = 0; i < 32; i++, inv += 4)
	{
		if (*inv != 0xFF)
		{
			items++;
		}
	}

	return items;
}
