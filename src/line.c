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

	arg = get_arg_in_braces(arg, left, FALSE);

	if (*left == 0)
	{
		tintin_printf(ses, "#SYNTAX: #LINE {GAG|LOG} {argument}.");
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
			line_table[cnt].fun(ses, arg);
		}
	}
	return ses;
}

DO_LINE(line_gag)
{
	SET_BIT(ses->flags, SES_FLAG_GAG);
}

DO_LINE(line_log)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left, 0);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);

	arg = get_arg_in_braces(arg, right, 1);

	if (ses->logline)
	{
		return;
	}

	if ((ses->logline = fopen(left, "a")))
	{
		fseek(ses->logline, 0, SEEK_END);

		if (ftell(ses->logline) == 0 && HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
		{
			write_html_header(ses->logline);
		}

		if (*right)
		{
			substitute(ses, right, right, SUB_ESC|SUB_COL|SUB_VAR|SUB_FUN|SUB_LNF);

			logit(ses, right, ses->logline, FALSE);

			fclose(ses->logline);
			ses->logline = NULL;
		}
	}
	else
	{
		tintin_printf(ses, "#ERROR: #LINE LOG {%s} - COULDN'T OPEN FILE.", left);
	}
}

DO_LINE(line_logverbatim)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left,  0);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);

	arg = get_arg_in_braces(arg, right, 1);

	if (ses->logline)
	{
		return;
	}

	if ((ses->logline = fopen(left, "a")))
	{
		fseek(ses->logline, 0, SEEK_END);

		if (ftell(ses->logline) == 0 && HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
		{
			write_html_header(ses->logline);
		}

		if (*right)
		{
			substitute(ses, right, right, SUB_ESC|SUB_LNF);

			logit(ses, right, ses->logline, FALSE);

			fclose(ses->logline);
			ses->logline = NULL;
		}
	}
	else
	{
		tintin_printf(ses, "#ERROR: #LINE LOGVERBATIM {%s} - COULDN'T OPEN FILE.", left);
	}
}

