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
*                          coded by Bill Reiss 1993                           *
******************************************************************************/


#include "tintin.h"


DO_COMMAND(do_variable)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_VARIABLE];

	arg = get_arg_in_braces(arg, temp,  FALSE);
	substitute(ses, temp, left, SUB_VAR|SUB_FUN);

	arg = get_arg_in_braces(arg, temp, TRUE);
	substitute(ses, temp, right, SUB_VAR|SUB_FUN);

	if (!*left)
	{
		show_list(ses, root, LIST_VARIABLE);
	}
	else if (*left && !*right)
	{
		if (show_node_with_wild(ses, left, LIST_VARIABLE) == FALSE)
		{
			show_message(ses, LIST_VARIABLE, "#VARIABLE: NO MATCH(ES) FOUND FOR {%s}.", left);
		}
	}
	else
	{
		sprintf(temp, "{%s} {%s}", left, right);

		do_internal_variable(ses, temp);

		show_message(ses, LIST_VARIABLE, "#OK. $%s IS NOW SET TO {%s}.", left, right);
	}
	return ses;
}


DO_COMMAND(do_internal_variable)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE], index[BUFFER_SIZE], *pti, *pto;
	struct listnode *node;

	arg = get_arg_in_braces(arg, temp,  FALSE);
	substitute(ses, temp, left, SUB_VAR|SUB_FUN);

	arg = get_arg_in_braces(arg, temp, TRUE);
	substitute(ses, temp, right, SUB_VAR|SUB_FUN);

	pti = left;
	pto = temp;

	while (*pti && *pti != '[')
	{
		*pto++ = *pti++;
	}
	*pto = 0;

	pto = index;

	if (*pti == '[')
	{
		pti++;
	}

	while (*pti && *pti != ']')
	{
		
		*pto++ = *pti++;
	}
	*pto = 0;

	if (*index && *pti == ']')
	{
		if ((node = searchnode_list(ses->list[LIST_VARIABLE], temp)) == NULL)
		{
			updatenode_list(ses, temp, "", "0", LIST_VARIABLE);

			node = searchnode_list(ses->list[LIST_VARIABLE], temp);
		}
		set_list_item(ses, node, index, right);
	}
	else if (*left)
	{
		updatenode_list(ses, left, right, "0", LIST_VARIABLE);
	}
	return ses;
}


DO_COMMAND(do_unvariable)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int found = FALSE;

	root = ses->list[LIST_VARIABLE];

	arg = get_arg_in_braces(arg,left,1);

	while ((node = search_node_with_wild(root, left)) != NULL)
	{
		show_message(ses, LIST_VARIABLE, "#OK. $%s IS NO LONGER A VARIABLE.", node->left);

		deletenode_list(ses, node, LIST_VARIABLE);

		found = TRUE;
	}
	if (found == FALSE)
	{
		show_message(ses, LIST_VARIABLE, "#UNVARIABLE: NO MATCH(ES) FOUND FOR {%s}.", ses);
	}
	return ses;
}


DO_COMMAND(do_replacestring)
{
	char var[BUFFER_SIZE], old[BUFFER_SIZE], new[BUFFER_SIZE], result[BUFFER_SIZE], *pti, *ptr, *pto;
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_VARIABLE];

	arg = get_arg_in_braces(arg, var, FALSE);
	arg = get_arg_in_braces(arg, old, FALSE);
	arg = get_arg_in_braces(arg, new, TRUE);

	if (*var == 0 || *old == 0)
	{
		show_message(ses, LIST_VARIABLE, "#Syntax: #replacestring <var> <oldtext> <newtext>", ses);
	}
	else	if ((node = searchnode_list(root, var)) == NULL)
	{
		show_message(ses, LIST_VARIABLE, "#REPLACESTRING: VARIABLE {%s} NOT FOUND.", var);
	}
	else if ((ptr = strstr(node->right, old)) == NULL)
	{
		show_message(ses, LIST_VARIABLE, "#REPLACESTRING: {%s} NOT FOUND IN {%s}", old, node->right);
	}
	else
	{
		pti = node->right;
		pto = var;

		while ((ptr = strstr(pti, old)) != NULL)
		{
			while (pti != ptr)
			{
				*pto++ = *pti++;
			}

			strcpy(pto, new);
			pto += strlen(new);

			pti += strlen(old);
		}

		while (*pti)
		{
			*pto++ = *pti++;
		}

		*pto = 0;

		sprintf(result, "{%s} {%s}", node->left, var);

		show_message(ses, LIST_VARIABLE, "#REPLACESTRING: $%s IS NOW SET TO {%s}", node->left, var);

		do_internal_variable(ses, result);
	}
	return ses;
}


