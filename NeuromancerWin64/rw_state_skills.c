#include "data.h"
#include "resource_manager.h"
#include "scene_real_world.h"
#include "drawing_control.h"
#include "neuro_menu_control.h"
#include "window_animation.h"
#include "items.h"
#include "globals.h"
#include <neuro_routines.h>
#include <string.h>

typedef enum skills_state_t {
	SS_OPEN_SKILLS = 0,
	SS_NO_SKILLS_WFI,
	SS_CANT_USE_WFI,
	SS_SKILLS_PAGE,
	  SS_SKILL_WAREZ_ANALYSIS_ITEM_PAGE,
	    SS_SKILL_WAREZ_ANAYSIS_WFI,
	  SS_SKILL_DEBUG_ITEM_PAGE,
	    SS_SKILL_DEBUG_WFI,
	  SS_SKILL_HW_REPAIR_ITEM_PAGE,
	    SS_SKILL_HW_REPAIR_WFI,
	  SS_SKILL_CRYPTOLOGY,
	    SS_SKILL_CRYPTOLOGY_WFI,
	  SS_SKILL_MUSICIANSHIP,
	SS_CLOSE_SKILLS,
} skills_state_t;

typedef enum skills_t {
	BARGAINING = 0,
	COPTALK,
	WAREZ_ANALYSIS,
	DEBUG,
	HW_REPAIR,
	ICE_BRAKING,
	EVASION,
	CRYPTOLOGY,
	JAPANESE,
	LOGIC,
	PSYCHOANALYSIS,
	PHENOMENOLOGY,
	PHILOSOPHY,
	SOPHISTRY,
	ZEN,
	MUSICIANSHIP,
} skills_t;

static skills_state_t g_state = SS_OPEN_SKILLS;
static uint16_t g_skills_total = 0;
static uint16_t g_skills[16] = { 0x00, };
static uint16_t g_listed_skills[4] = { 0x00 };
static int g_wfi_release_lock = 0;

static skills_state_t skills_page(int next)
{
	static int listed = 0;

	neuro_menu_flush();
	neuro_menu_flush_items();
	neuro_menu_draw_text("SKILLS", 9, 0);

	if (next == 0)
	{
		listed = 0;
	}

	if (g_skills_total > 4)
	{
		neuro_menu_draw_text("more", 12, 5);
		neuro_menu_add_item(12, 5, 4, 10, 'm');
	}

	neuro_menu_draw_text("exit", 7, 5);
	neuro_menu_add_item(7, 5, 4, 11, 'x');

	for (int i = 0; i < 4; i++)
	{
		char skill_string[25] = { 0, };
		sprintf(skill_string, "%24s", " ");

		if (listed < g_skills_total)
		{
			skill_string[0] = i + '1';
			skill_string[1] = '.';

			char *skill_name = get_item_name(g_skills[listed], NULL);
			memmove(skill_string + 3, skill_name, strlen(skill_name));

			int skill_index = g_skills[listed] - 0x43;
			int skill_level = g_3f85.skills[skill_index];
			char skill_level_string[3] = { 0, };

			sprintf(skill_level_string, "%2d", skill_level + 1);
			memmove(skill_string + 21, skill_level_string, 2);

			neuro_menu_add_item(0, i + 1, 24, i, i + '1');
			g_listed_skills[i] = g_skills[listed];
			listed++;
		}

		neuro_menu_draw_text(skill_string, 0, i + 1);
	}

	listed %= g_skills_total;

	return SS_SKILLS_PAGE;
}

typedef void(*pfn_item_page_draw_item_cb)(uint16_t, uint16_t);
typedef void(*pfn_item_page_setup_cb)(uint16_t, uint16_t);
static uint8_t *g_inventory = NULL;
static uint8_t *g_listed_items[4];

static item_page_draw_item(uint16_t _item_num, uint16_t t, int item)
{
	uint8_t *inventory = g_inventory;
	uint8_t *item_ptr;
	uint16_t item_num = _item_num;

	do {
		while (*inventory == 0xFF)
		{
			inventory += 4;
		}

		item_ptr = inventory;
		inventory += 4;

	} while (item_num--);

	g_listed_items[_item_num % 4] = item_ptr;

	char s[30] = { 0 };
	char prefix = (item_ptr[2] == 0) ? ' ' : '-';
	char *name = get_item_name(item_ptr[0], NULL);

	if (item == 0)
	{	
		sprintf(s, "%c%-15s%2d.0", prefix, name, item_ptr[1]);
	}
	else
	{
		sprintf(s, "%c%s", prefix, name);
	}

	neuro_menu_draw_text(s, 3, t);
}

