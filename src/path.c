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
/* file: path.c - stuff for the path feature                         */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                    coded by peter unold 1992                      */
/*                  recoded by Jeremy C. Jack 1994                   */
/*                  recoded by Igor van den Hoven 2004               */
/*********************************************************************/
/* the path is implemented as a fix-sized queue. It gets a bit messy */
/* here and there, but it should work....                            */
/*********************************************************************/


#include "tintin.h"

/*
	the #mark command
*/

DO_COMMAND(do_mark)
{
	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		tintin_puts2("#YOU ARE NO LONGER MAPPING.", ses);
	}
	else
	{
		kill_list(ses, LIST_PATH);

		tintin_puts2("#YOU ARE NOW MAPPING.", ses);
	}
	TOG_BIT(ses->flags, SES_FLAG_MAPPING);

	return ses;
}

/*
	the #map command
*/

DO_COMMAND(do_map)
{
	struct listroot *root;
	struct listnode *node;
	char buf[BUFFER_SIZE];

	root = ses->list[LIST_PATH];

	sprintf(buf, "%-10s", "#PATH:");

	for (node = root->f_node ; node ; node = node->next)
	{
		if (strlen(buf) + strlen(node->left) > ses->cols)
		{
			tintin_puts2(buf, ses);
			sprintf(buf, "%-8s", "#PATH:");
		}
		strcat(buf, node->left);
		strcat(buf, " ");
	}
	if (strlen(buf) > 8)
	{
		tintin_puts2(buf, ses);
	}
	return ses;
}


DO_COMMAND(do_savepath)
{
	char result[BUFFER_SIZE], left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_PATH];

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);

	if (!is_abbrev(left, "FORWARD") && !is_abbrev(left, "BACKWARD"))
	{
		tintin_puts2("#SYNTAX: #SAVEPATH <FORWARD|BACKWARD> <ALIAS NAME>", ses);
	}
	else if (root->f_node == NULL)
	{
		tintin_puts2("#LOAD OR CREATE A PATH FIRST.", ses);
	}
	else if (*right == 0)
	{
		tintin_puts2("#YOU MUST PROVIDE AN ALIAS TO SAVE THE PATH INTO.", ses);
	}
	else
	{
		sprintf(result, "%calias {%s} {", gtd->tintin_char, right);

		if (is_abbrev(left, "FORWARD"))
		{
			for (node = root->f_node ; node ; node = node->next)
			{
				if (strlen(result) + strlen(node->left) < BUFFER_SIZE - 10)
				{
					strcat(result, node->left);

					if (node->next)
					{
						strcat(result, ";");
					}
				}
				else
				{
					tintin_puts("#ERROR, PATH TOO LONG FOR BUFFER, PARTIAL SAVE.", ses);
					break;
				}
			}
		}
		else
		{
			for (node = root->l_node ; node ; node = node->prev)
			{
				if (strlen(result) + strlen(node->left) < BUFFER_SIZE - 10)
				{
					strcat(result, node->right);

					if (node->prev)
					{
						strcat(result, ";");
					}
				}
				else
				{
					tintin_puts("#ERROR, PATH TOO LONG FOR BUFFER, PARTIAL SAVE.", ses);
					break;
				}
			}
		}
		strcat(result, "}");

		parse_input(result, ses);
	}
	return ses;
}


DO_COMMAND(do_loadpath)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_PATH];

	arg = get_arg_in_braces(arg, left, FALSE);

	if ((node = searchnode_list(ses->list[LIST_ALIAS], left)) == NULL)
	{
		show_message(ses, LIST_PATH, "#ALIAS {%s} NOT FOUND.", left);
	}
	else
	{
		kill_list(ses, LIST_PATH);

		arg = node->right;

		while (*arg)
		{
			if (*arg == ';')
			{
				arg++;
			}

			arg = get_arg_with_spaces(arg, left);

			addnode_list(root, left, "", "0");

			if (HAS_BIT(root->flags, LIST_FLAG_DEBUG))
			{
				tintin_printf2(ses, "#DEBUG #LOADPATH: %3d %s", root->count, left);
			}
		}

		show_message(ses, LIST_PATH, "#OK. PATH WITH %d NODES LOADED.", root->count);
	}
	return ses;
}

DO_COMMAND(do_path)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	if (*left == 0)
	{
		tintin_printf(ses, "#SYNTAX: #PATH {command} {reversed command}");
	}
	else if (*right == 0 && searchnode_list(ses->list[LIST_PATHDIR], left))
	{
		check_insert_path(left, ses);
	}
	else
	{
		addnode_list(ses->list[LIST_PATH], left, right, "0");

		show_message(ses, LIST_PATH, "#OK. #PATH - {%s} = {%s} ADDED.", left, right);
	}
	return ses;
}


DO_COMMAND(do_unpath)
{
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_PATH];
	node = root->l_node;

	if (root->l_node)
	{
		show_message(ses, LIST_PATH, "#OK. #UNPATH - DELETED MOVE {%s}", root->l_node->left);
		deletenode_list(ses, root->l_node, LIST_PATH);
	}
	else
	{
		tintin_puts("#ERROR: #UNPATH - NO MOVES LEFT.", ses);
	}
	return ses;
}


DO_COMMAND(do_walk)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	long long flags;

	root = ses->list[LIST_PATH];

	arg = get_arg_in_braces(arg, left, FALSE);

	if (root->f_node == NULL)
	{
		tintin_puts("#END OF PATH.", ses);
	}
	else
	{
		flags = ses->flags;

		DEL_BIT(ses->flags, SES_FLAG_MAPPING);

		switch (tolower(*left))
		{
			case 'b':
				strcpy(left, root->l_node->right);
				parse_input(left, ses);
				deletenode_list(ses, root->l_node, LIST_PATH);
				break;

			case '\0':
			case 'f':
				strcpy(left, root->f_node->left);
				parse_input(left, ses);
				deletenode_list(ses, root->f_node, LIST_PATH);
				break;

			default:
				tintin_printf(ses, "#SYNTAX: #WALK {FORWARD|BACKWARD}");
				break;
		}
		ses->flags = flags;
	}
	return ses;
}


DO_COMMAND(do_pathdir)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_PATHDIR];

	arg = get_arg_in_braces(arg, left, 0);
	arg = get_arg_in_braces(arg, right, 1);

	if (!*left)
	{
		show_list(ses, root, LIST_PATHDIR);
	}
	else if (*left && !*right)
	{
		if (show_node_with_wild(ses, left, LIST_PATHDIR) == FALSE)
		{
			show_message(ses, LIST_PATHDIR, "#NO MATCH(ES) FOUND FOR {%s}", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, "0", LIST_PATHDIR);

		show_message(ses, LIST_PATHDIR, "#OK: DIRECTION {%s} WILL BE REVERSED AS {%s}", left, right);
	}
	return ses;
}

void check_insert_path(const char *command, struct session *ses)
{
	struct listnode *node;

	if ((node = searchnode_list(ses->list[LIST_PATHDIR], command)))
	{
		addnode_list(ses->list[LIST_PATH], node->left, node->right, "0");
	}
}
