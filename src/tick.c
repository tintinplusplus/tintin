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
*                         coded by Peter Unold 1992                           *
*                     recoded by Igor van den Hoven 2004                      *
******************************************************************************/


#include "tintin.h"


DO_COMMAND(do_tick)
{
	char *left, *right, *rank, *temp;
	struct listroot *root;

	root = ses->list[LIST_TICKER];

	arg = get_arg_in_braces(arg, &left,  FALSE);
	arg = get_arg_in_braces(arg, &right, TRUE);
	arg = get_arg_in_braces(arg, &temp,  TRUE);

	get_number_string(ses, temp, &rank);

	if (*rank == 0)
	{
		rank = string_alloc("60");
	}

	if (!*left)
	{
		show_list(ses, root, LIST_TICKER);
	}
	else if (*left && *right == 0)
	{
		if (show_node_with_wild(ses, left, LIST_TICKER) == FALSE) 
		{
			show_message(ses, LIST_TICKER, "#TICK, NO MATCH(ES) FOUND FOR {%s}", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, rank, LIST_TICKER);

		show_message(ses, LIST_TICKER, "#OK {%s} NOW EXECUTES {%s} EVERY {%s} SECONDS.", left, right, rank);
	}
	return ses;
}


DO_COMMAND(do_untick)
{
	delete_node_with_wild(ses, LIST_TICKER, arg);

	return ses;
}


DO_COMMAND(do_delay)
{
	char *left, *right, *temp;
	struct listroot *root;

	root = ses->list[LIST_DELAY];

	arg = get_arg_in_braces(arg, &left,  FALSE);
	arg = get_arg_in_braces(arg, &right, TRUE);

	if (*right == 0)
	{
		show_list(ses, root, LIST_DELAY);
	}
	else if (*left && *right == 0)
	{
		tintin_puts2(ses, "#SYNTAX: #DELAY {seconds} {command}");
	}
	else
	{
		get_number_string(ses, left, &temp);

		left = stringf_alloc("%lld", utime());

		updatenode_list(ses, left, right, temp, LIST_DELAY);

		show_message(ses, LIST_TICKER, "#OK, IN {%s} SECONDS {%s} IS EXECUTED.", temp, right);
	}
	return ses;
}

