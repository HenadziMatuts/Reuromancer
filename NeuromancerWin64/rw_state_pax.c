#include "data.h"
#include "globals.h"
#include "resource_manager.h"
#include "scene_real_world.h"
#include "neuro_window_control.h"
#include "drawing_control.h"
#include "window_animation.h"
#include <neuro_routines.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum pax_state_t {
	PS_OPEN_PAX = 0,
	PS_MAIN_MENU,
	PS_USER_INFO,
	  PS_USER_INFO_WFI,
	  PS_USER_INFO_END_WFI,
	PS_BANKING,
	  PS_BANK_DOWNLOAD,
	  PS_BANK_UPLOAD,
	  PS_BANK_TRANSACTIONS_WFI,
	PS_NEWS_MENU,
	  PS_NEWS,
	    PS_NEWS_WFI,
	    PS_NEWS_END_WFI,
	PS_BOARD_MENU,
	  PS_BOARD_VIEW_MENU,
	   PS_BOARD_MSG,
	     PS_BOARD_MSG_WFI,
	     PS_BOARD_MSG_END_WFI,
	  PS_BOARD_SEND_MSG_ADDRESSEE,
	  PS_BOARD_SEND_MSG_TEXT,
	  PS_BOARD_SEND_MSG_ACCEPT,
	PS_CLOSE_PAX
} pax_state_t;

static pax_state_t g_state = PS_OPEN_PAX;
static uint8_t g_pax_data[9216] = { 0, };

static char g_send_msg_carriage_pos = 0;
static char g_send_msg_addressee[12] = { 0, };
static char g_send_msg_text[116] = { 0, };

static text_scrolling_data_t g_text_scrolling_data = {
	.frame_cap = 18,
};

static char* trim(char *s)
{
	char *start = s;
	char *end = s + strlen(s);

	while (*start == ' ')
	{
		start++;
	}

	if (start == end)
	{
		*s = 0;
		return s;
	}

	while (*--end == ' ');
	
	int64_t len = end - start + 1;

	memmove(s, start, len);
	s[len] = 0;

	return s;
}

void pax_send_mgs()
{
	char *addressee = trim(g_send_msg_addressee);
	char *text = g_send_msg_text;

	for (size_t s = 0; s < strlen(addressee); s++)
	{
		addressee[s] = tolower(addressee[s]);
	}

	if (strcmp(addressee, "armitage") ||
		!strstr(text, "056306118") ||
		g_4bae.msg_to_armitage_sent != 0)
	{
		return;
	}

	g_4bae.msg_to_armitage_sent = 1;
	g_4bae.x4c5c = 0;

	uint16_t index = g_4bae.bank_last_transacton_record_index;

	g_4bae.bank_transaction_record[index].op = (g_4bae.date_day & 0x3F) | 0x80;
	g_4bae.bank_transaction_record[index].amount = 10000;

	g_4bae.bank_last_transacton_record_index = (index + 1) & 3;
	g_4bae.bank_account += 10000;
}

static pax_state_t pax_board_send()
{
	char date_string[9] = { 0 };
	char string[80] = { 0 };

	neuro_window_clear();
	neuro_window_flush_buttons();

	build_date_string(date_string, g_4bae.date_day);
	sprintf(string, ""
		"     Send Message\n"
		"\n"
		"Press ESC when done\n"
		"\n"
		"date : %s\n"
		"to   : <",
		date_string);

	neuro_window_set_draw_string_offt(8, 8);
	neuro_window_draw_string(string, 1);

	memset(g_send_msg_addressee, 0x20, 11);
	g_send_msg_addressee[11] = 0;

	memset(g_send_msg_text, 0x20, 115);
	g_send_msg_text[38] = '\n';
	g_send_msg_text[77] = '\n';
	g_send_msg_text[115] = 0;

	g_send_msg_carriage_pos = 0;

	return PS_BOARD_SEND_MSG_ADDRESSEE;
}

typedef enum pax_info_menu_type_t {
	PIMT_NEWS = 0,
	PIMT_BOARD,
} pax_info_menu_type_t;

