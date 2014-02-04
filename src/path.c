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

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	if (*left == 0)
	{
		show_message(ses, LIST_PATH, "#SYNTAX: #PATH {DEL|END|INS|LOAD|NEW|RUN|SAVE|SHOW|UNZIP|WALK|ZIP} {argument}.");
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
		kill_list(ses->list[LIST_PATH]);

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

DO_PATH(path_show)
{
	struct listroot *root = ses->list[LIST_PATH];
	char buf[BUFFER_SIZE];
	int i;

	if (root->used == 0)
	{
		show_message(ses, LIST_PATH, "#PATH SHOW: EMPTY PATH.");
	}
	else
	{
		sprintf(buf, "%-8s", "#PATH:");

		for (i = 0 ; i < root->used ; i++)
		{
			if ((int) strlen(buf) + (int) strlen(root->list[i]->left) > ses->cols)
			{
				tintin_puts2(ses, buf);

				sprintf(buf, "%-8s", "");
			}
			cat_sprintf(buf, "%s ", root->list[i]->left);
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
	int i;

	root = ses->list[LIST_PATH];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, right, GET_ONE, SUB_VAR|SUB_FUN);

	if (!is_abbrev(left, "FORWARDS") && !is_abbrev(left, "BACKWARDS"))
	{
		tintin_puts2(ses, "#SYNTAX: #PATH SAVE <FORWARD|BACKWARD> <VARIABLE NAME>");
	}
	else if (root->used == 0)
	{
		tintin_puts2(ses, "#PATH SAVE: LOAD OR CREATE A PATH FIRST.");
	}
	else if (*right == 0)
	{
		tintin_puts2(ses, "#PATH SAVE: YOU MUST PROVIDE A VARIABLE TO SAVE THE PATH TO.");
	}
	else
	{
		result[0] = 0;

		if (is_abbrev(left, "FORWARDS"))
		{
			for (i = 0 ; i < root->used ; i++)
			{
				strcat(result, root->list[i]->left);

				if (i != root->used - 1)
				{
					cat_sprintf(result, "%c", COMMAND_SEPARATOR);
				}
			}
		}
		else
		{
			for (i = root->used - 1 ; i >= 0 ; i--)
			{
				strcat(result, root->list[i]->right);

				if (i != 0)
				{
					cat_sprintf(result, "%c", COMMAND_SEPARATOR);
				}
			}
		}
		set_nest_node(ses->list[LIST_VARIABLE], right, "%s", result);
	}
}


DO_PATH(path_load)
{
	char left[BUFFER_SIZE], temp[BUFFER_SIZE];
	struct listnode *node;

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	if ((node = search_node_list(ses->list[LIST_VARIABLE], left)) == NULL)
	{
		arg = left;
	}
	else
	{
		arg = node->right;
	}

	kill_list(ses->list[LIST_PATH]);

	while (*arg)
	{
		if (*arg == ';')
		{
			arg++;
		}

		arg = get_arg_in_braces(ses, arg, temp, TRUE);

		if ((node = search_node_list(ses->list[LIST_PATHDIR], temp)))
		{
			insert_node_list(ses->list[LIST_PATH], node->left, node->right, "0");
		}
		else
		{
			insert_node_list(ses->list[LIST_PATH], temp, temp, "0");
		}
	}
	show_message(ses, LIST_PATH, "#OK. PATH WITH %d NODES LOADED.", ses->list[LIST_PATH]->used);
}

DO_PATH(path_del)
{
	struct listroot *root = ses->list[LIST_PATH];

	if (root->used)
	{
		show_message(ses, LIST_PATH, "#PATH DEL: DELETED MOVE {%s}.", root->list[root->used - 1]->left);

		delete_index_list(ses->list[LIST_PATH], root->used - 1);
	}
	else
	{
		tintin_puts(ses, "#PATH DEL: NO MOVES LEFT.");
	}
}

DO_PATH(path_ins)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, right, GET_ONE, SUB_VAR|SUB_FUN);

	if (*left == 0 && *right == 0)
	{
		show_message(ses, LIST_PATH, "#PATH INS: YOU MUST GIVE A COMMAND TO INSERT");
	}
	else
	{
		insert_node_list(ses->list[LIST_PATH], left, right, "0");

		show_message(ses, LIST_PATH, "#PATH INS: FORWARD {%s} BACKWARD {%s}.", left, right);
	}
}

