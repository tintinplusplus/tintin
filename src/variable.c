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
*                     recoded by Igor van den Hoven 2004                      *
******************************************************************************/

#include "tintin.h"

DO_COMMAND(do_variable)
{
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE];

	struct listroot *root = ses->list[LIST_VARIABLE];

	arg = sub_arg_in_braces(ses, arg, arg1, GET_NST, SUB_VAR|SUB_FUN);
	      sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
	{
		show_list(root, 0);
	}
	else if (*arg1 && *arg == 0)
	{
		struct listnode *node = search_nest_node(root, arg1);

		if (node)
		{
			show_node(root, node, 0);
		}
		else if (show_node_with_wild(ses, arg1, LIST_VARIABLE) == FALSE)
		{
			show_message(ses, LIST_VARIABLE, "#VARIABLE: NO MATCH(ES) FOUND FOR {%s}.", arg1);
		}
	}
	else
	{
		set_nest_node(root, arg1, "%s", arg2);

		show_message(ses, LIST_VARIABLE, "#OK. VARIABLE {%s} HAS BEEN SET TO {%s}.", arg1, arg2);
	}
	return ses;
}

DO_COMMAND(do_unvariable)
{
	char arg1[BUFFER_SIZE];

	get_arg_in_braces(arg, arg1, TRUE);

	if (delete_nest_node(ses->list[LIST_VARIABLE], arg1))
	{
		show_message(ses, LIST_VARIABLE, "#OK. {%s} IS NO LONGER A VARIABLE.", arg1);
	}
	else
	{
		delete_node_with_wild(ses, LIST_VARIABLE, arg1);
	}
	return ses;
}


DO_COMMAND(do_replace)
{
	char arg1[BUFFER_SIZE], arg2[BUFFER_SIZE], arg3[BUFFER_SIZE], buf[STRING_SIZE], *pti, *ptr, *pto;
	struct listroot *root;
	struct listnode *node;

	root = ses->list[LIST_VARIABLE];

	arg = get_arg_in_braces(arg, arg1, GET_NST);
	arg = sub_arg_in_braces(ses, arg, arg2, GET_ONE, SUB_VAR|SUB_FUN|SUB_ESC);
	arg = sub_arg_in_braces(ses, arg, arg3, GET_ALL, SUB_VAR|SUB_FUN);

	if (*arg1 == 0 || *arg2 == 0)
	{
		show_message(ses, LIST_VARIABLE, "#Syntax: #replace <var> <old text> <new text>", ses);
	}
	else if ((node = search_nest_node(root, arg1)) == NULL)
	{
		show_message(ses, LIST_VARIABLE, "#REPLACE: VARIABLE {%s} NOT FOUND.", arg1);
	}
	else if ((ptr = strstr(node->right, arg2)) == NULL)
	{
		show_message(ses, LIST_VARIABLE, "#REPLACE: {%s} NOT FOUND IN {%s}.", arg2, node->right);
	}
	else
	{
		pti = node->right;
		pto = buf;

		while ((ptr = strstr(pti, arg2)) != NULL)
		{
			while (pti != ptr)
			{
				*pto++ = *pti++;
			}

			strcpy(pto, arg3);
			pto += strlen(arg3);

			pti += strlen(arg2);
		}

		while (*pti)
		{
			*pto++ = *pti++;
		}

		*pto = 0;

		set_nest_node(ses->list[LIST_VARIABLE], arg1, "%s", buf);
	}
	return ses;
}


/*
	support routines for #format - Igor
*/

void colorstring(char *str)
{
	char result[BUFFER_SIZE], *pti;
	int cnt;

	result[0] = 0;

	for (pti = str ; *pti ; pti++)
	{
		for (cnt = 0 ; *color_table[cnt].name ; cnt++)
		{
			if (is_abbrev(color_table[cnt].name, pti))
			{
				strcat(result, color_table[cnt].code);
				pti += strlen(color_table[cnt].name) - 1;

				break;
			}
		}
	}
	strcpy(str, result);
}

