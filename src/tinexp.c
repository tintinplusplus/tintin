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
*   file: regexp.c - funtions related to tintin's regexp                      *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                        coded by peter unold 1992                            *
*                   recoded by Igor van den Hoven 2004                        *
******************************************************************************/

#include "tintin.h"


/******************************************************************************
* Simple regexp that skips vt102 codes and supports the * and ? wildcards     *
******************************************************************************/

int regexp(const char *exp, const char *str)
{
	short cnt;

	for ( ; *exp != 0 ; exp++, str++)
	{
		while ((cnt = skip_vt102_codes(str)) != 0)
		{
			str += cnt;
		}

		switch (*exp)
		{
			case '?':
				if (*str == 0)
				{
					return FALSE;
				}
				break;

			case '*':
				for (cnt = strlen(str) ; cnt >= 0 ; cnt--)
				{
					if (regexp(exp + 1, &str[cnt]))
					{
						return TRUE;
					}
				}
				return FALSE;

			case '\\':
				if (*++exp != *str)
				{
					return FALSE;
				}
				break;

			default:

				if (*exp != *str)
				{
					return FALSE;
				}

				break;
		}
	}
	return (*exp == *str);
}

/******************************************************************************
* copy *string into *result, but substitute the various expressions with the  *
* values they stand for.                                                      *
******************************************************************************/

void substitute(struct session *ses, const char *string, char *result, int flags)
{
	struct listnode *node;
	char temp[BUFFER_SIZE], funcargs[BUFFER_SIZE], *pti, *pto, *ptv, *ptf;
	const char *pte;
	short i, cnt;

	pti = (char *) string;
	pto = (char *) result;

	while (TRUE)
	{
		switch (*pti)
		{
			case '\0':
				if (HAS_BIT(flags, SUB_EOL))
				{
					*pto++ = '\r';
					*pto++ = '\n';
				}
				*pto = 0;
				return;

			case '$':
				if (HAS_BIT(flags, SUB_VAR) && (pti[1] == DEFAULT_OPEN || isalnum(pti[1])))
				{
					if (pti[1] == DEFAULT_OPEN)
					{
						for (ptv = temp, i = 2 ; pti[i] && pti[i] != DEFAULT_CLOSE ; i++)
						{
							*ptv++ = pti[i];
						}

						if (pti[i] == DEFAULT_CLOSE)
						{
							*ptv = 0;
						}
						else
						{
							temp[0] = 0;
						}
					}

					if (isalnum(pti[1]))
					{
						for (ptv = temp, i = 1 ; isalnum(pti[i]) ; i++)
						{
							*ptv++ = pti[i];
						}
						*ptv = 0;
					}

					if ((node = searchnode_list(ses->list[LIST_VARIABLE], temp)) != NULL)
					{
						substitute(ses, node->right, pto, flags - SUB_VAR);

						if (pti[1] == DEFAULT_OPEN)
						{
							pto += strlen(pto);
							pti += 3 + strlen(temp);
						}
						else
						{
							pto += strlen(pto);
							pti += 1 + strlen(temp);
						}
					}
					else
					{
						*pto++ = *pti++;
					}
				}
				else
				{
					*pto++ = *pti++;
				}
				break;

			case '<':
				if (HAS_BIT(flags, SUB_COL) && isdigit(pti[1]) && isdigit(pti[2]) && isdigit(pti[3]) && pti[4] == '>')
				{
					if (pti[1] != '8' || pti[2] != '8' || pti[3] != '8')
					{
						*pto++ = '\033';
						*pto++ = '[';

						switch (pti[1])
						{
							case '2':
								*pto++ = '2';
								*pto++ = '2';
								*pto++ = ';';
								break;
							case '8':
								break;
							default:
								*pto++ = pti[1];
								*pto++ = ';';
						}
						switch (pti[2])
						{
							case '8':
								break;
							default:
								*pto++ = '3';
								*pto++ = pti[2];
								*pto++ = ';';
								break;
						}
						switch (pti[3])
						{
							case '8':
								break;
							default:
								*pto++ = '4';
								*pto++ = pti[3];
								*pto++ = ';';
								break;
						}
						pto--;
						*pto++ = 'm';
					}
					pti += 5;
				}
				else
				{
					*pto++ = *pti++;
				}
				break;

			case '@':
				if (HAS_BIT(flags, SUB_FUN))
				{
					for (ptf = temp, i = 1 ; isalnum(pti[i]) ; i++)
					{
						*ptf++ = pti[i];
					}
					*ptf = 0;

					node = searchnode_list(ses->list[LIST_FUNCTION], temp);

					if (node == NULL || pti[i] != DEFAULT_OPEN)
					{
						*pto++ = *pti++;

						continue;
					}

					pti = (char *) get_arg_in_braces(&pti[i], funcargs, FALSE);
					pte = funcargs;

					for (i = 0 ; i < 10 ; i++)
					{
						pte = get_arg_in_braces(pte, temp, FALSE);

						substitute(ses, temp, gtd->cmds[i], HAS_BIT(flags, SUB_ARG) ? SUB_ARG|SUB_FUN : SUB_FUN);
					}
					substitute(ses, node->right, temp, SUB_CMD);

					parse_input(temp, ses);

					substitute(ses, "$result", temp, SUB_VAR);
					substitute(ses, temp, pto, flags - SUB_FUN);

					if (HAS_BIT(ses->list[LIST_FUNCTION]->flags, LIST_FLAG_DEBUG))
					{
						tintin_printf2(ses, "#FUNCTION DEBUG: (%s) (%s)", node->left, pto);
					}
					pto += strlen(pto);
				}
				else
				{
					*pto++ = *pti++;
				}
				break;

			case '%':
				if (HAS_BIT(flags, SUB_ARG) && isdigit(pti[1]))
				{
					for (cnt = 0 ; gtd->vars[pti[1] - '0'][cnt] ; cnt++)
					{
						if (HAS_BIT(flags, SUB_SEC))
						{
							switch (gtd->vars[pti[1] - '0'][cnt])
							{
								case ';':
									*pto++ = '\\';
									*pto++ = ';';
									break;

								case '\\':
									*pto++ = '\\';
									*pto++ = '\\';
									break;

								default:
									*pto++ = gtd->vars[pti[1] - '0'][cnt];
									break;
							}
						}
						else
						{
							*pto++ = gtd->vars[pti[1] - '0'][cnt];
						}
					}
					pti += 2;
				}
				else
				{
					*pto++ = *pti++;
				}
				break;

			case '^':
				if (HAS_BIT(flags, SUB_ANC) && (pti == string || pti[1] == 0))
				{
					pti++;
				}
				else
				{
					*pto++ = *pti++;
				}
				break;

			case '~':
				if (HAS_BIT(flags, SUB_ANC) && pti == string)
				{
					string++;
					pti++;
				}
				else
				{
					*pto++ = *pti++;
				}
				break;

			case '&':
				if (HAS_BIT(flags, SUB_CMD) && isdigit(pti[1]))
				{
					for (cnt = 0 ; gtd->cmds[pti[1] - '0'][cnt] ; cnt++)
					{
						*pto++ = gtd->cmds[pti[1] - '0'][cnt];
					}
					pti += 2;
				}
				else
				{
					*pto++ = *pti++;
				}
				break;

			case '\\':
				if (HAS_BIT(flags, SUB_ESC))
				{
					pti++;

					switch (*pti)
					{
						case 'a':
							*pto++ = '\a';
							break;
						case 'e':
							*pto++ = '\e';
							break;
						case 'n':
							*pto++ = '\n';
							break;
						case 'r':
							*pto++ = '\r';
							break;
						case 't':
							*pto++ = '\t';
							break;
						case '[':
							*pto++ = '{';
							break;
						case ']':
							*pto++ = '}';
							break;
						case '\0':
							*pto = 0;
							return;
						default:
							*pto++ = *pti;
							break;
					}
					pti++;
				}
				else
				{
					*pto++ = *pti++;
				}
				break;

			default:
				*pto++ = *pti++;
				break;
		}
	}
}


