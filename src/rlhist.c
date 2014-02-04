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

/* New for v2.0: readline support -- daw */

/*
	now searches history on ! for an abbreviation - Scandum
*/

#include "tintin.h"

/*
	custom history support - Scandum

char *add_line_history(struct session *ses, char *line)
{
	struct listroot *root;

	root = ses->list[LIST_HISTORY];

	if (root->l_node && !strcmp(line, root->l_node->left))
	{
		return line;
	}

	if (*line == 0)
	{
		if (root->l_node && HAS_BIT(gtd->ses->flags, SES_FLAG_REPEATENTER))
		{
			free(line);
			line = strdup(root->l_node->left);
		}
		return line;
	}

	if (*line == gtd->repeat_char)
	{
		return search_line_history(line);
	}

	insertnode_list(ses, line, "", "", LIST_HISTORY);

	while (root->count > gtd->history_size)
	{
		deletenode_list(ses, root->f_node, LIST_HISTORY);
	}

	return line;
}


char *search_line_history(struct session *ses, char *line)
{
	struct listroot *root;
	struct listnode *node;

	root = ses->list->[LIST_HISTORY];

	for (node = root->l_node; node ; node = node->prev)
	{
		if (is_abbrev(&line[1], node->left))
		{
			free(line);

			return strdup(node->left);
		}
	}
	tintin_printf2(ses, "#REPEAT: NO MATCH FOUND FOR '%s'", line);

	free(line);

	return NULL;
}

*/

char *search_history_line(char *line)
{
	int cnt;

	for (cnt = history_length - 1 ; cnt >= 0 ; cnt--)
	{
		if (is_abbrev(&line[1], (*(history_list() + cnt))->line))
		{
			free(line);

			return strdup((*(history_list() + cnt))->line);
		}
	}
	tintin_printf2(NULL, "#REPEAT: NO MATCH FOUND FOR '%s'", line);

	free(line);

	return NULL;
}

char *add_history_line(char *line)
{
	if (!strcmp(line, (*(history_list() + history_length - 1))->line))
	{
		return line;
	}

	add_history(line);

	return line;
}

char *rlhist_expand(char *line)
{
	push_call("rlhist_expand(%p)",line);

	if (history_max_entries == 0)
	{
		pop_call();
		return line;
	}

	if (history_length == 0)
	{
		add_history(line);
	}

	if (*line == 0)
	{
		if (HAS_BIT(gtd->ses->flags, SES_FLAG_REPEATENTER))
		{
			free(line);
			line = strdup((*(history_list() + history_length - 1))->line);
		}
		pop_call();
		return line;
	}

	if (history_length == 0)
	{
		add_history(line);
	}

	if (line[0] == gtd->repeat_char)
	{
		pop_call();
		return search_history_line(line);
	}

	pop_call();
	return add_history_line(line);
}
