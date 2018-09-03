#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <neuro_routines.h>
#include <SFML\Window\Event.h>
#include <SFML\System\Clock.h>
#include <stdint.h>

extern uint8_t g_exit_game;

extern sfClock *g_timer;

void update_cursor();

sfKeyCode sfHandleTextInput(uint32_t u32_char,
	char *string, uint32_t size, int digits_only, int insert);

void sfSetKeyRepeat(int enabled);

int extract_line(char **text, char *line, int length);

sfKeyCode ascii_toSfKeyCode(char c);

#endif