static int pax_info_menu_prepare_list(uint16_t *infos_list, int type)
{
	int entries = 0;

	union {
		pax_news_hdr_t *news;
		pax_board_msg_hdr_t *msg;
		uint8_t *p;
	} info;

	info.p = (type == PIMT_NEWS) ?
		(uint8_t*)g_seg004.pax_news : (uint8_t*)g_seg004.pax_board_msg;

	for (int i = 0; i < 32; i++, info.p += 40)
	{
		char date_string[9] = { 0, };

		if (info.news->flag & 0x80)
		{
			build_date_string(date_string, g_4bae.date_day);
			strcpy(info.news->date, date_string);
			info.news->flag &= 0x7F;
		}

		switch (info.news->flag & 0x0F) {
		case 0x00:
			infos_list[entries++] = i;
			continue;

		case 0x01: {
			uint16_t offt = info.news->addr - 0x4BAE;
			int8_t *p = g_4bae.x4bae + offt;

			if (*p > info.news->val)
			{
				infos_list[entries++] = i;
			}

			continue;
		}

		case 0x02: {
			uint16_t offt = info.news->addr - 0x4BAE;
			int8_t *p = g_4bae.x4bae + offt;

			if (*p != info.news->val)
			{
				infos_list[entries++] = i;
			}

			continue;
		}

		case 0x03:
			i = 31;
			continue;

		default:
			continue;
		}
	}

	return entries;
}

static neuro_button_t *g_info_menu_items = (neuro_button_t*)(g_stack + 0x7C4);
static uint16_t g_info_menu_entries[5];

static pax_state_t pax_info_menu(int type, int next_page)
{
	static int items_listed = 0;
	int list_entries = 0;
	uint16_t infos_list[32];
	int max_items = 0;
	neuro_button_t *item = g_info_menu_items;

	union {
		pax_news_hdr_t *news;
		pax_board_msg_hdr_t *msg;
		uint8_t *p;
	} info;

	info.p = (type == PIMT_NEWS) ?
		(uint8_t*)g_seg004.pax_news : (uint8_t*)g_seg004.pax_board_msg;

	list_entries = pax_info_menu_prepare_list(infos_list, type);

	if (next_page == 0)
	{
		items_listed = 0;
	}

	max_items = list_entries - items_listed;
	max_items = (max_items > 5) ? 5 : max_items;

	neuro_window_flush_buttons();

	int i = 0;
	for (i = 0; i < 5; i++, item++)
	{
		neuro_window_set_draw_string_offt(8, (i * 8) + 40);

		if (i < max_items)
		{
			char item_string[40] = { 0, };

			if (type == PIMT_NEWS)
			{
				pax_news_hdr_t *p = &info.news[infos_list[items_listed]];
				sprintf(item_string, "%d. %8s %s", i + 1, p->date, p->subject);
			}
			else
			{
				pax_board_msg_hdr_t *p = &info.msg[infos_list[items_listed]];
				sprintf(item_string, "%d. %8s %-12s %-13s", i + 1, p->date,
					(*p->to == 0x01) ? g_4bae.name + 2 : p->to, p->from);
			}

			g_info_menu_entries[i] = infos_list[items_listed++];
			items_listed %= list_entries;

			item->left = 8;
			item->top = (i * 8) + 44;
			item->right = 311;
			item->bottom = (i * 8) + 51;
			item->code = i;
			item->label = '0x31' + i;

			neuro_window_add_button(item);
			neuro_window_draw_string(item_string, 1);
		}
		else
		{
			neuro_window_draw_string(""
				"                    "
				"                    ", 1);
		}
	}
	items_listed %= list_entries;

	char exit_string[10] = { 0 };
	strcpy(exit_string, "exit");
	neuro_window_add_button(&g_pax_info_menu_buttons.exit);

	if (list_entries > 5)
	{
		strcat(exit_string, " more");
		neuro_window_add_button(&g_pax_info_menu_buttons.more);
	}

	neuro_window_set_draw_string_offt(120, 88);
	neuro_window_draw_string(exit_string, 2);

	return (type == PIMT_NEWS) ? PS_NEWS_MENU : PS_BOARD_VIEW_MENU;
}

