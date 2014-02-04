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
*               (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                  *
*                                                                             *
*                        coded by Peter Unold 1992                            *
******************************************************************************/

#include "tintin.h"


DO_COMMAND(do_alias)
{
	char *left, *right, *rank;
	struct listroot *root;

	root = ses->list[LIST_ALIAS];

	arg = get_arg_in_braces(arg, &left,  FALSE);
	arg = get_arg_in_braces(arg, &right, TRUE);
	arg = get_arg_in_braces(arg, &rank,  FALSE);

	if (*rank == 0)
	{
		rank = string_alloc("5");
	}

	if (*left == 0)
	{
		show_list(ses, root, LIST_ALIAS);
	}
	else if (*right == 0)
	{
		if (show_node_with_wild(ses, left, LIST_ALIAS) == FALSE)
		{
			show_message(ses, LIST_ALIAS, "#ALIAS: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		updatenode_list(ses, left, right, rank, LIST_ALIAS);

		show_message(ses, LIST_ALIAS, "#OK. {%s} NOW ALIASSES {%s} @ {%s}.", left, right, rank);
	}
	return ses;
}


DO_COMMAND(do_unalias)
{
	delete_node_with_wild(ses, LIST_ALIAS, arg);

	return ses;
}

int alias_regexp(char *exp, char *str)
{
	short cnt;

	while (*exp)
	{
#ifdef BIG5
		if (*exp & 0x80 && *str & 0x80)
		{
			exp++;
			str++;
			if (*exp++ != *str++)
			{
				return FALSE;
			}
			continue;
		}
#endif

		switch (exp[0])
		{
			case '%':
				if (exp[1] >= '0' && exp[1] <= '9')
				{
					if (exp[2] == 0)
					{
						gtd->vars[exp[1] - '0'] = string_realloc(gtd->vars[exp[1] - '0'], str);

						return TRUE;
					}

					for (cnt = 0 ; cnt < 1000 ; cnt++)
					{
						if (alias_regexp(exp + 2, &str[cnt]))
						{
							gtd->vars[exp[1] - '0'] = string_realloc(gtd->vars[exp[1] - '0'], str);
							gtd->vars[exp[1] - '0'][cnt] = 0;

							return TRUE;
						}
						if (str[cnt] == 0)
						{
							return FALSE;
						}
					}
				}
				break;

			case '^':
				if (exp[1] == 0)
				{
					return (exp[1] == str[0]);
				}
				break;
		}

		if (*exp != *str)
		{
			return FALSE;
		}
		exp++;
		str++;
	}

	if (*str == ' ' || *str == 0)
	{
		return TRUE;
	}
	return FALSE;
}

int check_all_aliases(struct session *ses, char *str1, char *str2, char **command)
{
	struct listnode *node;
	struct listroot *root;
	char *arg;
	int i;

	root = ses->list[LIST_ALIAS];

	gtd->vars[0] = string_alloc(str2);

	arg = str2;

	for (i = 1 ; i < 10 ; i++)
	{
		arg = get_arg_in_braces(arg, &gtd->vars[i], FALSE);
	}

	arg = stringf_alloc("%s%s%s", str1, *str2 ? " " : "", str2);

	for (node = root->update = root->f_node ; node ; node = root->update)
	{
		root->update = node->next;

		if (alias_regexp(node->left, arg))
		{
			substitute(ses, node->right, command, SUB_ARG);

			if (!strcmp(node->right, *command) && *str2)
			{
				*command = stringf_alloc("%s %s", *command, str2);
			}

			show_debug(ses, LIST_ALIAS, "#ALIAS DEBUG: %s : %s", str1, str2);

			return TRUE;
		}
	}
	return FALSE;
}
