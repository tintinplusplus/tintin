/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2007 (See CREDITS file)                                     *
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
******************************************************************************/

/******************************************************************************
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2007                       *
******************************************************************************/


#include "tintin.h"

DO_COMMAND(do_event)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listroot *root;
	int cnt;

	root = ses->list[LIST_EVENT];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	if (*left == 0)
	{
		tintin_header(ses, " EVENTS ");

		for (cnt = 0 ; *event_table[cnt].name != 0 ; cnt++)
		{
			tintin_printf2(ses, "  [%-13s]  %s", event_table[cnt].name, event_table[cnt].desc);
		}
		tintin_header(ses, "");
	}
	else if (*right == 0)
	{
		if (show_node_with_wild(ses, left, LIST_EVENT) == FALSE)
		{
			show_message(ses, LIST_ALIAS, "#EVENT: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		for (cnt = 0 ; *event_table[cnt].name != 0 ; cnt++)
		{
			if (is_abbrev(left, event_table[cnt].name))
			{
				show_message(ses, LIST_EVENT, "#EVENT {%s} HAS BEEN SET TO {%s}.", event_table[cnt].name, right);

				updatenode_list(ses, event_table[cnt].name, right, "", LIST_EVENT);

				return ses;
			}
		}
		tintin_printf(ses, "#EVENT {%s} IS NOT A VALID OPTION.", capitalize(left));
	}
	return ses;
}

void check_all_events(struct session *ses, char *line)
{
	struct listnode *node;

	for (node = ses->list[LIST_EVENT]->f_node ; node ; node = node->next)
	{
		if (!strcmp(node->left, line))
		{
			parse_input(ses, node->right);
		}
	}
}
