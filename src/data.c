/******************************************************************************
*    TinTin++  Copyright (C) 2004 (See CREDITS file)                          *
*                                                                             *
*    This program is free software: you can redistribute it and/or modify     *
*    it under the terms of the GNU General Public License as published by     *
*    the Free Software Foundation, either version 2 of the License, or        *
*    (at your option) any later version.                                      *
*                                                                             *
*    This program is distributed in the hope that it will be useful,          *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
*    GNU General Public License for more details.                             *
*                                                                             *
*    You should have received a copy of the GNU General Public License        *
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
******************************************************************************/

/******************************************************************************
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                         coded by Peter Unold 1992                           *
*                     recoded by Igor van den Hoven 2004                      *
******************************************************************************/

#include "tintin.h"


struct listroot *init_list(struct session *ses, int type, int size)
{
	struct listroot *listhead;

	if ((listhead = (struct listroot *) calloc(1, sizeof(struct listroot))) == NULL)
	{
		fprintf(stderr, "couldn't alloc listhead\n");
		exit(1);
	}

	listhead->ses  = ses;
	listhead->list = (struct listnode **) calloc(size, sizeof(struct listnode *));
	listhead->size = size;
	listhead->type = type;

	listhead->flags = list_table[type].flags;

	return listhead;
}


void kill_list(struct listroot *root)
{
	while (root->used)
	{
		delete_index_list(root, root->used - 1);
	}
}

void free_list(struct listroot *root)
{
	kill_list(root);

	free(root->list);

	free(root);
}

struct listroot *copy_list(struct session *ses, struct listroot *sourcelist, int type)
{
	int i;
	struct listnode *node;

	push_call("copy_list(%p,%p,%p)",ses,sourcelist,type);

	ses->list[type] = init_list(ses, type, sourcelist->size);

	if (HAS_BIT(sourcelist->flags, LIST_FLAG_INHERIT))
	{
		for (i = 0 ; i < sourcelist->used ; i++)
		{
			node = (struct listnode *) calloc(1, sizeof(struct listnode));

			node->left  = strdup(sourcelist->list[i]->left);
			node->right = strdup(sourcelist->list[i]->right);
			node->pr    = strdup(sourcelist->list[i]->pr);
			node->group = strdup(sourcelist->list[i]->group);

			switch (type)
			{
				case LIST_ALIAS:
					node->regex = tintin_regexp_compile(ses, node, node->left, PCRE_ANCHORED);
					break;

				case LIST_ACTION:
				case LIST_GAG:
				case LIST_HIGHLIGHT:
				case LIST_PROMPT:
				case LIST_SUBSTITUTE:
					node->regex = tintin_regexp_compile(ses, node, node->left, 0);
					break;

				case LIST_VARIABLE:
					copy_nest_node(ses->list[type], node, sourcelist->list[i]);
					break;
			}
			ses->list[type]->list[i] = node;
		}
		ses->list[type]->used = sourcelist->used;
	}
	ses->list[type]->flags = sourcelist->flags;

	pop_call();
	return ses->list[type];
}

/*
	create a node and stuff it into the list in the desired order
*/

struct listnode *insert_node_list(struct listroot *root, char *ltext, char *rtext, char *prtext)
{
	int index;
	struct listnode *node;

	node = (struct listnode *) calloc(1, sizeof(struct listnode));

	node->left  = strdup(ltext);
	node->right = strdup(rtext);
	node->pr    = strdup(prtext);

	node->group = HAS_BIT(root->flags, LIST_FLAG_CLASS) ? strdup(root->ses->group) : strdup("");

	switch (root->type)
	{
		case LIST_ALIAS:
			node->regex = tintin_regexp_compile(root->ses, node, node->left, PCRE_ANCHORED);
			break;

		case LIST_ACTION:
		case LIST_GAG:
		case LIST_HIGHLIGHT:
		case LIST_PROMPT:
		case LIST_SUBSTITUTE:
			node->regex = tintin_regexp_compile(root->ses, node, node->left, 0);
			break;
	}

	index = locate_index_list(root, ltext, prtext);

	return insert_index_list(root, node, index);
}


struct listnode *update_node_list(struct listroot *root, char *ltext, char *rtext, char *prtext)
{
	int index;
	struct listnode *node;

	index = search_index_list(root, ltext, NULL);

