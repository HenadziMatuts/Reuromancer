#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "neuro86.h"
#include <neuro_routines.h>
#include <SFML\Window\Event.h>
#include <SFML\System\Clock.h>
#include <stdint.h>

extern uint8_t g_exit_game;

extern sfClock *g_timer;

extern cpu_t *g_cpu;

void update_cursor();

sfKeyCode sfHandleTextInput(uint32_t u32_char,
	char *string, uint32_t size, int digits_only, int insert);

void sfSetKeyRepeat(int enabled);

sfKeyCode ascii_toSfKeyCode(char c);

void extract_line_prepare(char *text, char *dst, uint16_t length);
int extract_line();

#endif
