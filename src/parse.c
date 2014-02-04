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


/*
	Recursive and alters input, so keep it on a tight leash
*/

struct session *pre_parse_input(struct session *ses, char *input, int flags)
{
	char buffer[BUFFER_SIZE];

	if (flags)
	{
		substitute(ses, input, buffer, flags);
	}
	else
	{
		strcpy(buffer, input);
	}

	return parse_input(ses, buffer);
}

struct session *parse_input(struct session *ses, char *input)
{
	char line[BUFFER_SIZE];

	if (push_call("[%s] parse_input(%p,%s)",ses->name,ses,input))
	{
		pop_call();
		return ses;
	}

	DEL_BIT(ses->flags, SES_FLAG_BREAK);

	if (*input == 0)
	{
		write_mud(ses, "", SUB_EOL);

		pop_call();
		return ses;
	}

	if (HAS_BIT(ses->flags, SES_FLAG_VERBATIM) && *input != gtd->tintin_char)
	{
		write_mud(ses, input, SUB_EOL);

		pop_call();
		return ses;
	}

	if (*input == gtd->verbatim_char)
	{
		write_mud(ses, input+1, SUB_EOL);

		pop_call();
		return ses;
	}

	while (*input)
	{
		input = get_arg_all(input, line);

		if (parse_command(ses, line))
		{
			ses = parse_input(ses, line);
		}
		else if (*line == gtd->tintin_char)
		{
			ses = parse_tintin_command(ses, line+1);
		}
		else if (check_all_aliases(ses, line))
		{
			ses = parse_input(ses, line);
		}
		else if (HAS_BIT(ses->flags, SES_FLAG_SPEEDWALK) && is_speedwalk(line))
		{
			process_speedwalk(ses, line);
		}
		else
		{
			write_mud(ses, line, SUB_VAR|SUB_FUN|SUB_ESC|SUB_EOL);
		}

		if (HAS_BIT(ses->flags, SES_FLAG_BREAK))
		{
			break;
		}

		if (*input == ';')
		{
			input++;
		}
	}

	pop_call();
	return ses;
}

struct session *parse_command(struct session *ses, char *input)
{
	char cmd1[BUFFER_SIZE], cmd2[BUFFER_SIZE], *arg, argument[BUFFER_SIZE];

	push_call("parse_command(%p,%p)",ses,input);

	arg = get_arg_stop_spaces(input, cmd1);

	substitute(ses, cmd1, cmd2, SUB_VAR|SUB_FUN);

	if (!strcmp(cmd1, cmd2))
	{
		pop_call();
		return NULL;
	}

	strcpy(argument, arg);

	if (*argument)
	{
		sprintf(input, "%s %s", cmd2, argument);
	}
	else
	{
		strcpy(input, cmd2);
	}

	pop_call();
	return ses;
}

int is_speedwalk(char *input)
{
	int flag = FALSE;

	while (*input)
	{
		switch (*input)
		{
			case 'n':
			case 'e':
			case 's':
			case 'w':
			case 'u':
			case 'd':
				flag = TRUE;
				break;

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
				flag = FALSE;
				break;

			default:
				return FALSE;
		}
		input++;
	}
	return flag;
}


void process_speedwalk(struct session *ses, char *input)
{
	char dir[2];
	int cnt, i;

	push_call("do_speedwalk(%s,%p)",input,ses);

	for (dir[1] = 0 ; *input ; input++)
	{
		if (isdigit(*input))
		{
			sscanf(input, "%d%c", &cnt, dir);

			for (i = 0 ; i < cnt ; i++)
			{
				write_mud(ses, dir, SUB_EOL);
			}

			while (*input != dir[0])
			{
				input++;
			}
		}
		else
		{
			dir[0] = *input;

			write_mud(ses, dir, SUB_EOL);
		}
	}
	pop_call();
	return;
}

/*
	Deals with all # stuff
*/

struct session *parse_tintin_command(struct session *ses, char *input)
{
	char argument[BUFFER_SIZE];
	int cmd;
	struct session *sesptr;

	push_call("parse_tintin_command(%p,%s)", ses, input);

	input = get_arg_stop_spaces(input, argument);

	for (sesptr = gts ; sesptr ; sesptr = sesptr->next)
	{
		if (!strcmp(sesptr->name, argument))
		{
			if (*input)
			{
				input = get_arg_in_braces(input, argument, TRUE);

				pre_parse_input(sesptr, argument, SUB_VAR|SUB_FUN);

				pop_call();
				return ses;
			}
			else
			{
				pop_call();
				return activate_session(sesptr);
			}
		}
	}

	cmd = atoi(argument);

	if (cmd > 0)
	{
		get_arg_in_braces(input, argument, TRUE);

		while (cmd-- > 0)
		{
			ses = pre_parse_input(ses, argument, SUB_NONE);
		}
		pop_call();
		return ses;
	}

	if (isalpha(*argument))
	{
		cmd = gtd->command_ref[tolower(*argument) - 'a'];

		for ( ; *command_table[cmd].name != 0 ; cmd++)
		{
			if (is_abbrev(argument, command_table[cmd].name))
			{
				if (HAS_BIT(command_table[cmd].flags, CMD_FLAG_SUB))
				{
					substitute(ses, input, argument, SUB_VAR|SUB_FUN);

					ses = (*command_table[cmd].command) (ses, argument);
				}
				else
				{
					strcpy(argument, input);

					ses = (*command_table[cmd].command) (ses, argument);
				}
				pop_call();
				return ses;
			}
		}
	}

