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
	char left[BUFFER_SIZE], right[BUFFER_SIZE], rank[BUFFER_SIZE], *str;
	struct listroot *root;

	root = ses->list[LIST_SUBSTITUTE];

	str = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(str, right, 1);
	arg = get_arg_in_braces(arg, rank,  1);

	if (*rank == 0)
	{
		strcpy(rank, "5");
	}

	if (*left == 0)
	{
		show_list(ses, root, LIST_SUBSTITUTE);
	}
	else if (*str == 0)
	{
		if (show_node_with_wild(ses, left, LIST_SUBSTITUTE) == FALSE)
		{
			show_message(ses, LIST_SUBSTITUTE, "#SUBSTITUTE: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, rank, LIST_SUBSTITUTE);

		show_message(ses, LIST_SUBSTITUTE, "#OK. {%s} IS NOW SUBSTITUTED AS {%s} @ {%s}.", left, right, rank);
	}
	return ses;
}


DO_COMMAND(do_unsubstitute)
{
	delete_node_with_wild(ses, LIST_SUBSTITUTE, arg);

	return ses;
}

void check_all_substitutions(struct session *ses, char *original, char *line)
{
	struct listnode *node;
	char match[BUFFER_SIZE], subst[BUFFER_SIZE], output[BUFFER_SIZE], *pt1, *pt2, *ptm;

	for (node = ses->list[LIST_SUBSTITUTE]->f_node ; node ; node = node->next)
	{
		if (check_one_regexp(ses, node, line, original, 0))
		{
			pt1 = line;
			pt2 = original;

			*output = 0;

			do
			{
				if (*gtd->vars[0] == 0)
				{
					break;
				}
				strcpy(match, gtd->vars[0]);

				substitute(ses, node->right, subst, SUB_ARG|SUB_VAR|SUB_FUN|SUB_COL|SUB_ESC);

				ptm = strstr(pt2, match);

				if (ptm == NULL)
				{
					break;
				}

				*ptm = 0;

				cat_sprintf(output, "%s%s", pt2, subst);

				pt1 = pt1 + strip_vt102_strlen(pt2) + strip_vt102_strlen(match);
				pt2 = ptm + strlen(match);

				show_debug(ses, LIST_SUBSTITUTE, "#DEBUG SUBSTITUTE {%s} {%s}", node->left, match);
			}
			while (check_one_regexp(ses, node, pt1, pt2, 0));

			strcat(output, pt2);

			strcpy(original, output);
		}
	}
}