void headerstring(char *str)
{
	char buf[BUFFER_SIZE];

	if ((int) strlen(str) > gtd->ses->cols - 2)
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

void wrapstring(struct session *ses, char *str)
{
	char *pti, *lis, *sos, *soc, buf[BUFFER_SIZE], color[BUFFER_SIZE], tmp[BUFFER_SIZE];
	int col = 1, cnt = 1;

	push_call("wrapstring(%p,%p)",ses,str);

	pti = lis = sos = soc = str;

	buf[0] = color[0] = 0;

	while (*pti != 0)
	{
		if (skip_vt102_codes(pti))
		{
			pti += skip_vt102_codes(pti);

			continue;
		}

		if (*pti == ' ')
		{
			lis = pti;
		}

		if (col > ses->cols)
		{
			col = 1;

			if (pti - lis > 15)
			{
				sprintf(tmp, "%.*s", sos - soc, soc);

				get_color_codes(color, tmp, color);

				cat_sprintf(buf, "{%d}{%s%.*s}", cnt++, color, pti - sos, sos);

				soc = sos;
				lis = sos = pti;
			}
			else
			{
				sprintf(tmp, "%.*s", sos - soc, soc);

				get_color_codes(color, tmp, color);

				cat_sprintf(buf, "{%d}{%s%.*s}", cnt++, color, lis - sos, sos);

				lis++;

				soc = sos;
				pti = sos = lis;
			}
		}
		else
		{
			pti++;
			col++;
		}
	}
	sprintf(tmp, "%.*s", sos - soc, soc);

	get_color_codes(color, tmp, color);

	cat_sprintf(buf, "{%d}{%s%s}", cnt, color, sos);

	strcpy(str, buf);

	pop_call();
	return;
}

int stringlength(struct session *ses, char *str)
{
	substitute(ses, str, str, SUB_VAR|SUB_FUN|SUB_COL);

	return strip_vt102_strlen(str);
}

void timestring(struct session *ses, char *str)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], *arg;

	struct tm timeval_tm;
	time_t    timeval_t;

	arg = get_arg_in_braces(str, left, TRUE);
	arg = get_arg_in_braces(arg, right, TRUE);

	if (*right)
	{
		timeval_t = (time_t) atoll(right);
	}
	else
	{
		timeval_t = (time_t) time(NULL);
	}

	timeval_tm = *localtime(&timeval_t);

	strftime(str, BUFFER_SIZE, left, &timeval_tm);
}

DO_COMMAND(do_format)
{
	char temp[BUFFER_SIZE], destvar[BUFFER_SIZE], format[BUFFER_SIZE], arglist[20][BUFFER_SIZE], *ptf, *ptt;
	struct tm timeval_tm;
	time_t    timeval_t;
	int i;

	substitute(ses, arg, temp, SUB_VAR|SUB_FUN);

	arg = temp;

	arg = get_arg_in_braces(arg, destvar,  GET_NST);
	arg = get_arg_in_braces(arg, format,   GET_ONE);

	if (*destvar == 0 || *format == 0)
	{
		tintin_printf2(ses, "#SYNTAX: #format {variable} {format} {arg1} {arg2}");

		return ses;
	}

	for (i = 0 ; i < 20 ; i++)
	{
		arg = get_arg_in_braces(arg, arglist[i], GET_ONE);
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
			ptt = temp;

			*ptt++ = *ptf++;

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
					*ptt++ = *ptf++;
				}

				if (*ptf == 0)
				{
					break;
				}

				switch (*ptf)
				{
					case 'a':
						sprintf(arglist[i], "%c", (char) get_number(ses, arglist[i]));
						break;

					case 'c':
						colorstring(arglist[i]);
						break;

					case 'd':
						strcpy(ptt, "lld");
						sprintf(arglist[i], temp, (long long) get_number(ses, arglist[i]));
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
						timestring(ses, arglist[i]);
						break;

					case 'u':
						upperstring(arglist[i]);
						break;

					case 'w':
						substitute(ses, arglist[i], arglist[i], SUB_VAR|SUB_FUN|SUB_COL|SUB_ESC);
						wrapstring(ses, arglist[i]);
						break;

					case 'C':
						sprintf(arglist[i], "%d", ses->cols);
						break;

					case 'D':
						timeval_t  = (time_t) *arglist[i] ? atoll(arglist[i]) : time(NULL);
						timeval_tm = *localtime(&timeval_t);
						strftime(arglist[i], BUFFER_SIZE, "%d", &timeval_tm);
						break;

					case 'G':
						thousandgroupingstring(arglist[i]);
						break;

					case 'L':
						sprintf(arglist[i], "%d", stringlength(ses, arglist[i]));
						break;

					case 'M':
						timeval_t  = (time_t) *arglist[i] ? atoll(arglist[i]) : time(NULL);
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
						timeval_t  = (time_t) *arglist[i] ? atoll(arglist[i]) : time(NULL);
						timeval_tm = *localtime(&timeval_t);
						strftime(arglist[i], BUFFER_SIZE, "%Y", &timeval_tm);
						break;

					default:
						show_message(ses, LIST_VARIABLE, "#FORMAT: UNKNOWN ARGUMENT {%%%c}.", *ptf);
						break;
				}
				*ptf = 's';
				i++;
			}
		}
	}

	sprintf(temp, format, arglist[0], arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6], arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12], arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18], arglist[19]);

	set_nest_node(ses->list[LIST_VARIABLE], destvar, "%s", temp);

	return ses;
}