/*
	support routines for #format - Igor
*/

void colorstring(char *str)
{
	char result[BUFFER_SIZE] = { 0 }, *pti;
	int cnt;

	pti = str;

	for (pti = str ; *pti ; pti++)
	{
		for (cnt = 0 ; *color_table[cnt].name ; cnt++)
		{
			if (is_abbrev(color_table[cnt].name, pti))
			{
				strcat(result, color_table[cnt].code);
				pti += strlen(color_table[cnt].name);
				break;
			}
		}
	}
	strcpy(str, result);
}

void headerstring(char *str)
{
	char buf[BUFFER_SIZE];

	if (strlen(str) > gtd->ses->cols - 2)
	{
		str[gtd->ses->cols - 2] = 0;
	}

	memset(buf, '#', gtd->ses->cols);

	memcpy(&buf[(gtd->ses->cols - strlen(str)) / 2], str, strlen(str));

	buf[gtd->ses->cols] = 0;

	strcpy(str, buf);
}

void lowerstring(char *str)
{
	char *pts;

	for (pts = str ; *pts ; pts++)
	{
		*pts = tolower(*pts);
	}
}

void upperstring(char *str)
{
	char *pts;

	for (pts = str ; *pts ; pts++)
	{
		*pts = toupper(*pts);
	}
}

void reversestring(char *str)
{
	char t;
	int a = 0, z = strlen(str) - 1;

	while (z > a)
	{
		t = str[z];
		str[z--] = str[a];
		str[a++] = t;
	}
}

void mathstring(struct session *ses, char *str)
{
	get_number_string(ses, str, str);
}

void thousandgroupingstring(char *str)
{
	char result[BUFFER_SIZE], strold[BUFFER_SIZE];
	int cnt1, cnt2, cnt3;

	sprintf(strold, "%lld", atoll(str));

	cnt1 = strlen(strold);
	cnt2 = strlen(strold) + (strlen(strold) - 1 - (atoll(strold) < 0)) / 3;

	for (cnt3 = 0 ; cnt2 >= 0 ; cnt1--, cnt2--, cnt3++)
	{
		result[cnt2] = strold[cnt1];

		if (cnt3 && cnt3 % 3 == 0)
		{
			result[--cnt2] = ',';
		}
	}

	if (atoll(strold) < 0)
	{
		result[0] = '-';
	}

	strcpy(str, result);
}


void stripspaces(char *str)
{
	int cnt;

	for (cnt = strlen(str) - 1 ; cnt >= 0 ; cnt--)
	{
		if (!isspace(str[cnt]))
		{
			break;
		}
		str[cnt] = 0;
	}

	for (cnt = 0 ; str[cnt] != 0 ; cnt++)
	{
		if (!isspace(str[cnt]))
		{
			break;
		}
	}
	strcpy(str, &str[cnt]);
}