static pax_state_t pax_board_view()
{
	assert(resource_manager_load_resource("PAXBBS.BIH", g_pax_data));

	neuro_window_clear();
	neuro_window_set_draw_string_offt(8, 8);
	neuro_window_draw_string(""
		"Bulletin Board\n"
		"\n"
		"       date to           from");

	return pax_info_menu(PIMT_BOARD, 0);
}

static pax_state_t pax_board()
{
	neuro_window_clear();
	neuro_window_flush_buttons();

	neuro_window_set_draw_string_offt(8, 8);
	neuro_window_draw_string(""
		"Bulletin Board\n"
		"\n"
		"X. Exit To Main\n"
		"V. View Messages\n"
		"S. Send Message", 1);

	neuro_window_add_button(&g_pax_board_menu_buttons.exit);
	neuro_window_add_button(&g_pax_board_menu_buttons.view);
	neuro_window_add_button(&g_pax_board_menu_buttons.send);

	return PS_BOARD_MENU;
}

static pax_state_t pax_news()
{
	assert(resource_manager_load_resource("NEWS.BIH", g_pax_data));

	neuro_window_clear();
	neuro_window_set_draw_string_offt(8, 8);
	neuro_window_draw_string(""
		"      Night City News\n"
		"\n"
		"   date     subject", 1);

	return pax_info_menu(PIMT_NEWS, 0);
}

static pax_state_t pax_bank_transactions()
{
	neuro_window_flush_buttons();

	neuro_window_set_draw_string_offt(8, 48);
	neuro_window_draw_string(""
		"day      type       amount\n"
		"                     \n"
		"                     \n"
		"                     \n"
		"                     \n"
		"Button or [space] to continue.", 1);

	for (int i = 0, index = g_4bae.bank_last_transacton_record_index; i < 4; i++)
	{
		char op_string[11] = { 0 };
		char date_string[9] = { 0 };
		char transaction_string[32] = { 0 };
		uint16_t op = g_4bae.bank_transaction_record[index].op;

		switch (op >> 6) {
		case 0: strcpy(op_string, "Upload");
			    break;
		case 1: strcpy(op_string, "Download");
			    break;
		case 2: strcpy(op_string, "TransferIn");
			    break;
		case 3: strcpy(op_string, "Fined");
			    break;
		}
		
		build_date_string(date_string, op & 0x3F);
		sprintf(transaction_string, "%s %-10s %8d",
			date_string, op_string, g_4bae.bank_transaction_record[index].amount);

		neuro_window_set_draw_string_offt(8, (i * 8) + 56);
		neuro_window_draw_string(transaction_string, 1);

		index = (index + 1) & 3;
	}

	return PS_BANK_TRANSACTIONS_WFI;
}

static pax_state_t pax_bank_account_operation(int download)
{
	neuro_window_flush_buttons();

	neuro_window_set_draw_string_offt(8, 48);
	neuro_window_draw_string(""
		"Enter amount : <\n"
		"                     \n"
		"                     \n"
		"                     \n"
		"                     \n", 1);

	neuro_window_set_draw_string_offt(128, 48);
	return (download) ? PS_BANK_DOWNLOAD : PS_BANK_UPLOAD;
}

static void pax_banking_prepare()
{
	/* stub */
}

static pax_state_t pax_banking()
{
	char str[9] = { 0, };

	pax_banking_prepare();

	neuro_window_clear();
	neuro_window_flush_buttons();

	neuro_window_set_draw_string_offt(8, 8);
	neuro_window_draw_string(""
		"  First Orbital Bank of Switzerland\n"
		"\n"
		"  name:           BAMA id = 056306118\n"
		"  chip =          account =\n"
		"\n"
		"X. Exit To Main\n"
		"D. Download credits\n"
		"U. Upload credits\n"
		"T. Transaction record", 1);

	neuro_window_set_draw_string_offt(72, 24);
	neuro_window_draw_string(g_4bae.name + 2, 2);

	neuro_window_set_draw_string_offt(80, 32);
	sprintf(str, "%d", g_4bae.cash);
	neuro_window_draw_string(str, 2);

	neuro_window_set_draw_string_offt(232, 32);
	sprintf(str, "%d", g_4bae.bank_account);
	neuro_window_draw_string(str, 2);

	neuro_window_add_button(&g_pax_banking_buttons.exit);
	neuro_window_add_button(&g_pax_banking_buttons.download);
	neuro_window_add_button(&g_pax_banking_buttons.upload);
	neuro_window_add_button(&g_pax_banking_buttons.transactions);

	return PS_BANKING;
}

