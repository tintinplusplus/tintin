/******************************************************************************
*   This file is part of TinTin++                                             *
*                                                                             *
*   Copyright 2004-2019 Igor van den Hoven                                    *
*                                                                             *
*   TinTin++ is free software; you can redistribute it and/or modify          *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 3 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with TinTin++.  If not, see https://www.gnu.org/licenses.           *
******************************************************************************/

/******************************************************************************
*              (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                   *
*                                                                             *
*                     coded by Igor van den Hoven 2004                        *
******************************************************************************/


#include "tintin.h"


DO_COMMAND(do_class)
{
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE], arg3[BUFFER_SIZE];
	int i;

	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_CLASS];

	arg = sub_arg_in_braces(ses, arg, arg1, 0, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg2, 0, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, arg3, 0, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
	{
		tintin_header(ses, " CLASSES ");

		for (root->update = 0 ; root->update < root->used ; root->update++)
		{
			node = root->list[root->update];

			tintin_printf2(ses, "%-20s  %4d  %6s  %6s  %3d", node->left, count_class(ses, node), !strcmp(ses->group, node->left) ? "ACTIVE" : "", atoi(node->pr) ? "OPEN" : "CLOSED", atoi(node->pr));
		}
	}
	else if (*arg2 == 0)
	{
		class_list(ses, arg1, arg2);
	}
	else
	{
		for (i = 0 ; *class_table[i].name ; i++)
		{
			if (is_abbrev(arg2, class_table[i].name))
			{
				break;
			}
		}

		if (*class_table[i].name == 0)
		{
			show_error(ses, LIST_COMMAND, "#SYNTAX: CLASS {name} {OPEN|CLOSE|READ|SIZE|WRITE|KILL}.", arg1, capitalize(arg2));
		}
		else
		{
			if (!search_node_list(ses->list[LIST_CLASS], arg1))
			{
				update_node_list(ses->list[LIST_CLASS], arg1, arg2, arg3);
			}
			class_table[i].group(ses, arg1, arg3);
		}
	}
	return ses;
}


int count_class(struct session *ses, struct listnode *group)
{
	int list, cnt, index;

	for (cnt = list = 0 ; list < LIST_MAX ; list++)
	{
		if (!HAS_BIT(ses->list[list]->flags, LIST_FLAG_CLASS))
		{
			continue;
		}

		for (index = 0 ; index < ses->list[list]->used ; index++)
		{
			if (!strcmp(ses->list[list]->list[index]->group, group->left))
			{
				cnt++;
			}
		}
	}
	return cnt;
}


DO_CLASS(class_open)
{
	int count;

	if (!strcmp(ses->group, left))
	{
		show_message(ses, LIST_CLASS, "#CLASS {%s} IS ALREADY OPENED AND ACTIVATED.", left);
	}
	else
	{
		if (*ses->group)
		{
			check_all_events(ses, SUB_ARG, 0, 1, "CLASS DEACTIVATED", ses->group);
		}
		RESTRING(ses->group, left);

		count = atoi(ses->list[LIST_CLASS]->list[0]->pr);

		update_node_list(ses->list[LIST_CLASS], left, "", ntos(--count));

		show_message(ses, LIST_CLASS, "#CLASS {%s} HAS BEEN OPENED AND ACTIVATED.", left);

		check_all_events(ses, SUB_ARG, 0, 1, "CLASS ACTIVATED", left);
	}

	return ses;
}

DO_CLASS(class_close)
{
	struct listnode *node;

	node = search_node_list(ses->list[LIST_CLASS], left);

	if (node == NULL)
	{
		show_message(ses, LIST_CLASS, "#CLASS {%s} DOES NOT EXIST.", left);
	}
	else
	{
		if (atoi(node->pr) == 0)
		{
			show_message(ses, LIST_CLASS, "#CLASS {%s} IS ALREADY CLOSED.", left);
		}
		else
		{
			show_message(ses, LIST_CLASS, "#CLASS {%s} HAS BEEN CLOSED.", left);

			update_node_list(ses->list[LIST_CLASS], left, "", "0");

			if (!strcmp(ses->group, left))
			{
				check_all_events(ses, SUB_ARG, 0, 1, "CLASS DEACTIVATED", ses->group);

				node = ses->list[LIST_CLASS]->list[0];

				if (atoi(node->pr))
				{
					RESTRING(ses->group, node->left);

					show_message(ses, LIST_CLASS, "#CLASS {%s} HAS BEEN ACTIVATED.", node->left);

					check_all_events(ses, SUB_ARG, 0, 1, "CLASS ACTIVATED", node->left);
				}
				else
				{
					RESTRING(ses->group, "");
				}
			}
		}
	}
	return ses;
}

