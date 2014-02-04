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

struct session *parse_input(struct session *ses, char *input)
{
	char *line;

	push_call("parse_input(%s,%s)",ses->name,input);

	if (*input == 0)
	{
		write_mud(ses, input, SUB_EOL);

		pop_call();
		return ses;
	}

	if (HAS_BIT(ses->flags, SES_FLAG_VERBATIM) && HAS_BIT(gtd->flags, TINTIN_FLAG_USERCOMMAND))
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

	line = malloc(BUFFER_SIZE);

	while (*input)
	{
		input = get_arg_all(input, line);

		if (parse_command(ses, line))
		{
			ses = script_driver(ses, -1, line);
		}
		else if (check_all_aliases(ses, line))
		{
			ses = script_driver(ses, LIST_ALIAS, line);
		}
		else if (is_speedwalk(ses, line))
		{
			process_speedwalk(ses, line);
		}
		else
		{
			write_mud(ses, line, SUB_VAR|SUB_FUN|SUB_ESC|SUB_EOL);
		}

		if (*input == ';')
		{
			input++;
		}
	}

	free(line);

	pop_call();
	return ses;
}

/*
	Deal with variables and functions used as commands.
*/

struct session *parse_command(struct session *ses, char *input)
{
	char *arg, line[BUFFER_SIZE], cmd1[BUFFER_SIZE], cmd2[BUFFER_SIZE];

	push_call("parse_command(%p,%p)",ses,input);

	arg = get_arg_stop_spaces(input, cmd1);

	substitute(ses, cmd1, cmd2, SUB_VAR|SUB_FUN);

	if (!strcmp(cmd1, cmd2))
	{
		pop_call();
		return NULL;
	}

	sprintf(line, "%s%s%s", cmd2, *arg ? " " : "", arg);

	strcpy(input, line);

	pop_call();
	return ses;
}

int is_speedwalk(struct session *ses, char *input)
{
	int flag = FALSE;

	if (!HAS_BIT(ses->flags, SES_FLAG_SPEEDWALK))
	{
		return FALSE;
	}

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
	char line[BUFFER_SIZE];
	struct session *sesptr;

	input = get_arg_stop_spaces(input, line);

	substitute(ses, line, line, SUB_VAR|SUB_FUN);

	if (is_number(line))
	{
		int cnt = atoi(line);

		input = get_arg_in_braces(input, line, TRUE);

		while (cnt-- > 0)
		{
			ses = script_driver(ses, -1, line);
		}
		return ses;
	}

	for (sesptr = gts ; sesptr ; sesptr = sesptr->next)
	{
		if (!strcmp(sesptr->name, line))
		{
			if (*input)
			{
				input = get_arg_in_braces(input, line, TRUE);

				substitute(ses, line, line, SUB_VAR|SUB_FUN);

				script_driver(sesptr, -1, line);

				return ses;
			}
			else
			{
				return activate_session(sesptr);
			}
		}
	}

	tintin_printf(ses, "#ERROR: #UNKNOWN TINTIN-COMMAND '%s'.", line);

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

	substitute(ses, command, output, flags);

	write_line_mud(output, ses);
}


/******************************************************************************
* do all of the functions to one line of buffer, VT102 codes and variables    *
* substituted beforehand                                                      *
******************************************************************************/

void do_one_line(char *line, struct session *ses)
{
	char strip[BUFFER_SIZE];

	push_call("[%s] do_one_line(%s)",ses->name,line);

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
