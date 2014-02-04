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
*   file: scrsize.c - funtions related to the terminal size                   *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                     coded by Igor van den Hoven 2004                        *
******************************************************************************/

#include "tintin.h"

void init_screen_size(struct session *ses)
{
	int top, bot;

	top = ses->top_row == 0 ? 1 : ses->top_row;
	bot = ses->bot_row == 0 ? 0 : ses->rows - ses->bot_row;

	if (HAS_BIT(gts->flags, SES_FLAG_PREPPED))
	{
		rl_deprep_terminal();
		rl_resize_terminal();
		rl_prep_terminal(0);
	}
	else
	{
		rl_resize_terminal();
	}

	rl_get_screen_size(&ses->rows, &ses->cols);

	if (ses->rows <= 0)
	{
		ses->rows = SCREEN_HEIGHT;
	}

	if (ses->cols <= 0)
	{
		ses->cols = SCREEN_WIDTH;
	}

	ses->top_row = top;
	ses->bot_row = ses->rows - bot;

	if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
	{
		init_split(ses, ses->top_row, ses->bot_row);
	}
	SET_BIT(gts->flags, SES_FLAG_RESETBUFFER);
}

int get_scroll_size(struct session *ses)
{
	return (ses->bot_row - ses->top_row);
}
