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
*   file: macro.c - funtions related to key binding                           *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                     coded by Igor van den Hoven 2004                        *
******************************************************************************/


#include "tintin.h"

DO_COMMAND(do_macro)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_MACRO];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	if (*left == 0)
	{
		show_list(ses, root, LIST_MACRO);
	}
	else if (*left && *right == 0)
	{
		if (show_node_with_wild(ses, left, LIST_MACRO) == FALSE)
		{
			tintin_printf2(ses, "#MACRO, NO MATCH(ES) FOUND FOR {%s}", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, "0", LIST_MACRO);

		sprintf(temp, "[[%s]]", right);

		rl_macro_bind(left, temp, gtd->keymap);

		tintin_printf2(ses, "#OK {%s} MACROS {%s}", left, right);
	}
	return ses;
}


DO_COMMAND(do_unmacro)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int flag = FALSE;

	root = ses->list[LIST_MACRO];

	arg = get_arg_in_braces(arg, left, TRUE);

	while ((node = search_node_with_wild(root, left)))
	{
		if (show_message(ses, LIST_MACRO))
		{
			tintin_printf(ses, "#OK {%s} IS NO LONGER A MACRO.", node->left);
		}

		deletenode_list(ses, node, LIST_MACRO);

		flag = TRUE;
	}
	if (!flag && show_message(ses, LIST_MACRO))
	{
		tintin_printf2(ses, "#NO MATCH(ES) FOUND FOR {%s}", left);
	}
	return ses;
}


void macro_update(void)
{
	char *buf = rl_line_buffer, command[BUFFER_SIZE];
	static int p;
	int i, a, z;

	for (i = 0 ; buf[i] ; i++)
	{
		if (buf[i] == '[' && buf[i+1] == '[')
		{
			for (a = z = i+2 ; buf[z] ; z++)
			{
				if (buf[z] == ']' && buf[z+1] == ']')
				{
					strcpy(command, rl_copy_text(a, z));

					rl_point = p < a - 2 ? p : a - 2;

					rl_delete_text(a-2, z+2);

					rl_redisplay();

					echo_command(gtd->ses, "", TRUE);

					parse_input(command, gtd->ses);

					break;
				}
			}
		}
	}

	p = rl_point;
}
