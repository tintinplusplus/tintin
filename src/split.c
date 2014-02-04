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
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                          coded by Bill Reiss 1993                           *
******************************************************************************/

#include "tintin.h"


DO_COMMAND(do_split)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(ses, arg, left,  FALSE);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);

	arg = get_arg_in_braces(ses, arg, right, FALSE);
	substitute(ses, right, right, SUB_VAR|SUB_FUN);

	if (*left == 0 && *right == 0)
	{
		init_split(ses, 1, ses->rows - 2);
	}
	else if (*right == 0)
	{
		init_split(ses, 1 + atoi(left), ses->rows - 2);
	}
	else
	{
		init_split(ses, 1 + atoi(left), ses->rows - 1 - atoi(right));
	}
	return ses;
}


DO_COMMAND(do_unsplit)
{
	clean_screen(ses);

	if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
	{
		if (HAS_BIT(ses->telopts, TELOPT_FLAG_NAWS))
		{
			send_sb_naws(ses, 0, NULL);
		}
		DEL_BIT(ses->flags, SES_FLAG_SPLIT);
	}
	return ses;
}

/*
	turn on split mode
*/

void init_split(struct session *ses, int top, int bot)
{
	if (top < 1)
	{
		top = 1;
	}

	if (bot > ses->rows)
	{
		bot = ses->rows;
	}

	scroll_region(ses, top, bot);

	SET_BIT(ses->flags, SES_FLAG_SPLIT);

	for (bot = 1 ; ses->rows - bot > ses->bot_row ; bot++)
	{
		do_one_prompt(ses, "", +bot);
	}

	for (top = 1 ; top < ses->top_row ; top++)
	{
		do_one_prompt(ses, "", -top);
	}

	goto_rowcol(ses, ses->rows, 1);

	if (HAS_BIT(ses->telopts, TELOPT_FLAG_NAWS))
	{
		send_sb_naws(ses, 0, NULL);
	}

	if (ses->map && HAS_BIT(ses->map->flags, MAP_FLAG_VTMAP))
	{
		SET_BIT(ses->flags, SES_FLAG_UPDATEVTMAP);
	}
}


/*
	get a clean screen, useful for ^Z, quitting, etc
*/

void clean_screen(struct session *ses)
{
	reset_scroll_region(ses);

	if (ses == gtd->ses)
	{
		goto_rowcol(ses, ses->rows, 1);
	}
}


/*
	undo clean_screen(); useful after ^Z
*/

void dirty_screen(struct session *ses)
{
	printf("\033=");

	if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
	{
		init_split(ses, ses->top_row, ses->bot_row);
	}
	else if (IS_SPLIT(ses))
	{
		scroll_region(ses, ses->top_row, ses->bot_row);
	}
	else
	{
		clean_screen(ses);
	}

	if (IS_SPLIT(ses) && ses == gtd->ses)
	{
		goto_rowcol(ses, ses->rows, 1);
	}
}
