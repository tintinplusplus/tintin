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
		tintin_header(ses, " PATH COMMANDS ");

		for (cnt = 0 ; *path_table[cnt].fun != NULL ; cnt++)
		{
			if (*path_table[cnt].desc)
			{
				tintin_printf2(ses, "  [%-13s] %s", path_table[cnt].name, path_table[cnt].desc);
			}
		}
		tintin_header(ses, "");

		return ses;
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


DO_PATH(path_create)
{
	struct listroot *root = ses->list[LIST_PATH];

	root->update = 0;

	kill_list(root);

	show_message(ses, LIST_PATH, "#PATH CREATE: YOU START MAPPING A NEW PATH.");

	SET_BIT(ses->flags, SES_FLAG_PATHMAPPING);
}


DO_PATH(path_destroy)
{
	struct listroot *root = ses->list[LIST_PATH];

	root->update = 0;

	kill_list(root);

	DEL_BIT(ses->flags, SES_FLAG_PATHMAPPING);

	show_message(ses, LIST_PATH, "#PATH DESTROY: PATH DESTROYED.");
}


DO_PATH(path_start)
{
	if (HAS_BIT(ses->flags, SES_FLAG_PATHMAPPING))
	{
		show_message(ses, LIST_PATH, "#PATH START: ERROR: YOU ARE ALREADY MAPPING A PATH.");
	}
	else
	{
		SET_BIT(ses->flags, SES_FLAG_PATHMAPPING);

		show_message(ses, LIST_PATH, "#PATH START: YOU START MAPPING A PATH.");
	}
}

DO_PATH(path_stop)
{
	if (HAS_BIT(ses->flags, SES_FLAG_PATHMAPPING))
	{
		show_message(ses, LIST_PATH, "#PATH STOP: YOU STOP MAPPING A PATH.");

		DEL_BIT(ses->flags, SES_FLAG_PATHMAPPING);
	}
	else
	{
		show_message(ses, LIST_PATH, "#PATH STOP: ERROR: YOU ARE NOT MAPPING A PATH.");
	}
}


DO_PATH(path_map)
{
	struct listroot *root = ses->list[LIST_PATH];
	char buf[BUFFER_SIZE];
	int i = 0;

	if (root->used == 0)
	{
		show_message(ses, LIST_PATH, "#PATH MAP: EMPTY PATH.");
	}
	else
	{
		sprintf(buf, "%-7s", "#PATH:");

		for (i = 0 ; i < root->update ; i++)
		{
			if ((int) strlen(buf) + (int) strlen(root->list[i]->left) > ses->cols - 4)
			{
				tintin_puts2(ses, buf);

				sprintf(buf, "%-7s", "");
			}
			cat_sprintf(buf, " %s", root->list[i]->left);
		}

		if (i != root->used)
		{
			cat_sprintf(buf, " [%s]", root->list[i++]->left);

			for (i = root->update + 1 ; i < root->used ; i++)
			{
				if ((int) strlen(buf) + (int) strlen(root->list[i]->left) > ses->cols - 4)
				{
					tintin_puts2(ses, buf);

					sprintf(buf, "%-7s", "");
				}
				cat_sprintf(buf, " %s", root->list[i]->left);
			}
		}

		if (root->update == root->used)
		{
			cat_sprintf(buf, " [ ]");
		}

		if (strlen(buf) > 7)
		{
			tintin_puts2(ses, buf);
		}
	}
}


DO_PATH(path_save)
{
	struct listroot *root = ses->list[LIST_PATH];
	char result[STRING_SIZE], left[BUFFER_SIZE], right[BUFFER_SIZE];
	int i;

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

		show_message(ses, LIST_PATH, "#PATH SAVE: PATH SAVED TO {%s}", right);
	}
}


