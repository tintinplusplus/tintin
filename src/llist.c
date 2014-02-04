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
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                         coded by Peter Unold 1992                           *
******************************************************************************/

#include "tintin.h"


struct listroot *init_list(int index)
{
	struct listroot *listhead;

	if ((listhead = calloc(1, sizeof(struct listroot))) == NULL)
	{
		fprintf(stderr, "couldn't alloc listhead\n");
		exit(1);
	}
	listhead->flags = list_table[index].flags;

	return listhead;
}


void kill_list(struct session *ses, int index)
{
	struct listnode *node;

	while ((node = ses->list[index]->f_node))
	{
		deletenode_list(ses, node, index);
	}
}


struct listroot *copy_list(struct session *ses, struct listroot *sourcelist, int index)
{
	struct listnode *node, *newnode;

	ses->list[index] = init_list(index);

	if (HAS_BIT(sourcelist->flags, LIST_FLAG_INHERIT))
	{
		for (node = sourcelist->f_node ; node ; node = node->next)
		{
			newnode = calloc(1, sizeof(struct listnode));

			newnode->left  = strdup(node->left);
			newnode->right = strdup(node->right);
			newnode->pr    = strdup(node->pr);
			newnode->class = strdup(node->class);

			ses->list[index]->count++;

			LINK(newnode, ses->list[index]->f_node, ses->list[index]->l_node);
		}
	}
	ses->list[index]->flags = sourcelist->flags;

	return ses->list[index];
}

/*
	create a node and stuff it into the list in the desired order
*/

void insertnode_list(struct session *ses, char *ltext, char *rtext, char *prtext, int index)
{
	struct listnode *node, *newnode;

	push_call("insertnode_list(%p,%p,%p)",ses,ltext,rtext);

	newnode = calloc(1, sizeof(struct listnode));

	newnode->left  = strdup(ltext);
	newnode->right = strdup(rtext);
	newnode->pr    = strdup(prtext);
	newnode->class = HAS_BIT(ses->list[index]->flags, LIST_FLAG_CLASS) ? strdup(ses->class) : strdup("");

	ses->list[index]->count++;

	switch (list_table[index].mode)
	{
		case PRIORITY:
			for (node = ses->list[index]->f_node ; node ; node = node->next)
			{
				if (atof(prtext) < atof(node->pr) || (atof(prtext) == atof(node->pr) && strcmp(ltext, node->left) < 0))
				{
					INSERT_LEFT(newnode, node, ses->list[index]->f_node);
					pop_call();
					return;
				}
			}
			LINK(newnode, ses->list[index]->f_node, ses->list[index]->l_node);
			break;

		case ALPHA:
			for (node = ses->list[index]->f_node ; node ; node = node->next)
			{
				if (strcmp(ltext, node->left) < 0)
				{
					INSERT_LEFT(newnode, node, ses->list[index]->f_node);
					pop_call();
					return;
				}
			}
			LINK(newnode, ses->list[index]->f_node, ses->list[index]->l_node);
			break;

		case APPEND:
			LINK(newnode, ses->list[index]->f_node, ses->list[index]->l_node);
			break;

		default:
			tintin_printf2(NULL, "#BUG: insertnode_list: bad list_table mode: %d", list_table[index].mode);
			break;
	}
	pop_call();
	return;
}


void updatenode_list(struct session *ses, char *ltext, char *rtext, char *prtext, int index)
{
	struct listnode *node;

	push_call("updatenode_list(%p,%p,%p,%p,%p)",ses,ltext,rtext,prtext,index);

	for (node = ses->list[index]->f_node ; node ; node = node->next)
	{
		if (strcmp(node->left, ltext) == 0)
		{
			if (strcmp(node->right, rtext) != 0)
			{
				free(node->right);
				node->right = strdup(rtext);
			}

			node->data = 0;

			switch (list_table[index].mode)
			{
				case PRIORITY:
					if (atof(node->pr) == atof(prtext))
					{
						pop_call();
						return;
					}
					deletenode_list(ses, node, index);
					insertnode_list(ses, ltext, rtext, prtext, index);
					break;

				case APPEND:
					if (strcmp(node->pr, prtext) != 0)
					{
						free(node->pr);
						node->pr = strdup(prtext);
					}
					UNLINK(node, ses->list[index]->f_node, ses->list[index]->l_node);
					LINK(node, ses->list[index]->f_node, ses->list[index]->l_node);
					break;

				case ALPHA:
					if (strcmp(node->pr, prtext) != 0)
					{
						free(node->pr);
						node->pr = strdup(prtext);
					}
					break;

				default:
					tintin_printf2(ses, "#BUG: updatenode_list: bad mode");
					break;
			}
			pop_call();
			return;
		}
	}
	insertnode_list(ses, ltext, rtext, prtext, index);
	pop_call();
	return;
}