DO_PATH(path_run)
{
	char left[BUFFER_SIZE], time[BUFFER_SIZE], name[BUFFER_SIZE];
	struct listroot *root;
	int i;

	root = ses->list[LIST_PATH];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	if (root->used == 0)
	{
		tintin_puts(ses, "#END OF PATH.");
	}
	else
	{
		DEL_BIT(ses->flags, SES_FLAG_MAPPING);

		if (*left)
		{
			double delay = 0;

			for (i = 0 ; i < root->used ; i++)
			{
				sprintf(name, "PATH %lld", utime());
				sprintf(time, "%f", delay);

				delay += get_number(ses, left);

				update_node_list(ses->list[LIST_DELAY], name, root->list[i]->left, time);
			}
		}
		else
		{
			for (i = 0 ; i < root->used ; i++)
			{
				script_driver(ses, LIST_PATH, root->list[i]->left);
			}
		}
		kill_list(ses->list[LIST_PATH]);
	}
}


DO_PATH(path_walk)
{
	char left[BUFFER_SIZE], temp[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_PATH];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	if (root->used == 0)
	{
		tintin_puts(ses, "#END OF PATH.");
	}
	else
	{
		DEL_BIT(ses->flags, SES_FLAG_MAPPING);

		switch (tolower((int) *left))
		{
			case 'b':
				strcpy(temp, root->list[root->used - 1]->right);
				delete_index_list(ses->list[LIST_PATH], root->used - 1);
				script_driver(ses, LIST_PATH, temp);
				break;

			case '\0':
			case 'f':
				strcpy(temp, root->list[0]->left);
				delete_index_list(ses->list[LIST_PATH], 0);
				script_driver(ses, LIST_PATH, temp);
				break;

			default:
				tintin_printf(ses, "#SYNTAX: #WALK {FORWARD|BACKWARD}.");
				break;
		}
		if (root->used == 0)
		{
			check_all_events(ses, SUB_ARG|SUB_SEC, 0, 0, "END OF PATH");
		}
	}
}

DO_PATH(path_zip)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listroot *root;
	int i, cnt;

	cnt   =  1;
	root  =  ses->list[LIST_PATH];
	*left =  0;
	*right = 0;

	for (i = 0 ; i < root->used ; i++)
	{
		if (search_node_list(ses->list[LIST_PATHDIR], root->list[i]->left) == NULL || strlen(root->list[i]->left) != 1)
		{
			if (i && search_node_list(ses->list[LIST_PATHDIR], root->list[i - 1]->left) != NULL && strlen(root->list[i - 1]->left) == 1)
			{
				cat_sprintf(left, "%c", COMMAND_SEPARATOR);
			}
			cat_sprintf(left, "%s", root->list[i]->left);

			if (i < root->used - 1)
			{
				cat_sprintf(left, "%c", COMMAND_SEPARATOR);
			}
			continue;
		}

		if (i < root->used - 1 && !strcmp(root->list[i]->left, root->list[i + 1]->left))
		{
			cnt++;
		}
		else
		{
			if (cnt > 1)
			{
				cat_sprintf(left, "%d%s", cnt, root->list[i]->left);
			}
			else
			{
				cat_sprintf(left, "%s", root->list[i]->left);
			}
			cnt = 1;
		}
	}

	for (i = root->used - 1 ; i >= 0 ; i--)
	{
		if (search_node_list(ses->list[LIST_PATHDIR], root->list[i]->right) == NULL || strlen(root->list[i]->right) != 1)
		{
			if (i != root->used - 1 && search_node_list(ses->list[LIST_PATHDIR], root->list[i + 1]->right) != NULL && strlen(root->list[i + 1]->right) == 1)
			{
				cat_sprintf(right, "%c", COMMAND_SEPARATOR);
			}
			cat_sprintf(right, "%s", root->list[i]->right);

			if (i > 0)
			{
				cat_sprintf(right, "%c", COMMAND_SEPARATOR);
			}
			continue;
		}

		if (i > 0 && !strcmp(root->list[i]->right, root->list[i - 1]->right))
		{
			cnt++;
		}
		else
		{
			if (cnt > 1)
			{
				cat_sprintf(right, "%d%s", cnt, root->list[i]->right);
			}
			else
			{
				cat_sprintf(right, "%s", root->list[i]->right);
			}
			cnt = 1;
		}
	}

	kill_list(ses->list[LIST_PATH]);

	insert_node_list(ses->list[LIST_PATH], left, right, "0");

	show_message(ses, LIST_PATH, "#OK. THE PATH HAS BEEN ZIPPED TO {%s} {%s}.", left, right);
}

