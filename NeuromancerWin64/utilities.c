#include "data.h"
#include <stdint.h>
#include <string.h>
#include "SFML\Window\Mouse.h"

int sfMouse_isLeftMouseButtonClicked()
{
	static int state = 0;

	if (state == 0)
	{
		if (!sfMouse_isButtonPressed(sfMouseLeft))
		{
			state = 1;
		}
	}
	else if (state == 1)
	{
		if (sfMouse_isButtonPressed(sfMouseLeft))
		{
			state = 2;
		}
	}
	else if (state == 2)
	{
		if (!sfMouse_isButtonPressed(sfMouseLeft))
		{
			state = 0;
			return 1;
		}
	}

	return 0;
}

int sfMouse_isRightMouseButtonClicked()
{
	static int state = 0;

	if (state == 0)
	{
		if (!sfMouse_isButtonPressed(sfMouseRight))
		{
			state = 1;
		}
	}
	else if (state == 1)
	{
		if (sfMouse_isButtonPressed(sfMouseRight))
		{
			state = 2;
		}
	}
	else if (state == 2)
	{
		if (!sfMouse_isButtonPressed(sfMouseRight))
		{
			state = 0;
			return 1;
		}
	}

	return 0;
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
				memset(line, 0x20, length);
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

