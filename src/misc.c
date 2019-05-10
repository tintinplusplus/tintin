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


DO_COMMAND(do_bell)
{
	printf("\007");

	return ses;
}


DO_COMMAND(do_commands)
{
	char buf[BUFFER_SIZE] = { 0 }, add[BUFFER_SIZE];
	int cmd;

	tintin_header(ses, " %s ", "COMMANDS");

	for (cmd = 0 ; *command_table[cmd].name != 0 ; cmd++)
	{
		if (*arg && !is_abbrev(arg, command_table[cmd].name))
		{
			continue;
		}
		if ((int) strlen(buf) + 20 > ses->cols)
		{
			tintin_puts2(ses, buf);
			buf[0] = 0;
		}
		sprintf(add, "%20s", command_table[cmd].name);
		strcat(buf, add);
	}
	if (buf[0])
	{
		tintin_puts2(ses, buf);
	}
	tintin_header(ses, "");

	return ses;
}


DO_COMMAND(do_cr)
{
	write_mud(ses, "", SUB_EOL);

	return ses;
}


DO_COMMAND(do_echo)
{
	char format[BUFFER_SIZE], result[BUFFER_SIZE], temp[BUFFER_SIZE], *output, left[BUFFER_SIZE];
	int lnf;

	arg = sub_arg_in_braces(ses, arg, format, GET_ONE, SUB_VAR|SUB_FUN);

	format_string(ses, format, arg, result);

	arg = result;

	if (*arg == DEFAULT_OPEN)
	{
		arg = get_arg_in_braces(ses, arg, left, TRUE);
		      get_arg_in_braces(ses, arg, temp, TRUE);

		if (*temp)
		{
			int row = (int) get_number(ses, temp);

			substitute(ses, left, temp, SUB_COL|SUB_ESC);

			do_one_prompt(ses, temp, row);

			return ses;
		}
	}

	lnf = !str_suffix(arg, "\\");

	substitute(ses, arg, temp, SUB_COL|SUB_ESC);

	if (strip_vt102_strlen(ses, ses->more_output) != 0)
	{
		output = str_dup_printf("\n\e[0m%s\e[0m", temp);
	}
	else
	{
		output = str_dup_printf("\e[0m%s\e[0m", temp);
	}

	add_line_buffer(ses, output, lnf);

	if (ses == gtd->ses)
	{
		if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
		{
			save_pos(ses);
			goto_rowcol(ses, ses->bot_row, 1);
		}

		printline(ses, &output, lnf);

		if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
		{
			restore_pos(ses);
		}
	}
	str_free(output);

	return ses;
}


DO_COMMAND(do_end)
{
	char left[BUFFER_SIZE];

	if (*arg)
	{
		sub_arg_in_braces(ses, arg, left, GET_ALL, SUB_VAR|SUB_FUN|SUB_COL|SUB_ESC|SUB_LNF);

		quitmsg(left);
	}
	else
	{
		quitmsg(NULL);
	}
	return NULL;
}


DO_COMMAND(do_forall)
{
	tintin_printf2(ses, "\e[1;31m#NOTICE: The #forall command no longer exists, please switch to the #foreach command.\n");

	return ses;
}


DO_COMMAND(do_nop)
{
	return ses;
}


DO_COMMAND(do_send)
{
	char left[BUFFER_SIZE];

	push_call("do_send(%p,%p)",ses,arg);

	get_arg_in_braces(ses, arg, left, TRUE);

	write_mud(ses, left, SUB_VAR|SUB_FUN|SUB_ESC|SUB_EOL);

	pop_call();
	return ses;
}


DO_COMMAND(do_showme)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[STRING_SIZE], *output;
	int lnf;

	arg = get_arg_in_braces(ses, arg, left, TRUE);

	lnf = !str_suffix(left, "\\");

	substitute(ses, left, temp, SUB_VAR|SUB_FUN);
	substitute(ses, temp, left, SUB_COL|SUB_ESC);

	arg = sub_arg_in_braces(ses, arg, right, GET_ONE, SUB_VAR|SUB_FUN);

	do_one_line(left, ses);

	if (HAS_BIT(ses->flags, SES_FLAG_GAG))
	{
		DEL_BIT(ses->flags, SES_FLAG_GAG);

		gtd->ignore_level++;

		show_info(ses, LIST_GAG, "#INFO GAG {%s}", left);

		gtd->ignore_level--;

		return ses;
	}

	if (*right)
	{
		do_one_prompt(ses, left, (int) get_number(ses, right));

		return ses;
	}

	if (strip_vt102_strlen(ses, ses->more_output) != 0)
	{
		output = str_dup_printf("\n\e[0m%s\e[0m", left);
	}
	else
	{
		output = str_dup_printf("\e[0m%s\e[0m", left);
	}

	add_line_buffer(ses, output, lnf);

	if (ses == gtd->ses)
	{
		if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
		{
			save_pos(ses);
			goto_rowcol(ses, ses->bot_row, 1);
		}

		printline(ses, &output, lnf);

		if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
		{
			restore_pos(ses);
		}
	}

	str_free(output);

	return ses;
}


DO_COMMAND(do_test)
{
	long long x, time1, time2;
	time_t time3;
	char *str, buf[100001];

	time3 = time(NULL);

	tintin_printf2(ses, "size: %d time: %d", sizeof(time_t), time3);

	return ses;

	tintin_printf2(ses, "  input_level %d", gtd->input_level);
	tintin_printf2(ses, "verbose_level %d", gtd->verbose_level);
	tintin_printf2(ses, "  quiet_level %d", gtd->quiet);
	tintin_printf2(ses, "  debug_level %d", gtd->debug_level);

	return ses;

	tintin_printf2(ses, "testing");

	buf[0] = 0;

	time1 = utime();

	str = buf;

	for (x = 0 ; x < 100000 ; x++)
	{
		*str++ = '*';
	}
	*str = 0;

	time2 = utime();

	tintin_printf(ses, "time for strcat: %lld %d", time2 - time1, strlen(buf));

	str = str_dup("");

	time1 = utime();

	for (x = 0 ; x < 100000 ; x++)
	{
		str_cpy_printf(&str, "%c", '*');
	}

	time2 = utime();

	tintin_printf(ses, "time for str_cat: %lld %d", time2 - time1, strlen(str));

	str_free(str);

	return ses;
}