DO_COMMAND(do_format)
{
	char destvar[BUFFER_SIZE], format[BUFFER_SIZE], argument[BUFFER_SIZE], arglist[20][BUFFER_SIZE], *ptf;
	struct tm timeval_tm;
	time_t    timeval_t;
	int i;

	arg = get_arg_in_braces(arg, destvar,  FALSE);
	arg = get_arg_in_braces(arg, format,   FALSE);
	arg = get_arg_in_braces(arg, argument, TRUE);

	if (*destvar == 0 || *format == 0)
	{
		tintin_printf2(ses, "#SYNTAX: #format {variable} {format} {arguments}");

		return ses;
	}

	arg = argument;

	for (i = 0 ; i < 20 ; i++)
	{
		arg = get_arg_in_braces(arg, arglist[i], FALSE);
	}

	i = 0;

	for (ptf = format ; *ptf ; ptf++)
	{
		if (i == 20)
		{
			break;
		}

		if (*ptf == '%')
		{
			ptf++;

			if (*ptf == 0)
			{
				break;
			}
			else if (*ptf == '%')
			{
				ptf++;
			}
			else
			{
				while (!isalpha(*ptf))
				{
					ptf++;
				}

				if (*ptf == 0)
				{
					break;
				}

				switch (*ptf)
				{
					case 'a':
						sprintf(arglist[i], "%c", atoi(arglist[i]));
						break;

					case 'c':
						colorstring(arglist[i]);
						break;

					case 'd':
						timeval_t  = (time_t) atoi(arglist[i]);
						timeval_tm = *localtime(&timeval_t);
						strftime(arglist[i], BUFFER_SIZE, "%d-%m-%Y", &timeval_tm);
						break;

					case 'h':
						headerstring(arglist[i]);
						break;

					case 'l':
						lowerstring(arglist[i]);
						break;

					case 'm':
						mathstring(ses, arglist[i]);
						break;

					case 'n':
						arglist[i][0] = toupper(arglist[i][0]);
						break;

					case 'p':
						stripspaces(arglist[i]);
						break;

					case 'r':
						reversestring(arglist[i]);
						break;

					case 's':
						break;

					case 't':
						timeval_t  = (time_t) atoi(arglist[i]);
						timeval_tm = *localtime(&timeval_t);
						strftime(arglist[i], BUFFER_SIZE, "%H:%M", &timeval_tm);
						break;

					case 'u':
						upperstring(arglist[i]);
						break;


					case 'C':
						sprintf(arglist[i], "%d", ses->cols);
						break;

					case 'D':
						timeval_t  = (time_t) atoi(arglist[i]);
						timeval_tm = *localtime(&timeval_t);
						strftime(arglist[i], BUFFER_SIZE, "%d", &timeval_tm);
						break;

					case 'G':
						thousandgroupingstring(arglist[i]);
						break;

					case 'L':
						sprintf(arglist[i], "%d", (int) strlen(arglist[i]));
						break;

					case 'M':
						timeval_t  = (time_t) atoi(arglist[i]);
						timeval_tm = *localtime(&timeval_t);
						strftime(arglist[i], BUFFER_SIZE, "%m", &timeval_tm);
						break;

					case 'R':
						sprintf(arglist[i], "%d", ses->rows);
						break;

					case 'T':
						sprintf(arglist[i], "%d", (int) time(NULL));
						break;

					case 'U':
						sprintf(arglist[i], "%lld", utime());
						break;

					case 'Y':
						timeval_t  = (time_t) atoi(arglist[i]);
						timeval_tm = *localtime(&timeval_t);
						strftime(arglist[i], BUFFER_SIZE, "%Y", &timeval_tm);
						break;

					default:
						show_message(ses, LIST_VARIABLE, "#FORMAT: UNKNOWN ARGUMENT {%%%c}", *ptf);
						break;
				}
				*ptf = 's';
				i++;
			}
		}
	}

	sprintf(argument, format, arglist[0], arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6], arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12], arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18], arglist[19]);

	sprintf(format, "{%s} {%s}", destvar, argument);

	show_message(ses, LIST_VARIABLE, "#FORMAT: $%s IS NOW SET TO {%s}", destvar, argument);

	do_internal_variable(ses, format);

	return ses;
}
