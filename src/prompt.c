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
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2004                       *
******************************************************************************/

#include "tintin.h"


DO_COMMAND(do_prompt)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], pr[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_PROMPT];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);
	arg = get_arg_in_braces(arg, pr,    TRUE);

	if (!*pr)
	{
		sprintf(pr, "%d", 1);
	}

	if (*left == 0)
	{
		show_list(ses, root, LIST_PROMPT);
	}
	else if (*left && !*right)
	{
		if (show_node_with_wild(ses, left, LIST_PROMPT) == FALSE)
		{
			show_message(ses, LIST_PROMPT, "#PROMPT: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, pr, LIST_PROMPT);

		show_message(ses, LIST_PROMPT, "#OK. {%s} NOW PROMPTS {%s}.", left, right);
	}
	return ses;
}


DO_COMMAND(do_unprompt)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int found = FALSE;

	root = ses->list[LIST_PROMPT];

	arg = get_arg_in_braces(arg, left, 1);

	while ((node = search_node_with_wild(root, left)))
	{
		show_message(ses, LIST_PROMPT, "#OK. {%s} IS NO LONGER A PROMPT.", node->left);

		deletenode_list(ses, node, LIST_PROMPT);

		found = TRUE;
	}
	if (found == FALSE)
	{
		show_message(ses, LIST_PROMPT, "#UNPROMPT: NO MATCH(ES) FOUND FOR.", ses);
	}
	return ses;
}


void check_all_prompts(struct session *ses, char *original, char *line)
{
	struct listnode *node;

	for (node = ses->list[LIST_PROMPT]->f_node ; node ; node = node->next)
	{
		if (check_one_action(line, original, node->left, ses))
		{
			if (*node->right)
			{
				substitute(ses, node->right, original, SUB_ARG|SUB_VAR|SUB_FUN|SUB_COL);
			}

			show_debug(ses, LIST_PROMPT, "#PROMPT DEBUG: %s", original);

			do_one_prompt(ses, original, atoi(node->pr));

			SET_BIT(ses->flags, SES_FLAG_GAG);
		}
	}
}

void do_one_prompt(struct session *ses, char *prompt, int row)
{
	char temp[BUFFER_SIZE];

	if (ses != gtd->ses)
	{
		return;
	}

	if (row < 0)
	{
		row = -1 * row;
	}
	else
	{
		row = ses->rows - row;
	}

	if (row <= ses->top_row && row >= ses->bot_row)
	{
		show_message(ses, LIST_PROMPT, "#ERROR: INVALID PROMPT ROW: {%s} {%d}", prompt, row);

		return;
	}
	strip_vt102_codes(prompt, temp);

	if (strlen(temp) == 0)
	{
		sprintf(temp, "%.*s", ses->cols + 4, "\033[0m----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	}
	else if ((int) strlen(temp) <= ses->cols)
	{
		sprintf(temp, "%s", prompt);
	}
	else
	{
		sprintf(temp, "#PROMPT SIZE (%d) LONGER THAN ROW SIZE (%d)", (int) strlen(temp), ses->cols);
	}

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		save_pos(ses);
	}

	/*
		goto row, erase to eol, print prompt, goto bot_row
	*/

	printf("\033[%d;1H\033[K%s\033[%d;1H", row, temp, ses->bot_row);

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		restore_pos(ses);
	}
}