static pax_state_t pax_user_info()
{
	assert(resource_manager_load_resource("FTUSER.TXH", g_pax_data));

	neuro_window_clear();
	neuro_window_flush_buttons();

	neuro_window_set_draw_string_offt(8, 8);
	neuro_window_draw_string(g_pax_data, 1);

	g_text_scrolling_data.text = g_pax_data + strlen(g_pax_data) + 1;
	window_animation_setup(WA_TYPE_TEXT_SCROLLING, &g_text_scrolling_data);

	return PS_USER_INFO;
}

static pax_state_t pax_main_menu()
{
	neuro_window_clear();

	neuro_window_set_draw_string_offt(8, 8);
	neuro_window_draw_string(""
		"X. Exit System\n"
		"1. First Time PAX User Info.\n"
		"2. Access Banking Interlink\n"
		"3. Night City News\n"
		"4. Bulletin Board\n"
		"\n\n\n"
		"        choose a function", 1);

	neuro_window_flush_buttons();
	neuro_window_add_button(&g_pax_buttons.exit);
	neuro_window_add_button(&g_pax_buttons.user_info);
	neuro_window_add_button(&g_pax_buttons.banking);
	neuro_window_add_button(&g_pax_buttons.news);
	neuro_window_add_button(&g_pax_buttons.board);

	return PS_MAIN_MENU;
}

static pax_state_t on_pax_board_send_msg_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0: /* yes */
		pax_send_mgs();
	case 1: /* no */
		return pax_board();
	}

	return PS_BOARD_SEND_MSG_ACCEPT;
}

static pax_state_t on_pax_board_view_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4: { /* items */
		uint16_t index = g_info_menu_entries[button->code];
		char *text = g_pax_data;

		for (int i = 0; i < index; i++)
		{
			while (*text++);
		}

		neuro_window_clear();
		neuro_window_flush_buttons();

		neuro_window_set_draw_string_offt(8, 8);
		neuro_window_draw_string(g_seg004.pax_board_msg[index].date, 1);

		g_text_scrolling_data.text = text;
		window_animation_setup(WA_TYPE_TEXT_SCROLLING, &g_text_scrolling_data);

		return PS_BOARD_MSG;
	}

	case 0x0A: /* exit */
		return pax_board();

	case 0x0B: /* more */
		return pax_info_menu(PIMT_BOARD, 1);

	default:
		break;
	}

	return PS_BOARD_VIEW_MENU;
}

static pax_state_t on_pax_board_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0: /* exit */
		return pax_main_menu();

	case 1: /* view */
		return pax_board_view();

	case 2: /* send */
		return pax_board_send();

	default:
		break;
	}

	return PS_BOARD_MENU;
}

static pax_state_t on_pax_news_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4: { /* items */
		uint16_t index = g_info_menu_entries[button->code];
		char *text = g_pax_data;

		for (int i = 0; i < index; i++)
		{
			while (*text++);
		}

		neuro_window_clear();
		neuro_window_flush_buttons();
		neuro_window_set_draw_string_offt(8, 8);

		char subject_string[40] = { 0 };
		sprintf(subject_string, "%s %s",
			g_seg004.pax_news[index].date, g_seg004.pax_news[index].subject);
		neuro_window_draw_string(subject_string, 1);

		g_text_scrolling_data.text = text;
		window_animation_setup(WA_TYPE_TEXT_SCROLLING, &g_text_scrolling_data);
		return PS_NEWS;
	}

	case 0x0A: /*exit*/
		return pax_main_menu();

	case 0x0B: /* more */
		return pax_info_menu(PIMT_NEWS, 1);

	default:
		break;
	}

	return PS_NEWS_MENU;
}

