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

/**************************************************************************/
/* parse input, check for TINTIN commands and aliases and send to session */
/**************************************************************************/

struct session *parse_input(struct session *ses, char *input)
{
	char *command, *arg1, *arg2;

	if (push_call("[%s] parse_input(%p,%s)",ses->name,ses,input))
	{
		pop_call();
		return ses;
	}

	DEL_BIT(ses->flags, SES_FLAG_BREAK);

	if (*input == 0)
	{
		write_line_mud("\r\n", ses);

		pop_call();
		return(ses);
	}

	if (HAS_BIT(ses->flags, SES_FLAG_VERBATIM) && *input != gtd->tintin_char)
	{
		substitute(ses, input, &command, SUB_EOL);

		write_line_mud(command, ses);

		pop_call();
		return(ses);
	}

	if (*input == gtd->verbatim_char)
	{
		substitute(ses, &input[1], &command, SUB_EOL);

		write_line_mud(command, ses);

		pop_call();
		return ses;
	}

	while (*input)
	{
		if (*input == COMMAND_SEPARATOR)
		{
			if (*++input == 0)
			{
				break;
			}
		}
		input = get_arg_stop_spaces(input, &command);

		substitute(ses, command, &arg1, SUB_VAR|SUB_FUN);

		if (strcmp(arg1, command))
		{
			command = stringf_alloc("%s%s", arg1, input);

			ses = parse_input(ses, command);

			pop_call();
			return ses;
		}

		input = get_arg_all(input, &arg2);

		if (*arg1 == gtd->tintin_char)
		{
			ses = parse_tintin_command(ses, arg1 + 1, arg2);
		}
		else
		{
			if (check_all_aliases(ses, arg1, arg2, &command))
			{
				DEL_BIT(gtd->flags, TINTIN_FLAG_USERCOMMAND); /* get rid of spam for aliasses */

				ses = parse_input(ses, command);
			}
			else
			{
				if (*arg2)
				{
					command = stringf_alloc("%s %s", arg1, arg2);
				}
				else
				{
					command = string_alloc(arg1);
				}
			
				if (HAS_BIT(ses->flags, SES_FLAG_SPEEDWALK) && *arg2 == 0 && is_speedwalk_dirs(command))
				{
					process_speedwalk(command, ses);
				}
				else
				{
					write_mud(ses, command);
				}
			}
		}

		if (HAS_BIT(ses->flags, SES_FLAG_BREAK))
		{
			break;
		}
	}

	pop_call();
	return ses;
}

/**********************************************************************/
/* return TRUE if commands only consists of capital letters N,S,E ... */
/**********************************************************************/

int is_speedwalk_dirs(char *cp)
{
	int flag = FALSE;

	while (*cp)
	{
		switch (*cp)
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
		cp++;
	}
	return flag;
}

/**************************/
/* do the speedwalk thing */
/**************************/

void process_speedwalk(char *cp, struct session *ses)
{
	char sc[2];
	int loopcnt, i;

	push_call("do_speedwalk(%s,%p)",cp,ses);

	for (sc[1] = 0 ; *cp ; cp++)
	{
		if (isdigit(*cp))
		{
			sscanf(cp, "%d%c", &loopcnt, sc);

			for (i = 0 ; i < loopcnt ; i++)
			{
				write_mud(ses, sc);
			}
			while (*cp != sc[0])
			{
				cp++;
			}
		}
		else
		{
			sc[0] = *cp;

			write_mud(ses, sc);
		}
	}
	pop_call();
	return;
}


/*************************************/
/* parse most of the tintin-commands */
/*************************************/

struct session *parse_tintin_command(struct session *ses, char *command, char *arg)
{
	char *temp;
	int cmd;
	struct session *sesptr;

	for (sesptr = gts ; sesptr ; sesptr = sesptr->next)
	{
		if (!strcmp(sesptr->name, command))
		{
			if (*arg)
			{
				get_arg_in_braces(arg, &temp, TRUE);

				substitute(ses, temp, &command, SUB_VAR|SUB_FUN);

				parse_input(sesptr, command);

				return ses;
			}
			else
			{
				return activate_session(sesptr);
			}
		}
	}

