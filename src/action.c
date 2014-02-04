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
	char left[BUFFER_SIZE], right[BUFFER_SIZE], pr[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_ACTION];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);
	arg = get_arg_in_braces(arg, pr,    TRUE);

	if (!*pr)
	{
		sprintf(pr, "%s", "5"); 
	}

	if (!*left)
	{
		show_list(ses, root, LIST_ACTION);
	}
	else if (*left && !*right) 
	{
		if (show_node_with_wild(ses, left, LIST_ACTION) == FALSE) 
		{
			show_message(ses, LIST_ACTION, "#ACTION: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, pr, LIST_ACTION);

		show_message(ses, LIST_ACTION, "#OK. {%s} NOW TRIGGERS {%s} @ {%s}.", left, right, pr);
	}
	return ses;
}


DO_COMMAND(do_unaction)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int flag = FALSE;

	root = ses->list[LIST_ACTION];

	arg = get_arg_in_braces(arg, left, TRUE);

	while ((node = search_node_with_wild(root, left))) 
	{
		show_message(ses, LIST_ACTION, "#OK. {%s} IS NO LONGER A TRIGGER.", node->left);

		deletenode_list(ses, node, LIST_ACTION);

		flag = TRUE;
	}

	if (!flag)
	{
		show_message(ses, LIST_ACTION, "#ACTION: NO MATCH(ES) FOUND FOR {%s}.", left);
	}
	return ses;
}


void check_all_actions(struct session *ses, char *original, char *line)
{
	struct listnode *node;
	struct listroot *root;

	root = ses->list[LIST_ACTION];

	for (node = root->update = root->f_node ; node ; node = root->update)
	{
		root->update = node->next;

		if (check_one_action(line, original, node->left, ses))
		{
			char buffer[BUFFER_SIZE];

			substitute(ses, node->right, buffer, SUB_ARG|SUB_SEC);

			show_debug(ses, LIST_ACTION, "#ACTION DEBUG: %s", buffer);

			parse_input(ses, buffer);

			return;
		}
	}
	return;
}