DO_CLASS(class_list)
{
	int i, j;

	if (search_node_list(ses->list[LIST_CLASS], left))
	{
		tintin_header(ses, " %s ", left);

		for (i = 0 ; i < LIST_MAX ; i++)
		{
			if (!HAS_BIT(ses->list[i]->flags, LIST_FLAG_CLASS))
			{
				continue;
			}

			if (*right && !is_abbrev(right, list_table[i].name) && !is_abbrev(right, list_table[i].name_multi))
			{
				continue;
			}

			for (j = 0 ; j < ses->list[i]->used ; j++)
			{
				if (!strcmp(ses->list[i]->list[j]->group, left))
				{
					show_node(ses->list[i], ses->list[i]->list[j], 0);
				}
			}
		}
	}
	else
	{
		show_error(ses, LIST_CLASS, "#CLASS {%s} DOES NOT EXIST.", left);
	}
	return ses;
}

DO_CLASS(class_read)
{
	class_open(ses, left, right);

	do_read(ses, right);

	class_close(ses, left, right);

	return ses;
}


DO_CLASS(class_write)
{
	FILE *file;
	int list, index;

	if (!search_node_list(ses->list[LIST_CLASS], left))
	{
		return show_error(ses, LIST_CLASS, "#CLASS {%s} DOES NOT EXIST.", left);
	}

	if (*right == 0 || (file = fopen(right, "w")) == NULL)
	{
		return show_error(ses, LIST_CLASS, "#ERROR: #CLASS WRITE {%s} - COULDN'T OPEN FILE TO WRITE.", right);
	}

	fprintf(file, "%cCLASS {%s} OPEN\n\n", gtd->tintin_char, left);

	for (list = 0 ; list < LIST_MAX ; list++)
	{
		if (!HAS_BIT(ses->list[list]->flags, LIST_FLAG_CLASS))
		{
			continue;
		}

		for (index = 0 ; index < ses->list[list]->used ; index++)
		{
			if (!strcmp(ses->list[list]->list[index]->group, left))
			{
				write_node(ses, list, ses->list[list]->list[index], file);
			}
		}
	}

	fprintf(file, "\n%cCLASS {%s} CLOSE\n", gtd->tintin_char, left);

	fclose(file);

	show_message(ses, LIST_CLASS, "#CLASS {%s} HAS BEEN WRITTEN TO FILE.", left);

	return ses;
}

DO_CLASS(class_kill)
{
	int type, index, group;

	if (!search_node_list(ses->list[LIST_CLASS], left))
	{
		update_node_list(ses->list[LIST_CLASS], left, "", "0");
	}
	group = search_index_list(ses->list[LIST_CLASS], left, NULL);

	for (type = 0 ; type < LIST_MAX ; type++)
	{
		if (!HAS_BIT(ses->list[type]->flags, LIST_FLAG_CLASS))
		{
			continue;
		}

		for (index = 0 ; index < ses->list[type]->used ; index++)
		{
			if (!strcmp(ses->list[type]->list[index]->group, left))
			{
				delete_index_list(ses->list[type], index--);
			}
		}
	}

	delete_index_list(ses->list[LIST_CLASS], group);

	show_message(ses, LIST_CLASS, "#CLASS {%s} HAS BEEN KILLED.", left);

	if (!strcmp(ses->group, left))
	{
		check_all_events(ses, SUB_ARG, 0, 1, "CLASS DEACTIVATED", ses->group);

		struct listnode *node = ses->list[LIST_CLASS]->list[0];

		if (node && atoi(node->pr))
		{
			RESTRING(ses->group, node->left);

			show_message(ses, LIST_CLASS, "#CLASS {%s} HAS BEEN ACTIVATED.", node->left);

			check_all_events(ses, SUB_ARG, 0, 1, "CLASS ACTIVATED", left);
		}
		else
		{
			RESTRING(ses->group, "");
		}
	}

	return ses;
}

DO_CLASS(class_size)
{
	struct listnode *node;

	if (*left == 0 || *right == 0)
	{
		return show_error(ses, LIST_CLASS, "#SYNTAX: #CLASS {<class name>} SIZE {<variable>}.");
	}

	node = search_node_list(ses->list[LIST_CLASS], left);

	if (node == NULL)
	{
		set_nest_node(ses->list[LIST_VARIABLE], right, "0");
	}
	else
	{
		set_nest_node(ses->list[LIST_VARIABLE], right, "%d", count_class(ses, node));
	}
	return ses;
}
