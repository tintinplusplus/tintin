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

/* New for v2.0: readline support -- daw */

/* this is the main bunch of code for readline; lots of misc stuff here */

#include "tintin.h"

#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>

/*
	some readline stuff
*/


void initrl(void)
{
	rl_initialize();
	using_history();

	rl_readline_name             = "tintin++";
	rl_completion_entry_function = (rl_compentry_func_t *) tab_complete;

	rl_event_hook                = (rl_hook_func_t *) mainloop;

	rl_set_keyboard_input_timeout(10000);

	stifle_history(gtd->history_size);

	rl_variable_bind("horizontal-scroll-mode", "on");
#ifdef USE_ISO_LATIN_ONE
	rl_variable_bind("meta-flag", "on");
	rl_variable_bind("convert-meta", "off");
	rl_variable_bind("output-meta", "on");
#endif

	gtd->keymap = rl_get_keymap();

	rl_macro_bind("\e[5~", "[[#buffer u]]", gtd->keymap);
	rl_macro_bind("\e[6~", "[[#buffer d]]", gtd->keymap);
	rl_macro_bind("\e[7~", "[[#buffer h]]", gtd->keymap);
	rl_macro_bind("\e[H",  "[[#buffer h]]", gtd->keymap);
	rl_macro_bind("\e[8~", "[[#buffer e]]", gtd->keymap);
	rl_macro_bind("\e[F",  "[[#buffer e]]", gtd->keymap);
}


void printline(struct session *ses, const char *str, int prompt)
{
	char wrapped_str[BUFFER_SIZE];

	if (ses->scroll_line != -1 && HAS_BIT(ses->flags, SES_FLAG_SCROLLLOCK))
	{
		return;
	}

	word_wrap(ses, str, wrapped_str, TRUE);

	if (prompt)
	{
		printf("%s", wrapped_str);
	}
	else
	{
		printf("%s\n\r", wrapped_str);
	}
}

void bait(void)
{
	fd_set readfds, excfds;
	struct session *ses, *ses_next;
	struct timeval to;
	int rv;

	to.tv_sec  = 0;

	{
		FD_ZERO(&readfds);

		if (!FD_ISSET(0, &readfds))
		{
			FD_ZERO(&readfds);
			FD_ZERO(&excfds);
			FD_SET(0, &readfds);	/* stdin */

			for (ses = gts->next ; ses ; ses = ses->next)
			{
				if (HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
				{
					FD_SET(ses->socket, &readfds);
				}
			}

			to.tv_usec = 0;

			rv = select(FD_SETSIZE, &readfds, NULL, &excfds, &to);

			if (rv <= 0)
			{
				if (rv == 0 || errno == EINTR)
				{
					return;
				}
				syserr("select");
				
			}

			for (ses = gts->next ; ses ; ses = ses_next)
			{
				ses_next = ses->next;

				if (HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
				{
					if (FD_ISSET(ses->socket, &readfds))
					{
						readmud(ses);
					}
				}
			}
		}
	}
	readline_echoing_p = HAS_BIT(gtd->ses->flags, SES_FLAG_LOCALECHO);
}


void mainloop(void)
{
	utime();

	tick_update();

	macro_update();

	bait();
}


void commandloop(void)
{
	char *line, buffer[BUFFER_SIZE];

	while (TRUE)
	{
		if (HAS_BIT(gtd->ses->flags, SES_FLAG_CONVERTMETA))
		{
			line = readkeyboard();
		}
		else
		{
			line = readline(NULL);
		}

		if (line == NULL)
		{
			continue;
		}

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_LOCALECHO))
		{
			if ((line = rlhist_expand(line)) == NULL)
			{
				continue;
			}
		}


		sprintf(buffer, "\033[0m%s", line);

		if (!HAS_BIT(gtd->ses->flags, SES_FLAG_LOCALECHO))
		{
			memset(&buffer[4], '*', strlen(line));
		}

		if (HAS_BIT(gtd->ses->flags, SES_FLAG_SPLIT))
		{
			if (HAS_BIT(gtd->ses->flags, SES_FLAG_ECHOCOMMAND) || gtd->ses->list[LIST_PROMPT]->f_node == NULL)
			{
				SET_BIT(gtd->ses->flags, SES_FLAG_SCROLLSTOP);

				if (gtd->ses->more_output[0] && gtd->ses->list[LIST_PROMPT]->f_node == NULL)
				{
					gtd->ses->check_output = 0;

					tintin_printf2(gtd->ses, "%s%s", gtd->ses->more_output, buffer);
				}
				else
				{
					tintin_puts2(buffer, gtd->ses);
				}
				DEL_BIT(gtd->ses->flags, SES_FLAG_SCROLLSTOP);
			}
		}

		add_line_buffer(gtd->ses, buffer, -1);

		SET_BIT(gts->flags, SES_FLAG_USERCOMMAND);

		if (gtd->ses->scroll_line != -1)
		{
			buffer_e();
		}

		parse_input(line, gtd->ses);

		DEL_BIT(gts->flags, SES_FLAG_USERCOMMAND);

		if (IS_SPLIT(gtd->ses))
		{
			erase_toeol();
		}
		free(line);
	}
}

