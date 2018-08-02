#include <stdint.h>
#include <string.h>

int extract_line(char **text, char *line, int length)
{
	char *ps = *text, *pl = line;
	int word_len = 0;

	while (1)
	{
		switch (*ps)
		{
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
			if (word_len == 0)
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
			if (word_len == 0)
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
			continue;
		}

		break;
	}

	return 0;
}