void deletenode_list(struct session *ses, struct listnode *node, int index)
{
	push_call("deletenode_list(%p,%p,%p)", ses, node, index);

	if ((node->next == NULL && node != ses->list[index]->l_node)
	||  (node->prev == NULL && node != ses->list[index]->f_node))
	{
		tintin_printf2(NULL, "#BUG: delete_nodelist: unlink error.");
		dump_stack();
	}
	else
	{
		if (node == ses->list[index]->update)
		{
			ses->list[index]->update = node->next;
		}

		UNLINK(node, ses->list[index]->f_node, ses->list[index]->l_node);

		if (index == LIST_CLASS && !strcmp(ses->class, node->left))
		{
			RESTRING(ses->class, "");
		}
		free(node->left);
		free(node->right);
		free(node->pr);
		free(node->class);
		free(node);

		ses->list[index]->count--;
	}
	pop_call();
	return;
}

/*
	search for a node containing the ltext in left-field
*/

struct listnode *searchnode_list(struct listroot *listhead, char *cptr)
{
	struct listnode *node;

	for (node = listhead->f_node ; node ; node = node->next)
	{
		if (!strcmp(node->left, cptr))
		{
			return node;
		}
	}
	return NULL;
}

/*
	search for a node that has cptr as a beginning
*/

struct listnode *searchnode_list_begin(struct listroot *listhead, char *cptr, int mode)
{
	struct listnode *node;
	int len;

	len = strlen(cptr);

	for (node = listhead->f_node ; node ; node = node->next)
	{
		if (strncmp(node->left, cptr, len) != 0)
		{
			continue;
		}

		if (node->left[len] == ' ' || node->left[len] == '\0')
		{
			return node;
		}
	}
	return NULL;
}

/*
	show contens of a node on screen
*/

void shownode_list(struct session *ses, struct listnode *nptr, int index)
{
	char buf[BUFFER_SIZE], out[BUFFER_SIZE];

	switch (list_table[index].args)
	{
		case 3:
			sprintf(buf, "#%s <118>{<088>%s<118>}<168>=<118>{<088>%s<118>} <168>@ <118>{<088>%s<118>}", list_table[index].name, nptr->left, nptr->right, nptr->pr);
			break;
		case 2:
			sprintf(buf, "#%s <118>{<088>%s<118>}<168>=<118>{<088>%s<118>}", list_table[index].name, nptr->left, nptr->right);
			break;
		case 1:
			sprintf(buf, "#%s <118>{<088>%s<118>}", list_table[index].name, nptr->left);
			break;
		default:
			sprintf(buf, "#BUG: list_table[index].args == 0");
			break;
	}

	substitute(ses, buf, out, SUB_COL);

	tintin_printf2(ses, "%s", out);
}

/*
	list contens of a list on screen
*/

void show_list(struct session *ses, struct listroot *listhead, int index)
{
	struct listnode *node;

	tintin_header(ses, " %s ", list_table[index].name_multi);

	for (node = listhead->f_node ; node ; node = node->next)
	{
		shownode_list(ses, node, index);
	}
}


int show_node_with_wild(struct session *ses, char *cptr, int index)
{
	struct listnode *node;
	int flag = FALSE;

	for (node = ses->list[index]->f_node ; node ; node = node->next)
	{
		if (regexp(cptr, node->left, TRUE))
		{
			shownode_list(ses, node, index);
			flag = TRUE;
		}
	}
	return flag;
}

struct listnode *search_node_with_wild(struct listroot *listhead, char *cptr)
{
	struct listnode *node;

	for (node = listhead->f_node ; node ; node = node->next)
	{
		if (regexp(cptr, node->left, TRUE) || !strcmp(cptr, node->left))
		{
			return node;
		}
	}
	return NULL;
}


/*
	create a node containing the ltext, rtext fields and place at the
	end of a list - as insertnode_list(), but not alphabetical
*/

void addnode_list(struct listroot *listhead, char *ltext, char *rtext, char *prtext)
{
	struct listnode *newnode;

	if ((newnode = calloc(1, sizeof(struct listnode))) == NULL)
	{
		fprintf(stderr, "couldn't calloc listhead");
		exit(1);
	}
	newnode->left  = calloc(1, strlen(ltext)  + 1);
	newnode->right = calloc(1, strlen(rtext)  + 1);
	newnode->pr    = calloc(1, strlen(prtext) + 1);

	sprintf(newnode->left,  "%s", ltext);
	sprintf(newnode->right, "%s", rtext);
	sprintf(newnode->pr,    "%s", prtext);

	LINK(newnode, listhead->f_node, listhead->l_node);

	listhead->count++;
}


int count_list(struct listroot *listhead)
{
	struct listnode *node;
	int cnt = 0;
	for (node = listhead->f_node ; node ; node = node->next)
	{
		cnt++;
	}
	return cnt;
}