static void hw_item_page_draw_item(uint16_t item_num, uint16_t t)
{
	item_page_draw_item(item_num, t, 1);
}

static void warez_item_page_draw_item(uint16_t item_num, uint16_t t)
{
	item_page_draw_item(item_num, t, 0);
}

static void item_page_setup(char *title, uint16_t max_items, uint16_t total_items)
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_add_item(8, 5, 4, 0x0A, 'x');
	neuro_menu_draw_text("exit", 8, 5);

	if (total_items > max_items)
	{
		neuro_menu_add_item(13, 5, 4, 0x0B, 'm');
		neuro_menu_draw_text("more", 13, 5);
	}

	neuro_menu_draw_text(title, 10, 0);
}

static void hw_item_page_setup(uint16_t max_items, uint16_t total_items)
{
	item_page_setup("Items", max_items, total_items);
}

static void warez_item_page_setup(uint16_t max_items, uint16_t total_items)
{
	item_page_setup("Warez", max_items, total_items);
}

static void item_page(uint16_t max_items, uint16_t total_items, int software,
	pfn_item_page_setup_cb fn_setup, pfn_item_page_draw_item_cb fn_draw_item,
	uint16_t t, uint16_t w, int next)
{
	static int listed = 0;

	if (next == 0)
	{
		listed = 0;
	}

	fn_setup(max_items, total_items);

	if (total_items)
	{
		if (total_items - listed < max_items)
		{
			max_items = total_items - listed;
		}

		for (int i = 0; i < max_items; i++)
		{
			char s[3] = { 0 };

			sprintf(s, "%d.", i + 1);
			neuro_menu_draw_text(s, 0, i + t);
			neuro_menu_add_item(0, i + t, w, i, i + '1');

			fn_draw_item(listed, i + t);
			listed++;
		}

		listed %= total_items;
	}
}

static void skills_item_page(int software, int next)
{
	uint16_t items = count_items(software);
	
	if (software)
	{
		g_inventory = g_3f85.inventory.software;
		item_page(4, items, software,
			warez_item_page_setup, warez_item_page_draw_item,
			1, 24, next);
	}
	else
	{
		g_inventory = g_3f85.inventory.items;
		item_page(4, items, software,
			hw_item_page_setup, hw_item_page_draw_item,
			1, 24, next);
	}
}

static void skills_draw_item_desc(uint8_t code, uint8_t version,
	uint16_t status, uint16_t l, uint16_t t)
{
	char s[40] = { 0 };

	if (code == 0x7F)
	{
		sprintf(s, "Credits %d", g_4bae.cash);
		neuro_menu_draw_text(s, l, t);
	}
	else
	{
		char *item_name = get_item_name(code, NULL);

		if (g_inventory_item_operations[code] & 0xC0)
		{
			neuro_menu_draw_text(item_name, l, t);
		}
		else
		{
			char prefix = (status == 0) ? ' ' : '-';
			sprintf(s, "%c%-11s %d.0", prefix, item_name, version);
			neuro_menu_draw_text(s, l, t);
		}
	}
}

/* 0x23D8 */
static uint16_t g_fix_difficulty[4] = {
	0x3F, 0x7F, 0xBF, 0xFF
};

static skills_state_t skill_hw_repair_apply(uint8_t *item)
{
	uint8_t item_code = item[0];

	if (item_code == 0)
	{
		item_code = 0x7F;
	}

	neuro_menu_draw_text("Hardware Repair", 4, 0);
	skills_draw_item_desc(item_code, 0, 0, 0, 2);

	if (item_code == 0x53 ||
		(item_code >= 0x1D && item_code <= 0x34))
	{
		if (item[2] == 0)
		{
			neuro_menu_draw_text("Hardware has no bugs", 0, 3);
		}
		else
		{
			uint8_t skill_level = g_3f85.skills[HW_REPAIR];

			if (item[2] > g_fix_difficulty[skill_level])
			{
				/* play track 6 */
				neuro_menu_draw_text("Unable to repair damage.", 0, 3);
			}
			else
			{
				item[2] = 0;
				/* play track 11 */
				neuro_menu_draw_text("Damage repaired,\nhardware okay", 0, 3);
			}
		}
	}
	else
	{
		neuro_menu_draw_text("Item not reparable", 0, 3);
	}

	neuro_menu_draw_text("Button or [space].", 3, 5);
	return SS_SKILL_HW_REPAIR_WFI;
}

