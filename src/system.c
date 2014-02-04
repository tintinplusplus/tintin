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
*               (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                  *
*                                                                             *
*                     coded by Igor van den Hoven 2007                        *
******************************************************************************/

#include "tintin.h"

#ifdef HAVE_PTY_H
#include <pty.h>
#else
#ifdef HAVE_UTIL_H
#include <util.h>
#endif
#endif

extern char **environ;

DO_COMMAND(do_run)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE];
	int desc;
	struct winsize size;

	char *argv[4] = {"sh", "-c", "", NULL};

	arg = get_arg_in_braces(arg, left,  FALSE);
	substitute(ses, right, right, SUB_VAR|SUB_FUN);
	arg = get_arg_in_braces(arg, right, TRUE);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);

	if (*left == 0 || *right == 0)
	{
		tintin_printf2(ses, "#RUN: TWO ARGUMENTS REQUIRED.");

		return ses;
	}

	size.ws_row = get_scroll_size(ses);
	size.ws_col = ses->cols;

	switch(forkpty(&desc, NULL, &gtd->old_terminal, &size))
	{
		case -1:
			perror("forkpty");
			break;

		case 0:
			sprintf(temp, "exec %s", right);
			argv[2] = temp;
			execve("/bin/sh", argv, environ);
			break;

		default:
			ses = new_session(ses, left, right, desc);
			break;
	}
	return gtd->ses;
}

DO_COMMAND(do_scan)
{
	FILE *fp;
	char filename[BUFFER_SIZE];

	get_arg_in_braces(arg, filename, TRUE);
	substitute(ses, filename, filename, SUB_VAR|SUB_FUN);

	if ((fp = fopen(filename, "r")) == NULL)
	{
		tintin_printf(ses, "#ERROR: #SCAN {%s} - FILE NOT FOUND.", filename);
		return ses;
	}

	SET_BIT(ses->flags, SES_FLAG_SCAN);

	if (STRING_SIZE > gtd->mud_output_max)
	{
		gtd->mud_output_max  = STRING_SIZE;
		gtd->mud_output_buf  = realloc(gtd->mud_output_buf, gtd->mud_output_max);
	}

	while (fgets(gtd->mud_output_buf, STRING_SIZE, fp))
	{
		gtd->mud_output_len = 1;

		readmud(ses);
	}

	DEL_BIT(ses->flags, SES_FLAG_SCAN);

	show_message(ses, LIST_MESSAGE, "#OK. FILE READ.", filename);

	fclose(fp);

	return ses;
}

DO_COMMAND(do_script)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], *cptr, buffer[BUFFER_SIZE];
	struct listnode *node;
	FILE *script;

	arg = get_arg_in_braces(arg, left, TRUE);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);
	arg = get_arg_in_braces(arg, right, TRUE);
	substitute(ses, right, right, SUB_VAR|SUB_FUN);

	if (*left == 0)
	{
		show_message(ses, LIST_MESSAGE, "#SCRIPT: ONE ARGUMENT REQUIRED.");
	}
	else if (*right == 0)
	{
		script = popen(left, "r");

		while (fgets(buffer, BUFFER_SIZE - 1, script))
		{
			cptr = strchr(buffer, '\n');

			if (cptr)
			{
				*cptr = 0;
			}

			ses = script_driver(ses, -1, buffer);
		}

		pclose(script);
	}
	else
	{
		script = popen(right, "r");

		internal_variable(ses, "{%s}", left);

		node = searchnode_list(ses->list[LIST_VARIABLE], left);

		while (fgets(buffer, BUFFER_SIZE - 1, script))
		{
			cptr = strchr(buffer, '\n');

			if (cptr)
			{
				*cptr = 0;
			}
			array_ins(ses, node, "-1", buffer);
		}

		pclose(script);
	}
	return ses;
}

DO_COMMAND(do_system)
{
	char left[BUFFER_SIZE];

	get_arg_in_braces(arg, left, TRUE);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);

	if (*left == 0)
	{
		tintin_printf(ses, "#SYNTAX: #SYSTEM {COMMAND}.");
		return ses;
	}

	show_message(ses, LIST_MESSAGE, "#OK: EXECUTING '%s'", left);

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		save_pos(ses);
		goto_rowcol(ses, ses->bot_row, 1);
	}

	fflush(stdout);

	system(left);

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		restore_pos(ses);
	}

	return ses;
}


DO_COMMAND(do_textin)
{
	FILE *fp;
	char filename[BUFFER_SIZE], buffer[BUFFER_SIZE], *cptr;

	get_arg_in_braces(arg, filename, TRUE);
	substitute(ses, filename, filename, SUB_VAR|SUB_FUN);

	if ((fp = fopen(filename, "r")) == NULL)
	{
		tintin_printf(ses, "#ERROR: #TEXTIN {%s} - FILE NOT FOUND.", filename);
		return ses;
	}

	while (fgets(buffer, BUFFER_SIZE - 1, fp))
	{
		cptr = strchr(buffer, '\n');

		if (cptr)
		{
			*cptr = 0;
		}

		write_mud(ses, buffer, SUB_EOL);
	}
	fclose(fp);

	show_message(ses, LIST_MESSAGE, "#OK. FILE READ.");

	return ses;
}

