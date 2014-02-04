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
	char left[BUFFER_SIZE], right[BUFFER_SIZE], delay[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_TICKER];

	arg = get_arg_in_braces(arg, left,  0);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);
	arg = get_arg_in_braces(arg, right, 1);
	arg = get_arg_in_braces(arg, delay, 1);

	if (*delay == 0)
	{
		strcpy(delay, "60");
	}
	else
	{
		get_number_string(ses, delay, delay);
	}

	if (*left == 0)
	{
		show_list(ses, root, LIST_TICKER);
	}
	else if (*left && *right == 0)
	{
		if (show_node_with_wild(ses, left, LIST_TICKER) == FALSE) 
		{
			show_message(ses, LIST_TICKER, "#TICK, NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, delay, LIST_TICKER);

		show_message(ses, LIST_TICKER, "#OK {%s} NOW EXECUTES {%s} EVERY {%s} SECONDS.", left, right, delay);
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
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE], arg3[BUFFER_SIZE], time[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_DELAY];

	arg = get_arg_in_braces(arg, arg1, 0);
	arg = get_arg_in_braces(arg, arg2, 1);
	arg = get_arg_in_braces(arg, arg3, 1);

	if (*arg1 == 0)
	{
		show_list(ses, root, LIST_DELAY);
	}
	else if (*arg2 == 0)
	{
		if (show_node_with_wild(ses, arg1, LIST_DELAY) == FALSE)
		{
			show_message(ses, LIST_DELAY, "#DELAY: NO MATCH(ES) FOUND FOR {%s}.", arg1);
		}
	}
	else
	{
		if (*arg3 == 0)
		{
			sprintf(time, "%lld", utime() + (long long) (get_number(ses, arg1) * 1000000LL));

			updatenode_list(ses, time, arg2, time, LIST_DELAY);

			show_message(ses, LIST_TICKER, "#OK, IN {%s} SECONDS {%s} IS EXECUTED.", arg1, arg2);
		}
		else
		{
			sprintf(time, "%lld", utime() + (long long) (get_number(ses, arg3) * 1000000LL));

			updatenode_list(ses, arg1, arg2, time, LIST_DELAY);

			show_message(ses, LIST_TICKER, "#OK, IN {%s} SECONDS {%s} IS EXECUTED.", arg3, arg2);
		}
	}
	return ses;
}

DO_COMMAND(do_undelay)
{
	delete_node_with_wild(ses, LIST_DELAY, arg);

	return ses;
}