	if (index != -1)
	{
		node = root->list[index];

		if (strcmp(node->right, rtext) != 0)
		{
			free(node->right);
			node->right = strdup(rtext);
		}

		node->data = 0;

		switch (list_table[root->type].mode)
		{
			case PRIORITY:
				if (atof(node->pr) != atof(prtext))
				{
					delete_index_list(root, index);
					return insert_node_list(root, ltext, rtext, prtext);
				}
				break;

			case APPEND:
				delete_index_list(root, index);
				return insert_node_list(root, ltext, rtext, prtext);
				break;

			case ALPHA:
				if (strcmp(node->pr, prtext) != 0)
				{
					free(node->pr);
					node->pr = strdup(prtext);
				}
				break;

			default:
				tintin_printf2(root->ses, "#BUG: update_node_list: unknown mode: %d", list_table[root->type].mode);
				break;
		}
		return node;
	}
	else
	{
		return insert_node_list(root, ltext, rtext, prtext);
	}
}

struct listnode *insert_index_list(struct listroot *root, struct listnode *node, int index)
{
	root->used++;

	if (root->used == root->size)
	{
		root->size *= 2;

		root->list = (struct listnode **) realloc(root->list, (root->size) * sizeof(struct listnode *));
	}

	memmove(&root->list[index + 1], &root->list[index], (root->used - index) * sizeof(struct listnode *));

	root->list[index] = node;

	return node;
}

void delete_node_list(struct session *ses, int type, struct listnode *node)
{
	int index = search_index_list(ses->list[type], node->left, node->pr);

	delete_index_list(ses->list[type], index);
}

void delete_index_list(struct listroot *root, int index)
{
	struct listnode *node = root->list[index];

	if (node->root)
	{
		free_list(node->root);
	}

	if (index <= root->update)
	{
		root->update--;
	}

	free(node->left);
	free(node->right);
	free(node->pr);
	free(node->group);

	if (node->regex)
	{
		free(node->regex);
	}
	free(node);

	memmove(&root->list[index], &root->list[index + 1], (root->used - index) * sizeof(struct listnode *));

	root->used--;

	return;
}

struct listnode *search_node_list(struct listroot *root, char *text)
{
	int index;

	push_call("search_node_list(%p,%p)",root,text);

	switch (list_table[root->type].mode)
	{
		case ALPHA:
			index = bsearch_alpha_list(root, text, 0);
			break;

		default:
			index = nsearch_list(root, text);
			break;
	}

	if (index != -1)
	{
		pop_call();
		return root->list[index];
	}
	else
	{
		pop_call();
		return NULL;
	}
}

int search_index_list(struct listroot *root, char *text, char *priority)
{
	if (list_table[root->type].mode == ALPHA)
	{
		return bsearch_alpha_list(root, text, 0);
	}

	if (list_table[root->type].mode == PRIORITY && priority)
	{
		return bsearch_priority_list(root, text, priority, 0);
	}

	return nsearch_list(root, text);
}
 
/*
	Return insertion index.
*/

int locate_index_list(struct listroot *root, char *text, char *priority)
{
	switch (list_table[root->type].mode)
	{
		case ALPHA:
			return bsearch_alpha_list(root, text, 1);

		case PRIORITY:
			return bsearch_priority_list(root, text, priority, 1);

		default:
			return root->used;
	}
}

/*
	Yup, all this for a bloody binary search.
*/

int bsearch_alpha_list(struct listroot *root, char *text, int seek)
{
	long long bot, top, val;
	double toi, toj, srt;

	push_call("bsearch_alpha_list(%p,%p,%d)",root,text,seek);

	bot = 0;
	top = root->used - 1;
	val = top;

//	toi = get_number(root->ses, text);

	if (seek == 0 && (*text == '+' || *text == '-') && is_math(root->ses, text) && HAS_BIT(list_table[root->type].flags, LIST_FLAG_NEST))
	{
		toi = get_number(root->ses, text);

		if (toi > 0 && toi <= root->used)
		{
			pop_call();
			return toi - 1;
		}
		if (toi < 0 && toi + root->used >= 0)
		{
			pop_call();
			return root->used + toi;
		}
		else
		{
			pop_call();
			return -1;
		}
	}

	toi = is_number(text) ? tintoi(text) : 0;

	while (bot <= top)
	{
		toj = is_number(root->list[val]->left) ? tintoi(root->list[val]->left) : 0;

		if (toi)
		{
			srt = toi - toj;
		}
		else if (toj)
		{
			srt = -1;
		}
		else
		{
			srt = strcmp(text, root->list[val]->left);
		}

		if (srt == 0)
		{
			pop_call();
			return val;
		}

		if (srt < 0)
		{
			top = val - 1;
		}
		else
		{
			bot = val + 1;
		}

		val = bot + (top - bot) / 2;
	}

	if (seek)
	{
		pop_call();
		return UMAX(0, val);
	}
	else
	{
		pop_call();
		return -1;
	}
}