/******************************************************************************
* Calls action_regexp checking if the string matches, and automatically fills *
* in the text represented by the wildcards on success. Per default the entire *
* string is compared unless it starts or ends with the ^ character.           *
******************************************************************************/

int check_one_action(const char *line, const char *original, const char *action, struct session *ses)
{
	char result[BUFFER_SIZE], *ptr;
	const char *ptl;

	substitute(ses, action, result, SUB_VAR|SUB_FUN|SUB_ESC);

	ptr = result;

	if (*ptr == '~')
	{
		ptr++;
		ptl = original;
	}
	else
	{
		ptl = line;
	}

	if (*ptr == '^')
	{
		ptr++;
		return action_regexp(ptr, ptl);
	}

	if (ptr[0] == '%' && isdigit(ptr[1]))
	{
		return action_regexp(ptr, ptl);
	}

	while (*ptl)
	{
		if (*ptr == *ptl && action_regexp(ptr, ptl))
		{
			return TRUE;
		}
		ptl++;
	}
	return FALSE;
}


int action_regexp(const char *exp, const char *str)
{
	short cnt;

	while (*exp)
	{
		switch (exp[0])
		{
			case '%':
				if (exp[1] >= '0' && exp[1] <= '9')
				{
					for (cnt = strlen(str) ; cnt >= 0 ; cnt--)
					{
						if (action_regexp(exp + 2, &str[cnt]))
						{
							memcpy(gtd->vars[exp[1] - '0'], str, cnt);
							gtd->vars[exp[1] - '0'][cnt] = 0;

							return TRUE;
						}
					}
					return FALSE;
				}
				break;

			case '^':
				if (exp[1] == 0)
				{
					return (exp[1] == str[0]);
				}
				break;
		}

		if (*exp != *str)
		{
			return FALSE;
		}
		exp++;
		str++;
	}
	return TRUE;
}
