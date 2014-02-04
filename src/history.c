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
	struct listroot *root;
	struct listnode *node;

	root = gts->list[LIST_HISTORY];

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	switch (left[0])
	{
		case 'd':
			deletenode_list(gts, root->l_node, LIST_HISTORY);
			break;

		case 'i':
			insertnode_list(gts, right, "", "", LIST_HISTORY);
			break;

		default:
			for (cnt = 0, node = root->f_node ; node ; node = node->next)
			{
				tintin_printf2(ses, "%5d - %s", cnt++, node->left);
			}
			break;
	}
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

int write_history(struct session *ses, char *filename)
{
	struct listnode *node;
	FILE *file;

	file = fopen(filename, "w");

	if (file == NULL)
	{
		return FALSE;
	}

	for (node = ses->list[LIST_HISTORY]->f_node ; node ; node = node->next)
	{
		fprintf(file, "%s\n", node->left);
	}

	fclose(file);

	return TRUE;
}

int read_history(struct session *ses, char *filename)
{
	FILE *file;
	char *cptr, buffer[BUFFER_SIZE];

	file = fopen(filename, "r");

	if (file == NULL)
	{
		return FALSE;
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

	return TRUE;
}
