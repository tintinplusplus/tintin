/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2008 (See CREDITS file)                                     *
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
*                      coded by Igor van den Hoven 2008                       *
******************************************************************************/

#include "tintin.h"

DO_COMMAND(do_line)
{
	char left[BUFFER_SIZE];
	int cnt;

	arg = get_arg_in_braces(ses, arg, left, FALSE);

	if (*left == 0)
	{
		show_error(ses, LIST_COMMAND, "#SYNTAX: #LINE {<OPTION>} {argument}.");
	}
	else
	{
		for (cnt = 0 ; *line_table[cnt].name ; cnt++)
		{
			if (is_abbrev(left, line_table[cnt].name))
			{
				break;
			}
		}

		if (*line_table[cnt].name == 0)
		{
			do_line(ses, "");
		}
		else
		{
			ses = line_table[cnt].fun(ses, arg);
		}
	}
	return ses;
}


DO_LINE(line_gag)
{
	char left[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);

	show_debug(ses, LIST_GAG, "#DEBUG LINE GAG");

	SET_BIT(ses->flags, SES_FLAG_GAG);

	return ses;
}

// Without an argument mark next line to be logged, otherwise log the given line to file.

DO_LINE(line_log)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	FILE *logfile;

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, right, GET_ALL, SUB_VAR|SUB_FUN);

	if (*left && *right)
	{
		substitute(ses, right, right, SUB_ESC|SUB_COL|SUB_LNF);

		if (!strcmp(ses->logline_name, left))
		{
			logit(ses, right, ses->logline_file, FALSE);
		}
		else
		{
			if ((logfile = fopen(left, "a")))
			{
				if (ses->logline_file)
				{
					fclose(ses->logline_file);
				}
				ses->logline_name = restring(ses->logline_name, left);
				ses->logline_file = logfile;

				if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
				{
					fseek(logfile, 0, SEEK_END);

					if (ftell(logfile) == 0)
					{
						write_html_header(ses, logfile);
					}
				}

				logit(ses, right, ses->logline_file, FALSE);
			}
			else
			{
				show_error(ses, LIST_COMMAND, "#ERROR: #LINE LOG {%s} - COULDN'T OPEN FILE.", left);
			}
		}
	}
	else
	{
		if (!strcmp(ses->lognext_name, left))
		{
			SET_BIT(ses->flags, SES_FLAG_LOGNEXT);
		}
		else if ((logfile = fopen(left, "a")))
		{
			if (ses->lognext_file)
			{
				fclose(ses->lognext_file);
			}
			ses->lognext_name = restring(ses->lognext_name, left);
			ses->lognext_file = logfile;

			SET_BIT(ses->flags, SES_FLAG_LOGNEXT);
		}
		else
		{
			show_error(ses, LIST_COMMAND, "#ERROR: #LINE LOG {%s} - COULDN'T OPEN FILE.", left);
		}
	}
	return ses;
}

DO_LINE(line_logverbatim)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	FILE *logfile;

	arg = sub_arg_in_braces(ses, arg, left, GET_ONE, SUB_VAR|SUB_FUN);
	arg = get_arg_in_braces(ses, arg, right, GET_ALL);

	if (*left && *right)
	{
		if (!strcmp(ses->logline_name, left))
		{
			logit(ses, right, ses->logline_file, TRUE);
		}
		else
		{
			if ((logfile = fopen(left, "a")))
			{
				if (ses->logline_file)
				{
					fclose(ses->logline_file);
				}
				ses->logline_name = restring(ses->logline_name, left);
				ses->logline_file = logfile;

				if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
				{
					fseek(logfile, 0, SEEK_END);

					if (ftell(logfile) == 0)
					{
						write_html_header(ses, logfile);
					}
				}

				logit(ses, right, ses->logline_file, TRUE);
			}
			else
			{
				show_error(ses, LIST_COMMAND, "#ERROR: #LINE LOGVERBATIM {%s} - COULDN'T OPEN FILE.", left);
			}
		}
	}
	else
	{
		if (!strcmp(ses->lognext_name, left))
		{
			SET_BIT(ses->flags, SES_FLAG_LOGNEXT);
		}
		else if ((logfile = fopen(left, "a")))
		{
			if (ses->lognext_file)
			{
				fclose(ses->lognext_file);
			}
			ses->lognext_name = restring(ses->lognext_name, left);
			ses->lognext_file = logfile;

			SET_BIT(ses->flags, SES_FLAG_LOGNEXT);
		}
		else
		{
			show_error(ses, LIST_COMMAND, "#ERROR: #LINE LOGVERBATIM {%s} - COULDN'T OPEN FILE.", left);
		}
	}
	return ses;
}

