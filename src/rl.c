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
	rl_variable_bind("enable-keypad", "on");
#ifdef USE_ISO_LATIN_ONE
	rl_variable_bind("meta-flag", "on");
	rl_variable_bind("convert-meta", "off");
	rl_variable_bind("output-meta", "on");
#endif

	gtd->keymap = rl_get_keymap();

	rl_macro_bind("\033[5~", "[[#buffer u]]", gtd->keymap);
	rl_macro_bind("\033[6~", "[[#buffer d]]", gtd->keymap);
	rl_macro_bind("\033[7~", "[[#buffer h]]", gtd->keymap);
	rl_macro_bind("\033[H",  "[[#buffer h]]", gtd->keymap);
	rl_macro_bind("\033[8~", "[[#buffer e]]", gtd->keymap);
	rl_macro_bind("\033[F",  "[[#buffer e]]", gtd->keymap);
	rl_macro_bind("\\C-t",   "[[#buffer l]]", gtd->keymap);
}


void printline(struct session *ses, const char *str, int prompt)
{
	char wrapped_str[BUFFER_SIZE];

	if (ses->scroll_line != -1 && HAS_BIT(ses->flags, SES_FLAG_SCROLLLOCK))
 	{
		return;
	}

	if (HAS_BIT(ses->flags, SES_FLAG_SCAN) && !HAS_BIT(ses->flags, SES_FLAG_VERBOSE))
	{
		return;
	}

	word_wrap(ses, str, wrapped_str, TRUE);

/*	strcpy(wrapped_str, str); */

	if (prompt)
	{
		printf("%s", wrapped_str);
	}
	else
	{
		printf("%s\r\n", wrapped_str);
	}

}

void bait(void)
{
	fd_set readfds, writefds, excfds;
	struct session *ses;
	struct chat_data *buddy;
	static struct timeval to;
	int rv;

	to.tv_sec  = 0;
	to.tv_usec = 0;

	{
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&excfds);

		if (!FD_ISSET(0, &readfds))
		{
			FD_SET(0, &readfds);  /* stdin */

			if (gtd->chat)
			{
				FD_SET(gtd->chat->fd, &readfds);
			}

			for (ses = gts->next ; ses ; ses = ses->next)
			{
				if (HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
				{
					FD_SET(ses->socket, &readfds);
					FD_SET(ses->socket, &writefds);
					FD_SET(ses->socket, &excfds);
				}
			}

			if (gtd->chat)
			{
				for (buddy = gtd->chat->next ; buddy ; buddy = buddy->next)
				{
					FD_SET(buddy->fd, &readfds);
					FD_SET(buddy->fd, &writefds);
					FD_SET(buddy->fd, &excfds);
				}
			}

			rv = select(FD_SETSIZE, &readfds, &writefds, &excfds, &to);

			if (rv <= 0)
			{
				if (rv == 0 || errno == EINTR)
				{
					return;
				}
				syserr("select");
			}

			if (gtd->chat)
			{
				process_chat_connections(&readfds, &writefds, &excfds);
			}

			for (ses = gts->next ; ses ; ses = gtd->update)
			{
				gtd->update = ses->next;

				if (FD_ISSET(ses->socket, &excfds))
				{
					FD_CLR(ses->socket, &readfds);
					FD_CLR(ses->socket, &writefds);

					cleanup_session(ses);
				}
			}

			for (ses = gts->next ; ses ; ses = gtd->update)
			{
				gtd->update = ses->next;

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
	readline_echoing_p = HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_ECHO);
}


void mainloop(void)
{
	utime();

	tick_update();

	macro_update();

	bait();
}


/*
	Read data, handle telopts, handle triggers, display data
*/

void readmud(struct session *ses)
{
	char *line, *next_line;
	char linebuf[BUFFER_SIZE];

	push_call("readmud(%p)", ses);

	if (!HAS_BIT(ses->flags, SES_FLAG_SCAN))
	{
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
	}
	gtd->mud_output_len = 0;

	/* separate into lines and print away */

	if (HAS_BIT(gtd->ses->flags, SES_FLAG_SPLIT))
	{
		save_pos(gtd->ses);
		goto_rowcol(gtd->ses, gtd->ses->bot_row, 1);
	}

	SET_BIT(gtd->ses->flags, SES_FLAG_READMUD);

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
			if (!HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_PROMPT))
			{
				if (gts->check_output)
				{
					strcat(ses->more_output, line);
					ses->check_output = utime() + gts->check_output;
					break;
				}
			}
		}


		if (*line && ses->more_output[0])
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
	DEL_BIT(gtd->ses->telopts, TELOPT_FLAG_PROMPT);

	DEL_BIT(gtd->ses->flags, SES_FLAG_READMUD);

	if (HAS_BIT(gtd->ses->flags, SES_FLAG_SPLIT))
	{
		restore_pos(gtd->ses);
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

		DEL_BIT(ses->flags, SES_FLAG_GAG);

		return;
	}

	add_line_buffer(ses, linebuf, prompt);

	if (ses == gtd->ses)
	{
		printline(ses, linebuf, prompt);
	}
	else if (HAS_BIT(ses->flags, SES_FLAG_SNOOP))
	{
		strip_vt102_codes_non_graph(linebuf, linebuf);

		tintin_printf2(gtd->ses, "[%s] %s", ses->name, linebuf);
	}
}