static pax_state_t on_pax_banking_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0: /*to main menu*/
		return pax_main_menu();

	case 1: /* download credits */
		return pax_bank_account_operation(1);

	case 2: /* upload credits */
		return pax_bank_account_operation(0);

	case 3: /* show transactions */
		return pax_bank_transactions();

	default:
		break;
	}

	return PS_BANKING;
}

static pax_state_t on_pax_main_menu_button(neuro_button_t *button)
{
	switch (button->code) {
	case 0: /*exit*/
		return PS_CLOSE_PAX;

	case 1: /* user_info */
		return pax_user_info();

	case 2: /* banking */
		return pax_banking();

	case 3: /* news */
		return pax_news();

	case 4: /* board */
		return pax_board();

	default:
		break;
	}

	return PS_MAIN_MENU;
}

void rw_pax_handle_button_press(int *state, neuro_button_t *button)
{
	switch (*state) {
	case PS_MAIN_MENU:
		*state = on_pax_main_menu_button(button);
		break;

	case PS_BANKING:
		*state = on_pax_banking_button(button);
		break;

	case PS_NEWS_MENU:
		*state = on_pax_news_menu_button(button);
		break;

	case PS_BOARD_MENU:
		*state = on_pax_board_menu_button(button);
		break;

	case PS_BOARD_VIEW_MENU:
		*state = on_pax_board_view_menu_button(button);
		break;

	case PS_BOARD_SEND_MSG_ACCEPT:
		*state = on_pax_board_send_msg_button(button);
		break;

	default:
		break;
	}
}

static pax_state_t on_pax_board_send_msg_text_enter(int state, sfTextEvent *event)
{
	if (state == PS_BOARD_SEND_MSG_ADDRESSEE)
	{
		sfKeyCode key = sfHandleTextInput(event->unicode,
			g_send_msg_addressee + g_send_msg_carriage_pos,
			12 - g_send_msg_carriage_pos, 0, 1);
		size_t l = strlen(g_send_msg_addressee);

		if (key == sfKeyBack && g_send_msg_carriage_pos > 0)
		{
			g_send_msg_addressee[--g_send_msg_carriage_pos] = ' ';
		}
		else if (key == sfKeyCount && g_send_msg_carriage_pos < 11)
		{
			g_send_msg_carriage_pos++;
		}
		else if (key == sfKeyReturn)
		{
			neuro_window_set_draw_string_offt(64 + (g_send_msg_carriage_pos * 8), 48);
			neuro_window_draw_string(" ", 1);
			g_send_msg_carriage_pos = 0;

			return PS_BOARD_SEND_MSG_TEXT;
		}

		neuro_window_set_draw_string_offt(64, 48);
		neuro_window_draw_string("            ", 1);
		neuro_window_draw_string(g_send_msg_addressee, 1);
	}
	else
	{
		sfKeyCode key = sfHandleTextInput(event->unicode,
			g_send_msg_text + g_send_msg_carriage_pos,
			116 - g_send_msg_carriage_pos, 0, 1);

		if (key == sfKeyBack && g_send_msg_carriage_pos > 0)
		{
			if (g_send_msg_text[--g_send_msg_carriage_pos] == '\n')
			{
				while (g_send_msg_text[--g_send_msg_carriage_pos] == ' ');
				if (g_send_msg_text[g_send_msg_carriage_pos + 1] != '\n')
				{
					g_send_msg_carriage_pos++;
				}
			}

			g_send_msg_text[g_send_msg_carriage_pos] = ' ';
		}
		else if (key == sfKeyCount && g_send_msg_carriage_pos < 115)
		{
			if (g_send_msg_text[++g_send_msg_carriage_pos] == '\n')
			{
				g_send_msg_carriage_pos++;
			}
		}
		else if (key == sfKeyReturn)
		{
			if (g_send_msg_carriage_pos < 38)
			{
				g_send_msg_carriage_pos = 39;
			}
			else if (g_send_msg_carriage_pos >= 39 &&
				g_send_msg_carriage_pos < 77)
			{
				g_send_msg_carriage_pos = 78;
			}
		}
		else if (key == sfKeyEscape)
		{
			neuro_window_set_draw_string_offt(8, 24);
			neuro_window_draw_string("Send this message (Y/N)", 1);

			neuro_window_add_button(&g_pax_board_send_msg_buttons.yes);
			neuro_window_add_button(&g_pax_board_send_msg_buttons.no);

			state = PS_BOARD_SEND_MSG_ACCEPT;
		}

		neuro_window_set_draw_string_offt(8, 64);
		neuro_window_draw_string(""
			"                                      \n"
			"                                      \n"
			"                                      ", 1);
		neuro_window_draw_string(g_send_msg_text, 1);
	}

	return state;
}

