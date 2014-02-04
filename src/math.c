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

#define EXP_VARIABLE         0
#define EXP_STRING           1
#define EXP_OPERATOR         2

#define EXP_PR_DICE          0
#define EXP_PR_INTMUL        1
#define EXP_PR_INTADD        2
#define EXP_PR_BITSHIFT      3
#define EXP_PR_LOGLTGT       4
#define EXP_PR_LOGCOMP       5
#define EXP_PR_BITAND        6
#define EXP_PR_BITXOR        7
#define EXP_PR_BITOR         8
#define EXP_PR_LOGAND        9
#define EXP_PR_LOGXOR       10
#define EXP_PR_LOGOR        11
#define EXP_PR_VAR          12
#define EXP_PR_LVL          13

struct listnode *mathnode_s;
struct listnode *mathnode_e;
struct session  *mathses;
int precision;

DO_COMMAND(do_math)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], result[BUFFER_SIZE];
	struct listroot *root;

	root = ses->list[LIST_VARIABLE];

	arg = get_arg_in_braces(arg, left,  FALSE);
	arg = get_arg_in_braces(arg, right, TRUE);

	if (*left == 0 || *right == 0)
	{
		tintin_printf2(ses, "#SYNTAX: #MATH {variable} {expression}");
	}
	else
	{
		sprintf(result, "{%s} {%.*f}", left, precision, mathexp(ses, right));

		do_internal_variable(ses, result);
	}
	return ses;
}


DO_COMMAND(do_if)
{
	char left[BUFFER_SIZE], true[BUFFER_SIZE], false[BUFFER_SIZE], temp[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, true,  1);
	arg = get_arg_in_braces(arg, false, 1);

	if (*left == 0 || *true == 0)
	{
		tintin_printf2(ses, "#SYNTAX: #IF {expression} {true} {false}");
	}
	else
	{
		substitute(ses, left, temp, SUB_VAR|SUB_FUN);

		if (mathexp(ses, temp))
		{
			ses = parse_input(ses, true);
		}
		else if (*false)
		{
			ses = parse_input(ses, false);
		}
	}
	return ses;
}

double get_number(struct session *ses, char *str)
{
	char number[BUFFER_SIZE];

	substitute(ses, str, number, SUB_VAR|SUB_FUN);

	return mathexp(ses, number);
}

void get_number_string(struct session *ses, char *str, char *result)
{
	char number[BUFFER_SIZE];

	substitute(ses, str, number, SUB_VAR|SUB_FUN);

	sprintf(result, "%.*f", precision, mathexp(ses, number));
}

/*
	Flexible tokenized mathematical expression interpreter - Igor
*/

double mathexp(struct session *ses, char *str)
{
	struct listnode *node;

	mathses = ses;

	if (mathexp_tokenize(ses, str) == FALSE)
	{
		tintin_printf2(ses, "#MATH EXP: INVALID INPUT {%s}", str);

		return 0;
	}

	node = mathses->list[LIST_MATH]->f_node;

	while (node->prev || node->next)
	{
		if (node->next == NULL || atoi(node->next->left) < atoi(node->left))
		{
			mathexp_level(node);

			node = mathses->list[LIST_MATH]->f_node;
		}
		else
		{
			node = node->next;
		}
	}
	return tintoi(node->pr);
}

#define MATH_NODE(buffercheck, priority, newstatus)               \
{                                                                 \
	if (buffercheck && (pta == buf3 || valid == FALSE))          \
	{                                                            \
		return FALSE;                                           \
	}                                                            \
	*pta = 0;                                                    \
	sprintf(buf1, "%02d", level);                                \
	sprintf(buf2, "%02d", priority);                             \
	insertnode_list(mathses, buf1, buf2, buf3, LIST_MATH);       \
	status = newstatus;                                          \
	pta = buf3;                                                  \
	valid = FALSE;                                               \
	point = -1;                                                  \
}

