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
	char left[BUFFER_SIZE], right[BUFFER_SIZE], rank[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_ALIAS];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);
	arg = get_arg_in_braces(arg, rank,  FALSE);

	if (*rank == 0)
	{
		strcpy(rank, "5");
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

		show_message(ses, LIST_ALIAS, "#OK. {%s} NOW ALIASES {%s} @ {%s}.", left, right, rank);
	}
	return ses;
}


DO_COMMAND(do_unalias)
{
	delete_node_with_wild(ses, LIST_ALIAS, arg);

	return ses;
}

int alias_glob(char *str, char *exp)
{
	short cnt, arg;

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
				if (isdigit(exp[1]))
				{
					arg = isdigit(exp[2]) ? (exp[1] - '0') * 10 + exp[2] - '0' : exp[1] - '0';

					if (exp[isdigit(exp[2]) ? 3 : 2] == 0)
					{
						RESTRING(gtd->vars[arg], str);

						return TRUE;
					}

					for (cnt = 0 ; str[cnt] ; cnt++)
					{
						if (alias_glob(&str[cnt], exp + (isdigit(exp[2]) ? 3 : 2)))
						{
							gtd->vars[arg] = refstring(gtd->vars[arg], "%.*s", cnt, str);

							return TRUE;
						}
					}
					return FALSE;
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

int check_all_aliases(struct session *ses, char *input)
{
	struct listnode *node;
	struct listroot *root;
	char left[BUFFER_SIZE], right[BUFFER_SIZE], *arg;
	int i;

	root = ses->list[LIST_ALIAS];

	substitute(ses, input, left, SUB_VAR|SUB_FUN);

	if (HAS_BIT(ses->flags, SES_FLAG_REGEXP))
	{
		for (node = root->update = root->f_node ; node ; node = root->update)
		{
			root->update = node->next;

			substitute(ses, node->left, right, SUB_VAR|SUB_FUN);

			if (tintin_regexp(left, right))
			{
				substitute(ses, node->right, input, SUB_ARG);

				show_debug(ses, LIST_ALIAS, "#DEBUG ALIAS {%s}", node->left);

				DEL_BIT(gtd->flags, TINTIN_FLAG_SHOWMESSAGE);

				return TRUE;
			}
		}
	}
	else
	{
		arg = get_arg_in_braces(left, right, FALSE);

		RESTRING(gtd->vars[0], arg);

		for (i = 1 ; i < 100 ; i++)
		{
			arg = get_arg_in_braces(arg, right, FALSE);

			RESTRING(gtd->vars[i], right);
		}

		for (node = root->update = root->f_node ; node ; node = root->update)
		{
			root->update = node->next;

			substitute(ses, node->left, right, SUB_VAR|SUB_FUN);

			if (alias_glob(left, right))
			{
				substitute(ses, node->right, right, SUB_ARG);

				if (!strcmp(node->right, right) && *gtd->vars[0])
				{
					sprintf(input, "%s %s", right, gtd->vars[0]);
				}
				else
				{
					sprintf(input, "%s", right);
				}

				show_debug(ses, LIST_ALIAS, "#DEBUG ALIAS {%s}", node->left);

				DEL_BIT(gtd->flags, TINTIN_FLAG_SHOWMESSAGE);

				return TRUE;
			}
		}
	}
	return FALSE;
}