int bsearch_priority_list(struct listroot *root, char *text, char *priority, int seek)
{
	int bot, top, val;
	double srt;

	bot = 0;
	top = root->used - 1;
	val = top;

	while (bot <= top)
	{
		srt = atof(priority) - atof(root->list[val]->pr);

		if (!srt)
		{
			srt = strcmp(text, root->list[val]->left);
		}

		if (srt == 0)
		{
			return val;
		}

		if (srt < 0)
		{
			top = val - 1;
		}
		else
		{
			bot = val + 1;
		}

		val = bot + (top - bot) / 2;
	}

	if (seek)
	{
		return UMAX(0, val);
	}
	else
	{
		return -1;
	}
}

int nsearch_list(struct listroot *root, char *text)
{
	int i;

	for (i = 0 ; i < root->used ; i++)
	{
		if (!strcmp(text, root->list[i]->left))
		{
			return i;
		}
	}
	return -1;
}

/*
	show contens of a node on screen
*/

void show_node(struct listroot *root, struct listnode *node, int level)
{
	char *str = str_dup("");

	show_nest_node(node, &str, TRUE);

	switch (list_table[root->type].args)
	{
		case 3:
			tintin_printf2(root->ses, "%*s#%s \033[1;31m{\033[0m%s\033[1;31m}\033[1;36m=\033[1;31m{\033[0m%s\033[1;31m} \033[1;36m@ \033[1;31m{\033[0m%s\033[1;31m}", level * 2, "", list_table[root->type].name, node->left, str, node->pr);
			break;
		case 2:
			tintin_printf2(root->ses, "%*s#%s \033[1;31m{\033[0m%s\033[1;31m}\033[1;36m=\033[1;31m{\033[0m%s\033[1;31m}", level * 2, "", list_table[root->type].name, node->left, str);
			break;
		case 1:
			tintin_printf2(root->ses, "%*s#%s \033[1;31m{\033[0m%s\033[1;31m}", level * 2, "", list_table[root->type].name, node->left);
			break;
	}
	str_free(str);
}

/*
	list contens of a list on screen
*/

void show_list(struct listroot *root, int level)
{
	int i;

	if (root == root->ses->list[root->type])
	{
		tintin_header(root->ses, " %s ", list_table[root->type].name_multi);
	}

	for (i = 0 ; i < root->used ; i++)
	{
		show_node(root, root->list[i], level);
	}
}


int show_node_with_wild(struct session *ses, char *text, struct listroot *root)
{
	struct listnode *node;
	int i, flag = FALSE;

	push_call("show_node_with_wild(%p,%p,%p)",ses,text,root);

	node = search_node_list(root, text);

	if (node)
	{
		show_node(root, node, 0);

		pop_call();
		return TRUE;
	}

	for (i = 0 ; i < root->used ; i++)
	{
		if (match(ses, root->list[i]->left, text, SUB_VAR|SUB_FUN))
		{
			show_node(root, root->list[i], 0);

			flag = TRUE;
		}
	}
	pop_call();
	return flag;
}


