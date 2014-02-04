/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2006 (See CREDITS file)                                     *
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

DO_COMMAND(do_history)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int cnt;

	arg = get_arg_in_braces(arg, left,  FALSE);

	arg = get_arg_in_braces(arg, right, TRUE);
	substitute(ses, right, right, SUB_VAR|SUB_FUN);

	if (*left == 0)
	{
		tintin_header(ses, " HISTORY COMMANDS ");

		for (cnt = 0 ; *history_table[cnt].name != 0 ; cnt++)
		{
			tintin_printf2(ses, "  [%-13s] %s", history_table[cnt].name, history_table[cnt].desc);
		}
		tintin_header(ses, "");

		return ses;
	}

	for (cnt = 0 ; *history_table[cnt].name ; cnt++)
	{
		if (!is_abbrev(left, history_table[cnt].name))
		{
			continue;
		}

		history_table[cnt].fun(ses, right);

		return ses;
	}

	do_history(ses, "");

	return ses;
}


void add_line_history(struct session *ses, char *line)
{
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_HISTORY];

	if (root->l_node && !strcmp(line, root->l_node->left))
	{
		return;
	}

	if (*line == 0)
	{
		if (root->l_node && HAS_BIT(gtd->ses->flags, SES_FLAG_REPEATENTER))
		{
			strcpy(line, root->l_node->left);
		}
		return;
	}

	if (*line == gtd->repeat_char)
	{
		search_line_history(ses, line);
	}

	for (node = root->f_node ; node ; node = node->next)
	{
		if (!strcmp(line, node->left))
		{
			if (node == gtd->input_his)
			{
				gtd->input_his = NULL;
			}
			deletenode_list(ses, node, LIST_HISTORY);
			break;
		}
	}

	insertnode_list(ses, line, "", "", LIST_HISTORY);

	while (root->count > gtd->history_size)
	{
		if (root->f_node == gtd->input_his)
		{
			gtd->input_his = NULL;
		}
		deletenode_list(ses, root->f_node, LIST_HISTORY);
	}

	return;
}

void search_line_history(struct session *ses, char *line)
{
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_HISTORY];

	for (node = root->l_node; node ; node = node->prev)
	{
		if (is_abbrev(&line[1], node->left))
		{
			strcpy(line, node->left);

			return;
		}
	}
	tintin_printf2(ses, "#REPEAT: NO MATCH FOUND FOR '%s'", line);
}

DO_HISTORY(history_character)
{
	gtd->repeat_char = *arg;

	show_message(ses, LIST_HISTORY, "#HISTORY CHARACTER SET TO {%c}.", gtd->repeat_char);
}

DO_HISTORY(history_delete)
{
	if (gts->list[LIST_HISTORY]->l_node)
	{
		deletenode_list(gts, gts->list[LIST_HISTORY]->l_node, LIST_HISTORY);
	}

	return;
}

DO_HISTORY(history_insert)
{
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_HISTORY];

	if (root->l_node && !strcmp(arg, root->l_node->left))
	{
		return;
	}

	for (node = root->f_node ; node ; node = node->next)
	{
		if (!strcmp(arg, node->left))
		{
			if (node == gtd->input_his)
			{
				gtd->input_his = NULL;
			}
			deletenode_list(ses, node, LIST_HISTORY);
			break;
		}
	}

	insertnode_list(ses, arg, "", "", LIST_HISTORY);

	while (root->count > gtd->history_size)
	{
		if (root->f_node == gtd->input_his)
		{
			gtd->input_his = NULL;
		}
		deletenode_list(ses, root->f_node, LIST_HISTORY);
	}

	return;
}

DO_HISTORY(history_list)
{
	struct listroot *root;
	struct listnode *node;
	int cnt = 1;

	root = ses->list[LIST_HISTORY];

	for (node = root->f_node; node ; node = node->next)
	{
		tintin_printf2(ses, "%6d - %s", cnt++, node->left);
	}
	return;
}

DO_HISTORY(history_read)
{
	FILE *file;
	struct listroot *root;
	char *cptr, buffer[BUFFER_SIZE];

	root = ses->list[LIST_HISTORY];

	file = fopen(arg, "r");

	if (file == NULL)
	{
		tintin_printf2(ses, "#HISTORY: COULDN'T OPEN FILE {%s} TO READ.", arg);
		return;
	}

	while (root->f_node)
	{
		if (root->f_node == gtd->input_his)
		{
			gtd->input_his = NULL;
		}
		deletenode_list(ses, root->f_node, LIST_HISTORY);
	}

	while (fgets(buffer, BUFFER_SIZE-1, file))
	{
		cptr = strchr(buffer, '\n');

		if (cptr)
		{
			*cptr = 0;

			if (*buffer)
			{
				insertnode_list(ses, buffer, "", "", LIST_HISTORY);
			}
		}
	}
	insertnode_list(ses, "", "", "", LIST_HISTORY);

	fclose(file);

	return;
}

DO_HISTORY(history_size)
{
	if (atoi(arg) < 1 || atoi(arg) > 100000)
	{
		tintin_printf(ses, "#HISTORY SIZE: PROVIDE A NUMBER BETWEEN 1 and 100,000");
	}
	else
	{
		gtd->history_size = atoi(arg);
	}
	return;
}

DO_HISTORY(history_write)
{
	struct listnode *node;
	FILE *file;

	file = fopen(arg, "w");

	if (file == NULL)
	{
		tintin_printf2(ses, "#HISTORY: COULDN'T OPEN FILE {%s} TO WRITE.", arg);

		return;
	}

	for (node = ses->list[LIST_HISTORY]->f_node ; node ; node = node->next)
	{
		fprintf(file, "%s\n", node->left);
	}

	fclose(file);

	return;
}