static pax_state_t on_pax_bank_account_operation_text_enter(int state, sfTextEvent *event)
{
	static char input[9] = { 0 };
	sfKeyCode key = sfHandleTextInput(event->unicode, input, 8, 1, 0);
	char credits[10] = { 0 };

	if (key == sfKeyEscape)
	{
		memset(input, 0, 9);
		return pax_banking();
	}
	else if (key == sfKeyReturn)
	{
		uint32_t val = strlen(input) ? (uint32_t)atoi(input) : 0;
		memset(input, 0, 9);

		if (val == 0)
		{
			return pax_banking();
		}

		if (state == PS_BANK_DOWNLOAD)
		{
			if (val > g_4bae.bank_account)
			{
				return pax_banking();
			}

			g_4bae.bank_account -= val;
			g_4bae.cash += val;
		}
		else
		{
			if (val > g_4bae.cash)
			{
				return pax_banking();
			}

			g_4bae.cash -= val;
			g_4bae.bank_account += val;
		}

		uint16_t op = (state == PS_BANK_DOWNLOAD) ? 0x40 : 0;
		uint8_t index = g_4bae.bank_last_transacton_record_index++;

		g_4bae.bank_transaction_record[index].op = op | (g_4bae.date_day & 0x3F);
		g_4bae.bank_transaction_record[index].amount = val;
		g_4bae.bank_last_transacton_record_index &= 3;

		return pax_banking();
	}

	sprintf(credits, "%s<", input);
	memset(credits + strlen(credits), 0x20, 8 - strlen(credits));
	neuro_window_draw_string(credits, 1);

	return state;
}

void rw_pax_handle_text_enter(int *state, sfTextEvent *event)
{
	switch (*state) {
	case PS_BANK_DOWNLOAD:
	case PS_BANK_UPLOAD:
		*state = on_pax_bank_account_operation_text_enter(*state, event);
		break;

	case PS_BOARD_SEND_MSG_ADDRESSEE:
	case PS_BOARD_SEND_MSG_TEXT:
		*state = on_pax_board_send_msg_text_enter(*state, event);
		break;
	}
}

static void pax_board_send_msg_text_handle_kboard(sfKeyEvent *event)
{
	int pos = g_send_msg_carriage_pos;

	if (event->type == sfEvtKeyPressed)
	{
		if (event->code == sfKeyLeft && g_send_msg_carriage_pos > 0)
		{
			if (g_send_msg_text[--g_send_msg_carriage_pos] == '\n')
			{
				g_send_msg_carriage_pos--;
			}
		}
		else if (event->code == sfKeyRight && g_send_msg_carriage_pos < 115)
		{
			uint16_t cl = 8 + (g_send_msg_carriage_pos % 39) * 8;
			uint16_t ct = 64 + (g_send_msg_carriage_pos / 39) * 8;

			neuro_window_set_draw_string_offt(cl, ct);
			neuro_window_draw_string(" ", 1);

			if (g_send_msg_text[++g_send_msg_carriage_pos] == '\n')
			{
				g_send_msg_carriage_pos++;
			}
		}
		else if (event->code == sfKeyUp && g_send_msg_carriage_pos > 38)
		{
			g_send_msg_carriage_pos -= 39;
		}
		else if (event->code == sfKeyDown && g_send_msg_carriage_pos < 77)
		{
			g_send_msg_carriage_pos += 39;
		}
		else
		{
			return;
		}

		uint16_t cl = 8 + (pos % 39) * 8;
		uint16_t ct = 64 + (pos / 39) * 8;
		neuro_window_set_draw_string_offt(cl, ct);
		neuro_window_draw_string(" ", 1);
	}
}

