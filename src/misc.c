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
/* file: misc.c - misc commands                                      */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

/* note: a bunch of changes were made for readline support -- daw */

#include "tintin.h"


DO_COMMAND(do_commands)
{
	char buf[BUFFER_SIZE] = { 0 }, add[BUFFER_SIZE];
	int cmd;

	tintin_header(ses, " %s ", "COMMANDS");

	for (cmd = 0 ; *command_table[cmd].name != 0 ; cmd++)
	{
		if (!is_abbrev(arg, command_table[cmd].name))
		{
			continue;
		}
		if (strlen(buf) + 20 > ses->cols)
		{
			tintin_puts2(buf, ses);
			buf[0] = 0;
		}
		sprintf(add, "%20s", command_table[cmd].name);
		strcat(buf, add);
	}
	if (buf[0])
	{
		tintin_puts2(buf, ses);
	}
	tintin_header(ses, "");

	return ses;
}

DO_COMMAND(do_cr)
{
	write_line_mud(" ", ses);

	return ses;
}


DO_COMMAND(do_nop)
{
	return ses;
}


DO_COMMAND(do_suspend)
{
	tstphandler(0);

	return ses;
}


/********************/
/* the #all command */
/********************/

DO_COMMAND(do_all)
{
	char left[BUFFER_SIZE];
	struct session *sesptr, *next_ses;

	if (gts->next)
	{
		get_arg_in_braces(arg, left, TRUE);

		for (sesptr = gts->next ; sesptr ; sesptr = next_ses)
		{
			next_ses = sesptr->next;
			parse_input(left, sesptr);
		}
	}
	else
	{
		tintin_puts2("#BUT THERE ISN'T ANY SESSION AT ALL!", ses);
	}
	return ses;
}

/*********************/
/* the #bell command */
/*********************/

DO_COMMAND(do_bell)
{
	printf("\007");

	return ses;
}

/*********************/
/* the #boss command */
/*********************/

DO_COMMAND(do_boss)
{
	int i;

	for (i = 0 ; i < 50 ; i++)
	{
		tintin_printf2(ses, "in-order traverse of tree starting from node %2d resulted in %2d red nodes\n", i, 50-i);
	}
	/*
		stop screen from scrolling stuff
	*/
	getchar();

	return ses;
}


DO_COMMAND(do_end)
{
	if (*arg)
	{
		tintin_puts("#YOU HAVE TO WRITE #END - NO LESS, TO END!", ses);
	}
	else
	{
		quitmsg(NULL);
	}
	return NULL;
}


DO_COMMAND(do_echo)
{
	char temp[BUFFER_SIZE];

	sprintf(temp, "result %s", arg);

	do_internal_variable(ses, "result <#echo: error>");

	do_format(ses, temp);

	substitute(ses, "$result", temp, SUB_VAR);

	do_showme(ses, temp);

	return ses;
}

DO_COMMAND(do_showme)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], result[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left, TRUE);
	arg = get_arg_in_braces(arg, right, FALSE);

	substitute(ses, left, result, SUB_VAR|SUB_FUN|SUB_COL|SUB_ESC);

	if (*right)
	{
		do_one_prompt(ses, result, atoi(right));
	}
	else
	{
		tintin_puts(result, ses);
	}
	return ses;
}

/*********************/
/* the #loop command */
/*********************/

DO_COMMAND(do_loop)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE];

	int bound1, bound2, counter, step;

	arg = get_arg_in_braces(arg, temp, 0);
	substitute(ses, temp, left, SUB_VAR|SUB_FUN);

	arg = get_arg_in_braces(arg, right, 1);

	if (sscanf(left, "%d%c%d", &bound1, &temp[0], &bound2) != 3)
	{
		tintin_printf2(ses, "#LOOP: NEED 2 ARGUMENTS, FOUND: %s", left);
	}
	else
	{
		if (bound1 <= bound2)
		{
			step = 1;
			bound2++;
		}
		else
		{
			step = -1;
			bound2--;
		}

		for (counter = bound1 ; counter != bound2 ; counter += step)
		{
			sprintf(gtd->cmds[0], "%d", counter);

			sprintf(temp, "loop %d", counter);
			do_internal_variable(ses, temp);

			substitute(ses, right, temp, SUB_CMD);
			ses = parse_input(temp, ses);
		}
	}
	return ses;
}


DO_COMMAND(do_forall)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, temp,  TRUE);
	arg = get_arg_in_braces(arg, right, FALSE);

	substitute(ses, temp, left, SUB_VAR|SUB_FUN);

	if (*left == 0 || *right == 0)
	{
		tintin_printf2(ses, "#FORALL, PROVIDE 2 ARGUMENTS");
	}
	else
	{
		arg = left;

		while (*arg)
		{
			arg = get_arg_in_braces(arg, temp, FALSE);
			sprintf(gtd->cmds[0], "%s", temp);

			sprintf(temp, "forall %s", gtd->cmds[0]);
			do_internal_variable(ses, temp);

			substitute(ses, right, temp, SUB_CMD);

			ses = parse_input(temp, ses);
		}
	}
	return ses;
}