DO_PATH(path_unzip)
{
	char left[BUFFER_SIZE], temp[BUFFER_SIZE], *str;
	struct listnode *node;

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	if ((node = search_node_list(ses->list[LIST_VARIABLE], left)) == NULL)
	{
		arg = left;
	}
	else
	{
		arg = node->right;
	}

	kill_list(ses->list[LIST_PATH]);

	while (*arg)
	{
		if (*arg == ';')
		{
			arg++;
		}

		arg = get_arg_in_braces(ses, arg, temp, TRUE);

		if (is_speedwalk(ses, temp))
		{
			char dir[2];
			int cnt, i;

			str = temp;

			for (dir[1] = 0 ; *str ; str++)
			{
				if (isdigit((int) *str))
				{
					sscanf(str, "%d%c", &cnt, dir);

					while (*str != dir[0])
					{
						str++;
					}
				}
				else
				{
					cnt = 1;
					dir[0] = *str;
				}

				for (i = 0 ; i < cnt ; i++)
				{
					if ((node = search_node_list(ses->list[LIST_PATHDIR], dir)))
					{
						insert_node_list(ses->list[LIST_PATH], node->left, node->right, "0");
					}
					else
					{
						insert_node_list(ses->list[LIST_PATH], dir, dir, "0");
					}
				}
			}
		}
		else
		{
			if ((node = search_node_list(ses->list[LIST_PATHDIR], temp)))
			{
				insert_node_list(ses->list[LIST_PATH], node->left, node->right, "0");
			}
			else
			{
				insert_node_list(ses->list[LIST_PATH], temp, temp, "0");
			}
		}
	}
	show_message(ses, LIST_PATH, "#OK. PATH WITH %d NODES UNZIPPED.", ses->list[LIST_PATH]->used);
}


void check_insert_path(char *command, struct session *ses)
{
	struct listnode *node;

	if ((node = search_node_list(ses->list[LIST_PATHDIR], command)))
	{
		insert_node_list(ses->list[LIST_PATH], node->left, node->right, "0");
	}
}


DO_COMMAND(do_pathdir)
{
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE], arg3[BUFFER_SIZE];
	struct listnode *node;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ONE, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
	{
		show_list(ses->list[LIST_PATHDIR], 0);
	}
	else if (*arg2 == 0)
	{
		if (show_node_with_wild(ses, arg1, LIST_PATHDIR) == FALSE)
		{
			show_message(ses, LIST_PATHDIR, "#NO MATCH(ES) FOUND FOR {%s}.", arg1);
		}
	}
	else
	{
		if (*arg3 == 0)
		{
			if ((node = search_node_list(ses->list[LIST_PATHDIR], arg1)) != NULL)
			{
				strcpy(arg3, node->pr);
			}
			else
			{
				strcpy(arg3, "0");
			}
		}
		update_node_list(ses->list[LIST_PATHDIR], arg1, arg2, arg3);

		show_message(ses, LIST_PATHDIR, "#OK: DIRECTION {%s} WILL BE REVERSED AS {%s} @ {%s}.", arg1, arg2, arg3);
	}
	return ses;
}


DO_COMMAND(do_unpathdir)
{
	delete_node_with_wild(ses, LIST_PATHDIR, arg);

	return ses;
}
