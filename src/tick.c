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
*   file: tick.c - funtions related to the tick command                       *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                        coded by peter unold 1992                            *
*                   recoded by Igor van den Hoven 2004                        *
******************************************************************************/

#include "tintin.h"


DO_COMMAND(do_tick)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], pr[BUFFER_SIZE], temp[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_TICKER];

	arg = get_arg_in_braces(arg, left,  FALSE);

	arg = get_arg_in_braces(arg, right, TRUE);

	arg = get_arg_in_braces(arg, temp,  TRUE);
	substitute(ses, temp, pr, SUB_VAR|SUB_FUN);

	if (!*pr)
	{
		sprintf(pr, "%s", "60"); 
	}

	if (!*left)
	{
		show_list(ses, root, LIST_TICKER);
	}
	else if (*left && *right == 0)
	{
		if (show_node_with_wild(ses, root, left, PRIORITY) == FALSE) 
		{
			tintin_printf2(ses, "#TICK, NO MATCH(ES) found for {%s}", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, pr, LIST_TICKER);

		if (show_message(ses, LIST_TICKER))
		{
			tintin_printf2(ses, "#OK {%s} NOW EXECUTES {%s} EVERY {%s} SECONDS.", left, right, pr);
		}
	}
	return ses;
}


DO_COMMAND(do_untick)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int flag = FALSE;

	root = ses->list[LIST_TICKER];

	arg = get_arg_in_braces(arg, left, 1);

	while ((node = search_node_with_wild(root, left))) 
	{
		if (show_message(ses, LIST_TICKER))
		{
			tintin_printf2(ses, "#OK {%s} IS NO LONGER A TICKER.", node->left);
		}
		deletenode_list(ses, node, LIST_TICKER);

		flag = TRUE;
	}

	if (!flag && show_message(ses, LIST_TICKER)) 
	{
		tintin_printf(ses, "#NO MATCH(ES) FOUND FOR {%s}", left);
	}
	return ses;
}


DO_COMMAND(do_delay)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_DELAY];

	arg = get_arg_in_braces(arg, temp,  FALSE);
	substitute(ses, temp, left, SUB_VAR|SUB_FUN);

	arg = get_arg_in_braces(arg, right, TRUE);

	sprintf(temp, "%lld", utime());

	if (!*right)
	{
		show_list(ses, root, LIST_DELAY);
	}
	else if (*left && *right == 0)
	{
		tintin_puts2("#SYNTAX: #DELAY {seconds} {command}", ses);
	}
	else
	{
		updatenode_list(ses, temp, right, left, LIST_DELAY);

		if (show_message(ses, LIST_TICKER))
		{
			tintin_printf2(ses, "#OK, IN {%s} SECONDS {%s} IS EXECUTED.", left, right);
		}
	}
	return ses;
}


void tick_update(void)
{
	char result[BUFFER_SIZE];
	struct session *ses;
	struct listnode *node;
	struct listroot *root;

	push_call("tick_update(void)");

	for (ses = gts->next ; ses ; ses = gtd->update)
	{
		gtd->update = ses->next;

		root = ses->list[LIST_TICKER];

		for (node = root->f_node ; node ; node = root->update)
		{
			root->update = node->next;

			if (node->data == 0)
			{
				node->data = gtd->time + atof(node->pr) * 1000000L;
			}

			if (node->data <= gtd->time)
			{
				node->data += atof(node->pr) * 1000000LL;

				strcpy(result, node->right);

				parse_input(result, ses);
			}
		}

		root = ses->list[LIST_DELAY];	

		for (node = root->f_node ; node ; node = root->update)
		{
			root->update = node->next;

			if (node->data == 0)
			{
				node->data = gtd->time + atof(node->pr) * 1000000L;
			}

			if (node->data <= gtd->time)
			{
				strcpy(result, node->right);

				parse_input(result, ses);

				deletenode_list(ses, node, LIST_DELAY);
			}
		}

		if (ses->check_output && gtd->time > ses->check_output)
		{
			if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
			{
				save_pos(ses);
				goto_rowcol(ses, ses->bot_row, 1);
			}

			process_mud_output(ses, ses->more_output, TRUE);

			if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
			{
				restore_pos(ses);
			}
		}
	}

	fflush(stdout);

	pop_call();
	return;
}