/*
	Read data, handle telopts, handle triggers, display data
*/

void readmud(struct session *ses)
{
	char *line, *next_line;
	char linebuf[BUFFER_SIZE];

	push_call("readmud(%p)", ses);

	read_buffer_mud(ses);

	switch (gtd->mud_output_len)
	{
		case -1:
			cleanup_session(ses);

			pop_call();
			return;

		case  0:
			pop_call();
			return;
	}

	gtd->mud_output_len = 0;


	/* separate into lines and print away */

	if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
	{
		save_pos(ses);
		goto_rowcol(ses, ses->bot_row, 1);
	}

	SET_BIT(ses->flags, SES_FLAG_READMUD);

	for (line = gtd->mud_output_buf ; line && *line ; line = next_line)
	{
		next_line = strchr(line, '\n');

		if (next_line)
		{
			*next_line = 0;
			next_line++;
		}
		else if (*line == 0)
		{
			break;
		}

		if (next_line == NULL && strlen(ses->more_output) < BUFFER_SIZE / 2)
		{
			if (gts->check_output)
			{
				strcat(ses->more_output, line);
				ses->check_output = utime() + gts->check_output;
				break;
			}
		}

		if (ses->more_output[0])
		{
			if (ses->check_output || HAS_BIT(ses->flags, SES_FLAG_SPLIT))
			{
				sprintf(linebuf, "%s%s", ses->more_output, line);

				ses->more_output[0] = 0;
			}
			else
			{
				strcpy(linebuf, line);
			}
		}
		else
		{
			strcpy(linebuf, line);
		}

		process_mud_output(ses, linebuf, next_line == NULL);
	}

	DEL_BIT(ses->flags, SES_FLAG_READMUD);

	if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
	{
		restore_pos(ses);
	}

	fflush(stdout);

	pop_call();
	return;
}


void process_mud_output(struct session *ses, char *linebuf, int prompt)
{
	ses->check_output = 0;

	do_one_line(linebuf, ses);   /* changes linebuf */

	/*
		Take care of gags, vt102 support still goes
	*/

	if (HAS_BIT(ses->flags, SES_FLAG_GAG))
	{
		strip_non_vt102_codes(linebuf, ses->more_output);

		printf("%s", ses->more_output);

		ses->more_output[0] = 0;

		DEL_BIT(ses->flags, SES_FLAG_GAGPROMPT);
		DEL_BIT(ses->flags, SES_FLAG_GAG);

		return;
	}
/*
	if (!HAS_BIT(ses->flags, SES_FLAG_GAGPROMPT))
	{
		if (ses == gtd->ses || HAS_BIT(ses->flags, SES_FLAG_SNOOP))
		{
			if (HAS_BIT(gtd->ses->flags, SES_FLAG_SPLIT))
			{
				if (gtd->ses->more_output[0])
				{
					if (linebuf[0])
					{
						printf("\n");
					}
				}
			}
		}
	}
*/
	add_line_buffer(ses, linebuf, prompt);

	if (HAS_BIT(ses->flags, SES_FLAG_GAGPROMPT))
	{
		DEL_BIT(ses->flags, SES_FLAG_GAGPROMPT);
	}
	else if (ses == gtd->ses)
	{
		printline(ses, linebuf, prompt);
	}
	else if (HAS_BIT(ses->flags, SES_FLAG_SNOOP))
	{
		strip_vt102_codes_non_graph(linebuf, linebuf);

		tintin_printf2(gtd->ses, "[%s] %s", ses->name, linebuf);
	}
}