int mathexp_tokenize(struct session *ses, char *str)
{
	char buf1[BUFFER_SIZE], buf2[BUFFER_SIZE], buf3[BUFFER_SIZE], *pti, *pta;
	int level, status, valid, point;

	level     = 0;
	point     = -1;
	status    = EXP_VARIABLE;
	valid     = FALSE;
	precision = 0;

	pta = buf3;
	pti = str;

	kill_list(ses, LIST_MATH);

	while (*pti)
	{
		switch (status)
		{
			case EXP_VARIABLE:
				switch (*pti)
				{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						*pta++ = *pti++;
						valid = TRUE;

						if (point >= 0)
						{
							point++;

							if (precision < point)
							{
								precision = point;
							}
						}
						break;

					case '!':
					case '~':
					case '+':
					case '-':
						if (pta != buf3)
						{
							MATH_NODE(FALSE, EXP_PR_VAR, EXP_OPERATOR);
						}
						else
						{
							*pta++ = *pti++;
						}
						break;

					case '"':
						if (pta != buf3)
						{
							tintin_printf2(NULL, "MATH EXP: \" FOUND INSIDE A NUMBER");
							return FALSE;
						}
						*pta++ = *pti++;
						status = EXP_STRING;
						break;

					case '(':
						if (pta != buf3)
						{
							tintin_printf2(NULL, "#MATH EXP: PARANTESES FOUND INSIDE A NUMBER");
							return FALSE;
						}
						*pta++ = *pti++;
						MATH_NODE(FALSE, EXP_PR_LVL, EXP_VARIABLE);
						level++;
						break;

					case ',':
						*pti++;
						break;

					case '.':
						*pta++ = *pti++;
						if (point >= 0)
						{
							tintin_printf2(NULL, "#MATH EXP: MORE THAN ONE POINT FOUND INSIDE A NUMBER");
							return FALSE;
						}
						point++;
						break;

					case ' ':
						pti++;
						break;

					default:
						MATH_NODE(TRUE, EXP_PR_VAR, EXP_OPERATOR);
						break;
				}
				break;

			case EXP_STRING:
				switch (*pti)
				{
					case '"':
						*pta++ = *pti++;
						valid = TRUE;
						MATH_NODE(FALSE, EXP_PR_VAR, EXP_OPERATOR);
						break;

					default:
						*pta++ = *pti++;
						break;
				}
				break;

			case EXP_OPERATOR:
				switch (*pti)
				{
					case ' ':
						pti++;
						break;

					case ')':
						*pta++ = *pti++;
						level--;
						MATH_NODE(FALSE, EXP_PR_LVL, EXP_OPERATOR);
						break;

					case 'd':
						*pta++ = *pti++;
						MATH_NODE(FALSE, EXP_PR_DICE, EXP_VARIABLE);
						break;

					case '*':
					case '/':
					case '%':
						*pta++ = *pti++;
						MATH_NODE(FALSE, EXP_PR_INTMUL, EXP_VARIABLE);
						break;

					case '+':
					case '-':
						*pta++ = *pti++;
						MATH_NODE(FALSE, EXP_PR_INTADD, EXP_VARIABLE);
						break;

					case '<':
						*pta++ = *pti++;

						switch (*pti)
						{
							case '<':
								*pta++ = *pti++;
								MATH_NODE(FALSE, EXP_PR_BITSHIFT, EXP_VARIABLE);
								break;

							case '=':
								*pta++ = *pti++;
								MATH_NODE(FALSE, EXP_PR_LOGLTGT, EXP_VARIABLE);
								break;

							default:
								MATH_NODE(FALSE, EXP_PR_LOGLTGT, EXP_VARIABLE);
								break;
						}
						break;

					case '>':
						*pta++ = *pti++;

						switch (*pti)
						{
							case '>':
								*pta++ = *pti++;
								MATH_NODE(FALSE, EXP_PR_BITSHIFT, EXP_VARIABLE);
								break;

							case '=':
								*pta++ = *pti++;
								MATH_NODE(FALSE, EXP_PR_LOGLTGT, EXP_VARIABLE);
								break;

							default:
								MATH_NODE(FALSE, EXP_PR_LOGLTGT, EXP_VARIABLE);
								break;
						}
						break;

					case '&':
						*pta++ = *pti++;

						switch (*pti)
						{
							case '&':
								*pta++ = *pti++;
								MATH_NODE(FALSE, EXP_PR_LOGAND, EXP_VARIABLE);
								break;

							default:
								MATH_NODE(FALSE, EXP_PR_BITAND, EXP_VARIABLE);
								break;
						}
						break;

					case '^':
						*pta++ = *pti++;

						switch (*pti)
						{
							case '^':
								*pta++ = *pti++;
								MATH_NODE(FALSE, EXP_PR_LOGXOR, EXP_VARIABLE);
								break;

							default:
								MATH_NODE(FALSE, EXP_PR_BITXOR, EXP_VARIABLE);
								break;

						}
						break;

					case '|':
						*pta++ = *pti++;

						switch (*pti)
						{
							case '|':
								*pta++ = *pti++;
								MATH_NODE(FALSE, EXP_PR_LOGOR, EXP_VARIABLE);
								break;

							default:
								MATH_NODE(FALSE, EXP_PR_BITOR, EXP_VARIABLE);
								break;
						}
						break;

					case '=':
					case '!':
						*pta++ = *pti++;
						switch (*pti)
						{
							case '=':
								*pta++ = *pti++;
								MATH_NODE(FALSE, EXP_PR_LOGCOMP, EXP_VARIABLE);
								break;

							default:
								tintin_printf2(ses, "#MATH EXP: UNKNOWN OPERATOR: %c%c", pti[-1], pti[0]);
								return FALSE;
						}
						break;

					default:
						tintin_printf2(NULL, "#MATH EXP: UNKNOWN OPERATOR: %c", *pti);
						return FALSE;
				}
				break;
		}
	}

	if (level != 0)
	{
		tintin_printf2(NULL, "#MATH EXP: UNMATCHED PARENTHESES, LEVEL: %d", level);
		return FALSE;
	}

	if (status != EXP_OPERATOR)
	{
		MATH_NODE(TRUE, EXP_PR_VAR, EXP_OPERATOR);
	}

	return TRUE;
}