void delete_node_with_wild(struct session *ses, int type, char *text)
{
	struct listroot *root = ses->list[type];
	struct listnode *node;
	char arg1[BUFFER_SIZE];
	int i, found = FALSE;

	sub_arg_in_braces(ses, text, arg1, 1, SUB_VAR|SUB_FUN);

	node = search_node_list(root, arg1);

	if (node)
	{
		show_message(ses, type, "#OK. {%s} IS NO LONGER %s %s.", node->left, (*list_table[type].name == 'A' || *list_table[type].name == 'E') ? "AN" : "A", list_table[type].name);

		delete_node_list(ses, type, node);

		return;
	}

	for (i = root->used - 1 ; i >= 0 ; i--)
	{
		if (match(ses, root->list[i]->left, arg1, SUB_VAR|SUB_FUN))
		{
			show_message(ses, type, "#OK. {%s} IS NO LONGER %s %s.", root->list[i]->left, (*list_table[type].name == 'A' || *list_table[type].name == 'E') ? "AN" : "A", list_table[type].name);

			delete_index_list(root, i);

			found = TRUE;
		}
	}

	if (found == 0)
	{
		show_message(ses, type, "#KILL: NO MATCHES FOUND FOR %s {%s}.", list_table[type].name, arg1);
	}
}


DO_COMMAND(do_kill)
{
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE];
	int index;

	arg = get_arg_in_braces(ses, arg, arg1, 0);
	      get_arg_in_braces(ses, arg, arg2, 1);

	if (*arg1 == 0 || !strcasecmp(arg1, "ALL"))
	{
		for (index = 0 ; index < LIST_MAX ; index++)
		{
			kill_list(ses->list[index]);
		}
		show_message(ses, LIST_COMMAND, "#KILL - ALL LISTS CLEARED.");

		return ses;
	}

	for (index = 0 ; index < LIST_MAX ; index++)
	{
		if (!is_abbrev(arg1, list_table[index].name_multi))
		{
			continue;
		}

		if (*arg2 == 0 || !strcasecmp(arg2, "ALL"))
		{
			kill_list(ses->list[index]);

			show_message(ses, LIST_COMMAND, "#OK: #%s LIST CLEARED.", list_table[index].name);
		}
		else
		{
			delete_node_with_wild(ses, index, arg);
		}
		break;
	}

	if (index == LIST_MAX)
	{
		show_error(ses, LIST_COMMAND, "#ERROR: #KILL {%s} {%s} - NO MATCH FOUND.", arg1, arg2);
	}
	return ses;
}

DO_COMMAND(do_message)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int index, found = FALSE;

	arg = get_arg_in_braces(ses, arg, left,  FALSE);
	arg = get_arg_in_braces(ses, arg, right, FALSE);

	if (*left == 0)
	{
		tintin_header(ses, " MESSAGES ");

		for (index = 0 ; index < LIST_MAX ; index++)
		{
			if (!HAS_BIT(list_table[index].flags, LIST_FLAG_HIDE))
			{
				tintin_printf2(ses, "  %-20s %3s", list_table[index].name_multi, HAS_BIT(ses->list[index]->flags, LIST_FLAG_MESSAGE) ? "ON" : "OFF");
			}
		}

		tintin_header(ses, "");
	}
	else
	{
		for (index = found = 0 ; index < LIST_MAX ; index++)
		{
			if (HAS_BIT(list_table[index].flags, LIST_FLAG_HIDE))
			{
				continue;
			}

			if (!is_abbrev(left, list_table[index].name_multi) && strcasecmp(left, "ALL"))
			{
				continue;
			}

			if (*right == 0)
			{
				TOG_BIT(ses->list[index]->flags, LIST_FLAG_MESSAGE);
			}
			else if (is_abbrev(right, "ON"))
			{
				SET_BIT(ses->list[index]->flags, LIST_FLAG_MESSAGE);
			}
			else if (is_abbrev(right, "OFF"))
			{
				DEL_BIT(ses->list[index]->flags, LIST_FLAG_MESSAGE);
			}
			else
			{
				return show_error(ses, LIST_COMMAND, "#SYNTAX: #MESSAGE {%s} [ON|OFF]",  left);
			}
			show_message(ses, LIST_COMMAND, "#OK: #%s MESSAGES HAVE BEEN SET TO: %s.", list_table[index].name, HAS_BIT(ses->list[index]->flags, LIST_FLAG_MESSAGE) ? "ON" : "OFF");

			found = TRUE;
		}

		if (found == FALSE)
		{
			show_error(ses, LIST_COMMAND, "#ERROR: #MESSAGE {%s} - NO MATCH FOUND.", left);
		}
	}
	return ses;
}


