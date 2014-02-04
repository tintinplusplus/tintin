/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2004 (See CREDITS file)                                     *
*                                                                             *
*   This program is protected under the GNU GPL (See COPYING)                 *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
*******************************************************************************/

/******************************************************************************
*   file: vt102.c - funtions related to terminal emulation                    *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                   recoded by Igor van den Hoven 2004                        *
******************************************************************************/

#include "tintin.h"

/*
	don't forget to do a 'fflush(stdout);' after each of these commands!
*/

void save_pos(struct session *ses)
{
	printf("\0337"); 

	ses->sav_row = ses->cur_row;
}


void restore_pos(struct session *ses)
{
	printf("\0338"); 

	ses->cur_row = ses->sav_row;
}


void goto_rowcol(struct session *ses, int row, int col)
{
	printf("\033[%d;%dH", row, col);

	ses->cur_row = row;
}


void erase_toeol(void)
{
	printf("\033[K");
}

/*
	save cursor, goto top row, delete (bot - top) rows, restore cursor
*/

void erase_screen(struct session *ses)
{
	printf("\0337\033[%d;1H\033[%dM\0338", ses->top_row, ses->bot_row - ses->top_row);

	ses->sav_row = ses->cur_row;
}

/*
	doesn't do much
*/

void reset(void)
{
	printf("%cc", ESCAPE);
}


void scroll_region(struct session *ses, int top, int bot)
{
	printf("%c[%d;%dr", ESCAPE, top, bot);

	ses->top_row = top;
	ses->bot_row = bot;
}


void reset_scroll_region(struct session *ses)
{
	printf("%c[r", ESCAPE);

	ses->top_row = 1;
	ses->bot_row = ses->rows;
}


int skip_vt102_codes(const char *str)
{
	int skip;

	switch (str[0])
	{
		case   5:   /* ENQ */
		case   7:   /* BEL */
		case   8:   /* BS  */
		case   9:   /* HT  */
	/*	case  10: *//* LF  */
		case  11:   /* VT  */
		case  12:   /* FF  */
		case  13:   /* CR  */
		case  14:   /* SO  */
		case  15:   /* SI  */
		case  17:   /* DC1 */
		case  19:   /* DC3 */
		case  24:   /* CAN */
		case  26:   /* SUB */
		case 127:   /* DEL */
			return 1;

		case  27:   /* ESC */
			break;

		default:
			return 0;
	}

	switch (str[1])
	{
		case '\0':
			return 1;

		case '%':
		case '#':
		case '(':
		case ')':
			return 3;

		case ']':
			switch (str[2])
			{
				case 'P':
					return 10;
				case 'R':
					return 3;
			}
			return 2;

		case '[':
			break;

		default:
			return 2;
	}

	for (skip = 2 ; str[skip] != 0 ; skip++)
	{
		if (isalpha(str[skip]))
		{
			return skip + 1;
		}

		switch (str[skip])
		{
			case '@':
			case '`':
			case ']':
				return skip + 1;
		}
	}
	return skip;
}


int skip_vt102_codes_non_graph(const char *str)
{
	int skip = 0;

	switch (str[skip])
	{
		case   5:   /* ENQ */
		case   7:   /* BEL */
		case   8:   /* BS  */
		case   9:   /* HT  */
		case  14:   /* SO  */
		case  15:   /* SI  */
		case  17:   /* DC1 */
		case  19:   /* DC3 */
		case  24:   /* CAN */
		case  26:   /* SUB */
		case 127:   /* DEL */
			return 1;

		case  27:   /* ESC */
			break;

		default:
			return 0;
	}

	switch (str[1])
	{
		case '\0':
			return 0;

		case 'c':
		case 'D':
		case 'E':
		case 'H':
		case 'M':
		case 'Z':
		case '7':
		case '8':
		case '>':
		case '=':
			return 2;

		case '%':
		case '#':
		case '(':
		case ')':
			return 3;

		case ']':
			switch (str[2])
			{
				case 'P':
					return 10;
				case 'R':
					return 3;
			}
			return 2;

		case '[':
			break;

		default:
			return 2;
	}

	for (skip = 2 ; str[skip] != 0 ; skip++)
	{
		switch (str[skip])
		{
			case 'm':
				return 0;
			case '@':
			case '`':
			case ']':
				return skip + 1;
		}

		if (isalpha(str[skip]))
		{
			return skip + 1;
		}
	}
	return 0;
}


