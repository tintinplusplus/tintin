/******************************************************************************
*   This file is part of TinTin++                                             *
*                                                                             *
*   Copyright 2004-2019 Igor van den Hoven                                    *
*                                                                             *
*   TinTin++ is free software; you can redistribute it and/or modify          *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 3 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with TinTin++.  If not, see https://www.gnu.org/licenses.           *
******************************************************************************/

/******************************************************************************
*              (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                   *
*                                                                             *
*                     coded by Igor van den Hoven 2019                        *
******************************************************************************/

#include "tintin.h"

DO_COMMAND(do_screen)
{
	char left[BUFFER_SIZE];

	if (gtd->screen == NULL)
	{
		tintin_printf2(ses, "gtd->screen->rows: %d", gtd->screen->rows);
		tintin_printf2(ses, "gtd->screen: NULL");

		return ses;
	}

	arg = get_arg_in_braces(ses, arg, left, FALSE);

	tintin_printf2(ses, "gtd->ses->top_row: %d", gtd->ses->top_row);
	tintin_printf2(ses, "gtd->ses->bot_row: %d", gtd->ses->bot_row);
	tintin_printf2(ses, "gtd->ses->cur_row: %d", gtd->ses->cur_row);
	tintin_printf2(ses, "gtd->ses->cur_col: %d", gtd->ses->cur_col);

	tintin_printf2(ses, "gtd->screen->rows: %d", gtd->screen->rows);
	tintin_printf2(ses, "gtd->screen->top_row: %d", gtd->screen->top_row);
	tintin_printf2(ses, "gtd->screen->bot_row: %d", gtd->screen->bot_row);

	tintin_printf2(ses, "gtd->screen->sav_row: %d", gtd->screen->sav_row);
	tintin_printf2(ses, "gtd->screen->sav_col: %d", gtd->screen->sav_col);
	tintin_printf2(ses, "gtd->screen->cur_row: %d", gtd->screen->cur_row);
	tintin_printf2(ses, "gtd->screen->cur_col: %d", gtd->screen->cur_col);
	tintin_printf2(ses, "gtd->screen->max_row: %d", gtd->screen->max_row);

//	print_screen(gts);

	return ses;
}

void add_row_screen(int index)
{
	gtd->screen->lines[index] = (struct row_data *) calloc(1, sizeof(struct row_data));
	gtd->screen->lines[index]->str = strdup("");
}

void del_row_screen(int index)
{
	free(gtd->screen->lines[index]->str);
	free(gtd->screen->lines[index]);
}

void init_screen(int rows, int cols)
{
	int cnt;

//	disabled for now

	if (gtd->screen == NULL)
	{
		gtd->screen = calloc(1, sizeof(struct screen_data));
	}
	gtd->screen->rows = rows;
	gtd->screen->cols = cols;
		
	return;

	push_call("init_screen(%d,%d)",rows,cols);

	if (gtd->screen)
	{
		if (gtd->screen->max_row < rows)
		{
			gtd->screen->lines = (struct row_data **) realloc(gtd->screen->lines, rows * sizeof(struct row_data *));
			
			memset(gtd->screen->lines + gtd->screen->max_row * sizeof(struct row_data *), 0, (rows - gtd->screen->max_row) * sizeof(struct row_data *));

			gtd->screen->max_row = rows;
		}
	}
	else
	{
		gtd->screen = calloc(1, sizeof(struct screen_data));
		gtd->screen->lines = (struct row_data **) calloc(rows, sizeof(struct row_data *));

		gtd->screen->top_row = 1;
		gtd->screen->bot_row = rows;

		gtd->screen->max_row = rows;
	}

	gtd->screen->rows = rows;
	gtd->screen->sav_row = gtd->screen->cur_row = rows;

	gtd->screen->cols = cols;
	gtd->screen->sav_col = gtd->screen->cur_col = 0;

	for (cnt = 0 ; cnt < gtd->screen->max_row ; cnt++)
	{
		if (gtd->screen->lines[cnt] == NULL)
		{
			add_row_screen(cnt);
		}
	}
	pop_call();
	return;
}