static void pax_board_send_msg_adressee_handle_kboard(sfKeyEvent *event)
{
	int pos = g_send_msg_carriage_pos;

	if (event->type == sfEvtKeyPressed)
	{
		if (event->code == sfKeyLeft && g_send_msg_carriage_pos > 0)
		{
			g_send_msg_carriage_pos--;
		}
		else if (event->code == sfKeyRight && g_send_msg_carriage_pos < 11)
		{
			g_send_msg_carriage_pos++;
		}
		else
		{
			return;
		}

		neuro_window_set_draw_string_offt(64 + (pos * 8), 48);
		neuro_window_draw_string(" ", 1);
	}
}

void rw_pax_handle_kboard(int *state, sfKeyEvent *event)
{
	switch (*state) {
	case PS_BOARD_SEND_MSG_ADDRESSEE:
		pax_board_send_msg_adressee_handle_kboard(event);
		break;

	case PS_BOARD_SEND_MSG_TEXT: 
		pax_board_send_msg_text_handle_kboard(event);
		break;
	}
}

static pax_state_t handle_pax_wait_for_input(pax_state_t state, sfEvent *event)
{
	if (event->type == sfEvtMouseButtonReleased ||
		event->type == sfEvtKeyReleased)
	{
		switch (state) {
		case PS_USER_INFO_WFI:
			return PS_USER_INFO;

		case PS_USER_INFO_END_WFI:
			return pax_main_menu();

		case PS_BANK_TRANSACTIONS_WFI:
			return pax_banking();

		case PS_NEWS_WFI:
			return PS_NEWS;

		case PS_NEWS_END_WFI:
			return pax_news();

		case PS_BOARD_MSG_WFI:
			return PS_BOARD_MSG;

		case PS_BOARD_MSG_END_WFI:
			return pax_board_view();

		default:
			return state;
		}
	}

	return state;
}

void handle_pax_input(sfEvent *event)
{
	switch (g_state) {
	case PS_MAIN_MENU:
	case PS_BANKING:
	case PS_BANK_DOWNLOAD:
	case PS_BANK_UPLOAD:
	case PS_NEWS_MENU:
	case PS_BOARD_MENU:
	case PS_BOARD_VIEW_MENU:
	case PS_BOARD_SEND_MSG_ADDRESSEE:
	case PS_BOARD_SEND_MSG_TEXT:
	case PS_BOARD_SEND_MSG_ACCEPT:
		neuro_window_handle_input((int*)&g_state, event);
		break;

	case PS_USER_INFO_WFI:
	case PS_USER_INFO_END_WFI:
	case PS_BANK_TRANSACTIONS_WFI:
	case PS_NEWS_WFI:
	case PS_NEWS_END_WFI:
	case PS_BOARD_MSG_WFI:
	case PS_BOARD_MSG_END_WFI:
		g_state = handle_pax_wait_for_input(g_state, event);
		break;

	default:
		break;
	}
}

static void update_pax_board_send_msg(pax_state_t state)
{
	static int frame = 0;
	static int frame_cap_ms = 150;
	static int elapsed = 0;

	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));
	if (passed - elapsed <= frame_cap_ms)
	{
		return;
	}
	elapsed = passed;

	if (state == PS_BOARD_SEND_MSG_ADDRESSEE)
	{
		neuro_window_set_draw_string_offt(64, 48);
		neuro_window_draw_string(g_send_msg_addressee, 1);
		neuro_window_set_draw_string_offt(64 + (g_send_msg_carriage_pos * 8), 48);
	}
	else
	{
		neuro_window_set_draw_string_offt(8, 64);
		neuro_window_draw_string(g_send_msg_text, 1);

		uint16_t cl = 8 + (g_send_msg_carriage_pos % 39) * 8;
		uint16_t ct = 64 + (g_send_msg_carriage_pos / 39) * 8;
		neuro_window_set_draw_string_offt(cl, ct);
	}

	if (++frame == 1)
	{
		neuro_window_draw_string(" ", 1);
	}
	else if (frame == 2)
	{
		neuro_window_draw_string("<", 1);
		frame = 0;
	}
}

