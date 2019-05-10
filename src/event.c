/*****************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2007 (See CREDITS file)                                     *
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
*                      coded by Igor van den Hoven 2007                       *
******************************************************************************/


#include "tintin.h"

DO_COMMAND(do_event)
{
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE];
	int cnt;

	arg = sub_arg_in_braces(ses, arg, arg1, 0, SUB_VAR|SUB_FUN);
	arg = get_arg_in_braces(ses, arg, arg2, 1);

	if (*arg1 == 0)
	{
		tintin_header(ses, " EVENTS ");

		for (cnt = 0 ; *event_table[cnt].name != 0 ; cnt++)
		{
			tintin_printf2(ses, "  [%-20s] [%s] %s", event_table[cnt].name, search_node_list(ses->list[LIST_EVENT], event_table[cnt].name) ? "X" : " ", event_table[cnt].desc);
		}
		tintin_header(ses, "");
	}
	else if (*arg2 == 0)
	{
		if (show_node_with_wild(ses, arg1, ses->list[LIST_EVENT]) == FALSE)
		{
			show_message(ses, LIST_ALIAS, "#EVENT: NO MATCH(ES) FOUND FOR {%s}.", arg1);
		}
	}
	else
	{
		for (cnt = 0 ; *event_table[cnt].name != 0 ; cnt++)
		{
			if (!strncmp(event_table[cnt].name, arg1, strlen(event_table[cnt].name)))
			{
				show_message(ses, LIST_EVENT, "#EVENT {%s} HAS BEEN SET TO {%s}.", arg1, arg2);

				update_node_list(ses->list[LIST_EVENT], arg1, arg2, "");

				return ses;
			}
		}
		show_error(ses, LIST_EVENT, "#EVENT {%s} IS NOT AN EXISTING EVENT.", arg1);
	}
	return ses;
}

DO_COMMAND(do_unevent)
{
	delete_node_with_wild(ses, LIST_EVENT, arg);

	return ses;
}

int check_all_events(struct session *ses, int flags, int args, int vars, char *fmt, ...)
{
	struct session *ses_ptr;
	struct listnode *node;
	char name[BUFFER_SIZE], buf[BUFFER_SIZE];
	va_list list;
	int cnt;

	va_start(list, fmt);

	vsprintf(name, fmt, list);

	va_end(list); 

	push_call("check_all_events(%p,%d,%d,%d,%s, ...)",ses,flags,args,vars,name);

	for (ses_ptr = ses ? ses : gts ; ses_ptr ; ses_ptr = ses_ptr->next)
	{
		if (!HAS_BIT(ses_ptr->list[LIST_EVENT]->flags, LIST_FLAG_IGNORE))
		{
			show_info(ses_ptr, LIST_EVENT, "#INFO EVENT {%s}", name);

			node = search_node_list(ses_ptr->list[LIST_EVENT], name);

			if (node)
			{
				va_start(list, fmt);

				for (cnt = 0 ; cnt < args ; cnt++)
				{
					va_arg(list, char *);
				}

				for (cnt = 0 ; cnt < vars ; cnt++)
				{
					RESTRING(gtd->vars[cnt], va_arg(list, char *));
				}

				substitute(ses_ptr, node->right, buf, flags);

				if (HAS_BIT(ses_ptr->list[LIST_EVENT]->flags, LIST_FLAG_DEBUG))
				{
					show_debug(ses_ptr, LIST_ACTION, "#DEBUG EVENT {%s}", node->left);
				}

				script_driver(ses_ptr, LIST_EVENT, buf);

				va_end(list);

				if (ses)
				{
					pop_call();
					return 1;
				}
			}
		}

		if (ses)
		{
			pop_call();
			return 0;
		}
	}
	pop_call();
	return 0;
}

