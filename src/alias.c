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
*               (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                  *
*                                                                             *
*                        coded by Peter Unold 1992                            *
*                   recoded by Igor van den hoven 2009                        *
******************************************************************************/

#include "tintin.h"


DO_COMMAND(do_alias)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], rank[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_ALIAS];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);
	arg = get_arg_in_braces(arg, rank,  FALSE);

	if (*rank == 0)
	{
		strcpy(rank, "5");
	}

	if (*left == 0)
	{
		show_list(ses, root, LIST_ALIAS);
	}
	else if (*right == 0)
	{
		if (show_node_with_wild(ses, left, LIST_ALIAS) == FALSE)
		{
			show_message(ses, LIST_ALIAS, "#ALIAS: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, rank, LIST_ALIAS);

		show_message(ses, LIST_ALIAS, "#OK. {%s} NOW ALIASES {%s} @ {%s}.", left, right, rank);
	}
	return ses;
}


DO_COMMAND(do_unalias)
{
	delete_node_with_wild(ses, LIST_ALIAS, arg);

	return ses;
}

int check_all_aliases(struct session *ses, char *input)
{
	struct listnode *node;
	struct listroot *root;
	char line[BUFFER_SIZE], tmp[BUFFER_SIZE], *arg;
	int i;

	root = ses->list[LIST_ALIAS];

	if (HAS_BIT(root->flags, LIST_FLAG_IGNORE))
	{
		return FALSE;
	}

	substitute(ses, input, line, SUB_VAR|SUB_FUN);

	for (node = root->update = root->f_node ; node ; node = root->update)
	{
		root->update = node->next;

		if (check_one_regexp(ses, node, line, line, PCRE_ANCHORED))
		{
			i = strlen(node->left);

			if (!strncmp(node->left, line, i))
			{
				if (line[i] && line[i] != ' ')
				{
					continue;
				}
				
				arg = get_arg_in_braces(line, tmp, FALSE);

				RESTRING(gtd->vars[0], arg)

				for (i = 1 ; 1 < 100 && *arg ; i++)
				{
					arg = get_arg_in_braces(arg, tmp, FALSE);

					RESTRING(gtd->vars[i], tmp);
				}
			}

			substitute(ses, node->right, tmp, SUB_ARG);

			if (!strncmp(node->left, line, i) && !strcmp(node->right, tmp) && *gtd->vars[0])
			{
				sprintf(input, "%s %s", tmp, gtd->vars[0]);
			}
			else
			{
				sprintf(input, "%s", tmp);
			}

			show_debug(ses, LIST_ALIAS, "#DEBUG ALIAS {%s}", node->left);

			DEL_BIT(gtd->flags, TINTIN_FLAG_USERCOMMAND);

			return TRUE;
		}
	}
	return FALSE;
}