void mathexp_level(struct listnode *node)
{
	int priority;

	mathnode_e = node;

	while (node->prev)
	{
		if (atoi(node->prev->left) < atoi(node->left))
		{
			break;
		}
		node = node->prev;
	}

	mathnode_s = node;

	for (priority = 0 ; priority < EXP_PR_VAR ; priority++)
	{
		for (node = mathnode_s ; node ; node = node->next)
		{
			if (atoi(node->right) == priority)
			{
				mathexp_compute(node);
			}
			if (node == mathnode_e)
			{
				break;
			}
		}
	}

	node = mathnode_s;

	while (node->prev && node->next)
	{
		if (atoi(node->prev->right) == EXP_PR_LVL && atoi(node->next->right) == EXP_PR_LVL)
		{
			free(node->left);
			node->left = strdup(node->next->left);

			deletenode_list(mathses, node->next, LIST_MATH);
			deletenode_list(mathses, node->prev, LIST_MATH);
		}
		else
		{
			break;
		}
	}
	return;
}

void mathexp_compute(struct listnode *node)
{
	char temp[BUFFER_SIZE];
	double value;

	switch (node->pr[0])
	{
		case 'd':
			if (tintoi(node->next->pr) <= 0)
			{
				tintin_printf2(NULL, "#MATHEXP: INVALID DICE: %s", node->next->pr);
				value = 0;
			}
			else
			{
				value = tindice(node->prev->pr, node->next->pr);
			}
			break;
		case '*':
			value = tintoi(node->prev->pr) * tintoi(node->next->pr);
			break;
		case '/':
			if (tintoi(node->next->pr) == 0)
			{
				tintin_printf2(NULL, "#MATH ERROR: DIVISION ZERO.");
				value = tintoi(node->prev->pr);
			}
			else
			{
				if (precision)
				{
					value = tintoi(node->prev->pr) / tintoi(node->next->pr);
				}
				else
				{
					value = (long long) tintoi(node->prev->pr) / (long long) tintoi(node->next->pr);
				}
			}
			break;
		case '%':
			if (tintoi(node->next->pr) == 0)
			{
				tintin_printf2(NULL, "#MATH ERROR: MODULO ZERO.");
				value = tintoi(node->prev->pr);
			}
			else
			{
				value = (long long) tintoi(node->prev->pr) % (long long) tintoi(node->next->pr);
			}
			break;
		case '+':
			value = tintoi(node->prev->pr) + tintoi(node->next->pr);
			break;
		case '-':
			value = tintoi(node->prev->pr) - tintoi(node->next->pr);
			break;
		case '<':
			switch (node->pr[1])
			{
				case '=':
					value = tincmp(node->prev->pr, node->next->pr) <= 0;
					break;
				case '<':
					value = (long long) tintoi(node->prev->pr) << (long long) tintoi(node->next->pr);
					break;
				default:
					value = tincmp(node->prev->pr, node->next->pr) < 0;
					break;
			}
			break;
		case '>':
			switch (node->pr[1])
			{
				case '=':
					value = tincmp(node->prev->pr, node->next->pr) >= 0;
					break;
				case '>':
					value = (long long) tintoi(node->prev->pr) >> (long long) tintoi(node->next->pr);
					break;
				default:
					value = tincmp(node->prev->pr, node->next->pr) > 0;
					break;
			}
			break;

		case '&':
			switch (node->pr[1])
			{
				case '&':
					value = tintoi(node->prev->pr) && tintoi(node->next->pr);
					break;
				default:
					value = (long long) tintoi(node->prev->pr) & (long long) tintoi(node->next->pr);
					break;
			}
			break;
		case '^':
			switch (node->pr[1])
			{
				case '^':
					value = ((tintoi(node->prev->pr) || tintoi(node->next->pr)) && (!tintoi(node->prev->pr) != !tintoi(node->next->pr)));
					break;

				default:
					value = (long long) tintoi(node->prev->pr) ^ (long long) tintoi(node->next->pr);
					break;
			}
			break;
		case '|':
			switch (node->pr[1])
			{
				case '|':
					value = tintoi(node->prev->pr) || tintoi(node->next->pr);
					break;
				default:
					value = (long long) tintoi(node->prev->pr) | (long long) tintoi(node->next->pr);
					break;
			}
			break;
		case '=':
			value = (tineval(node->next->pr, node->prev->pr) != 0);
			break;
		case '!':
			value = (tineval(node->next->pr, node->prev->pr) == 0);
			break;
		default:
			tintin_printf2(NULL, "#COMPUTE EXP: UNKNOWN OPERATOR: %c%c", node->pr[0], node->pr[1]);
			value = 0;
			break;
	}

	if (node->prev == mathnode_s)
	{
		mathnode_s = node;
	}

	if (node->next == mathnode_e)
	{
		mathnode_e = node;
	}

	deletenode_list(mathses, node->next, LIST_MATH);
	deletenode_list(mathses, node->prev, LIST_MATH);

	sprintf(temp, "%d", EXP_PR_VAR);
	free(node->right);
	node->right = strdup(temp);

	sprintf(temp, "%f", value);
	free(node->pr);
	node->pr = strdup(temp);
}

double tintoi(char *str)
{
	switch (str[0])
	{
		case '!':
			return !atof(&str[1]);

		case '~':
			return (double) ~atoll(&str[1]);

		case '+':
			return +atof(&str[1]);

		case '-':
			return -atof(&str[1]);

		default:
			return atof(str);
	}
}

double tincmp(char *left, char *right)
{
	switch (left[0])
	{
		case '"':
			return strcmp(left, right);

		default:
			return tintoi(left) - tintoi(right);
	}
}

double tineval(char *left, char *right)
{
	switch (left[0])
	{
		case '"':
			return regexp(left, right, TRUE);

		default:
			return tintoi(left) == tintoi(right);
	}
}
			
double tindice(char *left, char *right)
{
	long long cnt, numdice, sizedice, sum;

	numdice  = (long long) tintoi(left);
	sizedice = (long long) tintoi(right);

	for (cnt = sum = 0 ; cnt < numdice ; cnt++)
	{
		sum += lrand48() % sizedice + 1;
	}
	return (double) sum;
}