DO_COMMAND(do_killall)
{
	char left[BUFFER_SIZE];
	int cnt, fnd = FALSE;

	if (arg == NULL || *arg == 0) 
	{
		for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
		{
			kill_list(ses, cnt);

			if (arg == NULL)
			{
				free(ses->list[cnt]);
			}
		}
		if (arg != NULL)
		{
			tintin_printf(ses, "#KILL - ALL LISTS CLEARED.");
		}
		return ses;
	}

	arg = get_arg_in_braces(arg, left,  0);

	for (cnt = fnd = 0 ; cnt < LIST_MAX ; cnt++)
	{
		if (!is_abbrev(left, list_table[cnt].name_multi) && strcasecmp(left, "ALL"))
		{
			continue;
		}
		kill_list(ses, cnt);

		tintin_printf(ses, "#OK: #%s LIST CLEARED.", list_table[cnt].name);

		fnd = TRUE;
	}
	if (fnd == FALSE)
	{
		tintin_printf(ses, "#ERROR: #KILL {%s} - NO MATCH FOUND.", left);
	}
	return ses;
}

DO_COMMAND(do_message)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int cnt, fnd = FALSE;

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 0);

	if (*left == 0)
	{
		tintin_header(ses, " MESSAGES ");

		for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
		{
			tintin_printf2(ses, "  %-20s %3s", list_table[cnt].name_multi, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE) ? "ON" : "OFF");
		}

		tintin_header(ses, "");
	}
	else
	{
		for (cnt = fnd = 0 ; cnt < LIST_MAX ; cnt++)
		{
			if (!is_abbrev(left, list_table[cnt].name_multi) && strcasecmp(left, "ALL"))
			{
				continue;
			}
			if (*right == 0)
			{
				TOG_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE);
			}
			else if (is_abbrev(right, "ON"))
			{
				SET_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE);
			}
			else if (is_abbrev(right, "OFF"))
			{
				DEL_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE);
			}
			else
			{
				tintin_printf(ses, "#SYNTAX: #MESSAGE [NAME] [ON|OFF]");
				break;
			}
			tintin_printf(ses, "#OK: #%s MESSAGES HAVE BEEN SET TO: %s.", list_table[cnt].name, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE) ? "ON" : "OFF");
			fnd = TRUE;
		}

		if (fnd == FALSE)
		{
			tintin_printf(ses, "#ERROR: #MESSAGE {%s} - NO MATCH FOUND.", left);
		}
	}
	return ses;
}


DO_COMMAND(do_ignore)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int cnt, fnd = FALSE;

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 0);

	if (*left == 0)
	{
		tintin_header(ses, " IGNORES ");

		for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
		{
			tintin_printf2(ses, "  %-20s %3s", list_table[cnt].name_multi, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE) ? "ON" : "OFF");
		}

		tintin_header(ses, "");
	}
	else
	{
		for (cnt = fnd = 0 ; cnt < LIST_MAX ; cnt++)
		{
			if (!is_abbrev(left, list_table[cnt].name_multi) && strcasecmp(left, "ALL"))
			{
				continue;
			}
			if (*right == 0)
			{
				TOG_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE);
			}
			else if (is_abbrev(right, "ON"))
			{
				SET_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE);
			}
			else if (is_abbrev(right, "OFF"))
			{
				DEL_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE);
			}
			else
			{
				tintin_printf(ses, "#SYNTAX: #IGNORE [NAME] [ON|OFF]");
				break;
			}
			tintin_printf(ses, "#OK: #%s IGNORE STATUS HAS BEEN SET TO: %s.", list_table[cnt].name, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE) ? "ON" : "OFF");
			fnd = TRUE;
		}

		if (fnd == FALSE)
		{
			tintin_printf(ses, "#ERROR: #IGNORE {%s} - NO MATCH FOUND.", left);
		}
	}
	return ses;
}


DO_COMMAND(do_debug)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int cnt, fnd = FALSE;

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 0);

	if (*left == 0)
	{
		tintin_header(ses, " DEBUGS ");

		for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
		{
			tintin_printf2(ses, "  %-20s %3s", list_table[cnt].name_multi, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG) ? "ON" : "OFF");
		}

		tintin_header(ses, "");
	}
	else
	{
		for (cnt = fnd = 0 ; cnt < LIST_MAX ; cnt++)
		{
			if (!is_abbrev(left, list_table[cnt].name_multi) && strcasecmp(left, "ALL"))
			{
				continue;
			}
			if (*right == 0)
			{
				TOG_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG);
			}
			else if (is_abbrev(right, "ON"))
			{
				SET_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG);
			}
			else if (is_abbrev(right, "OFF"))
			{
				DEL_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG);
			}
			else if (is_abbrev(right, "LOG"))
			{
				SET_BIT(ses->list[cnt]->flags, LIST_FLAG_LOG);
			}
			else
			{
				tintin_printf(ses, "#SYNTAX: #DEBUG [NAME] [ON|OFF|LOG]");
				break;
			}
			tintin_printf(ses, "#OK: #%s DEBUG STATUS HAS BEEN SET TO: %s.", list_table[cnt].name, is_abbrev(right, "LOG") ? "LOG" : HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG) ? "ON" : "OFF");
			fnd = TRUE;
		}

		if (fnd == FALSE)
		{
			tintin_printf2(ses, "#DEBUG {%s} - NO MATCH FOUND.", left);
		}
	}
	return ses;
}
