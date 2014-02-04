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
/* file: antisub.c - functions related to the substitute command     */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

#include "tintin.h"


DO_COMMAND(do_antisubstitute)
{
	char left[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_ANTISUBSTITUTE];

	arg = get_arg_in_braces(arg, left, 1);

	if (!*left)
	{
		tintin_header(ses, " ANTISUBSTITUTES ");
		show_list(ses, root, LIST_ANTISUBSTITUTE);
	}
	else
	{
		updatenode_list(ses, left, left, "0", LIST_ANTISUBSTITUTE);

		if (show_message(ses, LIST_ANTISUBSTITUTE))
		{
			tintin_printf2(ses, "#Ok. Any line with {%s} will not be subbed.", left);
		}
	}
	return ses;
}


DO_COMMAND(do_unantisubstitute)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int flag = FALSE;

	root = ses->list[LIST_ANTISUBSTITUTE];

	arg = get_arg_in_braces(arg, left, 1);

	while ((node = search_node_with_wild(root, left)) != NULL)
	{
		if (show_message(ses, LIST_ANTISUBSTITUTE))
		{
			tintin_printf(ses, "#Ok. Lines with {%s} will now be subbed.", node->left);
		}
		deletenode_list(ses, node, LIST_ANTISUBSTITUTE);
		flag = TRUE;
	}

	if (!flag && show_message(ses, LIST_ANTISUBSTITUTE))
	{
		tintin_puts2("#THAT ANTISUBSTITUTE IS NOT DEFINED.", ses);
	}
	return ses;
}


int check_all_antisubstitutions(const char *original, char *line, struct session *ses)
{
	struct listnode *node;

	for (node = ses->list[LIST_ANTISUBSTITUTE]->f_node ; node ; node = node->next)
	{
		if (check_one_action(line, original, node->left, ses))
		{
			return TRUE;
		}
	}
	return FALSE;
}
