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
*                     recoded by Igor van den Hoven 2004                      *
******************************************************************************/

#include <regex.h>

#include "tintin.h"


/******************************************************************************
* Simple regexp that skips vt102 codes and supports the * and ? wildcards     *
******************************************************************************/

int regexp(char *exp, char *str, unsigned char cs)
{
	short cnt;

	for ( ; *exp != 0 ; exp++, str++)
	{
		while ((cnt = skip_vt102_codes(str)) != 0)
		{
			str += cnt;
		}

#ifdef BIG5
		if (*exp & 0x80 && *str & 0x80)
		{
			if (*++exp != *++str)
			{
				return FALSE;
			}
			continue;
		}
#endif

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
					if (regexp(exp + 1, &str[cnt], cs))
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
				if (cs)
				{
					if (*exp != *str)
					{
						return FALSE;
					}
				}
				else
				{
					if (tolower(*exp) != tolower(*str))
					{
						return FALSE;
					}
				}
				break;
		}
	}
	return (*exp == *str);
}

DO_COMMAND(do_regexp)
{
	char *left, *right, *true, *false, *str, *exp;

	arg = get_arg_in_braces(arg, &left,  0);
	arg = get_arg_in_braces(arg, &right, 0);
	arg = get_arg_in_braces(arg, &true,  1);
	arg = get_arg_in_braces(arg, &false, 1);

	if (*left == 0 || *right == 0 || *true == 0)
	{
		tintin_printf2(ses, "SYNTAX: #REGEXP {string} {expression} {true} {false}");
	}
	else
	{
		substitute(ses, left,  &str, SUB_VAR|SUB_FUN);
		substitute(ses, right, &exp, SUB_VAR|SUB_FUN);

		if (regex(exp, str))
		{
			substitute(ses, true,  &true,  SUB_CMD);

			ses = parse_input(ses, true);
		}
		else if (*false)
		{
			ses = parse_input(ses, false);
		}
	}
	return ses;
}
		
int regex(char *exp, char *str)
{
	regex_t regex;
	regmatch_t match[10];
	int i;

	if (regcomp(&regex, exp, REG_EXTENDED))
	{
		return FALSE;
	}

	if (regexec(&regex, str, 10, match, 0))
	{
		regfree(&regex);

		return FALSE;
	}

	for (i = 0 ; i < 10 ; i++)
	{
		if (match[i].rm_so != -1)
		{
			gtd->cmds[i] = stringf_alloc("%.*s", match[i].rm_eo - match[i].rm_so, &str[match[i].rm_so]);
		}
		else
		{
			gtd->cmds[i] = stringf_alloc("");
		}
	}
	regfree(&regex);

	return TRUE;
}

/******************************************************************************
* copy *string into *result, but substitute the various expressions with the  *
* values they stand for.                                                      *
******************************************************************************/

