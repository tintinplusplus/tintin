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


DO_COMMAND(do_class)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], pr[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node, *class;
	int cnt;

	root = ses->list[LIST_CLASS];

	arg = get_arg_in_braces(arg, left, FALSE);
	arg = get_arg_in_braces(arg, right, FALSE);
	arg = get_arg_in_braces(arg, pr, FALSE);

	if (*left == 0)
	{
		tintin_header(ses, " CLASSES ");

		for (node = root->f_node ; node ; node = node->next)
		{
			tintin_printf2(ses, "%-20s %3lld %s",
			node->left,
			node->data / NODE_FLAG_MAX,
			HAS_BIT(node->data, NODE_FLAG_CLASS) ? "OPEN" : "CLOSED");
		}
	}
	else if (*right == 0)
	{
		class = search_node_with_wild(root, left);

		if (class)
		{
			tintin_header(ses, " %s ", left);

			for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
			{
				for (node = ses->list[cnt]->f_node ; node ; node = node->next)
				{
					if (node->class == class)
					{
						shownode_list(ses, node, cnt);
					}
				}
			}
		}
		else
		{
			tintin_printf2(ses, "#CLASS {%s} DOES NOT EXIST.", left);
		}
	}
	else
	{
			
		for (cnt = 0 ; cnt < CLASS_MAX ; cnt++)
		{
			if (is_abbrev(right, class_table[cnt].name))
			{
				break;
			}
		}

		if (cnt == CLASS_MAX)
		{
			tintin_printf2(ses, "#SYNTAX: CLASS {name} {OPEN|CLOSE|READ|WRITE|KILL}.", left, capitalize(right));
		}
		else
		{
			updatenode_list(ses, left, right, pr, LIST_CLASS);

			class_table[cnt].class(ses, left);
		}
	}
	return ses;
}


DO_COMMAND(do_unclass)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int flag = FALSE;

	root = ses->list[LIST_TAB];

	arg = get_arg_in_braces(arg, left, 1);

	while ((node = search_node_with_wild(root, left)) != NULL)
	{
		if (show_message(ses, LIST_CLASS))
		{
			tintin_printf(ses, "#Ok. {%s} removed from class list.", node->left);
		}
		deletenode_list(ses, node, LIST_CLASS);
		flag = TRUE;
	}

	if (!flag && show_message(ses, LIST_CLASS))
	{
		tintin_puts2("#THAT CLASS IS NOT DEFINED.", ses);
	}
	return ses;
}

DO_CLASS(class_open)
{
	ses->class = search_node_with_wild(ses->list[LIST_CLASS], arg);

	SET_BIT(ses->class->data, NODE_FLAG_CLASS);

	if (show_message(ses, LIST_CLASS))
	{
		tintin_printf2(ses, "#CLASS {%s} HAS BEEN OPENED.", arg);
	}
	return ses;
}

DO_CLASS(class_close)
{
	struct listnode *node;

	node = search_node_with_wild(ses->list[LIST_CLASS], arg);

	if (show_message(ses, LIST_CLASS))
	{
		tintin_printf2(ses, "#CLASS {%s} HAS BEEN CLOSED.", node->left);
	}
	DEL_BIT(node->data, NODE_FLAG_CLASS);

	if (node == ses->class)
	{
		ses->class = NULL;

		for (node = ses->list[LIST_CLASS]->l_node ; node ; node = node->prev)
		{
			if (HAS_BIT(node->data, NODE_FLAG_CLASS))
			{
				class_open(ses, node->left);

				break;
			}
		}
	}

	return ses;
}

DO_CLASS(class_read)
{
	struct listnode *node = search_node_with_wild(ses->list[LIST_CLASS], arg);

	DEL_BIT(node->data, NODE_FLAG_CLASS);

	readfile(ses, node->pr, node);

	return ses;
}

DO_CLASS(class_write)
{
	FILE *file;
	char temp[BUFFER_SIZE], filename[BUFFER_SIZE];
	struct listnode *node, *class;
	int cnt;

	class = search_node_with_wild(ses->list[LIST_CLASS], arg);

	get_arg_in_braces(class->pr, temp, TRUE);

	substitute(ses, temp, filename, SUB_VAR|SUB_FUN);

	if (*filename == 0 || (file = fopen(filename, "w")) == NULL)
	{
		tintin_printf2(ses, "#ERROR - COULDN'T OPEN '%s' TO WRITE.", filename);
		return ses;
	}

	for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
	{
		for (node = ses->list[cnt]->f_node ; node ; node = node->next)
		{
			if (node->class == class)
			{
				prepare_for_write(cnt, node, temp);

				fputs(temp, file);
			}
		}
	}

	fclose(file);

	if (show_message(ses, LIST_CLASS))
	{
		tintin_printf2(ses, "#CLASS {%s} HAS BEEN WRITTEN TO FILE.", class->left);
	}
	return ses;
}

DO_CLASS(class_kill)
{
	struct listnode *node, *class;
	int cnt;

	class = search_node_with_wild(ses->list[LIST_CLASS], arg);

	for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
	{
		for (node = ses->list[cnt]->f_node ; node ; node = ses->list[cnt]->update)
		{
			ses->list[cnt]->update = node->next;

			if (node->class == class)
			{
				deletenode_list(ses, node, cnt);
			}
		}
	}

	if (show_message(ses, LIST_CLASS))
	{
		tintin_printf2(ses, "#CLASS {%s} HAS BEEN CLEARED.", class->left);
	}

	deletenode_list(ses, class, LIST_CLASS);

	return ses;
}

void parse_class(struct session *ses, char *input, struct listnode *class)
{
	char temp[BUFFER_SIZE], command[BUFFER_SIZE], arg[BUFFER_SIZE];
	char *pti, *pta;
	struct listnode *ln;
	int i;

	push_call("parse_class(%p,%p,%p)",ses,input,class);

	pti = input;

	while (*pti)
	{
		if (*pti == ';')
		{
			if (*++pti == 0)
			{
				break;
			}
		}
		pti = (char *)get_arg_stop_spaces(pti, temp);
		pti = (char *)get_arg_all(pti, arg);

		substitute(ses, temp, command, SUB_VAR|SUB_FUN);

		if (*command == gtd->tintin_char)
		{
			for (i = 0 ; *command_table[i].name != 0 ; i++)
			{
				if (is_abbrev(&command[1], command_table[i].name))
				{
					break;
				}
			}
			if (strcmp(command_table[i].name, "class"))
			{
				continue;
			}
			if (!is_abbrev(class->left, arg))
			{
				continue;
			}
			(command_table[i].command) (ses, arg);
		}
		else if ((ln = searchnode_list_begin(ses->list[LIST_ALIAS], command, ALPHA)))
		{
			strcpy(gtd->vars[0], arg);

			pta = arg;

			for (i = 1 ; i < 10 ; i++)
			{
				pta = (char *) get_arg_in_braces(pta, gtd->vars[i], FALSE);
			}
			substitute(ses, ln->right, command, SUB_ARG);

			if (!strcmp(ln->right, command) && *arg)
			{
				sprintf(command, "%s %s", ln->right, arg);
			}
			parse_class(ses, command, class);
		}
	}
	return;
}