DO_COMMAND(do_ignore)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int index, found = FALSE;

	arg = get_arg_in_braces(ses, arg, left,  0);
	arg = get_arg_in_braces(ses, arg, right, 0);

	if (*left == 0)
	{
		tintin_header(ses, " IGNORES ");

		for (index = 0 ; index < LIST_MAX ; index++)
		{
			if (!HAS_BIT(list_table[index].flags, LIST_FLAG_HIDE))
			{
				tintin_printf2(ses, "  %-20s %3s", list_table[index].name_multi, HAS_BIT(ses->list[index]->flags, LIST_FLAG_IGNORE) ? "ON" : "OFF");
			}
		}

		tintin_header(ses, "");
	}
	else
	{
		for (index = found = 0 ; index < LIST_MAX ; index++)
		{
			if (HAS_BIT(list_table[index].flags, LIST_FLAG_HIDE))
			{
				continue;
			}

			if (!is_abbrev(left, list_table[index].name_multi) && strcasecmp(left, "ALL"))
			{
				continue;
			}

			if (*right == 0)
			{
				TOG_BIT(ses->list[index]->flags, LIST_FLAG_IGNORE);
			}
			else if (is_abbrev(right, "ON"))
			{
				SET_BIT(ses->list[index]->flags, LIST_FLAG_IGNORE);
			}
			else if (is_abbrev(right, "OFF"))
			{
				DEL_BIT(ses->list[index]->flags, LIST_FLAG_IGNORE);
			}
			else
			{
				return show_error(ses, LIST_COMMAND, "#SYNTAX: #IGNORE {%s} [ON|OFF]", left);
			}
			show_message(ses, LIST_COMMAND, "#OK: #%s IGNORE STATUS HAS BEEN SET TO: %s.", list_table[index].name, HAS_BIT(ses->list[index]->flags, LIST_FLAG_IGNORE) ? "ON" : "OFF");

			found = TRUE;
		}

		if (found == FALSE)
		{
			show_error(ses, LIST_COMMAND, "#ERROR: #IGNORE {%s} - NO MATCH FOUND.", left);
		}
	}
	return ses;
}


DO_COMMAND(do_debug)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int index, found = FALSE;

	arg = get_arg_in_braces(ses, arg, left,  0);
	arg = get_arg_in_braces(ses, arg, right, 0);

	if (*left == 0)
	{
		tintin_header(ses, " DEBUGS ");

		for (index = 0 ; index < LIST_MAX ; index++)
		{
			if (!HAS_BIT(list_table[index].flags, LIST_FLAG_HIDE))
			{
				tintin_printf2(ses, "  %-20s %3s", list_table[index].name_multi, HAS_BIT(ses->list[index]->flags, LIST_FLAG_DEBUG) ? "ON" : "OFF");
			}
		}

		tintin_header(ses, "");
	}
	else
	{
		for (index = found = 0 ; index < LIST_MAX ; index++)
		{
			if (HAS_BIT(list_table[index].flags, LIST_FLAG_HIDE))
			{
				continue;
			}

			if (!is_abbrev(left, list_table[index].name_multi) && strcasecmp(left, "ALL"))
			{
				continue;
			}

			if (*right == 0)
			{
				TOG_BIT(ses->list[index]->flags, LIST_FLAG_DEBUG);
			}
			else if (is_abbrev(right, "ON"))
			{
				SET_BIT(ses->list[index]->flags, LIST_FLAG_DEBUG);
			}
			else if (is_abbrev(right, "OFF"))
			{
				DEL_BIT(ses->list[index]->flags, LIST_FLAG_DEBUG);
				DEL_BIT(ses->list[index]->flags, LIST_FLAG_LOG);
			}
			else if (is_abbrev(right, "LOG"))
			{
				SET_BIT(ses->list[index]->flags, LIST_FLAG_LOG);
			}
			else
			{
				return show_error(ses, LIST_COMMAND, "#SYNTAX: #DEBUG {%s} [ON|OFF|LOG]", left);
			}
			show_message(ses, LIST_COMMAND, "#OK: #%s DEBUG STATUS HAS BEEN SET TO: %s.", list_table[index].name, is_abbrev(right, "LOG") ? "LOG" : HAS_BIT(ses->list[index]->flags, LIST_FLAG_DEBUG) ? "ON" : "OFF");

			found = TRUE;
		}

		if (found == FALSE)
		{
			show_error(ses, LIST_COMMAND, "#DEBUG {%s} - NO MATCH FOUND.", left);
		}
	}
	return ses;
}
