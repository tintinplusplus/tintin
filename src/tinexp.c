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
#include <sys/types.h>

#include "tintin.h"


/******************************************************************************
* Simple regexp that skips vt102 codes and supports the * and ? wildcards     *
******************************************************************************/

int match(struct session *ses, char *str, char *exp)
{
	char buf[BUFFER_SIZE];

	if (!HAS_BIT(ses->flags, SES_FLAG_REGEXP))
	{
		substitute(ses, exp, buf, SUB_VAR|SUB_FUN);

		return glob(str, buf);
	}
	else
	{
		substitute(ses, exp, buf, SUB_VAR|SUB_FUN|SUB_ESC);

		return regexp(str, buf);
	}
}

int glob(char *str, char *exp)
{
	short cnt;

	while (*exp)
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
				exp++;
				str++;
				break;

			case '*':
				for (cnt = strlen(str) ; cnt >= 0 ; cnt--)
				{
					if (glob(&str[cnt], exp + 1))
					{
						return TRUE;
					}
				}
				return FALSE;

			case '\\':
				exp++;

			default:
				if (*exp++ != *str++)
				{
					return FALSE;
				}
				break;
		}
	}
	return (*exp == *str);
}

DO_COMMAND(do_regexp)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], true[BUFFER_SIZE], false[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 0);
	arg = get_arg_in_braces(arg, true,  1);
	arg = get_arg_in_braces(arg, false, 1);

	if (*left == 0 || *right == 0 || *true == 0)
	{
		tintin_printf2(ses, "SYNTAX: #REGEXP {string} {expression} {true} {false}.");
	}
	else
	{
		substitute(ses, left,  left,  SUB_VAR|SUB_FUN);
		substitute(ses, right, right, SUB_VAR|SUB_FUN|SUB_ESC);

		if (regexp_cmds(left, right))
		{
			ses = pre_parse_input(ses, true, SUB_CMD);
		}
		else if (*false)
		{
			ses = pre_parse_input(ses, false, SUB_NONE);
		}
	}
	return ses;
}

int regexp_cmds(char *str, char *exp)
{
	regex_t regex;
	regmatch_t match[100];
	int i;

	if (regcomp(&regex, exp, REG_EXTENDED))
	{
		return FALSE;
	}

	if (regexec(&regex, str, 100, match, 0))
	{
		regfree(&regex);

		return FALSE;
	}

	for (i = 0 ; i < 100 ; i++)
	{
		if (match[i].rm_so != -1)
		{
			gtd->cmds[i] = refstring(gtd->cmds[i], "%.*s", match[i].rm_eo - match[i].rm_so, &str[match[i].rm_so]);
		}
		else
		{
			break;
		}
	}
	regfree(&regex);

	return TRUE;
}

int action_regexp(char *str, char *exp)
{
	regex_t regex;
	regmatch_t match[100];
	int i;

	if (regcomp(&regex, exp, REG_EXTENDED))
	{
		return FALSE;
	}

	if (regexec(&regex, str, 100, match, 0))
	{
		regfree(&regex);

		return FALSE;
	}

	for (i = 0 ; i < 100 ; i++)
	{
		if (match[i].rm_so == -1)
		{
			RESTRING(gtd->vars[i], "");
		}
		else
		{
			gtd->vars[i] = refstring(gtd->vars[i], "%.*s", match[i].rm_eo - match[i].rm_so, &str[match[i].rm_so]);
		}
	}

	regfree(&regex);

	return TRUE;
}

int regexp(char *str, char *exp)
{
	regex_t regex;
	regmatch_t match[1];

	if (regcomp(&regex, exp, REG_NOSUB))
	{
		return FALSE;
	}

	if (regexec(&regex, str, 0, match, REG_EXTENDED))
	{
		regfree(&regex);

		return FALSE;
	}
	else
	{
		regfree(&regex);

		return TRUE;
	}
}

/******************************************************************************
* copy *string into *result, but substitute the various expressions with the  *
* values they stand for.                                                      *
******************************************************************************/