pax_state_t update_pax_text_scrolling(pax_state_t state)
{
	window_animation_event_t evt = window_animation_update();

	if (evt == WA_EVENT_WAIT_FOR_INPUT)
	{
		switch (state) {
		case PS_USER_INFO:
			return PS_USER_INFO_WFI;

		case PS_NEWS:
			return PS_NEWS_WFI;

		case PS_BOARD_MSG:
			return PS_BOARD_MSG_WFI;
		}
	}
	else if (evt == WA_EVENT_COMPLETED)
	{
		switch (state) {
		case PS_USER_INFO:
			return PS_USER_INFO_END_WFI;

		case PS_NEWS:
			return PS_NEWS_END_WFI;

		case PS_BOARD_MSG:
			return PS_BOARD_MSG_END_WFI;
		}
	}

	return state;
}

static window_folding_frame_data_t g_close_frame_data[12] = {
	{ 320, 104,  0,  4 }, { 320, 102,  0,  5 }, { 320, 98,   0,  7 },
	{ 320,  86,  0, 13 }, { 320,  64,  0, 24 }, { 320, 30,   0, 41 },
	{ 314,   2,  3, 55 }, { 302,   2,  9, 55 }, { 278,  2,  21, 55 },
	{ 230,   2, 45, 55 }, { 134,   2, 93, 55 }, {   4,  2, 158, 55 },
}, g_open_frame_data[12] = {
	{   4,  2, 158, 55 }, {  10,  2, 155, 55 }, {  20,   2, 150, 55 },
	{  40,  2, 140, 55 }, {  80,  2, 120, 55 }, { 160,   2,  80, 55 },
	{ 320,  4,   0, 54 }, { 320,  6,   0, 53 }, { 320,  14,   0, 49 },
	{ 320, 26,   0, 43 }, { 320, 52,   0, 30 }, { 320, 104,   0,  4 }
};

static window_folding_data_t g_pax_anim_data = {
	.total_frames = 12,
	.frame_cap = 35,
	.pixels = g_seg011.data,
};

real_world_state_t update_pax()
{
	static int anim = 0;

	switch (g_state) {
	case PS_OPEN_PAX:
	case PS_CLOSE_PAX:
		if (!anim)
		{
			anim = 1;
			g_pax_anim_data.frame_data = (g_state == PS_OPEN_PAX) ?
				g_open_frame_data : g_close_frame_data;
			g_pax_anim_data.sprite_chain_index = (g_state == PS_OPEN_PAX) ?
				g_4bae.frame_sc_index : g_4bae.frame_sc_index + 1;
			window_animation_setup(WA_TYPE_WINDOW_FOLDING, &g_pax_anim_data);
		}
		else if (window_animation_update() == WA_EVENT_COMPLETED)
		{
			anim = 0;
			if (g_state == PS_OPEN_PAX)
			{
				neuro_window_setup(NWM_PAX);
				g_state = pax_main_menu();
			}
			else
			{
				g_state = PS_OPEN_PAX;
				drawing_control_remove_sprite_from_chain(++g_4bae.frame_sc_index);
				restore_window();
				return RWS_NORMAL;
			}
		}
		break;

	case PS_USER_INFO:
	case PS_NEWS:
	case PS_BOARD_MSG:
		g_state = update_pax_text_scrolling(g_state);
		break;

	case PS_BOARD_SEND_MSG_ADDRESSEE:
	case PS_BOARD_SEND_MSG_TEXT:
		update_pax_board_send_msg(g_state);
		break;
	}
	
	return RWS_PAX;
}