void strip_vt102_codes(const char *str, char *buf)
{
	char *pti, *pto;

	pti = (char *) str;
	pto = (char *) buf;

	while (*pti)
	{
		while (skip_vt102_codes(pti))
		{
			pti += skip_vt102_codes(pti);
		}

		if (*pti)
		{
			*pto++ = *pti++;
		}
	}
	*pto = 0;
}

void strip_vt102_codes_non_graph(const char *str, char *buf)
{
	char *pti, *pto;

	pti = (char *) str;
	pto = (char *) buf;

	while (*pti)
	{
		while (skip_vt102_codes_non_graph(pti))
		{
			pti += skip_vt102_codes_non_graph(pti);
		}

		if (*pti)
		{
			*pto++ = *pti++;
		}
	}
	*pto = 0;
}

void strip_non_vt102_codes(const char *str, char *buf)
{
	char *pti, *pto;
	int len;

	pti = (char *) str;
	pto = (char *) buf;

	while (*pti)
	{
		while ((len = skip_vt102_codes(pti)) != 0)
		{
			memcpy(pto, pti, len);
			pti += len;
			pto += len;
		}

		if (*pti)
		{
			pti++;
		}
	}
	*pto = 0;
}

int strip_vt102_strlen(const char *str)
{
	const char *pti;
	int i = 0;

	pti = str;

	while (*pti)
	{
		while (skip_vt102_codes(pti))
		{
			pti += skip_vt102_codes(pti);
		}


		if (*pti)
		{
			pti++;
			i++;
		}
	}
	return i;
}


void interpret_vt102_codes(struct session *ses, const char *str)
{
	char data[BUFFER_SIZE] = { 0 };
	int skip = 0, temp;

	switch (str[skip])
	{
		case  27:   /* ESC */
			break;

		default:
			return;
	}

	switch (str[1])
	{
		case '7':
			ses->sav_row = ses->cur_row;
			return;

		case '8':
			ses->cur_row = ses->sav_row;
			return;

		case '[':
			break;

		default:
			return;
	}

	for (skip = 2 ; str[skip] != 0 ; skip++)
	{
		switch (str[skip])
		{
			case '@':
			case '`':
			case ']':
				return;

			case 'A':
			case 'F':
				ses->cur_row += atoi(data);
				break;

			case 'B':
			case 'E':
			case 'e':
				ses->cur_row -= atoi(data);
				break;

			case 'H':
			case 'f':
				if (sscanf(data, "%d;%d", &ses->cur_row, &temp) != 2)
				{
					if (sscanf(data, "%d", &ses->cur_row) != 1)
					{
						ses->cur_row = 1;
					}
				}
				break;

			case 'd':
				ses->cur_row = atoi(data);
				break;

			case 'r':
				if (data[0])
				{
					sscanf(data, "%d;%d", &ses->top_row, &ses->bot_row);
				}
				else
				{
					ses->top_row = 1;
					ses->bot_row = ses->rows;
				}
				break;

			case 's':
				ses->sav_row = ses->cur_row;
				break;

			case 'u':
				ses->cur_row = ses->sav_row;
				break;

			default:
				data[skip - 2] = str[skip];
				data[skip - 1] = 0;
				break;
		}

		if (isalpha(str[skip]))
		{
			if (ses->cur_row < 1)
			{
				ses->cur_row = 1;
			}
			else if (ses->cur_row > ses->rows)
			{
				ses->cur_row = ses->rows;
			}
			else if (ses->top_row < 1)
			{
				ses->top_row = 1;
			}
			else if (ses->bot_row > ses->rows)
			{
				ses->bot_row = ses->rows;
			}
			return;
		}
	}
	return;
}