char * readkeyboard(void)
{
	char ch, buf[10], *line;

	while (TRUE)
	{
		ch = rl_read_key();

		switch (ch)
		{
			case '\e':
				rl_insert_text("\\e");
				break;			 

			case '\n':
			case '\r':
				printf("\n");
				line = strdup(rl_line_buffer);
				rl_point = 0;
				rl_delete_text(0, rl_end);
				rl_redisplay();
				return line;

			case 127:
				if (rl_point)
				{
					rl_point--;
					rl_delete_text(rl_point, rl_point+1);
				}
				break;

			default:
				if (ch < 27)
				{
					sprintf(buf, "\\C-%c", 'a' + ch - 1);
				}								 
				else
				{
					sprintf(buf, "%c", ch);
				}
				rl_insert_text(&buf[0]);
				break;
		}
		rl_redisplay();
	}
	return NULL;
}


int show_message(struct session *ses, int index)
{
	struct listroot *root;

	if (index == -1)
	{
		return HAS_BIT(gts->flags, SES_FLAG_USERCOMMAND);
	}

	root = ses->list[index];

	if (HAS_BIT(root->flags, LIST_FLAG_DEBUG))
	{
		return TRUE;
	}

	if (HAS_BIT(root->flags, LIST_FLAG_MESSAGE))
	{
		if (HAS_BIT(gts->flags, SES_FLAG_USERCOMMAND))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void tintin_header(struct session *ses, const char *format, ...)
{
	char arg[BUFFER_SIZE], buf[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(arg, format, args);
	va_end(args);

	if (strlen(arg) > gtd->ses->cols - 2)
	{
		arg[gtd->ses->cols - 2] = 0;
	}

	memset(buf, '#', gtd->ses->cols);

	memcpy(&buf[(gtd->ses->cols - strlen(arg)) / 2], arg, strlen(arg));

	buf[gtd->ses->cols] = 0;

	tintin_puts2(buf, ses);
}


void socket_printf(struct session *ses, size_t length, const char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	write(ses->socket, buf, length);
}

void tintin_printf2(struct session *ses, const char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	tintin_puts2(buf, ses);
}

void tintin_printf(struct session *ses, const char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	tintin_puts(buf, ses);
}

/*
	output to screen should go through this function
	the output is NOT checked for actions or anything
*/

void tintin_puts2(const char *cptr, struct session *ses)
{
	char output[BUFFER_SIZE];

	if (ses == NULL)
	{
		ses = gtd->ses;
	}

	if (ses != gtd->ses)
	{
		return;
	}

	if (!HAS_BIT(gtd->ses->flags, SES_FLAG_VERBOSE) && HAS_BIT(gts->flags, SES_FLAG_QUIET))
	{
		return;
	}

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		save_pos(ses);
		goto_rowcol(ses, ses->bot_row, 1);
	}

	sprintf(output, "\033[0m\033[0K%s\033[0m", cptr);

	add_line_buffer(ses, output, FALSE);

	printline(ses, output, FALSE);

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		restore_pos(ses);
	}
}

/*
	output to screen should go through this function
	the output IS treated as though it came from the mud
*/

void tintin_puts(const char *cptr, struct session *ses)
{
	/*
		bug! doesn't do_one_line() sometimes send output to stdout?
	*/

	if (ses)
	{
		char buf[BUFFER_SIZE];

		sprintf(buf, "%s", cptr);
		do_one_line(buf, ses);

		if (!HAS_BIT(ses->flags, SES_FLAG_GAG))
		{
			tintin_puts2(buf, ses);
		}
		else
		{
			DEL_BIT(ses->flags, SES_FLAG_GAG);
		}
	}
	else
	{
		tintin_puts2(cptr, ses);
	}
}


/*
	quit tintin++ and print a message
*/

void quitmsg(const char *m)
{
	struct session *ses;
	char filestring[256];

	while ((ses = gts->next) != NULL) 
	{
		cleanup_session(ses);
	}

	if (history_max_entries != 0)
	{
		if (getenv("TINTIN_HISTORY") == NULL)
		{
			sprintf(filestring, "%s/%s", getenv("HOME"), HISTORY_FILE);
		}
		else
		{
			sprintf(filestring, "%s/%s", getenv("TINTIN_HISTORY"), HISTORY_FILE);
		}
		write_history(filestring);
	}

	clean_screen(gtd->ses);

	if (m)
	{
		printf("%s\n", m);
	}

	printf("\nGoodbye from TinTin++\n\n");

	exit(0);
}

/* quit tintin++ fast!  for use with signal() */

RETSIGTYPE myquitsig(int no_care)
{
	quitmsg(NULL);
}
