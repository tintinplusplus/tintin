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
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE], arg3[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, arg1, 0, SUB_VAR|SUB_FUN);
	arg = get_arg_in_braces(ses, arg, arg2, 1);
	arg = get_arg_in_braces(ses, arg, arg3, 1);

	if (*arg3 == 0)
	{
		strcpy(arg3, "1");
	}

	if (*arg1 == 0)
	{
		show_list(ses->list[LIST_PROMPT], 0);
	}
	else if (*arg1 && *arg2 == 0)
	{
		if (show_node_with_wild(ses, arg1, LIST_PROMPT) == FALSE)
		{
			show_message(ses, LIST_PROMPT, "#PROMPT: NO MATCH(ES) FOUND FOR {%s}.", arg1);
		}
	}
	else
	{
		update_node_list(ses->list[LIST_PROMPT], arg1, arg2, arg3);

		show_message(ses, LIST_PROMPT, "#OK. {%s} NOW PROMPTS {%s} @ {%s}.", arg1, arg2, arg3);
	}
	return ses;
}


DO_COMMAND(do_unprompt)
{
	delete_node_with_wild(ses, LIST_PROMPT, arg);

	return ses;
}


void check_all_prompts(struct session *ses, char *original, char *line)
{
	struct listroot *root = ses->list[LIST_PROMPT];
	struct listnode *node;

	for (root->update = 0 ; root->update < root->used ; root->update++)
	{
		if (check_one_regexp(ses, root->list[root->update], line, original, 0))
		{
			node = root->list[root->update];

			if (*node->right)
			{
				substitute(ses, node->right, original, SUB_ARG);
				substitute(ses, original, original, SUB_VAR|SUB_FUN|SUB_COL|SUB_ESC);
			}

			show_debug(ses, LIST_PROMPT, "#DEBUG PROMPT {%s}", node->left);

			do_one_prompt(ses, original, atoi(node->pr));

			SET_BIT(ses->flags, SES_FLAG_GAG);
		}
	}
}

void do_one_prompt(struct session *ses, char *prompt, int row)
{
	char temp[BUFFER_SIZE];
	int original_row, len;

	if (ses != gtd->ses)
	{
		return;
	}

	original_row = row;

	if (row < 0)
	{
		row = -1 * row;
	}
	else
	{
		row = ses->rows - row;
	}

	if (row < 1 || row > ses->rows)
	{
		show_message(ses, LIST_PROMPT, "#ERROR: PROMPT ROW IS OUTSIDE THE SCREEN: {%s} {%d}.", prompt, original_row);

		return;
	}

	if (row > ses->top_row && row < ses->bot_row)
	{
		show_message(ses, LIST_PROMPT, "#ERROR: PROMPT ROW IS INSIDE THE SCROLLING REGION: {%s} {%d}.", prompt, original_row);

		return;
	}

	len = strip_vt102_strlen(ses, prompt);

	if (len == 0)
	{
		sprintf(temp, "%.*s", ses->cols + 4, "\033[0m----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	}
	else if (len <= ses->cols)
	{
		sprintf(temp, "%s", prompt);
	}
	else
	{
		sprintf(temp, "#PROMPT SIZE (%d) LONGER THAN ROW SIZE (%d)", len, ses->cols);
	}

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		save_pos(ses);
	}

	if (row == ses->rows)
	{
		gtd->input_off = len + 1;

		printf("\033[%d;1H\033[%d;1H\033[K%s%s\033[%d;%dH\0337\033[%d;1H", row, row, temp, gtd->input_buf, row, gtd->input_off + gtd->input_cur, ses->bot_row);
	}
	else
	{
		// goto row, erase to eol, print prompt, goto bot_row

		printf("\033[%d;1H\033[%d;1H\033[K%s\033[%d;1H", row, row, temp, ses->bot_row);
	}

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		restore_pos(ses);
	}
}