void destroy_screen()
{
	int cnt;

//	disabled for now

	return;

	for (cnt = 0 ; cnt < gtd->screen->max_row ; cnt++)
	{
		del_row_screen(cnt);
	}
	free(gtd->screen->lines);
	free(gtd->screen);

	gtd->screen = NULL;
}

void print_screen()
{
	int cnt;

	for (cnt = 0 ; cnt < gtd->screen->rows ; cnt++)
	{
		printf("%2d %s\n", cnt, gtd->screen->lines[cnt]->str);
	}
}


void add_line_screen(char *str)
{
	char *ptr;
	int cnt;

	// disabled for now

	return;

	push_call("add_line_screen(%p)",str);

	if (gtd->screen == NULL)
	{
		printf("screen == NULL!\n");

		pop_call();
		return;
	}

	while (str)
	{
		cnt = gtd->ses->top_row - 1;

		free(gtd->screen->lines[cnt]->str);

		while (cnt < gtd->ses->bot_row - 2)
		{
			gtd->screen->lines[cnt]->str = gtd->screen->lines[cnt + 1]->str;

			cnt++;
		}

		ptr = strchr(str, '\n');

		if (ptr)
		{
			gtd->screen->lines[cnt]->str = strndup(str, ptr - str);

			str = ptr + 1;
		}
		else
		{
			gtd->screen->lines[cnt]->str = strdup(str);

			str = NULL;
		}
//		gtd->screen->lines[cnt]->raw_len = strlen(gtd->screen->lines[cnt]->str);
//		gtd->screen->lines[cnt]->str_len = strip_vt102_strlen(gts, gtd->screen->lines[cnt]->str);
	}

	pop_call();
	return;
}



void set_line_screen(char *str, int row, int col)
{
	char buf[BUFFER_SIZE];

	// disabled for now
	
	return;

	push_call("set_line_screen(%p,%d)",str,row,col);

	strcpy(buf, gtd->screen->lines[row]->str);

	free(gtd->screen->lines[row]->str);

	strcpy(&buf[col], str);

	gtd->screen->lines[row]->str = strdup(buf);

	pop_call();
	return;
}

/*
	save cursor, goto top row, delete (bot - top) rows, restore cursor
*/

void erase_screen(struct session *ses)
{
//	int cnt;

	push_call("erase_screen(%p) (%d,%d)",ses,ses->top_row,ses->bot_row);

	printf("\e7\e[%d;1H\e[%dM\e8", ses->top_row, ses->bot_row - ses->top_row);
/*
	for (cnt = ses->top_row - 1 ; cnt < ses->bot_row - 1 ; cnt++)
	{
		free(gtd->screen->lines[cnt]->str);

		gtd->screen->lines[cnt]->str = strdup("");
	}
*/
	ses->sav_row = ses->cur_row;
	ses->sav_col = ses->cur_col;

	pop_call();
	return;
}

void get_line_screen(char *result, int row)
{
	strcpy(result, gtd->screen->lines[row]->str);
}

void get_word_screen(char *result, int row, int col)
{
	char *ptr;
	int i, j;

	strip_vt102_codes(gtd->screen->lines[row]->str, result);

	ptr = result;

	if (!isalnum((int) ptr[col]) && ptr[col] != '_')
	{
		sprintf(result, "%c", ptr[col]);

		return;
	}

	for (i = col ; i >= 0 ; i--)
	{
		if (!isalnum((int) ptr[i]) && ptr[i] != '_')
		{
			break;
		}
	}
	i++;

	for (j = col ; ptr[j] ; j++)
	{
		if (!isalnum((int) ptr[j]) && ptr[j] != '_')
		{
			break;
		}
	}

	strncpy(result, &ptr[i], j - i);

	result[j - i] = 0;
}
