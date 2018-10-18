#include "data.h"
#include "globals.h"
#include "resource_manager.h"
#include "drawing_control.h"
#include "neuro_window_control.h"
#include "scene_real_world.h"
#include <neuro_routines.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef enum dialog_state_t {
	DS_OPEN_DIALOG = 0,
	DS_CHOOSE_REPLY_WFI,
	DS_NEXT_REPLY,
	DS_ACCEPT_REPLY,
	DS_ACCEPT_REPLY_TEXT_INPUT,
	DS_ACCEPT_REPLY_WFI,
	DS_CLOSE_DIALOG
} dialog_state_t;

static dialog_state_t g_state = DS_OPEN_DIALOG;

void sub_1342E(char *str, uint16_t opcode);

uint8_t g_dialog_escapable = 1;
static uint8_t g_current_reply = 0;

static void dialog_first_reply()
{
	char *reply = g_a8e0.bih.bytes + g_a8e0.bih.hdr.text_offset;
	uint8_t first_reply = g_a642->first_dialog_reply;

	for (int i = 0; i < first_reply; i++)
	{
		while (*reply++);
	}

	sub_1342E(reply, NWM_PLAYER_DIALOG_CHOICE);
	g_current_reply = 0;
}

static void dialog_next_reply()
{
	char *reply = g_a8e0.bih.bytes + g_a8e0.bih.hdr.text_offset;
	uint8_t first_reply = g_a642->first_dialog_reply;
	uint8_t total_replies = g_a642->total_dialog_replies;

	if (++g_current_reply == total_replies)
	{
		g_current_reply = 0;
	}

	for (int i = 0; i < first_reply + g_current_reply; i++)
	{
		while (*reply++);
	}

	sub_1342E(reply, NWM_PLAYER_DIALOG_CHOICE);
}

static dialog_state_t dialog_accept_reply()
{
	char *reply = g_a8e0.bih.bytes + g_a8e0.bih.hdr.text_offset;
	uint8_t first_reply = g_a642->first_dialog_reply;

	for (int i = 0; i < first_reply + g_current_reply; i++)
	{
		while (*reply++);
	}

	sub_1342E(reply, NWM_PLAYER_DIALOG_REPLY);

	if (g_dlg_with_user_input)
	{
		neuro_window_draw_string("<               ", g_6a40, g_6a7a, 1);
		return DS_ACCEPT_REPLY_TEXT_INPUT;
	}

	return DS_ACCEPT_REPLY_WFI;
}

char g_dialog_user_input[17] = { 0, };

static dialog_state_t on_dialog_accept_reply_text_input(sfTextEvent *event, int ret)
{
	static char input[17] = { 0, };
	char printable[18] = { 0 };

	if (ret)
	{
		strcpy(g_dialog_user_input, input);
		memset(input, 0, 17);
		g_4bae.active_dialog_reply = g_current_reply + g_a642->first_dialog_reply;
		g_4bae.x4bae[0] = 0;
		return DS_CLOSE_DIALOG;
	}

	sfHandleTextInput(event->unicode, input, 17, 0, 0);
	sprintf(printable, "%s<", input);
	memset(printable + strlen(printable), 0x20, 17 - strlen(printable));
	neuro_window_draw_string(printable, g_6a40, g_6a7a, 1);

	return DS_ACCEPT_REPLY_TEXT_INPUT;
}

void rw_dialog_handle_text_enter(int *state, sfTextEvent *event)
{
	switch (*state) {
	case DS_ACCEPT_REPLY_TEXT_INPUT:
		*state = on_dialog_accept_reply_text_input(event, 0);
		break;
	}
}

static dialog_state_t on_dialog_accept_reply_kboard(sfKeyEvent *event)
{
	if (event->type == sfEvtKeyReleased &&
		event->code == sfKeyReturn)
	{
		return on_dialog_accept_reply_text_input(NULL, 1);
	}

	return DS_ACCEPT_REPLY_TEXT_INPUT;
}

void rw_dialog_handle_kboard(int *state, sfKeyEvent *event)
{
	switch (*state) {
	case DS_ACCEPT_REPLY_TEXT_INPUT:
		*state = on_dialog_accept_reply_kboard(event);
		break;
	}
}

