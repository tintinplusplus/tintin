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
*                         coded by Peter Unold 1992                           *
*                     recoded by Igor van den Hoven 2004                      *
******************************************************************************/

#include "tintin.h"


DO_COMMAND(do_path)
{
	char left[BUFFER_SIZE];
	int cnt;

	arg = get_arg_in_braces(arg, left, FALSE);

	if (*left == 0)
	{
		show_message(ses, LIST_PATH, "#SYNTAX: #PATH {NEW|END|SAVE|LOAD|WALK|DEL|INS|MAP} {argument}");


	}
	else
	{
		for (cnt = 0 ; *path_table[cnt].name ; cnt++)
		{
			if (is_abbrev(left, path_table[cnt].name))
			{
				break;
			}
		}

		if (*path_table[cnt].name == 0)
		{
			do_path(ses, "");
		}
		else
		{
			path_table[cnt].fun(ses, arg);
		}
	}
	return ses;
}

DO_PATH(path_new)
{
	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		show_message(ses, LIST_PATH, "#PATH: YOU ARE ALREADY MAPPING A PATH.");
	}
	else
	{
		kill_list(ses, LIST_PATH);

		show_message(ses, LIST_PATH, "#PATH: YOU ARE NOW MAPPING A PATH.");

		SET_BIT(ses->flags, SES_FLAG_MAPPING);
	}
}

DO_PATH(path_end)
{
	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		show_message(ses, LIST_PATH, "#PATH: YOU ARE NO LONGER MAPPING A PATH.");

		SET_BIT(ses->flags, SES_FLAG_MAPPING);
	}
	else
	{
		show_message(ses, LIST_PATH, "#PATH: YOU ARE NOT MAPPING A PATH.");
	}
}

DO_PATH(path_map)
{
	struct listroot *root;
	struct listnode *node;
	char left[BUFFER_SIZE];

	root = ses->list[LIST_PATH];

	if (root->f_node == NULL)
	{
		show_message(ses, LIST_PATH, "#PATH MAP: EMPTY PATH.");
	}
	else
	{
		sprintf(left, "%-8s", "#PATH:");

		for (node = root->f_node ; node ; node = node->next)
		{
			if (strlen(left) + strlen(node->left) > ses->cols)
			{
				tintin_puts2(left, ses);
				sprintf(left, "%-8s", "");
			}
			strcat(left, node->left);
			strcat(left, " ");
		}

		if (strlen(left) > 8)
		{
			tintin_puts2(left, ses);
		}
	}
}

DO_PATH(path_save)
{
	char result[BUFFER_SIZE], left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_PATH];

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);

	if (!is_abbrev(left, "FORWARD") && !is_abbrev(left, "BACKWARD"))
	{
		tintin_puts2("#SYNTAX: #PATH SAVE <FORWARD|BACKWARD> <ALIAS NAME>", ses);
	}
	else if (root->f_node == NULL)
	{
		tintin_puts2("#PATH SAVE: LOAD OR CREATE A PATH FIRST.", ses);
	}
	else if (*right == 0)
	{
		tintin_puts2("#PATH SAVE: YOU MUST PROVIDE AN ALIAS TO SAVE THE PATH TO.", ses);
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

		parse_input(ses, result);
	}
}


DO_PATH(path_load)
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

			arg = get_arg_in_braces(arg, left, TRUE);

			if ((node = searchnode_list(ses->list[LIST_PATHDIR], left)))
			{
				addnode_list(ses->list[LIST_PATH], node->left, node->right, "0");
			}
			else
			{
				addnode_list(root, left, left, "0");
			}
		}

		show_message(ses, LIST_PATH, "#OK. PATH WITH %d NODES LOADED.", root->count);
	}
}



DO_PATH(path_del)
{
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_PATH];
	node = root->l_node;

	if (root->l_node)
	{
		show_message(ses, LIST_PATH, "#PATH DEL: DELETED MOVE {%s}", root->l_node->left);
		deletenode_list(ses, root->l_node, LIST_PATH);
	}
	else
	{
		tintin_puts("#PATH DEL: NO MOVES LEFT.", ses);
	}
}

DO_PATH(path_ins)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);

	if (*left == 0)
	{
		show_message(ses, LIST_PATH, "#PATH INS: YOU MUST GIVE A DIRECTION TO INSERT");
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
}

DO_PATH(path_walk)
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
				parse_input(ses, left);
				deletenode_list(ses, root->l_node, LIST_PATH);
				break;

			case '\0':
			case 'f':
				strcpy(left, root->f_node->left);
				parse_input(ses, left);
				deletenode_list(ses, root->f_node, LIST_PATH);
				break;

			default:
				tintin_printf(ses, "#SYNTAX: #WALK {FORWARD|BACKWARD}");
				break;
		}
		ses->flags = flags;
	}
}


void check_insert_path(char *command, struct session *ses)
{
	struct listnode *node;

	if ((node = searchnode_list(ses->list[LIST_PATHDIR], command)))
	{
		addnode_list(ses->list[LIST_PATH], node->left, node->right, "0");
	}
}


DO_COMMAND(do_pathdir)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], coord[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_PATHDIR];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);
	arg = get_arg_in_braces(arg, coord, FALSE);

	if (*left == 0)
	{
		show_list(ses, root, LIST_PATHDIR);
	}
	else if (*right == 0)
	{
		if (show_node_with_wild(ses, left, LIST_PATHDIR) == FALSE)
		{
			show_message(ses, LIST_PATHDIR, "#NO MATCH(ES) FOUND FOR {%s}", left);
		}
	}
	else
	{
		if (*coord == 0)
		{
			strcpy(coord, "0");
		}
		updatenode_list(ses, left, right, coord, LIST_PATHDIR);

		show_message(ses, LIST_PATHDIR, "#OK: DIRECTION {%s} WILL BE REVERSED AS {%s}", left, coord, right);
	}
	return ses;
}


DO_COMMAND(do_unpathdir)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int flag = FALSE;

	root = ses->list[LIST_PATHDIR];

	arg = get_arg_in_braces(arg, left,  TRUE);

	while ((node = search_node_with_wild(root, left)))
	{
		show_message(ses, LIST_PATHDIR, "#OK. {%s} IS NO LONGER A PATHDIR.", node->left);

		deletenode_list(ses, node, LIST_PATHDIR);
		flag = TRUE;
	}
	if (!flag)
	{
		show_message(ses, LIST_PATHDIR, "#UNPATHDIR: NO MATCH(ES) FOUND FOR {%s}.", left);
	}
	return ses;
}