DO_LINE(line_strip)
{
	char left[BUFFER_SIZE], strip[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ALL, SUB_ESC|SUB_COL);

	if (*left == 0)
	{
		show_error(ses, LIST_COMMAND, "#SYNTAX: #LINE {STRIP} {command}.");

		return ses;
	}

	strip_vt102_codes(left, strip);

	ses = script_driver(ses, LIST_COMMAND, strip);

	return ses;
}

DO_LINE(line_substitute)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], subs[BUFFER_SIZE];
	int i, flags = 0;

	arg = get_arg_in_braces(ses, arg, left,  0);
	arg = get_arg_in_braces(ses, arg, right, 1);

	if (*right == 0)
	{
		return show_error(ses, LIST_COMMAND, "#SYNTAX: #LINE {SUBSTITUTE} {argument} {command}.");
	}

	arg = left;

	while (*arg)
	{
		arg = get_arg_in_braces(ses, arg, subs, 0);

		for (i = 0 ; *substitution_table[i].name ; i++)
		{
			if (is_abbrev(subs, substitution_table[i].name))
			{
				SET_BIT(flags, substitution_table[i].bitvector);
			}
		}

		if (*arg == COMMAND_SEPARATOR)
		{
			arg++;
		}
	}

	substitute(ses, right, subs, flags);

	ses = script_driver(ses, LIST_COMMAND, subs);

	return ses;
}

DO_LINE(line_verbatim)
{
	char left[BUFFER_SIZE];

	arg = get_arg_in_braces(ses, arg, left,  TRUE);

	if (*left == 0)
	{
		return show_error(ses, LIST_COMMAND, "#SYNTAX: #LINE {VERBATIM} {command}.");
	}

	gtd->verbatim_level++;

	ses = parse_input(ses, left);

	gtd->verbatim_level--;

	return ses;
}

DO_LINE(line_verbose)
{
	char left[BUFFER_SIZE];

	arg = get_arg_in_braces(ses, arg, left,  TRUE);

	if (*left == 0)
	{
		return show_error(ses, LIST_COMMAND, "#SYNTAX: #LINE {VERBOSE} {command}.");
	}

	gtd->verbose_level++;

	ses = script_driver(ses, LIST_COMMAND, left);

	gtd->verbose_level--;

	return ses;
}

DO_LINE(line_ignore)
{
	struct session *sesptr;
	char left[BUFFER_SIZE];

	arg = get_arg_in_braces(ses, arg, left,  TRUE);

	if (*left == 0)
	{
		return show_error(ses, LIST_COMMAND, "#SYNTAX: #LINE {IGNORE} {command}.");
	}

	sesptr = ses;

	SET_BIT(sesptr->flags, SES_FLAG_IGNORELINE);

	ses = script_driver(ses, LIST_COMMAND, left);

	DEL_BIT(sesptr->flags, SES_FLAG_IGNORELINE);

	return ses;
}

DO_LINE(line_quiet)
{
	char left[BUFFER_SIZE];

	arg = get_arg_in_braces(ses, arg, left,  TRUE);

	if (*left == 0)
	{
		return show_error(ses, LIST_COMMAND, "#SYNTAX: #LINE {QUIET} {command}.");
	}

	gtd->quiet++;

	ses = script_driver(ses, LIST_COMMAND, left);

	gtd->quiet--;

	return ses;
}
	