static skills_state_t on_skill_musicianship_button(neuro_button_t *button)
{
	switch (button->code) {
	case 1:
	case 2:
	case 3:
	case 4: /* music */
		g_4bae.active_skill = MUSICIANSHIP;
		g_4bae.active_skill_level = button->code - 1;
		return SS_CLOSE_SKILLS;

	case 0: /* exit */
		g_4bae.active_skill = 0xFF;
		return SS_CLOSE_SKILLS;
	}

	return SS_SKILL_MUSICIANSHIP;
}

static skills_state_t on_skill_hw_repair_item_page_button(neuro_button_t *button)
{
	uint16_t code = button->code;

	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* items */
		neuro_menu_destroy();
		return skill_hw_repair_apply(g_listed_items[code]);

	case 0x0A: /* exit */
		neuro_menu_destroy();
		g_4bae.active_skill = 0xFF;
		return SS_CLOSE_SKILLS;

	case 0x0B: /* more */
		skills_item_page(0, 1);
		break;

	default:
		break;
	}

	return SS_SKILL_HW_REPAIR_ITEM_PAGE;
}

static skills_state_t skill_debug_apply(uint8_t *item)
{
	neuro_menu_draw_text("Debug", 9, 0);
	skills_draw_item_desc(item[0], item[1], 0, 0, 2);

	if (item[2] == 0)
	{
		neuro_menu_draw_text("Program has no bugs", 0, 3);
	}
	else
	{
		uint8_t skill_level = g_3f85.skills[DEBUG];

		if (item[2] > g_fix_difficulty[skill_level])
		{
			/* play track 6 */
			neuro_menu_draw_text("Unable to fix bugs", 0, 3);
		}
		else
		{
			item[2] = 0;
			/* play track 11 */
			neuro_menu_draw_text("Bug fixed, program\nokay", 0, 3);
		}
	}

	neuro_menu_draw_text("Button or [space].", 3, 5);
	return SS_SKILL_DEBUG_WFI;
}

static skills_state_t on_skill_debug_item_page_button(neuro_button_t *button)
{
	uint16_t code = button->code;

	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* warez */
		neuro_menu_destroy();
		return skill_debug_apply(g_listed_items[code]);

	case 0x0A: /* exit */
		neuro_menu_destroy();
		g_4bae.active_skill = 0xFF;
		return SS_CLOSE_SKILLS;

	case 0x0B: /* more */
		skills_item_page(1, 1);
		break;

	default:
		break;
	}

	return SS_SKILL_DEBUG_ITEM_PAGE;
}

/* 0x23CA */
static uint8_t g_warez_analysis_op_string_map[14] = {
	0, 1, 2, 3, 4, 5, 0, 6, 6, 7, 8, 9, 10, 10
};
static char *g_warez_desc[11] = {
	"Unknown.",
	"A cyberspace info\n""program.",
	"A cyberspace ICE breaker\n""program.",
	"A cyberspace virus\n""program.",
	"A cyberspace shielding\n""program.",
	"A cyberspace ICE bypass\n""program.",
	"A cyberspace interface\n""corruptor program.",
	"A database password\n""generator.",
	"A database info program.",
	"A database chess program.",
	"A system communications\n""program.",
};

static skills_state_t skill_warez_analysis_apply(uint8_t *item)
{
	uint8_t item_code = item[0];
	uint8_t item_op = g_inventory_item_operations[item_code];
	uint8_t item_op_n = item_op & 0x0F;
	uint8_t op_string_n = 0;

	if (item_op_n == 9 || item_op_n == 11)
	{
		op_string_n = 0;
	}
	else
	{
		if ((item_op & 0x30) == 0x10)
		{
			op_string_n = item_op_n + 9;
		}
		else if ((item_op & 0x30) == 0x20)
		{
			op_string_n = item_op_n;
		}
		else if (item_op_n <= 8)
		{
			op_string_n = item_op_n + 12;
		}
	}

	op_string_n = g_warez_analysis_op_string_map[op_string_n];

	neuro_menu_draw_text("Software Analysis", 3, 0);
	skills_draw_item_desc(item_code, item[1], 0, 0, 2);
	neuro_menu_draw_text(g_warez_desc[op_string_n], 0, 3);

	neuro_menu_draw_text("Button or [space].", 3, 5);
	return SS_SKILL_WAREZ_ANAYSIS_WFI;
}

