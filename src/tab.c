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
*                      coded by Igor van den Hoven 2006                       *
******************************************************************************/


#include "tintin.h"


DO_COMMAND(do_tab)
{
	char left[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_TAB];

	get_arg_in_braces(arg, left, TRUE);

	if (*left == 0)
	{
		show_list(ses, root, LIST_TAB);
	}
	else
	{
		updatenode_list(ses, left, "", "0", LIST_TAB);

		show_message(ses, LIST_TAB, "#OK. {%s} IS NOW A TAB.", left);
	}
	return ses;
}


DO_COMMAND(do_untab)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int found = FALSE;

	root = ses->list[LIST_TAB];

	arg = get_arg_in_braces(arg, left, 1);

	while ((node = search_node_with_wild(root, left)) != NULL)
	{
		show_message(ses, LIST_TAB, "#OK. {%s} IS NO LONGER A TAB.", node->left);

		deletenode_list(ses, node, LIST_TAB);

		found = TRUE;
	}

	if (found == FALSE)
	{
		show_message(ses, LIST_TAB, "#TAB: NO MATCH(ES) FOUND FOR {%s}.", left);
	}
	return ses;
}