DO_COMMAND(do_message)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int cnt, fnd = FALSE;

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 0);

	if (*left == 0)
	{
		tintin_header(ses, " MESSAGES ");

		for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
		{
			tintin_printf2(ses, "  %-20s %3s", list_table[cnt].name_multi, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE) ? "ON" : "OFF");
		}

		tintin_header(ses, "");
	}
	else
	{
		for (cnt = fnd = 0 ; cnt < LIST_MAX ; cnt++)
		{
			if (!is_abbrev(left, list_table[cnt].name_multi))
			{
				continue;
			}
			if (*right == 0)
			{
				TOG_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE);
			}
			else if (is_abbrev(right, "ON"))
			{
				SET_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE);
			}
			else if (is_abbrev(right, "OFF"))
			{
				DEL_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE);
			}
			else
			{
				tintin_puts2("SYNTAX: #MESSAGE [NAME] [ON|OFF]", ses);
				break;
			}
			tintin_printf2(ses, "#%s MESSAGES HAVE BEEN SET TO: %s.", list_table[cnt].name, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE) ? "ON" : "OFF");
			fnd = TRUE;
		}

		if (fnd == FALSE)
		{
			tintin_printf2(ses, "#NO MATCH(es) found for {%s}", left);
		}
	}
	return ses;
}


DO_COMMAND(do_ignore)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int cnt, fnd = FALSE;

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 0);

	if (*left == 0)
	{
		tintin_header(ses, " IGNORES ");

		for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
		{
			tintin_printf2(ses, "  %-20s %3s", list_table[cnt].name_multi, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE) ? "ON" : "OFF");
		}

		tintin_header(ses, "");
	}
	else
	{
		for (cnt = fnd = 0 ; cnt < LIST_MAX ; cnt++)
		{
			if (!is_abbrev(left, list_table[cnt].name_multi))
			{
				continue;
			}
			if (*right == 0)
			{
				TOG_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE);
			}
			else if (is_abbrev(right, "ON"))
			{
				SET_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE);
			}
			else if (is_abbrev(right, "OFF"))
			{
				DEL_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE);
			}
			else
			{
				tintin_puts2("SYNTAX: #IGNORE [NAME] [ON|OFF]", ses);
				break;
			}
			tintin_printf2(ses, "#%s IGNORE STATUS HAS BEEN SET TO: %s.", list_table[cnt].name, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE) ? "ON" : "OFF");
			fnd = TRUE;
		}

		if (fnd == FALSE)
		{
			tintin_printf2(ses, "#NO MATCH(es) found for {%s}", left);
		}
	}
	return ses;
}

DO_COMMAND(do_debug)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];
	int cnt, fnd = FALSE;

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 0);

	if (*left == 0)
	{
		tintin_header(ses, " DEBUGS ");

		for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
		{
			tintin_printf2(ses, "  %-20s %3s", list_table[cnt].name_multi, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG) ? "ON" : "OFF");
		}

		tintin_header(ses, "");
	}
	else
	{
		for (cnt = fnd = 0 ; cnt < LIST_MAX ; cnt++)
		{
			if (!is_abbrev(left, list_table[cnt].name_multi))
			{
				continue;
			}
			if (*right == 0)
			{
				TOG_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG);
			}
			else if (is_abbrev(right, "ON"))
			{
				SET_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG);
			}
			else if (is_abbrev(right, "OFF"))
			{
				DEL_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG);
			}
			else
			{
				tintin_puts2("SYNTAX: #DEBUG [NAME] [ON|OFF]", ses);
				break;
			}
			tintin_printf2(ses, "#%s DEBUG STATUS HAS BEEN SET TO: %s.", list_table[cnt].name, HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG) ? "ON" : "OFF");
			fnd = TRUE;
		}

		if (fnd == FALSE)
		{
			tintin_printf2(ses, "#NO MATCH(es) found for {%s}", left);
		}
	}
	return ses;
}


DO_COMMAND(do_snoop)
{
	struct session *sesptr = ses;
	char left[BUFFER_SIZE];

	get_arg_in_braces(arg, left, 1);

	if (*left)
	{
		for (sesptr = gts->next ; sesptr ; sesptr = sesptr->next)
  		{
  			if (!strcmp(sesptr->name, left))
  			{
  				break;
  			}
  		}
		if (sesptr == NULL)
		{
			tintin_puts2("#NO SESSION WITH THAT NAME!", ses);
		}
	}
	else
	{
		sesptr = ses;
	}

	if (HAS_BIT(sesptr->flags, SES_FLAG_SNOOP))
	{
		tintin_printf2(ses, "#UNSNOOPING SESSION '%s'", sesptr->name);
	}
	else
	{
		tintin_printf2(ses, "#SNOOPING SESSION '%s'", sesptr->name);
	}
	TOG_BIT(sesptr->flags, SES_FLAG_SNOOP);

	return ses;
}


DO_COMMAND(do_system)
{
	char left[BUFFER_SIZE];

	get_arg_in_braces(arg, left, TRUE);

	if (!*left)
	{
		tintin_puts2("#EXECUTE WHAT COMMAND?", ses);
		return ses;
	}

	if (show_message(ses, -1))
	{
		tintin_printf2(ses, "#OK, EXECUTING '%s'", left);
	}

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


DO_COMMAND(do_zap)
{
	tintin_puts("#ZZZZZZZAAAAAAAAPPPP!!!!!!!!! LET'S GET OUTTA HERE!!!!!!!!", ses);

	if (ses == gts)
	{
		return do_end(NULL, "");
	}
	cleanup_session(ses);

	return gtd->ses;
}

DO_COMMAND(do_info)
{
	int cnt;

	tintin_header(ses, " INFORMATION ");

	for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
	{
		tintin_printf2(ses, "%-20s %3d  IGNORE %3s  MESSAGE %3s  DEBUG  %3s",
			list_table[cnt].name_multi,
			ses->list[cnt]->count,
			HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_IGNORE) ? "ON" : "OFF",
			HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_MESSAGE) ? "ON" : "OFF",
			HAS_BIT(ses->list[cnt]->flags, LIST_FLAG_DEBUG)   ? "ON" : "OFF");
	}
	tintin_header(ses, "");

	return ses;
}

DO_COMMAND(do_gagline)
{
	SET_BIT(ses->flags, SES_FLAG_GAG);

	return ses;
}
