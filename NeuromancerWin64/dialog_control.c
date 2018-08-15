#include "data.h"
#include "globals.h"
#include "resource_manager.h"
#include "drawing_control.h"
#include "neuro_window_control.h"
#include "dialog_control.h"
#include <neuro_routines.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void sub_1342E(char *str, uint16_t opcode);

uint8_t g_dialog_escapable = 1;
static uint8_t g_current_reply = 0;

static void dialog_first_reply()
{
	char *reply = g_a8e0.bih + g_bih_wrapper.bih->text_offset;
	uint8_t first_reply = g_a642[0];

	for (int i = 0; i < first_reply; i++)
	{
		while (*reply++);
	}

	sub_1342E(reply, NWM_PLAYER_DIALOG_CHOICE);
	g_current_reply = 0;
}

static void dialog_next_reply()
{
	char *reply = g_a8e0.bih + g_bih_wrapper.bih->text_offset;
	uint8_t first_reply = g_a642[0];
	uint8_t total_replies = g_a642[1];

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

static void dialog_accept_reply()
{
	char *reply = g_a8e0.bih + g_bih_wrapper.bih->text_offset;
	uint8_t first_reply = g_a642[0];

	for (int i = 0; i < first_reply + g_current_reply; i++)
	{
		while (*reply++);
	}

	/* also has replies with user input */

	sub_1342E(reply, NWM_PLAYER_DIALOG_REPLY);
}

static dialog_state_t dialog_wait_for_input(dialog_state_t state, sfEvent *event)
{
	switch (state) {
	case DS_CHOOSE_REPLY_WFI:
		if (sfMouse_isLeftMouseButtonClicked())
		{
			return DS_NEXT_REPLY;
		}
		else if (sfMouse_isRightMouseButtonClicked())
		{
			return DS_ACCEPT_REPLY;
		}
		else if (g_dialog_escapable && event->key.code == sfKeyEscape)
		{
			return DS_CLOSE_DIALOG;
		}
		break;

	case DS_ACCEPT_REPLY_WFI:
		if (sfMouse_isLeftMouseButtonClicked())
		{
			g_4bae.active_dialog_reply = g_current_reply + g_a642[0];
			g_4bae.x4bae[0] = 0;
			return DS_CLOSE_DIALOG;
		}
		break;
	}

	return state;
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
		else
		{
			dialog_accept_reply();
		}

		return (act == DA_NEXT_REPLY) ? DS_CHOOSE_REPLY_WFI : DS_ACCEPT_REPLY_WFI;
	}

	drawing_control_remove_sprite_from_chain(SCI_DIALOG_BUBBLE);
	drawing_control_remove_sprite_from_chain(++g_4bae.x4ccf);
	restore_window();

	return (act == DA_NEXT_REPLY) ? DS_NEXT_REPLY : DS_ACCEPT_REPLY;
}

static dialog_state_t update_dialog_open_close(int open)
{
	static int frame = 0;
	static int frame_cap_ms = 75;
	static int elapsed = 0;
	int passed = sfTime_asMilliseconds(sfClock_getElapsedTime(g_timer));

	uint8_t first_reply = g_a642[0];
	uint8_t total_replies = g_a642[1];

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
				dialog_accept_reply();
				return DS_ACCEPT_REPLY_WFI;
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
			drawing_control_remove_sprite_from_chain(++g_4bae.x4ccf);
			restore_window();
		}
	}

	return open ? DS_OPEN_DIALOG : DS_CLOSE_DIALOG;
}

real_world_state_t update_dialog(sfEvent *event)
{
	static dialog_state_t state = DS_OPEN_DIALOG;

	switch (state) {
	case DS_OPEN_DIALOG:
		state = update_dialog_open_close(1);
		return (state == DS_CLOSE_DIALOG) ? RWS_NORMAL : RWS_DIALOG;

	case DS_CHOOSE_REPLY_WFI:
	case DS_ACCEPT_REPLY_WFI:
		state = dialog_wait_for_input(state, event);
		return RWS_DIALOG;

	case DS_NEXT_REPLY:
	case DS_ACCEPT_REPLY:
		state = update_dialog_reply((state == DS_NEXT_REPLY) ?
			DA_NEXT_REPLY : DA_ACCEPT_REPLY);
		return RWS_DIALOG;

	case DS_CLOSE_DIALOG:
		state = update_dialog_open_close(0);
		return (state == DS_OPEN_DIALOG) ? RWS_NORMAL : RWS_DIALOG;
	}

	return RWS_DIALOG;
}