DO_PATH(path_load)
{
	struct listroot *root = ses->list[LIST_PATH];
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

	root->update = 0;

	kill_list(root);

	while (*arg)
	{
		if (*arg == ';')
		{
			arg++;
		}

		arg = get_arg_in_braces(ses, arg, temp, TRUE);

		if ((node = search_node_list(root, temp)))
		{
			insert_node_list(root, node->left, node->right, "0");
		}
		else
		{
			insert_node_list(root, temp, temp, "0");
		}
	}
	show_message(ses, LIST_PATH, "#PATH LOAD: PATH WITH %d NODES LOADED.", root->used);
}

DO_PATH(path_delete)
{
	struct listroot *root = ses->list[LIST_PATH];

	if (root->used)
	{
		show_message(ses, LIST_PATH, "#PATH DELETE: DELETED MOVE {%s}.", root->list[root->used - 1]->left);

		delete_index_list(root, root->used - 1);

		if (root->update >= root->used)
		{
			root->update--;
		}
	}
	else
	{
		tintin_puts(ses, "#PATH DELETE: NO MOVES LEFT.");
	}

}

DO_PATH(path_insert)
{
	struct listroot *root = ses->list[LIST_PATH];
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, right, GET_ONE, SUB_VAR|SUB_FUN);

	if (*left == 0 && *right == 0)
	{
		show_message(ses, LIST_PATH, "#PATH INSERT: ERROR: YOU MUST GIVE A COMMAND TO INSERT");
	}
	else
	{
		insert_node_list(root, left, right, "0");

		show_message(ses, LIST_PATH, "#PATH INSERT: FORWARD {%s} BACKWARD {%s}.", left, right);

		if (HAS_BIT(ses->flags, SES_FLAG_PATHMAPPING))
		{
			root->update = root->used;
		}
	}
}


DO_PATH(path_run)
{
	struct listroot *root = ses->list[LIST_PATH];
	char left[BUFFER_SIZE], time[BUFFER_SIZE], name[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	if (root->update == root->used)
	{
		tintin_puts(ses, "#PATH RUN: #END OF PATH.");
	}
	else
	{
		DEL_BIT(ses->flags, SES_FLAG_PATHMAPPING);

		if (*left)
		{
			double delay = 0;

			while (root->update < root->used)
			{
				sprintf(name, "PATH %lld", utime());
				sprintf(time, "%f", delay);

				delay += get_number(ses, left);

				update_node_list(root, name, root->list[root->update++]->left, time);
			}
		}
		else
		{
			while (root->update < root->used)
			{
				script_driver(ses, LIST_PATH, root->list[root->update++]->left);
			}
		}
	}
}


DO_PATH(path_walk)
{
	struct listroot *root = ses->list[LIST_PATH];
	char left[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	DEL_BIT(ses->flags, SES_FLAG_PATHMAPPING);

	if (is_abbrev(left, "BACKWARDS"))
	{
		if (root->update == 0)
		{
			tintin_puts(ses, "#PATH WALK: #START OF PATH.");
		}
		else
		{
			script_driver(ses, LIST_PATH, root->list[--root->update]->right);

			if (root->used == 0)
			{
				check_all_events(ses, SUB_ARG|SUB_SEC, 0, 0, "START OF PATH");
			}
		}
	}
	else if (*left == 0 || is_abbrev(left, "FORWARDS"))
	{
		if (root->update == root->used)
		{
			tintin_puts(ses, "#PATH WALK: #END OF PATH.");
		}
		else
		{
			script_driver(ses, LIST_PATH, root->list[root->update++]->left);

			if (root->update == root->used)
			{
				check_all_events(ses, SUB_ARG|SUB_SEC, 0, 0, "END OF PATH");
			}
		}
	}
	else
	{
		tintin_printf(ses, "#SYNTAX: #PATH WALK {FORWARD|BACKWARD}.");
	}
}


DO_PATH(path_swap)
{
	struct listroot *root = ses->list[LIST_PATH];
	struct listnode *node;
	int a, z;

	a = 0;
	z = root->used - 1;

	if (root->update)
	{
		root->update = root->used - root->update;
	}

	while (z > a)
	{
		arg = root->list[z]->left;
		root->list[z]->left = root->list[z]->right;
		root->list[z]->right = arg;

		arg = root->list[a]->left;
		root->list[a]->left = root->list[a]->right;
		root->list[a]->right = arg;

		node = root->list[z];
		root->list[z--] = root->list[a];
		root->list[a++] = node;
	}

	if (z == a)
	{
		arg = root->list[z]->left;
		root->list[z]->left = root->list[z]->right;
		root->list[z]->right = arg;
	}

	show_message(ses, LIST_PATH, "#PATH SWAP: PATH HAS BEEN SWAPPED.");
}


DO_PATH(path_zip)
{
	struct listroot *root = ses->list[LIST_PATH];
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int i, cnt;

	cnt   =  1;

	*left =  0;
	*right = 0;

	for (i = 0 ; i < root->used ; i++)
	{
		if (search_node_list(root, root->list[i]->left) == NULL || strlen(root->list[i]->left) != 1)
		{
			if (i && search_node_list(root, root->list[i - 1]->left) != NULL && strlen(root->list[i - 1]->left) == 1)
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
		if (search_node_list(root, root->list[i]->right) == NULL || strlen(root->list[i]->right) != 1)
		{
			if (i != root->used - 1 && search_node_list(root, root->list[i + 1]->right) != NULL && strlen(root->list[i + 1]->right) == 1)
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

	root->update = root->used;

	kill_list(root);

	insert_node_list(root, left, right, "0");

	show_message(ses, LIST_PATH, "#PATH ZIP: THE PATH HAS BEEN ZIPPED TO {%s} {%s}.", left, right);
}

DO_PATH(path_unzip)
{
	struct listroot *root = ses->list[LIST_PATH];
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

	root->update = 0;

	kill_list(root);

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
						insert_node_list(root, node->left, node->right, "0");
					}
					else
					{
						insert_node_list(root, dir, dir, "0");
					}
				}
			}
		}
		else
		{
			if ((node = search_node_list(ses->list[LIST_PATHDIR], temp)))
			{
				insert_node_list(root, node->left, node->right, "0");
			}
			else
			{
				insert_node_list(root, temp, temp, "0");
			}
		}
	}
	show_message(ses, LIST_PATH, "#PATH UNZIP: PATH WITH %d NODES UNZIPPED.", root->used);
}


