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
*               (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                  *
*                                                                             *
*                       coded by Sverre Normann 1999                          *
*                    recoded by Igor van den Hoven 2004                       *
******************************************************************************/

#include "tintin.h"


DO_COMMAND(do_function)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], *pti;
	struct listroot *root;

	root = ses->list[LIST_FUNCTION];

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 1);

	if (*left == 0)
	{
		show_list(ses, root, LIST_FUNCTION);
	}

	else if (*left && !*right)
	{
		if (show_node_with_wild(ses, left, LIST_FUNCTION) == FALSE)
		{
			show_message(ses, LIST_FUNCTION, "#FUNCTION: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		for (pti = left ; *pti ; pti++)
		{
			if (!isalnum(*pti) && *pti != '_')
			{
				tintin_printf2(ses, "#ERROR, {%s} IS NOT A VALID FUNCTION NAME.", left);
			}
			if (pti[1] == DEFAULT_OPEN)
			{
				break;
			}
		}
		updatenode_list(ses, left, right, "0", LIST_FUNCTION);

		show_message(ses, LIST_FUNCTION, "#OK. {%s} IS NOW A FUNCTION.", left);
	}
	return ses;
}


DO_COMMAND(do_unfunction)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int found = FALSE;

	root = ses->list[LIST_FUNCTION];

	arg = get_arg_in_braces(arg,left,1);

	while ((node = search_node_with_wild(root, left)) != NULL)
	{
		show_message(ses, LIST_FUNCTION, "#OK. {%s} IS NO LONGER A FUNCTION.", node->left);

		deletenode_list(ses, node, LIST_FUNCTION);

		found = TRUE;
	}
	if (found == FALSE)
	{
		show_message(ses, LIST_FUNCTION, "#FUNCTION: NO MATCH(ES) FOUND FOR {%s}.", left);
	}
	return ses;
}
