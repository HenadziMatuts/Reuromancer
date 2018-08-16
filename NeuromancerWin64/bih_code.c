#include "data.h"

/* this should be replaced with the 8086 emulator */

static void vm_bih_call_level_0(uint16_t offt)
{
	switch (offt) {
	case 0xDB: {
		x4bae_t *ctl = (x4bae_t*)g_bih_wrapper.ctrl_struct_addr;
		ctl->cash += ctl->cash_withdrawal;
		break;
	}

	default:
		break;
	}
}

void vm_bih_call(uint16_t offt)
{
	switch (g_level_n) {
	case 0:
		vm_bih_call_level_0(offt);
		break;

	default:
		break;
	}
}

static void sub105F6_bih_call_level_2(uint16_t offt)
{
	switch (offt) {
	case 0x16F: { /* init */
		x4bae_t *ctl = (x4bae_t*)g_bih_wrapper.ctrl_struct_addr;
		int enough_cash = ((int32_t)ctl->cash - 250 < 0) ? 0 : 1;

		ctl->x4c7c = enough_cash;
		uint8_t day = ctl->date_day + 1;

		ctl->x4bcd[33]++;
		if (ctl->x4bcd[33] == 4)
		{
			ctl->x4c59[1] = day;
		}
		else if (ctl->x4bcd[33] == 1)
		{
			ctl->x4c59[0] = day;
		}

		break;
	}

	case 0x1AF: { /* deinit */
		x4bae_t *ctl = (x4bae_t*)g_bih_wrapper.ctrl_struct_addr;
		ctl->x4bcd[30] = 1;

		g_bih_wrapper.init_deinit_cb(0);
		break;
	}

	default:
		break;
	}
}

static void sub105F6_bih_call_level_1(uint16_t offt)
{
	switch (offt) {
	case 0x40: /* deinit */
		g_bih_wrapper.init_deinit_cb(0);
		return;

	default:
		break;
	}
}

static void sub105F6_bih_call_level_0(uint16_t offt)
{
	return;
}

void sub105F6_bih_call(uint16_t offt)
{
	switch (g_level_n) {
	case 0:
		sub105F6_bih_call_level_0(offt);
		break;

	case 1:
		sub105F6_bih_call_level_1(offt);
		break;

	case 2:
		sub105F6_bih_call_level_2(offt);
		break;

	default:
		break;
	}
}