DO_PATH(path_goto)
{
	struct listroot *root = ses->list[LIST_PATH];
	char left[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	if (is_abbrev(left, "END"))
	{
		root->update = root->used;

		show_message(ses, LIST_PATH, "#PATH GOTO: POSITION SET TO {%d}.", root->update);
	}
	else if (is_abbrev(left, "START"))
	{
		root->update = 0;

		show_message(ses, LIST_PATH, "#PATH GOTO: POSITION SET TO %d.", root->update);
	}
	else if (is_number(left))
	{
		if (get_number(ses, left) < 0 || get_number(ses, left) > root->used)
		{
			show_message(ses, LIST_PATH, "#PATH GOTO: POSITION MUST BE BETWEEN 0 AND %d.", root->used);
		}
		else
		{
			root->update = get_number(ses, left);

			show_message(ses, LIST_PATH, "#PATH GOTO: POSITION SET TO %d.", root->update);
		}
	}
}


DO_PATH(path_move)
{
	struct listroot *root = ses->list[LIST_PATH];
	char left[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	if (is_abbrev(left, "BACKWARD"))
	{
		if (root->update == 0)
		{
			show_message(ses, LIST_PATH, "#PATH GOTO: ALREADY AT START OF PATH.", root->update);
		}
		else
		{
			root->update--;

			show_message(ses, LIST_PATH, "#PATH MOVE: POSITION SET TO %d.", root->update);
		}
	}
	else if (is_abbrev(left, "FORWARD"))
	{
		if (root->update == root->used)
		{
			show_message(ses, LIST_PATH, "#PATH MOVE: ALREADY AT END OF PATH.", root->update);
		}
		else
		{
			root->update++;

			show_message(ses, LIST_PATH, "#PATH MOVE: POSITION SET TO %d.", root->update);
		}
	}
	else if (is_number(left))
	{
		root->update = URANGE(0, root->update + get_number(ses, left), root->used);
		
		show_message(ses, LIST_PATH, "#PATH MOVE: POSITION SET TO %d.", root->update);
	}
}

DO_PATH(path_undo)
{
	struct listroot *root = ses->list[LIST_PATH];

	if (root->used == 0)
	{
		show_message(ses, LIST_PATH, "#PATH UNDO: ERROR: PATH IS EMPTY.");

		return;
	}

	if (root->update != root->used)
	{
		show_message(ses, LIST_PATH, "#PATH UNDO: ERROR: YOUR POSITION IS NOT AT END OF PATH.");
	
		return;
	}

	if (!HAS_BIT(ses->flags, SES_FLAG_PATHMAPPING))
	{
		show_message(ses, LIST_PATH, "#PATH UNDO: ERROR: YOU ARE NOT CURRENTLY MAPPING A PATH.");

		return;
	}

	DEL_BIT(ses->flags, SES_FLAG_PATHMAPPING);

	script_driver(ses, LIST_PATH, root->list[root->used - 1]->right);

	SET_BIT(ses->flags, SES_FLAG_PATHMAPPING);

	delete_index_list(root, root->used - 1);

	root->update = root->used;


	show_message(ses, LIST_PATH, "#PATH MOVE: POSITION SET TO %d.", root->update);
}

void check_insert_path(char *command, struct session *ses)
{
	struct listroot *root = ses->list[LIST_PATH];
	struct listnode *node;

	if ((node = search_node_list(ses->list[LIST_PATHDIR], command)))
	{
		insert_node_list(root, node->left, node->right, "0");

		root->update = root->used;
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
		if (show_node_with_wild(ses, arg1, ses->list[LIST_PATHDIR]) == FALSE)
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
		else
		{
			if (!is_math(ses, arg3) || get_number(ses, arg3) < 0 || get_number(ses, arg3) >= 64)
			{
				show_message(ses, LIST_PATHDIR, "#PATHDIR: THE THIRD ARGUMENT MUST BE A NUMBER BETWEEN 0 and 63.");
				return ses;
			}
			get_number_string(ses, arg3, arg3);
		}
		update_node_list(ses->list[LIST_PATHDIR], arg1, arg2, arg3);

		show_message(ses, LIST_PATHDIR, "#OK: DIRECTION {%s} WILL BE REVERSED AS {%s} @ {%s}.", arg1, arg2, arg3);
	}
	return ses;
}

DO_COMMAND(do_unpathdir)
{
	char arg1[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);

	do
	{
		if (delete_nest_node(ses->list[LIST_PATHDIR], arg1))
		{
			show_message(ses, LIST_VARIABLE, "#OK. {%s} IS NO LONGER A PATHDIR.", arg1);
		}
		else
		{
			delete_node_with_wild(ses, LIST_VARIABLE, arg1);
		}
		arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);
	}
	while (*arg1);

	return ses;
}

// Old commands, left for backward compatibility

DO_PATH(path_new)
{
	struct listroot *root = ses->list[LIST_PATH];

	if (HAS_BIT(ses->flags, SES_FLAG_PATHMAPPING))
	{
		show_message(ses, LIST_PATH, "#PATH NEW: YOU ARE ALREADY MAPPING A PATH.");
	}
	else
	{
		root->update = 0;

		kill_list(root);

		show_message(ses, LIST_PATH, "#PATH NEW: YOU ARE NOW MAPPING A PATH.");

		SET_BIT(ses->flags, SES_FLAG_PATHMAPPING);
	}
}

DO_PATH(path_end)
{
	if (HAS_BIT(ses->flags, SES_FLAG_PATHMAPPING))
	{
		show_message(ses, LIST_PATH, "#PATH END: YOU ARE NO LONGER MAPPING A PATH.");

		DEL_BIT(ses->flags, SES_FLAG_PATHMAPPING);
	}
	else
	{
		show_message(ses, LIST_PATH, "#PATH: YOU ARE NOT MAPPING A PATH.");
	}
}