void show_message(struct session *ses, int index, const char *format, ...)
{
	struct listroot *root;
	char buf[BUFFER_SIZE];
	va_list args;

	push_call("show_message(%p,%p,%p)",ses,index,format);

	va_start(args, format);

	vsprintf(buf, format, args);

	va_end(args);
	
	if (index == -1)
	{
		if (HAS_BIT(gts->flags, SES_FLAG_USERCOMMAND))
		{
			tintin_puts2(buf, ses);
		}
		pop_call();
		return;
	}

	root = ses->list[index];

	if (HAS_BIT(root->flags, LIST_FLAG_DEBUG))
	{
		tintin_puts2(buf, ses);

		pop_call();
		return;
	}

	if (HAS_BIT(root->flags, LIST_FLAG_MESSAGE))
	{
		if (HAS_BIT(gts->flags, SES_FLAG_USERCOMMAND))
		{
			tintin_puts2(buf, ses);

			pop_call();
			return;
		}
	}

	if (HAS_BIT(root->flags, LIST_FLAG_LOG))
	{
		if (ses->logfile)
		{
			logit(ses, buf, ses->logfile);
		}
	}
	pop_call();
	return;
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

	if (HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
	{
		write(ses->socket, buf, length);
	}
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

	if (!HAS_BIT(gtd->ses->flags, SES_FLAG_VERBOSE) && HAS_BIT(gts->flags, SES_FLAG_QUIET))
	{
		return;
	}

	sprintf(output, "\033[0m%s\033[0m", cptr);

	add_line_buffer(ses, output, FALSE);

	if (ses != gtd->ses)
	{
		return;
	}

	if (!HAS_BIT(ses->flags, SES_FLAG_READMUD) && IS_SPLIT(ses))
	{
		save_pos(ses);
		goto_rowcol(ses, ses->bot_row, 1);
	}

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
	char buf[BUFFER_SIZE];

	if (ses == NULL)
	{
		ses = gtd->ses;
	}

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


/*
	quit tintin++ and print a message
*/

void quitmsg(const char *m)
{
	struct session *ses;
	char filestring[256];

	if (HAS_BIT(gts->flags, SES_FLAG_PREPPED))
	{
		rl_initialize();
		rl_deprep_terminal();
	}

	while ((ses = gts->next) != NULL) 
	{
		cleanup_session(ses);
	}

	if (gtd->chat)
	{
		close(gtd->chat->fd);
	}

	if (history_max_entries != 0)
	{
		sprintf(filestring, "%s/%s", getenv("HOME"), HISTORY_FILE);

		write_history(filestring);
	}

	clean_screen(gtd->ses);

	rl_read_init_file ("");

	if (m)
	{
		printf("%s\n", m);
	}

	printf("\nGoodbye from TinTin++\n\n");

	exit(0);
}

/* quit tintin++ fast, for use with signal() */

RETSIGTYPE myquitsig(int no_care)
{
	if (gtd->ses->connect_retry > utime())
	{
		gtd->ses->connect_retry = 0;
	}
	else if (HAS_BIT(gts->flags, SES_FLAG_PREPPED))
	{
		socket_printf(gtd->ses, 1, "%c", 3);
	}
	else
	{
		quitmsg(NULL);
	}
}
