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
*   file: buffer.c - funtions related to the scroll back buffer               *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                     coded by Igor van den Hoven 2004                        *
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

		if (show_message(ses, LIST_TAB))
		{
			tintin_printf2(ses, "#Ok. {%s} added to tab list.", left);
		}
	}
	return ses;
}


DO_COMMAND(do_untab)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int flag = FALSE;

	root = ses->list[LIST_TAB];

	arg = get_arg_in_braces(arg, left, 1);

	while ((node = search_node_with_wild(root, left)) != NULL)
	{
		if (show_message(ses, LIST_TAB))
		{
			tintin_printf(ses, "#Ok. {%s} removed from tab list.", node->left);
		}
		deletenode_list(ses, node, LIST_TAB);
		flag = TRUE;
	}

	if (!flag && show_message(ses, LIST_TAB))
	{
		tintin_puts2("#THAT TAB IS NOT DEFINED.", ses);
	}
	return ses;
}


/*
	This function is called infinitely from completion_matches(), untill
	NULL is returned. *str is the partial word, int state is 0 the first
	time the function is called.

	Currently it returns the first match from the sorted tab list.
*/

char *tab_complete(const char *str, int state)
{
	struct listnode *node;

	if (state)
	{
		return NULL;
	}

	for (node = gtd->ses->list[LIST_TAB]->f_node ; node ; node = node->next)
	{
		if (is_abbrev(str, node->left))
		{
			return strdup(node->left);
		}
	}
	return NULL;
}
