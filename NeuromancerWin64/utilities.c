#include "data.h"
#include <stdint.h>
#include <string.h>
#include "SFML\Window\Mouse.h"
#include "SFML\Window\Keyboard.h"
#include "SFML\Window\Event.h"

sfKeyCode ascii_toSfKeyCode(char c)
{
	switch (c) {
		case 'a': return sfKeyA;
		case 'b': return sfKeyB;
		case 'c': return sfKeyC;
		case 'd': return sfKeyD;
		case 'e': return sfKeyE;
		case 'f': return sfKeyF;
		case 'g': return sfKeyG;
		case 'h': return sfKeyH;
		case 'i': return sfKeyI;
		case 'j': return sfKeyJ;
		case 'k': return sfKeyK;
		case 'l': return sfKeyL;
		case 'm': return sfKeyM;
		case 'n': return sfKeyN;
		case 'o': return sfKeyO;
		case 'p': return sfKeyP;
		case 'q': return sfKeyQ;
		case 'r': return sfKeyR;
		case 's': return sfKeyS;
		case 't': return sfKeyT;
		case 'u': return sfKeyU;
		case 'v': return sfKeyV;
		case 'w': return sfKeyW;
		case 'x': return sfKeyX;
		case 'y': return sfKeyY;
		case 'z': return sfKeyZ;
		case '0': return sfKeyNum0;
		case '1': return sfKeyNum1;
		case '2': return sfKeyNum2;
		case '3': return sfKeyNum3;
		case '4': return sfKeyNum4;
		case '5': return sfKeyNum5;
		case '6': return sfKeyNum6;
		case '7': return sfKeyNum7;
		case '8': return sfKeyNum8;
		case '9': return sfKeyNum9;
		default: return sfKeyUnknown;
	}
}

int extract_line(char **text, char *line, int length)
{
	char *ps = *text, *pl = line;
	int word_len = 0;
	int just_space = 0;

	while (1)
	{
		switch (*ps)
		{
		case 1:
			if (strlen(line) + strlen(g_4bae.name + 2) <= length)
			{
				memmove(pl, g_4bae.name + 2, strlen(g_4bae.name + 2));
				pl += strlen(g_4bae.name + 2);
				*text = ++ps;
				just_space = 0;
				word_len = 0;
			}
			else
			{
				break;
			}
			continue;

		case ' ':
			if (word_len == 0)
			{
				*text = ++ps;

				if (strlen(line) == length)
				{
					break;
				}
				else
				{
					*pl++ = ' ';
					just_space = 1;
				}
			}
			else if (strlen(line) + word_len <= length)
			{
				memmove(pl, *text, word_len);
				pl += word_len;
				*text = ps;
				word_len = 0;
			}
			else
			{
				break;
			}
			continue;

		case '\r':
			if (!just_space && (word_len == 0))
			{
				*text = ++ps;
				memset(line + strlen(line), 0x20, length - strlen(line));
			}
			else if (strlen(line) + word_len <= length)
			{
				memmove(pl, *text, word_len);
				*text = ++ps;
			}
			break;

		case '\0':
			if (!just_space && (word_len == 0))
			{
				*text = ++ps;
				memset(line, 0x20, length);
				return 1;
			}
			else if (strlen(line) + word_len <= length)
			{
				memmove(pl, *text, word_len);
				*text = ps;
			}
			break;

		default:
			word_len++;
			ps++;
			just_space = 0;
			continue;
		}

		break;
	}

	return 0;
}