void mouse_handler(struct session *ses, int flags, int x, int y, char type)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	static char last[100];
	static long long click[3];

	if (HAS_BIT(flags, MOUSE_FLAG_MOTION))
	{
		strcpy(left, "MOVED");
	}
	else
	{
		switch (type)
		{
			case 'M':
				if (HAS_BIT(flags, MOUSE_FLAG_WHEEL))
				{
					strcpy(left, "SCROLLED");
				}
				else
				{
					strcpy(left, "PRESSED");
				}
				break;
			case 'm':
				strcpy(left, "RELEASED");
				break;
			default:
				strcpy(left, "UNKNOWN");
				break;
		}
	}

	right[0] = 0;

	if (HAS_BIT(flags, MOUSE_FLAG_CTRL))
	{
		strcat(right, "CTRL ");
	}
	if (HAS_BIT(flags, MOUSE_FLAG_ALT))
	{
		strcat(right, "ALT ");
	}
	if (HAS_BIT(flags, MOUSE_FLAG_SHIFT))
	{
		strcat(right, "SHIFT ");
	}

	if (HAS_BIT(flags, MOUSE_FLAG_EXTRA))
	{
		strcat(right, "EXTRA ");
	}

	if (HAS_BIT(flags, MOUSE_FLAG_UNKNOWN))
	{
		strcat(right, "256 ");
	}

	if (HAS_BIT(flags, MOUSE_FLAG_WHEEL))
	{
		strcat(right, "MOUSE WHEEL ");
	}
	else
	{
		strcat(right, "MOUSE BUTTON ");
	}

	if (HAS_BIT(flags, MOUSE_FLAG_WHEEL))
	{
		if (HAS_BIT(flags, MOUSE_FLAG_BUTTON_A) && HAS_BIT(flags, MOUSE_FLAG_BUTTON_B))
		{
			strcat(right, "FOUR");
		}
		else if (HAS_BIT(flags, MOUSE_FLAG_BUTTON_B))
		{
			strcat(right, "THREE");
		}
		else if (HAS_BIT(flags, MOUSE_FLAG_BUTTON_A))
		{
			strcat(right, "DOWN");
		}
		else
		{
			strcat(right, "UP");
		}
	}
	else
	{
		if (HAS_BIT(flags, MOUSE_FLAG_BUTTON_A) && HAS_BIT(flags, MOUSE_FLAG_BUTTON_B))
		{
			strcat(right, "FOUR");
		}
		else if (HAS_BIT(flags, MOUSE_FLAG_BUTTON_B))
		{
			strcat(right, "THREE");
		}
		else if (HAS_BIT(flags, MOUSE_FLAG_BUTTON_A))
		{
			strcat(right, "TWO");
		}
		else
		{
			strcat(right, "ONE");
		}
	}

	check_all_events(ses, SUB_ARG, 2, 2, "%s %s", left, right, ntos(x), ntos(ses->rows - y));

	check_all_events(ses, SUB_ARG, 3, 2, "%s %s %d", left, right, 0 - y, ntos(x), ntos(ses->rows - y));

	check_all_events(ses, SUB_ARG, 3, 2, "%s %s %d", left, right, ses->rows - y, ntos(x), ntos(ses->rows - y));

	if (!strcmp(left, "PRESSED"))
	{
		sprintf(left, "PRESSED %s %d %d", right, x, y);

		if (!strcmp(left, last))
		{
			click[2] = click[1];
			click[1] = click[0];
			click[0] = utime();

			if (click[0] - click[1] < 500000)
			{
				if (click[0] - click[2] < 500000)
				{
					check_all_events(ses, SUB_ARG, 1, 2, "TRIPLE-CLICKED %s", right, ntos(x), ntos(ses->rows - y));

					check_all_events(ses, SUB_ARG, 2, 2, "TRIPLE-CLICKED %s %d", right, 0 - y, ntos(x), ntos(ses->rows - y));

					check_all_events(ses, SUB_ARG, 2, 2, "TRIPLE-CLICKED %s %d", right, ses->rows - y, ntos(x), ntos(ses->rows - y));

					click[0] = 0;
				}
				else
				{
					check_all_events(ses, SUB_ARG, 1, 2, "DOUBLE-CLICKED %s", right, ntos(x), ntos(ses->rows - y));

					check_all_events(ses, SUB_ARG, 2, 2, "DOUBLE-CLICKED %s %d", right, 0 - y, ntos(x), ntos(ses->rows - y));

					check_all_events(ses, SUB_ARG, 2, 2, "DOUBLE-CLICKED %s %d", right, ses->rows - y, ntos(x), ntos(ses->rows - y));
				}
			}
		}
		else
		{
			click[2] = 0;
			click[1] = 0;
			click[0] = utime();

			sprintf(last, "PRESSED %s %d %d", right, x, y);
		}
	}
	return;
}
