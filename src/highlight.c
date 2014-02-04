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


/*
	Now follows the standard tintin syntax because #read is not as flexible
	as it used to be - Igor
*/

DO_COMMAND(do_highlight)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], pr[BUFFER_SIZE], temp[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_HIGHLIGHT];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);
	arg = get_arg_in_braces(arg, pr,    TRUE);

	if (*pr == 0)
	{
		sprintf(pr, "%s", "5");
	}

	if (*left == 0)
	{
		show_list(ses, root, LIST_HIGHLIGHT);
	}
	else if (*left && *right == 0)
	{
		if (show_node_with_wild(ses, left, LIST_HIGHLIGHT) == FALSE)
		{
			show_message(ses, LIST_HIGHLIGHT, "#HIGHLIGHT: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		if (get_highlight_codes(ses, right, temp) == FALSE)
		{
			tintin_printf2(ses, "#HIGHLIGHT: VALID COLORS ARE:\r\n");
			tintin_printf2(ses, "reset, bold, faint, underscore, blink, reverse, dim, black, red, green, yellow, blue, magenta, cyan, white, b black, b red, b green, b yellow, b blue, b magenta, b cyan, b white");
		}
		else
		{
			updatenode_list(ses, left, right, pr, LIST_HIGHLIGHT);

			show_message(ses, LIST_HIGHLIGHT, "#OK. {%s} NOW HIGHLIGHTS {%s} @ {%s}", left, right, pr);
		}
	}
	return ses;
}


DO_COMMAND(do_unhighlight)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int found = FALSE;

	root = ses->list[LIST_HIGHLIGHT];

	arg = get_arg_in_braces(arg, left, TRUE);

	while ((node = search_node_with_wild(root, left)))
	{
		show_message(ses, LIST_HIGHLIGHT, "#OK. {%s} IS NO LONGER A HIGHLIGHT.", node->left);

		deletenode_list(ses, node, LIST_HIGHLIGHT);

		found = TRUE;
	}

	if (found == FALSE)
	{
		show_message(ses, LIST_HIGHLIGHT, "#HIGHLIGHT: NO MATCH(ES) FOUND FOR {%s}.", left);
	}
	return ses;
}


void check_all_highlights(struct session *ses, char *original, char *line)
{
	struct listnode *node;
	char *pt1, *pt2, *pt3;
	char buf1[BUFFER_SIZE], buf2[BUFFER_SIZE], buf3[BUFFER_SIZE], temp[BUFFER_SIZE];

	for (node = ses->list[LIST_HIGHLIGHT]->f_node ; node ; node = node->next)
	{
		if (check_one_action(line, original, node->left, ses))
		{
			substitute(ses, node->left, buf1, SUB_VAR|SUB_FUN|SUB_ARG|SUB_ANC|SUB_ESC);

			pt1 = strstr(original, buf1) ? original : line;
			pt2 = strstr(pt1, buf1);

			if (pt2 == NULL)
			{
				break;
			}
			*pt2 = 0;
			pt3 = pt2 + strlen(buf1);

			get_highlight_codes(ses, node->right, buf2);

			strip_non_vt102_codes(pt1, temp);
			strip_vt102_codes_non_graph(temp, buf3);

			sprintf(temp, "%s%s%s\033[0m%s%s", pt1, buf2, buf1, buf3, pt3);

			strcpy(original, temp);
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
		if (isalpha(*string))
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
		else if (isdigit(*string))
		{
			strcat(result, ";");

			while (isdigit(*string))
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
