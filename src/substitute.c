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
******************************************************************************/

/******************************************************************************
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                         coded by Peter Unold 1992                           *
******************************************************************************/


#include "tintin.h"

DO_COMMAND(do_substitute)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], pr[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_SUBSTITUTE];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);
	arg = get_arg_in_braces(arg, pr,    TRUE);

	if (*pr == 0)
	{
		sprintf(pr, "%s", "5");
	}

	if (*left == 0)
	{
		show_list(ses, root, LIST_SUBSTITUTE);
	}
	else if (*left && !*right)
	{
		if (show_node_with_wild(ses, left, LIST_SUBSTITUTE) == FALSE)
		{
			show_message(ses, LIST_SUBSTITUTE, "#SUBSTITUTE: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, pr, LIST_SUBSTITUTE);

		show_message(ses, LIST_SUBSTITUTE, "#OK. {%s} IS NOW SUBSTITUTED AS {%s} @ {%s}.", left, right, pr);
	}
	return ses;
}


DO_COMMAND(do_unsubstitute)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int found = FALSE;

	root = ses->list[LIST_SUBSTITUTE];

	arg = get_arg_in_braces(arg, left, 1);

	while ((node = search_node_with_wild(root, left)))
	{
		show_message(ses, LIST_SUBSTITUTE, "#OK. {%s} IS NO LONGER A SUBSTITUTION.", node->left);

		deletenode_list(ses, node, LIST_SUBSTITUTE);

		found = TRUE;
	}
	if (found == FALSE)
	{
		show_message(ses, LIST_SUBSTITUTE, "#SUBSTITUTE: NO MATCH(ES) FOUND FOR {%s}.", left);
	}
	return ses;
}

void check_all_substitutions(struct session *ses, char *original, char *line)
{
	struct listnode *node;

	for (node = ses->list[LIST_SUBSTITUTE]->f_node ; node ; node = node->next)
	{
		if (check_one_action(line, original, node->left, ses))
		{
			if (strcmp(node->right, "."))
			{
				substitute(ses, node->right, original, SUB_ARG|SUB_VAR|SUB_FUN|SUB_COL|SUB_ESC);
			}
			else
			{
				SET_BIT(ses->flags, SES_FLAG_GAG);
			}
			return;
		}
	}
}

