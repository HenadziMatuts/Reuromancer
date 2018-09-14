#include "data.h"
#include <stdint.h>
#include <string.h>
#include "SFML\Window\Mouse.h"
#include "SFML\Window\Keyboard.h"
#include "SFML\Window\Event.h"
#include "scene_real_world.h"

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

typedef struct line_extractor_t {
	uint16_t length;
	char *text;
	char *dst;
	int16_t line;
} line_extractor_t;

static line_extractor_t g_extractor_helper;

void extract_line_prepare(char *text, char *dst, uint16_t length)
{
	g_extractor_helper.length = length;
	g_extractor_helper.text = text;
	g_extractor_helper.dst = dst;
	g_extractor_helper.line = -1;

	g_dlg_with_user_input = 0;
}

static char* special_code_get_string(char code, char *date)
{
	if (code == 1)
	{
		return g_4bae.name + 2;
	}

	if (code == 2)
	{
		build_date_string(date, g_4bae.date_day);
		return date;
	}

	return NULL;
}

static int special_code_get_length(char code)
{
	if (code == 1) /* name */
	{
		return (int)strlen(g_4bae.name + 2);
	}

	return (code == 2) ? 8 : 0;
}

int extract_line()
{
	char *text = g_extractor_helper.text;
	char *dst;
	char date[9] = { 0 };
	uint16_t pos;
	char c;

	if (*text == 0)
	{
		return 0;
	}

	dst = g_extractor_helper.dst;
	g_extractor_helper.line++;
	pos = 0;

proceed:
	c = *text;

	if (c == '\r' || c == 0 || pos >= g_extractor_helper.length)
	{
		goto fin;
	}

	while (pos <= g_extractor_helper.length)
	{
		c = *text;
		if (c == 0 || c != ' ')
		{
			break;
		}

		*dst++ = *text++;
		pos++;
	}

	if (*text == '\r')
	{
		goto fin;
	}

	c = *text;
	if (c <= 5 && c != 0)
	{
		int i = special_code_get_length(c);
		i += pos;

		if (i > g_extractor_helper.length)
		{
			goto fin;
		}

		char *s = special_code_get_string(*text, date);
		strcpy(dst, s);

		dst += i;
		pos += i;
		text++;
	}

	char *t = text;
	int i = 0;

	while (1)
	{
		c = *t;
		if (*t == '\r' || *t == ' ' || *t == 0)
		{
			break;
		}
		
		t++;
		i++;
	}

	if (pos + i > g_extractor_helper.length)
	{
		goto fin;
	}

	memmove(dst, text, i);

	if (*dst == '@')
	{
		g_6a40 = pos;
		g_6a7a = g_extractor_helper.line;
		*dst = '-';
		g_dlg_with_user_input = 1;
	}

	dst += i;
	pos += i;
	text += i;

	goto proceed;

fin: /* 1fe72 */
	while (*text == ' ')
	{
		text++;
	}
	if (*text == '\r')
	{
		text++;
	}

	g_extractor_helper.text = text;
	*dst = 0;
	return 1;
}