static skills_state_t on_skill_warez_analysis_item_page_button(neuro_button_t *button)
{
	uint16_t code = button->code;

	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* warez */
		neuro_menu_destroy();
		return skill_warez_analysis_apply(g_listed_items[code]);

	case 0x0A: /* exit */
		neuro_menu_destroy();
		g_4bae.active_skill = 0xFF;
		return SS_CLOSE_SKILLS;

	case 0x0B: /* more */
		skills_item_page(1, 1);
		break;

	default:
		break;
	}

	return SS_SKILL_WAREZ_ANALYSIS_ITEM_PAGE;
}

static skills_state_t skills_use_cryptology()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	neuro_menu_draw_text("Cryptology", 7, 0);
	neuro_menu_draw_text("Enter word to decode:", 0, 1);
	neuro_menu_draw_text("<", 0, 2);

	return SS_SKILL_CRYPTOLOGY;
}

static skills_state_t skills_use_musicianship()
{
	neuro_menu_flush();
	neuro_menu_flush_items();

	skills_draw_item_desc(0x52, 0, 0, 6, 0);
	neuro_menu_draw_text("X. Exit Skill Chip", 3, 1);
	neuro_menu_draw_text("1. Play Dub", 3, 2);
	neuro_menu_draw_text("2. Play Jazz", 3, 3);
	neuro_menu_draw_text("3. Play New Wave", 3, 4);
	neuro_menu_draw_text("4. Play Classical", 3, 5);

	g_4bae.active_skill_level = 0xFF;

	neuro_menu_add_item(3, 1, 18, 0, 'x');

	for (int i = 1; i <= 4; i++)
	{
		neuro_menu_add_item(3, i + 1, 17, i, i + '0');
	}

	return SS_SKILL_MUSICIANSHIP;
}

static skills_state_t skills_use_hw_repair()
{
	neuro_menu_flush();
	neuro_menu_flush_items();
	neuro_menu_create(6, 6, 16, 24, 6, (uint8_t*)&g_seg011 + 0x5A0A);
	skills_item_page(0, 0);

	return SS_SKILL_HW_REPAIR_ITEM_PAGE;
}

static skills_state_t skills_use_warez_skill(uint16_t skill)
{
	neuro_menu_flush();
	neuro_menu_create(6, 6, 16, 24, 6, (uint8_t*)&g_seg011 + 0x5A0A);
	skills_item_page(1, 0);

	switch (skill) {
	case WAREZ_ANALYSIS:
		return SS_SKILL_WAREZ_ANALYSIS_ITEM_PAGE;

	case DEBUG:
		return SS_SKILL_DEBUG_ITEM_PAGE;
	}

	neuro_menu_destroy();
	return SS_CLOSE_SKILLS;
}

static skills_state_t skills_use(uint16_t skill)
{
	skill -= 0x43;

	if (g_4bae.ui_type == 1)
	{
		if (skill <= 15 &&
			(skill == BARGAINING || skill == COPTALK ||
			 skill == DEBUG || skill == HW_REPAIR ||
			 skill == JAPANESE || skill == MUSICIANSHIP))
		{
			/* play track 6 */
			neuro_menu_flush();
			neuro_menu_flush_items();

			neuro_menu_draw_text("SKILLS", 9, 0);
			neuro_menu_draw_text("Can't do that here.", 0, 3);

			return SS_CANT_USE_WFI;
		}
	}

	g_4bae.active_skill = (uint8_t)skill;
	g_4bae.active_skill_level = g_3f85.skills[skill];

	switch (skill) {
	case WAREZ_ANALYSIS:
	case DEBUG:
		return skills_use_warez_skill(skill);

	case HW_REPAIR:
		return skills_use_hw_repair();

	case CRYPTOLOGY:
		return skills_use_cryptology();

	case MUSICIANSHIP:
		return skills_use_musicianship();

	default:
		return SS_CLOSE_SKILLS;
	}

	return SS_SKILLS_PAGE;
}

static skills_state_t on_skills_page_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3: /* skills */
		return skills_use(g_listed_skills[button->code]);

	case 0x0A: /* more */
		return skills_page(1);

	case 0x0B: /* exit */
		return SS_CLOSE_SKILLS;

	default:
		break;
	}

	return SS_SKILLS_PAGE;
}

