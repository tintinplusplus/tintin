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

/*********************************************************************/
/* file: alias.c - funtions related the the alias command            */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

#include "tintin.h"


DO_COMMAND(do_alias)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_ALIAS];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	if (!*left)
	{
		show_list(ses, root, LIST_ALIAS);
	}
	else if (*left && !*right)
	{
		if (show_node_with_wild(ses, root, left, ALPHA) == FALSE)
		{
			if (show_message(ses, LIST_ALIAS))
			{
				tintin_printf2(ses, "#No match(es) found for {%s}", left);
			}
		}
	}
	else
	{
		updatenode_list(ses, left, right, "0", LIST_ALIAS);

		if (show_message(ses, LIST_ALIAS))
		{
			tintin_printf2(ses, "#Ok. {%s} aliases {%s}.", left, right);
		}
	}
	return ses;
}


DO_COMMAND(do_unalias)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int flag = FALSE;

	root = ses->list[LIST_ALIAS];

	arg = get_arg_in_braces(arg, left,  TRUE);

	while ((node = search_node_with_wild(root, left)))
	{
		if (show_message(ses, LIST_ALIAS))
		{
			tintin_printf(ses, "#Ok. {%s} is no longer an alias.", node->left);
		}
		deletenode_list(ses, node, LIST_ALIAS);
		flag = TRUE;
	}
	if (!flag && show_message(ses, LIST_ALIAS))
	{
		tintin_printf(ses, "#No match(es) found for {%s}", left);
	}
	return ses;
}