	tintin_printf(ses, "#ERROR: #UNKNOWN TINTIN-COMMAND '%s'.", argument);

	pop_call();
	return ses;
}


/*
	get all arguments - only check for unescaped command separators
*/


char *get_arg_all(char *string, char *result)
{
	char *pto, *pti;
	int nest = 0;

	pti = space_out(string);
	pto = result;

	while (*pti)
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

		if (*pti == '\\' && pti[1] == COMMAND_SEPARATOR)
		{
			*pto++ = *pti++;
		}
		else if (*pti == COMMAND_SEPARATOR && nest == 0)
		{
			break;
		}
		else if (*pti == DEFAULT_OPEN)
		{
			nest++;
		}
		else if (*pti == DEFAULT_CLOSE)
		{
			nest--;
		}
		*pto++ = *pti++;
	}
	*pto = '\0'; 

	return pti;
}

/*
	Braces are stripped in braced arguments leaving all else as is.
*/

char *get_arg_in_braces(char *string, char *result, int flag)
{
	char *pti, *pto;
	int nest = 1;

	pti = space_out(string);
	pto = result;

	if (*pti != DEFAULT_OPEN)
	{
		if (flag == FALSE)
		{
			pti = get_arg_stop_spaces(pti, result);
		}
		else
		{
			pti = get_arg_with_spaces(pti, result);
		}
		return pti;
	}

	pti++;

	while (*pti)
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

		if (*pti == DEFAULT_OPEN)
		{
			nest++;
		}
		else if (*pti == DEFAULT_CLOSE)
		{
			nest--;

			if (nest == 0)
			{
				break;
			}
		}
		*pto++ = *pti++;
	}

	if (*pti == 0)
	{
		tintin_printf2(NULL, "#Unmatched braces error!");
	}
	else
	{
		pti++;
	}
	*pto = '\0';

	return pti;
}


/*
	get all arguments
*/

char *get_arg_with_spaces(char *string, char *result)
{
	char *pto, *pti;
	int nest = 0;

	pti = space_out(string);
	pto = result;

	while (*pti)
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
		if (*pti == '\\' && pti[1] == COMMAND_SEPARATOR)
		{
			*pto++ = *pti++;
		}
		else if (*pti == COMMAND_SEPARATOR && nest == 0)
		{
			break;
		}
		else if (*pti == DEFAULT_OPEN)
		{
			nest++;
		}
		else if (*pti == DEFAULT_CLOSE)
		{
			nest--;
		}
		*pto++ = *pti++;
	}
	*pto = '\0'; 

	return pti;
}

/*
	get one arg, stop at spaces
*/

char *get_arg_stop_spaces(char *string, char *result)
{
	char *pto, *pti;
	int nest = 0;

	pti = space_out(string);
	pto = result;

	while (*pti)
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

		if (*pti == '\\' && pti[1] == COMMAND_SEPARATOR)
		{
			*pto++ = *pti++;
		}
		else if (*pti == COMMAND_SEPARATOR && nest == 0)
		{
			break;
		}
		else if (isspace(*pti) && nest == 0)
		{
			pti++;
			break;
		}
		else if (*pti == DEFAULT_OPEN)
		{
			nest++;
		}
		else if (*pti == DEFAULT_CLOSE)
		{
			nest--;
		}
		*pto++ = *pti++;
	}
	*pto = '\0';

	return pti;
}

/*
	advance ptr to next none-space
*/

char *space_out(char *string)
{
	while (isspace(*string))
	{
		string++;
	}
	return string;
}

/*
	send command+argument to the mud
*/

void write_mud(struct session *ses, char *command, int flags)
{
	char output[BUFFER_SIZE];

	if (ses->map && ses->map->in_room)
	{
		follow_map(ses, command);
	}

	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		check_insert_path(command, ses);
	}

	substitute(ses, command, output, SUB_VAR|SUB_FUN|SUB_ESC|SUB_EOL);

	write_line_mud(output, ses);
}


/******************************************************************************
* do all of the functions to one line of buffer, VT102 codes and variables    *
* substituted beforehand                                                      *
******************************************************************************/

void do_one_line(char *line, struct session *ses)
{
	char strip[BUFFER_SIZE];

	if (push_call("[%s] do_one_line(%s)",ses->name,line))
	{
		pop_call();
		return;
	}

	strip_vt102_codes(line, strip);

	check_all_events(ses, "RECEIVED LINE");

	if (!HAS_BIT(ses->list[LIST_ACTION]->flags, LIST_FLAG_IGNORE))
	{
		check_all_actions(ses, line, strip);
	}

	if (!HAS_BIT(ses->list[LIST_PROMPT]->flags, LIST_FLAG_IGNORE))
	{
		if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
		{
			check_all_prompts(ses, line, strip);
		}
	}

	if (!HAS_BIT(ses->list[LIST_GAG]->flags, LIST_FLAG_IGNORE))
	{
		check_all_gags(ses, line, strip);
	}

	if (!HAS_BIT(ses->list[LIST_SUBSTITUTE]->flags, LIST_FLAG_IGNORE))
	{
		check_all_substitutions(ses, line, strip);
	}

	if (!HAS_BIT(ses->list[LIST_HIGHLIGHT]->flags, LIST_FLAG_IGNORE))
	{
		check_all_highlights(ses, line, strip);
	}

	pop_call();
	return;
}