void skills_menu_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case SS_SKILLS_PAGE:
		*state = on_skills_page_menu_button(button);
		break;

	case SS_SKILL_WAREZ_ANALYSIS_ITEM_PAGE:
		*state = on_skill_warez_analysis_item_page_button(button);
		break;

	case SS_SKILL_DEBUG_ITEM_PAGE:
		*state = on_skill_debug_item_page_button(button);
		break;

	case SS_SKILL_HW_REPAIR_ITEM_PAGE:
		*state = on_skill_hw_repair_item_page_button(button);
		break;

	case SS_SKILL_MUSICIANSHIP:
		*state = on_skill_musicianship_button(button);
		break;

	default:
		break;
	}
}

/* 0x23E0 */
static char *g_decodable_words[20] = {
	"DUMBO",
	"IMASMURF",
	"SMEEGLDIPO",
	"ABURAKKOI",
	"KIKENNA",
	"PANCAKE",
	"SNORSKEE",
	"AGABATUR",
	"EINHOVEN",
	"VULCAN",
	"SELIM",
	"GALILEO",
	"GNU",
	"TRIDENT",
	"OGRAF",
	"EGGPLANT",
	"TURNIP",
	"PLEIADES",
	"CAL23LNZD8",
	"THERMOPYLAE"
};
static char *g_decoded_words[20] = {
	"ROMCARDS",
	"WARRANTS",
	"PERMAFROST",
	"UCHIKATSU",
	"PERILOUS",
	"VENDORS",
	"SUPERTAC",
	"EINTRITT",
	"VERBOTEN",
	"BIOSOFT",
	"BIOTECH",
	"APOLLO",
	"YAK",
	"FUNGEKI",
	"AUDIT",
	"LONGISLAND",
	"LOSER",
	"SUBARU",
	"NINJUTSU",
	"SOCRATES"
};

/* 0x2430 */
static uint8_t g_decoding_difficulty[20] = {
	0, 0, 1, 1, 1, 0, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 7, 8
};

static void skill_cryptology_apply(char *word)
{
	int i = 0;

	for (i = 0; i < 20; i++)
	{
		if (_strcmpi(word, g_decodable_words[i]))
		{
			continue;
		}

		uint8_t skill_level = g_3f85.skills[CRYPTOLOGY];
		
		if (g_decoding_difficulty[i] > skill_level)
		{
			i = 20;
			break;
		}

		neuro_menu_draw_text(g_decoded_words[i], 0, 4);
		neuro_menu_draw_text("Uncoded word is:", 0, 3);
		break;
	}

	if (i == 20)
	{
		neuro_menu_draw_text("Unable to decode word.", 0, 3);
	}

	neuro_menu_draw_text("Button or [space].", 3, 5);
}

static skills_state_t on_skill_cryptology_text_enter(sfTextEvent *event)
{
	static char word_to_decode[17] = { 0, };
	sfKeyCode key = sfHandleTextInput(event->unicode, word_to_decode, 17, 0, 0);

	if (key == sfKeyReturn)
	{
		skill_cryptology_apply(word_to_decode);
		*word_to_decode = 0;
		g_wfi_release_lock = 1;
		return SS_SKILL_CRYPTOLOGY_WFI;
	}
	else if (key == sfKeyEscape)
	{
		*word_to_decode = 0;
		return SS_CLOSE_SKILLS;
	}
	else
	{
		neuro_menu_draw_text("                 ", 0, 2);
		neuro_menu_draw_text(word_to_decode, 0, 2);
		neuro_menu_draw_text("<", (uint16_t)strlen(word_to_decode), 2);
	}

	return SS_SKILL_CRYPTOLOGY;
}

void skills_menu_handle_text_enter(int *state, sfTextEvent *event)
{
	switch (*state) {
	case SS_SKILL_CRYPTOLOGY:
		*state = on_skill_cryptology_text_enter(event);
		break;
	}
}