void substitute(struct session *ses, char *string, char *result, int flags)
{
	struct listnode *node;
	char temp[BUFFER_SIZE], buf[BUFFER_SIZE], buffer[BUFFER_SIZE], *pti, *pto, *ptt;
	char *pte;
	int i, cnt, flags_neol = flags;

	push_call("substitute(%p,%p,%p,%d)",ses,string,result,flags);

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

				strcpy(result, buffer);

				pop_call();
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
							substitute(ses, buf, temp, flags_neol);
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
						substitute(ses, node->right, pto, flags_neol - SUB_VAR);

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

								substitute(ses, temp, temp, SUB_VAR|SUB_FUN);

								get_list_item(ses, node, temp);
								strcpy(pto, temp);
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

					pti = get_arg_in_braces(&pti[i], temp, FALSE);

					substitute(ses, temp, buf, flags_neol);

					show_debug(ses, LIST_FUNCTION, "#FUNCTION DEBUG: {%s} {%s}.", node->left, buf);

					RESTRING(gtd->vars[0], buf);

					pte = buf;

					for (i = 1 ; i < 100 ; i++)
					{
						pte = get_arg_in_braces(pte, temp, FALSE);

						RESTRING(gtd->vars[i], temp);

						if (*gtd->vars[i] == 0)
						{
							break;
						}
					}

					pre_parse_input(ses, node->right, SUB_ARG);

					DEL_BIT(ses->flags, SES_FLAG_BREAK);

					substitute(ses, "$result", pto, flags_neol|SUB_VAR);

					show_debug(ses, LIST_FUNCTION, "#FUNCTION DEBUG: {%s} {%s}.", node->left, pto);

					pto += strlen(pto);
				}
				else
				{
					*pto++ = *pti++;
				}
				break;

			case '%':
				if (HAS_BIT(flags, SUB_ARG) && (isdigit(pti[1]) || pti[1] == '%'))
				{
					if (pti[1] == '%')
					{
						while (pti[1] == '%')
						{
							*pto++ = *pti++;
						}
						if (isdigit(pti[1]))
						{
							pti++;
						}
						else
						{
							*pto++ = *pti++;
						}
					}
					else
					{
						i = isdigit(pti[2]) ? (pti[1] - '0') * 10 + pti[2] - '0' : pti[1] - '0';

						ptt = gtd->vars[i];

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

									case '{':
										*pto++ = '\\';
										*pto++ = 'x';
										*pto++ = '7';
										*pto++ = 'B';
										break;

									case '}':
										*pto++ = '\\';
										*pto++ = 'x';
										*pto++ = '7';
										*pto++ = 'D';
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
						pti += isdigit(pti[2]) ? 3 : 2;
					}
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
				if (HAS_BIT(flags, SUB_CMD) && (isdigit(pti[1]) || pti[1] == '&'))
				{
					if (pti[1] == '&')
					{
						while (pti[1] == '&')
						{
							*pto++ = *pti++;
						}
						if (isdigit(pti[1]))
						{
							pti++;
						}
						else
						{
							*pto++ = *pti++;
						}
					}
					else
					{
						i = isdigit(pti[2]) ? (pti[1] - '0') * 10 + pti[2] - '0' : pti[1] - '0';

						for (cnt = 0 ; gtd->cmds[i][cnt] ; cnt++)
						{
							*pto++ = gtd->cmds[i][cnt];
						}
						pti += isdigit(pti[2]) ? 3 : 2;
					}
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
* Calls action_glob checking if the string matches, and automatically fills *
* in the text represented by the wildcards on success. Per default the entire *
* string is compared unless it starts or ends with the ^ character.           *
******************************************************************************/

int check_one_action(char *line, char *original, char *action, struct session *ses)
{
	char result[BUFFER_SIZE];
	char *exp, *str;

	substitute(ses, action, result, SUB_VAR|SUB_FUN|SUB_ESC);

	exp = result;

	if (*exp == '~')
	{
		exp++;
		str = original;
	}
	else
	{
		str = line;
	}

	if (HAS_BIT(ses->flags, SES_FLAG_REGEXP))
	{
		return action_regexp(str, exp);
	}

	if (*exp == '^')
	{
		exp++;
		return action_glob(str, exp, 255);
	}

	if (exp[0] == '%' && isdigit(exp[1]))
	{
		return action_glob(str, exp, 255);
	}

	while (*str)
	{
		if ((*exp == '[' || *exp == *str) && action_glob(str, exp, 255))
		{
			return TRUE;
		}
		str++;
	}
	return FALSE;
}


int action_glob(char *str, char *exp, unsigned char arg)
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
				if (isdigit(exp[1]))
				{
					arg = isdigit(exp[2]) ? (exp[1] - '0') * 10 + exp[2] - '0' : exp[1] - '0';

					if (exp[isdigit(exp[2]) ? 3 : 2] == 0)
					{
						RESTRING(gtd->vars[arg], str);

						return TRUE;
					}

					for (cnt = 0 ; str[cnt] ; cnt++)
					{
						if (action_glob(&str[cnt], exp + (isdigit(exp[2]) ? 3 : 2), arg))
						{
							gtd->vars[arg] = refstring(gtd->vars[arg], "%.*s", cnt, str);

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
				if (*str != '[' && glob(exp, "[*|*]*"))
				{
					cnt  = 1;
					cnt2 = 0;

					while (TRUE)
					{
						if (exp[cnt] == '|' || exp[cnt] == ']')
						{
							arg = (arg == 99) ? arg : arg + 1;
							cnt--;
							cnt2 = 0;

							while (exp[cnt] != '|' && exp[cnt] != '[')
							{
								cnt--;
								cnt2++;
								str++;
							}
							gtd->vars[arg] = refstring(gtd->vars[arg], "%.*s", cnt2, &exp[cnt+1]);

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