static dialog_state_t handle_dialog_wait_for_input(dialog_state_t state, sfEvent *event)
{
	if (state == DS_CHOOSE_REPLY_WFI)
	{
		if (event->type == sfEvtMouseButtonReleased)
		{
			if (event->mouseButton.button == sfMouseLeft)
			{
				return DS_NEXT_REPLY;
			}
			else if (event->mouseButton.button == sfMouseRight)
			{
				return DS_ACCEPT_REPLY;
			}
		}
		else if (event->type == sfEvtKeyReleased)
		{
			if (event->key.code == sfKeyEscape && g_dialog_escapable)
			{
				return DS_CLOSE_DIALOG;
			}
			else if (event->key.code == sfKeyReturn)
			{
				return DS_ACCEPT_REPLY;
			}
			else
			{
				return DS_NEXT_REPLY;
			}
		}
	}
	else if (state == DS_ACCEPT_REPLY_WFI)
	{
		if (event->type == sfEvtMouseButtonReleased ||
			event->type == sfEvtKeyReleased)
		{
			g_4bae.active_dialog_reply = g_current_reply + g_a642->first_dialog_reply;
			g_4bae.x4bae[0] = 0;
			return DS_CLOSE_DIALOG;
		}
	}

	return state;
}

void handle_dialog_input(sfEvent *event)
{
	switch (g_state) {
	case DS_CHOOSE_REPLY_WFI:
	case DS_ACCEPT_REPLY_WFI:
		g_state = handle_dialog_wait_for_input(g_state, event);

	case DS_ACCEPT_REPLY_TEXT_INPUT:
		neuro_window_handle_input((int*)&g_state, event);
		break;
	}
}

typedef enum dialog_action_t {
	DA_NEXT_REPLY,
	DA_ACCEPT_REPLY
} dialog_action_t;

static dialog_state_t update_dialog_reply(dialog_action_t act)
{
	static int frame = 0;
	static int frame_cap_ms = 75;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	if (passed - elapsed <= frame_cap_ms)
	{
		return (act == DA_NEXT_REPLY) ? DS_NEXT_REPLY : DS_ACCEPT_REPLY;
	}
	elapsed = passed;

	if (frame++ == 1)
	{
		frame = 0;
		if (act == DA_NEXT_REPLY)
		{
			dialog_next_reply();
		}

		return (act == DA_NEXT_REPLY) ? DS_CHOOSE_REPLY_WFI : dialog_accept_reply();
	}

	drawing_control_remove_sprite_from_chain(SCI_DIALOG_BUBBLE);
	drawing_control_remove_sprite_from_chain(++g_4bae.frame_sc_index);
	restore_window();

	return (act == DA_NEXT_REPLY) ? DS_NEXT_REPLY : DS_ACCEPT_REPLY;
}

static dialog_state_t update_dialog_open_close(int open)
{
	static int frame = 0;
	static int frame_cap_ms = 75;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	uint8_t first_reply = g_a642->first_dialog_reply;
	uint8_t total_replies = g_a642->total_dialog_replies;

	if (first_reply == 0xFF)
	{
		return open ? DS_CLOSE_DIALOG : DS_OPEN_DIALOG;
	}

	if (passed - elapsed <= frame_cap_ms)
	{
		return open ? DS_OPEN_DIALOG : DS_CLOSE_DIALOG;
	}
	elapsed = passed;

	if (frame++ == 1)
	{
		frame = 0;

		if (open)
		{
			if (total_replies == 0)
			{
				g_current_reply = 0;
				return dialog_accept_reply();
			}
			else
			{
				dialog_first_reply();
				return DS_CHOOSE_REPLY_WFI;
			}
		}
		else
		{
			return DS_OPEN_DIALOG;
		}
	}
	else
	{
		if (!open)
		{
			drawing_control_remove_sprite_from_chain(SCI_DIALOG_BUBBLE);
			drawing_control_remove_sprite_from_chain(++g_4bae.frame_sc_index);
			restore_window();
		}
	}

	return open ? DS_OPEN_DIALOG : DS_CLOSE_DIALOG;
}

real_world_state_t update_dialog()
{
	switch (g_state) {
	case DS_OPEN_DIALOG:
		g_state = update_dialog_open_close(1);
		return (g_state == DS_CLOSE_DIALOG) ? RWS_NORMAL : RWS_DIALOG;

	case DS_NEXT_REPLY:
	case DS_ACCEPT_REPLY:
		g_state = update_dialog_reply((g_state == DS_NEXT_REPLY) ?
			DA_NEXT_REPLY : DA_ACCEPT_REPLY);
		return RWS_DIALOG;

	case DS_CLOSE_DIALOG:
		g_state = update_dialog_open_close(0);
		return (g_state == DS_OPEN_DIALOG) ? RWS_NORMAL : RWS_DIALOG;
	}

	return RWS_DIALOG;
}