static skills_state_t handle_skills_wait_for_input(skills_state_t state, sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		if (g_wfi_release_lock)
		{
			g_wfi_release_lock = 0;
			return state;
		}

		switch (state) {
		case SS_NO_SKILLS_WFI:
			return SS_CLOSE_SKILLS;

		case SS_CANT_USE_WFI:
			return skills_page(0);

		case SS_SKILL_WAREZ_ANAYSIS_WFI:
			return skills_use_warez_skill(WAREZ_ANALYSIS);

		case SS_SKILL_DEBUG_WFI:
			return skills_use_warez_skill(DEBUG);

		case SS_SKILL_HW_REPAIR_WFI:
			return skills_use_hw_repair();

		case SS_SKILL_CRYPTOLOGY_WFI:
			return skills_use_cryptology();

		default:
			return state;
		}
	}

	return state;
}

void handle_skills_input(sfEvent *event)
{
	switch (g_state) {
	case SS_NO_SKILLS_WFI:
	case SS_CANT_USE_WFI:
	case SS_SKILL_WAREZ_ANAYSIS_WFI:
	case SS_SKILL_DEBUG_WFI:
	case SS_SKILL_HW_REPAIR_WFI:
	case SS_SKILL_CRYPTOLOGY_WFI:
		g_state = handle_skills_wait_for_input(g_state, event);
		break;

	case SS_SKILLS_PAGE:
	case SS_SKILL_WAREZ_ANALYSIS_ITEM_PAGE:
	case SS_SKILL_DEBUG_ITEM_PAGE:
	case SS_SKILL_HW_REPAIR_ITEM_PAGE:
	case SS_SKILL_CRYPTOLOGY:
	case SS_SKILL_MUSICIANSHIP:
		neuro_menu_handle_input(NMID_SKILLS_MENU, &g_neuro_menu, (int*)&g_state, event);
		break;

	default:
		break;
	}
}

static int skills_prepare()
{
	int j = 0;

	for (int i = 0; i < 16; i++)
	{
		if (g_3f85.skills[i] == 0xFF)
		{
			continue;
		}

		g_skills[j++] = i + 0x43;
	}

	return j;
}

static skills_state_t skills_show()
{
	neuro_menu_create(6, 7, 17, 25, 6, NULL);

	g_skills_total = skills_prepare();
	if (!g_skills_total)
	{
		/* play track 6 */
		neuro_menu_draw_text("No skills.", 0, 2);
		return SS_NO_SKILLS_WFI;
	}

	return skills_page(0);
}

static window_folding_frame_data_t g_close_frame_data[12] = {
	{ 216, 64, 48, 128 }, { 216, 62,  48, 129 }, { 216, 56,  48, 132 },
	{ 216, 48, 48, 136 }, { 216, 36,  48, 142 }, { 216, 20,  48, 150 },
	{ 210,  2, 51, 159 }, { 202,  2,  55, 159 }, { 188,  2,  62, 159 },
	{ 162,  2, 75, 159 }, { 108,  2, 102, 159 }, {   2,  2, 155, 159 },

}, g_open_frame_data[12] = {
	{   2,  2, 155, 159 }, {   6,  2, 153, 159 }, {  14,  2, 149, 159 },
	{  28,  2, 142, 159 }, {  54,  2, 129, 159 }, { 108,  2, 102, 159 },
	{ 216,  3,  48, 159 }, { 216,  5,  48, 157 }, { 216, 11,  48, 154 },
	{ 216, 21,  48, 149 }, { 216, 33,  48, 143 }, { 216, 64,  48, 128 }
};

static window_folding_data_t g_skills_anim_data = {
	.total_frames = 12,
	.frame_cap = 30,
	.pixels = g_seg011.data,
};

real_world_state_t update_skills()
{
	static int anim = 0;

	switch (g_state) {
	case SS_OPEN_SKILLS:
	case SS_CLOSE_SKILLS:
		if (!anim)
		{
			anim = 1;
			g_skills_anim_data.frame_data = (g_state == SS_OPEN_SKILLS) ?
				g_open_frame_data : g_close_frame_data;
			g_skills_anim_data.sprite_chain_index = (g_state == SS_OPEN_SKILLS) ?
				g_4bae.frame_sc_index : g_4bae.frame_sc_index + 1;
			window_animation_setup(WA_TYPE_WINDOW_FOLDING, &g_skills_anim_data);
		}
		else if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			anim = 0;
			if (g_state == SS_OPEN_SKILLS)
			{
				g_state = skills_show();
			}
			else
			{
				neuro_menu_destroy();
				g_state = SS_OPEN_SKILLS;
				return RWS_NORMAL;
			}
		}
		break;
	}

	return RWS_SKILLS;
}