void substitute(struct session *ses, char *string, char **result, int flags)
{
	struct listnode *node;
	char temp[BUFFER_SIZE], buf[BUFFER_SIZE], buffer[STRING_SIZE], *pti, *pto, *ptt, *tmp;
	char *pte;
	int i, cnt, flags_neol = flags;

	pti = string;
	pto = buffer;

	DEL_BIT(flags_neol, SUB_EOL);

	while (TRUE)
	{

#ifdef BIG5
		if (*pti & 0x80)
		{
			*pto++ = *pti++;
			if (*pti)
			{
				*pto++ = *pti++;
			}
			continue;
		}
#endif

		switch (*pti)
		{
			case '\0':
				if (HAS_BIT(flags, SUB_EOL))
				{
					*pto++ = '\r';
					*pto++ = '\n';
				}
				*pto = 0;

				*result = string_alloc(buffer);

				return;

			case '$':
				if (HAS_BIT(flags, SUB_VAR) && (pti[1] == DEFAULT_OPEN || isalnum(pti[1])))
				{
					if (pti[1] == DEFAULT_OPEN)
					{
						for (ptt = buf, i = 2 ; pti[i] && pti[i] != DEFAULT_CLOSE ; i++)
						{
							*ptt++ = pti[i];
						}

						if (pti[i] == DEFAULT_CLOSE)
						{
							*ptt = 0;
							substitute(ses, buf, &tmp, flags_neol);
							strcpy(temp, tmp);
						}
						else
						{
							strcpy(temp, "");
						}
						i++;
					}

					if (isalnum(pti[1]) || pti[1] == '_')
					{
						for (ptt = temp, i = 1 ; isalnum(pti[i]) || pti[i] == '_' ; i++)
						{
							*ptt++ = pti[i];
						}
						*ptt = 0;
					}

					if ((node = searchnode_list(ses->list[LIST_VARIABLE], temp)) != NULL)
					{
						substitute(ses, node->right, &tmp, flags_neol - SUB_VAR);
						strcpy(pto, tmp);

						if (pti[1] == DEFAULT_OPEN)
						{
							pti += 3 + strlen(buf);
						}
						else
						{
							pti += 1 + strlen(temp);
						}

						if (*pti == '[')
						{
							for (ptt = temp, i = 1 ; pti[i] && pti[i] != ']' ; i++)
							{
								*ptt++ = pti[i];
							}
							*ptt = 0;

							if (pti[i] == ']')
							{
								pti += 2 + strlen(temp);

								substitute(ses, temp, &tmp, SUB_VAR|SUB_FUN);

								strcpy(pto, get_list_item(ses, node, tmp));
							}
						}
						pto += strlen(pto);
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
				if (HAS_BIT(flags, SUB_COL))
				{
					if (isdigit(pti[1]) && isdigit(pti[2]) && isdigit(pti[3]) && pti[4] == '>')
					{
						if (pti[1] != '8' || pti[2] != '8' || pti[3] != '8')
						{
							*pto++ = ESCAPE;
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
					else if (pti[1] >= 'a' && pti[1] <= 'f' && pti[2] >= 'a' && pti[2] <= 'f' && pti[3] >= 'a' && pti[3] <= 'f' && pti[4] == '>')
					{
						*pto++ = ESCAPE;
						*pto++ = '[';
						*pto++ = '3';
						*pto++ = '8';
						*pto++ = ';';
						*pto++ = '5';
						*pto++ = ';';
						cnt = 16 + (pti[1] - 'a') * 36 + (pti[2] - 'a') * 6 + (pti[3] - 'a');
						*pto++ = '0' + cnt / 100;
						*pto++ = '0' + cnt % 100 / 10;
						*pto++ = '0' + cnt % 10;
						*pto++ = 'm';
						pti += 5;
					}
					else if (pti[1] >= 'A' && pti[1] <= 'F' && pti[2] >= 'A' && pti[2] <= 'F' && pti[3] >= 'A' && pti[3] <= 'F' && pti[4] == '>')
					{
						*pto++ = ESCAPE;
						*pto++ = '[';
						*pto++ = '4';
						*pto++ = '8';
						*pto++ = ';';
						*pto++ = '5';
						*pto++ = ';';
						cnt = 16 + (pti[1] - 'A') + (pti[2] - 'A') * 6 + (pti[3] - 'A') * 36;
						*pto++ = '0' + cnt / 100;
						*pto++ = '0' + cnt % 100 / 10;
						*pto++ = '0' + cnt % 10;
						*pto++ = 'm';
						pti += 5;
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

			case '@':
				if (HAS_BIT(flags, SUB_FUN))
				{
					for (ptt = temp, i = 1 ; isalnum(pti[i]) || pti[i] == '_' ; i++)
					{
						*ptt++ = pti[i];
					}
					*ptt = 0;

					node = searchnode_list(ses->list[LIST_FUNCTION], temp);

					if (node == NULL || pti[i] != DEFAULT_OPEN)
					{
						*pto++ = *pti++;

						continue;
					}

					pti = get_arg_in_braces(&pti[i], &tmp, FALSE);

					substitute(ses, tmp, &pte, flags_neol);

					show_debug(ses, LIST_FUNCTION, "#FUNCTION DEBUG: {%s} {%s}", node->left, pte);

					gtd->vars[0] = string_alloc(pte);

					for (i = 0 ; i < 10 ; i++)
					{
						pte = get_arg_in_braces(pte, &gtd->cmds[i], FALSE);
					}

					for (i = 0 ; i < 9 ; i++)
					{
						gtd->vars[i+1] = gtd->cmds[i];
					}

					substitute(ses, node->right, &tmp, SUB_CMD|SUB_ARG);

					parse_input(ses, tmp);

					DEL_BIT(ses->flags, SES_FLAG_BREAK);

					substitute(ses, "$result", &tmp, SUB_VAR);

					strcpy(pto, tmp);

					show_debug(ses, LIST_FUNCTION, "#FUNCTION DEBUG: {%s} {%s}", node->left, pto);

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
					ptt = gtd->vars[pti[1] - '0'];

					while (*ptt)
					{
#ifdef BIG5
						if (*ptt & 0x80)
						{
							*pto++ = *ptt++;
							if (*ptt)
							{
								*pto++ = *ptt++;
							}
							continue;
						}
#endif
						if (HAS_BIT(flags, SUB_SEC))
						{
							switch (*ptt)
							{
								case COMMAND_SEPARATOR:
									*pto++ = '\\';
									*pto++ = COMMAND_SEPARATOR;
									break;

								case '\\':
									*pto++ = '\\';
									*pto++ = '\\';
									break;

								default:
									*pto++ = *ptt;
									break;
							}
							ptt++;
						}
						else
						{
							*pto++ = *ptt++;
						}
					}
					pti += 2;
				}
				else if (HAS_BIT(flags, SUB_FMT))
				{
					*pto++ = '%';
					*pto++ = *pti++;
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
						case 'b':
							*pto++ = '\b';
							break;
						case 'e':
							*pto++ = '\033';
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
						case 'x':
							pti++;

							if (pti[0] && pti[1])
							{
								*pto++ = hex_number(pti);
								pti++;
							}
							break;
						case '[':
							*pto++ = '{';
							break;
						case ']':
							*pto++ = '}';
							break;
						case '\0':
							DEL_BIT(flags, SUB_EOL);
							continue;
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

int check_one_action(char *line, char *original, char *action, struct session *ses)
{
	char *result, *ptr;
	char *ptl;
	int cnt;

	for (cnt = 0 ; cnt < 10 ; cnt++)
	{
		gtd->vars[cnt] = string_alloc("");
	}

	substitute(ses, action, &result, SUB_VAR|SUB_FUN|SUB_ESC);

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
		return action_regexp(ptr, ptl, 255);
	}

	if (ptr[0] == '%' && isdigit(ptr[1]))
	{
		return action_regexp(ptr, ptl, 255);
	}

	while (*ptl)
	{
		if ((*ptr == '[' || *ptr == *ptl) && action_regexp(ptr, ptl, 255))
		{
			return TRUE;
		}
		ptl++;
	}
	return FALSE;
}


int action_regexp(char *exp, char *str, unsigned char arg)
{
	short cnt, cnt2;

	thestart:

	while (*exp)
	{

#ifdef BIG5
		if (*exp & 0x80 && *str & 0x80)
		{
			exp++;
			str++;
			if (*exp++ != *str++)
			{
				return FALSE;
			}
			continue;
		}
#endif

		switch (exp[0])
		{
			case '%':
				if (exp[1] >= '0' && exp[1] <= '9')
				{
					arg = exp[1] - '0';

					if (exp[2] == 0)
					{
						gtd->vars[exp[1] - '0'] = string_realloc(gtd->vars[exp[1] - '0'], str);

						return TRUE;
					}

					for (cnt = 0 ; str[cnt] ; cnt++)
					{
						if (action_regexp(exp + 2, &str[cnt], arg))
						{
							gtd->vars[exp[1] - '0'] = stringf_realloc(gtd->vars[exp[1] - '0'], "%.*s", cnt, str);
/*
							gtd->vars[exp[1] - '0'] = string_realloc(gtd->vars[exp[1] - '0'], str);
							gtd->vars[exp[1] - '0'][cnt] = 0;
*/
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

			case '[':
				if (*str != '[' && regexp("[*|*]*", exp, TRUE))
				{
					cnt  = 1;
					cnt2 = 0;

					while (TRUE)
					{
						if (exp[cnt] == '|' || exp[cnt] == ']')
						{
							arg = (arg == 9) ? arg : arg + 1;
							cnt--;
							cnt2 = 0;

							while (exp[cnt] != '|' && exp[cnt] != '[')
							{
								cnt--;
								cnt2++;
								str++;
							}
							gtd->vars[arg] = stringf_alloc("%.*s", cnt2, &exp[cnt+1]);

							while (*exp != ']')
							{
								exp++;
							}
							exp++;

							goto thestart;
						}

						if (str[cnt2] == 0)
						{
							break;
						}

						if (exp[cnt] == str[cnt2])
						{
							cnt++;
							cnt2++;
							continue;
						}
						else
						{
							cnt2 = 0;

							while (exp[cnt])
							{
								if (exp[cnt] == ']')
								{
									return FALSE;
								}
								if (exp[cnt++] == '|')
								{
									break;
								}

							}
						}
					}
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
