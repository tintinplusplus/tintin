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
*                          coded by Bill Reiss 1993                           *
*                     recoded by Igor van den Hoven 2004                      *
******************************************************************************/


#include "tintin.h"


DO_COMMAND(do_highlight)
{
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE], arg3[BUFFER_SIZE], temp[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, arg1, 0, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, 1, SUB_VAR|SUB_FUN);
	arg = get_arg_in_braces(arg, arg3, 1);

	if (*arg3 == 0)
	{
		strcpy(arg3, "5");
	}

	if (*arg1 == 0)
	{
		show_list(ses->list[LIST_HIGHLIGHT], 0);
	}
	else if (*arg1 && *arg2 == 0)
	{
		if (show_node_with_wild(ses, arg1, LIST_HIGHLIGHT) == FALSE)
		{
			show_message(ses, LIST_HIGHLIGHT, "#HIGHLIGHT: NO MATCH(ES) FOUND FOR {%s}.", arg1);
		}
	}
	else
	{
		if (get_highlight_codes(ses, arg2, temp) == FALSE)
		{
			tintin_printf2(ses, "#HIGHLIGHT: VALID COLORS ARE:\n");
			tintin_printf2(ses, "reset, bold, light, faint, dim, dark, underscore, blink, reverse, black, red, green, yellow, blue, magenta, cyan, white, b black, b red, b green, b yellow, b blue, b magenta, b cyan, b white");
		}
		else
		{
			update_node_list(ses->list[LIST_HIGHLIGHT], arg1, arg2, arg3);

			show_message(ses, LIST_HIGHLIGHT, "#OK. {%s} NOW HIGHLIGHTS {%s} @ {%s}.", arg1, arg2, arg3);
		}
	}
	return ses;
}


DO_COMMAND(do_unhighlight)
{
	delete_node_with_wild(ses, LIST_HIGHLIGHT, arg);

	return ses;
}

void check_all_highlights(struct session *ses, char *original, char *line)
{
	struct listroot *root = ses->list[LIST_HIGHLIGHT];
	struct listnode *node;
	char *ptl, *ptm;
	char match[BUFFER_SIZE], color[BUFFER_SIZE], reset[BUFFER_SIZE], output[BUFFER_SIZE], plain[BUFFER_SIZE];

	for (root->update = 0 ; root->update < root->used ; root->update++)
	{
		if (check_one_regexp(ses, root->list[root->update], original, original, 0))
		{
			node = root->list[root->update];

			get_highlight_codes(ses, node->right, color);

			*output = *reset = 0;

			ptl = original;

			do
			{
				if (*gtd->vars[0] == 0)
				{
					break;
				}

				strcpy(match, gtd->vars[0]);

				strip_vt102_codes(match, plain);

				ptm = strstr(ptl, match);

				*ptm = 0;

				get_color_codes(reset, ptl, reset);

				cat_sprintf(output, "%s%s%s\033[0m%s", ptl, color, plain, reset);

				ptl = ptm + strlen(match);

				show_debug(ses, LIST_HIGHLIGHT, "#DEBUG HIGHLIGHT {%s} {%s}", node->left, match);
			}
			while (check_one_regexp(ses, node, ptl, ptl, 0));

			strcat(output, ptl);

			strcpy(original, output);
		}
	}
}

int get_highlight_codes(struct session *ses, char *string, char *result)
{
	int cnt;

	if (*string == '<')
	{
		substitute(ses, string, result, SUB_COL);

		return TRUE;
	}

	sprintf(result, "\033[0");

	while (*string)
	{
		if (isalpha((int) *string))
		{
			for (cnt = 0 ; *color_table[cnt].name ; cnt++)
			{
				if (is_abbrev(color_table[cnt].name, string))
				{
					strcat(result, ";");
					strcat(result, color_table[cnt].number);
					break;
				}
			}

			if (*color_table[cnt].name == 0)
			{
				return FALSE;
			}

			string += strlen(color_table[cnt].name);
		}
		else if (isdigit((int) *string))
		{
			strcat(result, ";");

			while (isdigit((int) *string))
			{
				sprintf(&result[strlen(result)], "%c", *string++);
			}
		}

		switch (*string)
		{
			case ' ':
			case ',':
				string++;
				break;

			case 0:
				break;

			default:
				return FALSE;
		}
	}
	strcat(result, "m");

	return TRUE;
}