	if (atoi(command) > 0)
	{
		int i = atoi(command);

		get_arg_in_braces(arg, &command, TRUE);

		while (i-- > 0)
		{
			ses = parse_input(ses, command);
		}
		return ses;
	}

	if (isalpha(*command))
	{
		cmd = gtd->command_ref[tolower(*command) - 'a'];

		for ( ; *command_table[cmd].name != 0 ; cmd++)
		{
			if (is_abbrev(command, command_table[cmd].name))
			{
				if (HAS_BIT(command_table[cmd].flags, CMD_FLAG_SUB))
				{
					substitute(ses, arg, &command, SUB_VAR|SUB_FUN);

					ses = (*command_table[cmd].command) (ses, command);
				}
				else
				{
					ses = (*command_table[cmd].command) (ses, arg);
				}
				return ses;
			}
		}
	}
	tintin_printf(ses, "#ERROR: #UNKNOWN TINTIN-COMMAND '%s'.", command);

	return ses;
}


/*
	get all arguments - only check for unescaped command separators
*/

char *get_arg_all(char *string, char **result)
{
	char *pto, *pti, buffer[STRING_SIZE];
	int nest = 0;

	pti = space_out(string);
	pto = buffer;

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

	*result = string_alloc(buffer);

	return pti;
}

/*
	Braces are stripped in braced arguments leaving all else as is.
*/

char *get_arg_in_braces(char *string, char **result, int flag)
{
	char *pti, *pto, buffer[STRING_SIZE];
	int nest = 1;

	pti = space_out(string);
	pto = buffer;

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

	*result = string_alloc(buffer);

	return pti;
}


/*
	get all arguments
*/

char *get_arg_with_spaces(char *string, char **result)
{
	char *pto, *pti, buffer[STRING_SIZE];
	int nest = 0;

	pti = space_out(string);
	pto = buffer;

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

	*result = string_alloc(buffer);

	return pti;
}

/*
	get one arg, stop at spaces
*/

char *get_arg_stop_spaces(char *string, char **result)
{
	char *pto, *pti, buffer[STRING_SIZE];
	int nest = 0;

	pti = space_out(string);
	pto = buffer;

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

	*result = string_alloc(buffer);

	return pti;
}

/*
	get_arg_in_braces for threaded and old school routine.
*/

char *cpy_arg_in_braces(char *pti, char *pto, int flag)
{
	int nest = 1;

	pti = space_out(pti);

	if (*pti != DEFAULT_OPEN)
	{
		if (flag == FALSE)
		{
			pti = cpy_arg_stop_spaces(pti, pto);
		}
		else
		{
			pti = cpy_arg_with_spaces(pti, pto);
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

char *cpy_arg_with_spaces(char *pti, char *pto)
{
	int nest = 0;

	pti = space_out(pti);

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

char *cpy_arg_stop_spaces(char *pti, char *pto)
{
	int nest = 0;

	pti = space_out(pti);

	while (*pti)
	{
#ifdef BIG5
		if(*pti & 0x80)
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

void write_mud(struct session *ses, char *command)
{
	char *output;

	if (ses->map && ses->map->in_room)
	{
		follow_map(ses, command);
	}
	if (HAS_BIT(ses->flags, SES_FLAG_MAPPING))
	{
		check_insert_path(command, ses);
	}

	substitute(ses, command, &output, SUB_VAR|SUB_FUN|SUB_ESC|SUB_EOL);

	write_line_mud(output, ses);
}


/******************************************************************************
* do all of the functions to one line of buffer, VT102 codes and variables    *
* substituted beforehand                                                      *
******************************************************************************/

void do_one_line(char **line, struct session *ses)
{
	char strip[STRING_SIZE];

	push_call("[%s] do_one_line(%p,%p)",ses->name,*line,ses);

	strip_vt102_codes(*line, strip);

	if (!HAS_BIT(ses->list[LIST_ACTION]->flags, LIST_FLAG_IGNORE))
	{
		check_all_actions(ses, *line, strip);
	}

	if (!HAS_BIT(ses->list[LIST_PROMPT]->flags, LIST_FLAG_IGNORE))
	{
		if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
		{
			check_all_prompts(ses, *line, strip);
		}
	}

	if (!HAS_BIT(ses->list[LIST_GAG]->flags, LIST_FLAG_IGNORE))
	{
		check_all_gags(ses, *line, strip);
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
