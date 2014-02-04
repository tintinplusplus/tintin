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
******************************************************************************/

#include "tintin.h"

DO_COMMAND(do_action)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], rank[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_ACTION];

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 1);
	arg = get_arg_in_braces(arg, rank,  1);

	if (*rank == 0)
	{
		strcpy(rank, "5");
	}

	if (*left == 0)
	{
		show_list(ses, root, LIST_ACTION);
	}
	else if (*left && *right == 0)
	{
		if (show_node_with_wild(ses, left, LIST_ACTION) == FALSE)
		{
			show_message(ses, LIST_ACTION, "#ACTION: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, rank, LIST_ACTION);

		show_message(ses, LIST_ACTION, "#OK. {%s} NOW TRIGGERS {%s} @ {%s}.", left, right, rank);
	}
	return ses;
}


DO_COMMAND(do_unaction)
{
	delete_node_with_wild(ses, LIST_ACTION, arg);

	return ses;
}


void check_all_actions(struct session *ses, char *original, char *line)
{
	char buf[BUFFER_SIZE];
	struct listnode *node;
	struct listroot *root;

	root = ses->list[LIST_ACTION];

	for (node = root->update = root->f_node ; node ; node = root->update)
	{
		root->update = node->next;

		if (check_one_action(line, original, node->left, ses))
		{
			show_debug(ses, LIST_ACTION, "#DEBUG ACTION {%s}", node->left);

			substitute(ses, node->right, buf, SUB_ARG|SUB_SEC);

			script_driver(ses, LIST_ACTION, buf);

			return;
		}
	}
}
