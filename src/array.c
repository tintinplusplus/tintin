/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2005 (See CREDITS file)                                     *
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
*   file: class.c - funtions related to the scroll back buffer                *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                     coded by Igor van den Hoven 2004                        *
******************************************************************************/


#include "tintin.h"


DO_COMMAND(do_list)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int cnt;

	root = ses->list[LIST_VARIABLE];

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);

	if (*left == 0 || *right == 0)
	{
		tintin_printf(ses, "#SYNTAX: #LIST {listname} {INS|DEL|GET|SET|LEN} {argument}");
	}
	else
	{
		for (cnt = 0 ; cnt < ARRAY_MAX ; cnt++)
		{
			if (is_abbrev(right, array_table[cnt].name))
			{
				break;
			}
		}

		if (cnt == ARRAY_MAX)
		{
			tintin_printf2(ses, "#SYNTAX: #LIST {%s} {INS|DEL|GET|SET|LEN} {argument}", left);
		}
		else
		{
			if ((node = search_node_with_wild(root, left)) == NULL)
			{
				updatenode_list(ses, left, "", "0", LIST_VARIABLE);

				node = search_node_with_wild(root, left);
			}
			array_table[cnt].array(ses, node, arg);
		}
	}
	return ses;
}

int get_list_length(struct listnode *node)
{
	const char *arg;
	char temp[BUFFER_SIZE];
	int cnt = 0;

	push_call("get_list_length(%p)",node);

	for (arg = node->right ; *arg ; arg = get_arg_in_braces(arg, temp, FALSE))
	{
		cnt++;
	}
	pop_call();
	return cnt;
}

int get_list_index(struct listnode *node, char *arg)
{
	int index, size;

	push_call("get_list_index(%p,%p)",node,arg);

	index = atoi(arg);
	size  = get_list_length(node);

	if (size == 0 && (index == -1 || index == 1))
	{
		pop_call();
		return 0;
	}

	if (index < 0)
	{
		index = size + 1 + index;
	}

	if (size == 0 || index < 1 || index > size)
	{
		pop_call();
		return -1;
	}
	pop_call();
	return index;
}


DO_ARRAY(array_del)
{
	char left[BUFFER_SIZE], buf[BUFFER_SIZE], temp[BUFFER_SIZE];
	int cnt, index;

	arg = get_arg_in_braces(arg, left, FALSE);

	index = get_list_index(list, left);

	if (index <= 0)
	{
		tintin_printf(ses, "#LIST DEL: Invalid index: %s", left);

		return ses;
	}

	buf[0] = 0;
	arg = list->right;

	for (cnt = 1 ; cnt < index ; cnt++)
	{
		arg = get_arg_in_braces(arg, temp, FALSE);

		cat_sprintf(buf, "%s{%s}", buf[0] ? " " : "", temp);
	}

	arg = get_arg_in_braces(arg, temp, FALSE);

	while (*arg)
	{
		arg = get_arg_in_braces(arg, temp, FALSE);

		cat_sprintf(buf, "%s{%s}", buf[0] ? " " : "", temp);
	}

	updatenode_list(ses, list->left, buf, "0", LIST_VARIABLE);

	return ses;
}


DO_ARRAY(array_get)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], buf[BUFFER_SIZE], temp[BUFFER_SIZE];
	int cnt, index;

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);

	index = get_list_index(list, left);

	if (index <= 0)
	{
		tintin_printf(ses, "#LIST GET: Invalid index: %s", left);

		return ses;
	}

	buf[0] = 0;
	arg = list->right;

	for (cnt = 1 ; cnt <= index ; cnt++)
	{
		arg = get_arg_in_braces(arg, temp, FALSE);
	}

	sprintf(buf, "{%s} {%s}", right, temp);

	do_internal_variable(ses, buf);

	return ses;
}


DO_ARRAY(array_ins)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], buf[BUFFER_SIZE], temp[BUFFER_SIZE];
	int cnt, index;

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	index = get_list_index(list, left);

	if (index == -1)
	{
		tintin_printf(ses, "#LIST INS: Invalid index: %s", left);

		return ses;
	}

	if (index == 0)
	{
		sprintf(buf, "{%s}", right);
		updatenode_list(ses, list->left, buf, "0", LIST_VARIABLE);

		return ses;
	}

	if (atoi(left) < 0)
	{
		index++;
	}

	buf[0] = 0;
	arg = list->right;

	for (cnt = 1 ; cnt < index ; cnt++)
	{
		arg = get_arg_in_braces(arg, temp, FALSE);

		cat_sprintf(buf, "{%s} ", temp);
	}

	cat_sprintf(buf, "{%s}", right);

	while (*arg)
	{
		arg = get_arg_in_braces(arg, temp, FALSE);

		cat_sprintf(buf, " {%s}", temp);
	}

	updatenode_list(ses, list->left, buf, "0", LIST_VARIABLE);

	return ses;
}

DO_ARRAY(array_len)
{
	char left[BUFFER_SIZE], buf[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left, FALSE);

	sprintf(buf, "{%s} {%d}", left, get_list_length(list));

	do_internal_variable(ses, buf);

	return ses;
}


DO_ARRAY(array_set)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], buf[BUFFER_SIZE], temp[BUFFER_SIZE];
	int cnt, index;

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	index = get_list_index(list, left);

	if (index <= 0)
	{
		tintin_printf(ses, "#LIST DEL: Invalid index: %s", left);

		return ses;
	}

	buf[0] = 0;
	arg = list->right;

	for (cnt = 1 ; cnt < index ; cnt++)
	{
		arg = get_arg_in_braces(arg, temp, FALSE);

		cat_sprintf(buf, "%s{%s}", buf[0] ? " " : "", temp);
	}

	arg = get_arg_in_braces(arg, temp, FALSE);

	cat_sprintf(buf, "%s{%s}", buf[0] ? " " : "", right);

	while (*arg)
	{
		arg = get_arg_in_braces(arg, temp, FALSE);

		cat_sprintf(buf, "%s{%s}", buf[0] ? " " : "", temp);
	}

	updatenode_list(ses, list->left, buf, "0", LIST_VARIABLE);

	return ses;
}
