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
		show_message(ses, LIST_PATH, "#SYNTAX: #PATH {NEW|END|SAVE|LOAD|RUN|WALK|DEL|INS|MAP} {argument}");
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

		DEL_BIT(ses->flags, SES_FLAG_MAPPING);
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
	char buf[BUFFER_SIZE];

	root = ses->list[LIST_PATH];

	if (root->f_node == NULL)
	{
		show_message(ses, LIST_PATH, "#PATH MAP: EMPTY PATH.");
	}
	else
	{
		sprintf(buf, "%-8s", "#PATH:");

		for (node = root->f_node ; node ; node = node->next)
		{
			if (strlen(buf) + strlen(node->left) > ses->cols)
			{
				tintin_puts2(ses, buf);

				sprintf(buf, "%-8s", "");
			}
			cat_sprintf(buf, "%s ", node->left);
		}

		if (strlen(buf) > 8)
		{
			tintin_puts2(ses, buf);
		}
	}
}

DO_PATH(path_save)
{
	char result[STRING_SIZE], left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_PATH];

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);

	if (!is_abbrev(left, "FORWARD") && !is_abbrev(left, "BACKWARD"))
	{
		tintin_puts2(ses, "#SYNTAX: #PATH SAVE <FORWARD|BACKWARD> <ALIAS NAME>");
	}
	else if (root->f_node == NULL)
	{
		tintin_puts2(ses, "#PATH SAVE: LOAD OR CREATE A PATH FIRST.");
	}
	else if (*right == 0)
	{
		tintin_puts2(ses, "#PATH SAVE: YOU MUST PROVIDE AN ALIAS TO SAVE THE PATH TO.");
	}
	else
	{
		sprintf(result, "%calias {%s} {", gtd->tintin_char, right);

		if (is_abbrev(left, "FORWARD"))
		{
			for (node = root->f_node ; node ; node = node->next)
			{
				if (strlen(result) + strlen(node->left) < STRING_SIZE - 10)
				{
					strcat(result, node->left);

					if (node->next)
					{
						cat_sprintf(result, "%c", COMMAND_SEPARATOR);
					}
				}
				else
				{
					tintin_puts(ses, "#ERROR, PATH TOO LONG FOR BUFFER, PARTIAL SAVE.");
					break;
				}
			}
		}
		else
		{
			for (node = root->l_node ; node ; node = node->prev)
			{
				if (strlen(result) + strlen(node->left) < STRING_SIZE - 10)
				{
					strcat(result, node->right);

					if (node->prev)
					{
						cat_sprintf(result, "%c", COMMAND_SEPARATOR);
					}
				}
				else
				{
					tintin_puts(ses, "#ERROR, PATH TOO LONG FOR BUFFER, PARTIAL SAVE.");
					break;
				}
			}
		}
		strcat(result, "}");

		pre_parse_input(ses, result, SUB_NONE);
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
		tintin_puts(ses, "#PATH DEL: NO MOVES LEFT.");
	}
}

DO_PATH(path_ins)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 0);

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

DO_PATH(path_run)
{
	char left[BUFFER_SIZE], time[BUFFER_SIZE], name[BUFFER_SIZE];
	struct listroot *root;
	int wait;
	long long flags;

	root = ses->list[LIST_PATH];

	arg = get_arg_in_braces(arg, left, FALSE);

	if (root->f_node == NULL)
	{
		tintin_puts(ses, "#END OF PATH.");
	}
	else
	{
		flags = ses->flags;

		DEL_BIT(ses->flags, SES_FLAG_MAPPING);

		if (*left)
		{
			wait = 0;

			while (root->f_node)
			{
				sprintf(time, "%lld", utime() + wait);
				sprintf(name, "%lld", gtd->time);

				wait += (long long) (get_number(ses, left) * 1000000LL);

				updatenode_list(ses, name, root->f_node->left, time, LIST_DELAY);

				deletenode_list(ses, root->f_node, LIST_PATH);
			}
		}
		else
		{
			while (root->f_node)
			{
				pre_parse_input(ses, root->f_node->left, SUB_NONE);

				deletenode_list(ses, root->f_node, LIST_PATH);
			}
		}
		ses->flags = flags;
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
		tintin_puts(ses, "#END OF PATH.");
	}
	else
	{
		flags = ses->flags;

		DEL_BIT(ses->flags, SES_FLAG_MAPPING);

		switch (tolower(*left))
		{
			case 'b':
				pre_parse_input(ses, root->l_node->right, SUB_NONE);
				deletenode_list(ses, root->l_node, LIST_PATH);
				break;

			case '\0':
			case 'f':
				pre_parse_input(ses, root->f_node->left, SUB_NONE);
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

		show_message(ses, LIST_PATHDIR, "#OK: DIRECTION {%s} WILL BE REVERSED AS {%s} @ {%s}", left, right, coord);
	}
	return ses;
}


DO_COMMAND(do_unpathdir)
{
	delete_node_with_wild(ses, LIST_PATHDIR, arg);

	return ses;
}
