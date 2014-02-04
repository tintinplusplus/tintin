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

/*********************************************************************/
/* file: parse.c - some utility-functions                            */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

/* note: a bunch of changes were made here for readline support -- daw */

#include "tintin.h"

/**************************************************************************/
/* parse input, check for TINTIN commands and aliases and send to session */
/**************************************************************************/

struct session *parse_input(char *input, struct session *ses)
{
	char command[BUFFER_SIZE], arg[BUFFER_SIZE];

	if (push_call("[%s] parse_input(%s,%p)",ses->name,input,ses))
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

	if (ses != gts)
	{
		if (HAS_BIT(ses->flags, SES_FLAG_VERBATIM) && *input != gtd->tintin_char)
		{
			sprintf(command, "%s\r\n", input);

			write_line_mud(command, ses);

			pop_call();
			return(ses);
		}

		if (*input == gtd->verbatim_char)
		{
			sprintf(command, "%s\r\n", &input[1]);

			write_line_mud(command, ses);

			pop_call();
			return ses;
		}
	}

	while (*input)
	{
		if (*input == ';')
		{
			if (*++input == 0)
			{
				break;
			}
		}
		input = (char *) get_arg_stop_spaces(input, arg);

		substitute(ses, arg, command, SUB_VAR|SUB_FUN);

		input = (char *) get_arg_all(input, arg);

		if (*command == gtd->tintin_char)
		{
			ses = parse_tintin_command(command + 1, arg, ses);
		}
		else
		{
			if (*arg)
			{
				cat_sprintf(command, " %s", arg);
			}

			if (check_all_aliases(command, arg, ses))
			{
				ses = parse_input(command, ses);
			}
			else if (HAS_BIT(ses->flags, SES_FLAG_SPEEDWALK) && !*arg && is_speedwalk_dirs(command))
			{
				process_speedwalk(command, ses);
			}
			else
			{
				write_mud(ses, command);
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

int is_speedwalk_dirs(const char *cp)
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

void process_speedwalk(const char *cp, struct session *ses)
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

struct session *parse_tintin_command(const char *command, char *arg, struct session *ses)
{
	char newcommand[BUFFER_SIZE], argument[BUFFER_SIZE];
	int cmd;
	struct session *sesptr;

	for (sesptr = gts ; sesptr ; sesptr = sesptr->next)
	{
		if (!strcmp(sesptr->name, command))
		{
			if (*arg)
			{
				get_arg_with_spaces(arg, arg);
				substitute(ses, arg, newcommand, SUB_VAR|SUB_FUN);
				parse_input(newcommand, sesptr);  /* was: #sessioname commands */
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

		get_arg_in_braces(arg, arg, 1);

		while (i-- > 0)
		{
			ses = parse_input(arg, ses);
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
					substitute(ses, arg, argument, SUB_VAR|SUB_FUN);

					return (ses = (*command_table[cmd].command) (ses, argument));
				}
				else
				{
					return (ses = (*command_table[cmd].command) (ses, arg));
				}
			}
		}
	}
	tintin_printf(ses, "#ERROR: #UNKNOWN TINTIN-COMMAND '%s'.", command);

	return ses;
}



/*
	get all arguments - don't remove "s and \s
*/

const char *get_arg_all(const char *s, char *arg)
{
	int nest = 0;

	s = space_out(s);

	while (*s)
	{
		if (*s == '\\')
		{
			*arg++ = *s++;
			if (*s)
			{
				*arg++ = *s++;
			}
		}
#ifdef BIG5
		else if (*s & 0x80)
		{
			*arg++ = *s++;
			if (*s)
			{
				*arg++ = *s++;
			}
		}
#endif
		else if (*s == ';' && nest < 1)
		{
			break;
		}
		else if (*s == DEFAULT_OPEN)
		{
			nest++;
			*arg++ = *s++;
		}
		else if (*s == DEFAULT_CLOSE)
		{
			nest--;
			*arg++ = *s++;
		}
		else
		{
			*arg++ = *s++;
		}
	}
	*arg = '\0';
	return(s);
}

/**************************************/
/* get all arguments - remove "s etc. */
/* Example:                           */
/* In: "this is it" way way hmmm;     */
/* Out: this is it way way hmmm       */ 
/**************************************/

const char *get_arg_with_spaces(const char *s, char *arg)
{
	int nest = 0;

	s = space_out(s);

	while (*s)
	{
		if (*s == '\\')
		{
			*arg++ = *s++;
			if (*s == 0)
			{
				break;
			}
		}
		else if (*s == ';' && !nest)
		{
			break;
		}
		else if (*s == DEFAULT_OPEN)
		{
			nest++;
		}
		else if (*s == DEFAULT_CLOSE)
		{
			nest--;
		}
#ifdef BIG5
		else if (*s & 0x80)
		{
			*arg++ = *s++;
			if (*s == 0)
			{
				break;
			}
		}
#endif
		*arg++ = *s++;
	}
	*arg = '\0'; 
	return(s);
}

/*
	my own routine
*/

const char *get_arg_in_braces(const char *s, char *arg, int flag)
{
	int nest = 0;
	const char *ptr;

	s = space_out(s);
	ptr = s;

	if (*s != DEFAULT_OPEN)
	{
		if (flag == FALSE)
		{
			s = get_arg_stop_spaces(ptr, arg);
		}
		else
		{
			s = get_arg_with_spaces(ptr, arg);
		}
		return(s);
	}
	s++;

	while (*s && !(*s == DEFAULT_CLOSE && !nest))
	{
		if (*s == DEFAULT_OPEN)
		{
			nest++;
		}
		else if (*s == DEFAULT_CLOSE)
		{
			nest--;
		}
#ifdef BIG5
		else if (*s & 0x80)
		{
			*arg++ = *s++;
			if (*s == 0)
			{
				break;
			}
		}
#endif
		*arg++ = *s++;
	}

	if (!*s)
	{
		tintin_puts2("#Unmatched braces error!", NULL);
	}
	else
	{
		s++;
	}
	*arg = '\0';

	return(s);
}

/*
	get one arg, stop at spaces, remove quotes
*/

const char *get_arg_stop_spaces(const char *s, char *arg)
{
	int inside = FALSE;

	s = space_out(s);

	while (*s)
	{
		if (*s == '\\')
		{
			*arg++ = *s++;
			if (*s)
			{
				*arg++ = *s++;
			}
		}
#ifdef BIG5
		else if(*s & 0x80)
		{
			*arg++ = *s++;
			if (*s)
			{
				*arg++ = *s++;
			}
		}
#endif
		else if (*s == ';')
		{
			if (inside)
			{
				*arg++ = *s++;
			}
			else
			{
				break;
			}
		}
		else if (!inside && *s == ' ')
		{
			break;
		}
		else
		{
			*arg++ = *s++;
		}
	}
	*arg = '\0';

	return(s);
}

/*
	spaceout - advance ptr to next none-space
*/

const char *space_out(const char *s)
{
	while (isspace(*s))
	{
		s++;
	}
	return(s);
}

/*
	send command+argument to the mud
*/

void write_mud(struct session *ses, const char *command)
{
	char output[BUFFER_SIZE];

	if (ses->in_room)
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

	push_call("[%s] do_one_line(%p,%p)",ses->name,line,ses);

	strip_vt102_codes(line, strip);

	push_call("1");

	if (!HAS_BIT(ses->list[LIST_ACTION]->flags, LIST_FLAG_IGNORE))
	{
		check_all_actions(line, strip, ses);
	}

	push_call("2");

	if (!HAS_BIT(ses->list[LIST_PROMPT]->flags, LIST_FLAG_IGNORE))
	{
		if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
		{
			check_all_prompts(line, strip, ses);
		}
	}

	push_call("3");

	if (!HAS_BIT(ses->list[LIST_SUBSTITUTE]->flags, LIST_FLAG_IGNORE))
	{
		check_all_substitutions(line, strip, ses);
	}

	push_call("4");

	if (!HAS_BIT(ses->list[LIST_HIGHLIGHT]->flags, LIST_FLAG_IGNORE))
	{
		check_all_highlights(line, strip, ses);
	}
	pop_call(); pop_call(); pop_call(); pop_call();

	pop_call();
	return;
}
