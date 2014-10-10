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

DO_COMMAND(do_all)
{
	char left[BUFFER_SIZE];
	struct session *sesptr, *next_ses;

	if (gts->next)
	{
		get_arg_in_braces(ses, arg, left, TRUE);

		substitute(ses, left, left, SUB_VAR|SUB_FUN);

		for (sesptr = gts->next ; sesptr ; sesptr = next_ses)
		{
			next_ses = sesptr->next;

			script_driver(sesptr, LIST_COMMAND, left);
		}
	}
	else
	{
		show_error(ses, LIST_COMMAND, "#ALL: THERE AREN'T ANY SESSIONS.");
	}
	return ses;
}


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
		output = str_dup_printf("\n\033[0m%s\033[0m", temp);
	}
	else
	{
		output = str_dup_printf("\033[0m%s\033[0m", temp);
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
	if (*arg)
	{
		quitmsg(arg);
	}
	else
	{
		quitmsg(NULL);
	}
	return NULL;
}


DO_COMMAND(do_forall)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE];

	arg = sub_arg_in_braces(ses, arg, left, GET_ALL, SUB_VAR|SUB_FUN);
	arg = get_arg_in_braces(ses, arg, right, TRUE);

	if (*left == 0 || *right == 0)
	{
		tintin_printf2(ses, "#ERROR: #FORALL - PROVIDE 2 ARGUMENTS");
	}
	else
	{
		arg = left;

		while (*arg)
		{
			arg = get_arg_in_braces(ses, arg, temp, TRUE);

			RESTRING(gtd->cmds[0], temp);

			substitute(ses, right, temp, SUB_CMD);

			ses = script_driver(ses, LIST_COMMAND, temp);

			if (*arg == COMMAND_SEPARATOR)
			{
				arg++;
			}

		}
	}
	return ses;
}


DO_COMMAND(do_info)
{
	int cnt;

	if (*arg == 'c')
	{
		show_cpu(ses);

		return ses;
	}

	if (*arg == 's')
	{
		dump_stack();

		return ses;
	}

	tintin_header(ses, " INFORMATION ");

	for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
	{
		if (HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_HIDE))
		{
			continue;
		}
		tintin_printf2(ses, "%-20s  %5d  IGNORE %3s  MESSAGE %3s  DEBUG %3s",
			list_table[cnt].name_multi,
			ses->list[cnt]->used,
			HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE) ? "ON" : "OFF",
			HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE) ? "ON" : "OFF",
			HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG)   ? "ON" : "OFF");
	}
	tintin_header(ses, "");

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

		return ses;
	}

	if (*right)
	{
		do_one_prompt(ses, left, (int) get_number(ses, right));

		return ses;
	}

	if (strip_vt102_strlen(ses, ses->more_output) != 0)
	{
		output = str_dup_printf("\n\033[0m%s\033[0m", left);
	}
	else
	{
		output = str_dup_printf("\033[0m%s\033[0m", left);
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

DO_COMMAND(do_snoop)
{
	struct session *sesptr = ses;
	char left[BUFFER_SIZE];

	get_arg_in_braces(ses, arg, left, 1);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);

	if (*left)
	{
		sesptr = find_session(left);

		if (sesptr == NULL)
		{
			return show_error(ses, LIST_COMMAND, "#SNOOP: THERE'S NO SESSION NAMED {%s}", left);
		}
	}
	else
	{
		sesptr = ses;
	}

	if (HAS_BIT(sesptr->flags, SES_FLAG_SNOOP))
	{
		show_message(ses, LIST_COMMAND, "#NO LONGER SNOOPING SESSION '%s'", sesptr->name);
	}
	else
	{
		show_message(ses, LIST_COMMAND, "#SNOOPING SESSION '%s'", sesptr->name);
	}
	TOG_BIT(sesptr->flags, SES_FLAG_SNOOP);

	return ses;
}

DO_COMMAND(do_suspend)
{
	suspend_handler(0);

	return ses;
}

DO_COMMAND(do_test)
{
	long long x, time1, time2;
	char *str, buf[100001];

	tintin_printf2(ses, "input_level %d", gtd->input_level);
	tintin_printf2(ses, "noise_level %d", gtd->noise_level);
	tintin_printf2(ses, "quiet_level %d", gtd->quiet);
	tintin_printf2(ses, "debug_level %d", gtd->debug_level);

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

DO_COMMAND(do_zap)
{
	struct session *sesptr;
	char left[BUFFER_SIZE];

	push_call("do_zap(%p,%p)",ses,arg);

	sub_arg_in_braces(ses, arg, left, GET_ALL, SUB_VAR|SUB_FUN);

	if (*left)
	{
		sesptr = find_session(left);

		if (sesptr == NULL)
		{
			show_error(ses, LIST_COMMAND, "#ZAP: THERE'S NO SESSION WITH THAT NAME!");

			pop_call();
			return ses;
		}
	}
	else
	{
		sesptr = ses;
	}

	tintin_puts(sesptr, "");

	tintin_puts(sesptr, "#ZZZZZZZAAAAAAAAPPPP!!!!!!!!! LET'S GET OUTTA HERE!!!!!!!!");

	if (sesptr == gts)
	{
		pop_call();
		return do_end(NULL, "");
	}
	cleanup_session(sesptr);

	pop_call();
	return gtd->ses;
}

