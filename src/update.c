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
*               (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                  *
*                                                                             *
*                     coded by Igor van den Hoven 1996                        *
******************************************************************************/

#include "tintin.h"

#include <sys/types.h>
#include <sys/time.h>
#include <termios.h>
#include <errno.h>


void mainloop(void)
{
	static struct timeval curr_time, wait_time, last_time;
	int usec_loop, usec_wait;

	short int pulse_poll_input      = PULSE_POLL_INPUT;
	short int pulse_poll_sessions   = PULSE_POLL_SESSIONS;
	short int pulse_poll_chat       = PULSE_POLL_CHAT;
	short int pulse_update_ticks    = PULSE_UPDATE_TICKS;
	short int pulse_update_delays   = PULSE_UPDATE_DELAYS;
	short int pulse_update_packets  = PULSE_UPDATE_PACKETS;
	short int pulse_update_chat     = PULSE_UPDATE_CHAT;
	short int pulse_update_terminal = PULSE_UPDATE_TERMINAL;

	wait_time.tv_sec  = 0;

	while (TRUE)
	{
		gettimeofday(&last_time, NULL);

		if (--pulse_poll_input == 0)
		{
			open_timer(TIMER_POLL_INPUT);

			pulse_poll_input = PULSE_POLL_INPUT;

			poll_input();

			close_timer(TIMER_POLL_INPUT);
		}

		if (--pulse_poll_sessions == 0)
		{
			pulse_poll_sessions = PULSE_POLL_SESSIONS;

			poll_sessions();
		}

		if (--pulse_poll_chat == 0)
		{
			pulse_poll_chat = PULSE_POLL_CHAT;

			poll_chat();
		}	

		if (--pulse_update_ticks == 0)
		{
			pulse_update_ticks = PULSE_UPDATE_TICKS;

			tick_update();
		}

		if (--pulse_update_delays == 0)
		{
			pulse_update_delays = PULSE_UPDATE_DELAYS;

			delay_update();
		}

		if (--pulse_update_packets == 0)
		{
			pulse_update_packets = PULSE_UPDATE_PACKETS;

			packet_update();
		}

		if (--pulse_update_chat == 0)
		{
			pulse_update_chat = PULSE_UPDATE_CHAT;

			chat_update();
		}

		if (--pulse_update_terminal == 0)
		{
			pulse_update_terminal = PULSE_UPDATE_TERMINAL;

			terminal_update();
		}

		gettimeofday(&curr_time, NULL);

		if (curr_time.tv_sec == last_time.tv_sec)
		{
			usec_loop = curr_time.tv_usec - last_time.tv_usec;
		}
		else
		{
			usec_loop = 1000000 - last_time.tv_usec + curr_time.tv_usec;
		}

		usec_wait = 1000000 / PULSE_PER_SECOND - usec_loop;

		wait_time.tv_usec = usec_wait;

		gtd->total_io_exec  += usec_loop;
		gtd->total_io_delay += usec_wait;

		if (usec_wait > 0)
		{
			select(0, NULL, NULL, NULL, &wait_time);
		}
	}
}

void poll_input(void)
{
	fd_set readfds;
	static struct timeval to;

	FD_ZERO(&readfds);

	FD_SET(0, &readfds);

	if (select(FD_SETSIZE, &readfds, NULL, NULL, &to) <= 0)
	{
		return;
	}

	if (FD_ISSET(0, &readfds))
	{
		process_input();

		poll_input();
	}
}

void poll_sessions(void)
{
	fd_set readfds, writefds, excfds;
	static struct timeval to;
	struct session *ses;
	int rv;

	open_timer(TIMER_POLL_SESSIONS);

	if (gts->next)
	{
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&excfds);

		for (ses = gts->next ; ses ; ses = ses->next)
		{
			if (HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
			{
				FD_SET(ses->socket, &readfds);
				FD_SET(ses->socket, &writefds);
				FD_SET(ses->socket, &excfds);
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
	close_timer(TIMER_POLL_SESSIONS);
}

void poll_chat(void)
{
	fd_set readfds, writefds, excfds;
	static struct timeval to;
	struct chat_data *buddy;
	int rv;

	open_timer(TIMER_POLL_CHAT);

	if (gtd->chat)
	{
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&excfds);

		FD_SET(gtd->chat->fd, &readfds);

		for (buddy = gtd->chat->next ; buddy ; buddy = buddy->next)
		{
			FD_SET(buddy->fd, &readfds);
			FD_SET(buddy->fd, &writefds);
			FD_SET(buddy->fd, &excfds);
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
		process_chat_connections(&readfds, &writefds, &excfds);
	}
	close_timer(TIMER_POLL_CHAT);
}

void tick_update(void)
{
	char result[BUFFER_SIZE];
	struct session *ses;
	struct listnode *node;
	struct listroot *root;

	open_timer(TIMER_UPDATE_TICKS);

	utime();

	for (ses = gts->next ; ses ; ses = gtd->update)
	{
		gtd->update = ses->next;

		root = ses->list[LIST_TICKER];

		for (node = root->f_node ; node ; node = root->update)
		{
			root->update = node->next;

			if (node->data == 0)
			{
				node->data = gtd->time + get_number(ses, node->pr) * 1000000L;
			}

			if (node->data <= gtd->time)
			{
				node->data += atof(node->pr) * 1000000LL;

				strcpy(result, node->right);

				show_debug(ses, LIST_TICKER, "#TICKER DEBUG: %s", result);

				parse_input(ses, result);
			}
		}
	}
	close_timer(TIMER_UPDATE_TICKS);
}

void delay_update(void)
{
	char result[BUFFER_SIZE];
	struct session *ses;
	struct listnode *node;
	struct listroot *root;

	open_timer(TIMER_UPDATE_DELAYS);

	for (ses = gts ; ses ; ses = gtd->update)
	{
		gtd->update = ses->next;

		root = ses->list[LIST_DELAY];	

		for (node = root->f_node ; node ; node = root->update)
		{
			root->update = node->next;

			if (node->data == 0)
			{
				node->data = gtd->time + get_number(ses, node->pr) * 1000000L;
			}

			if (node->data <= gtd->time)
			{
				strcpy(result, node->right);

				deletenode_list(ses, node, LIST_DELAY);

				show_debug(ses, LIST_DELAY, "#DELAY DEBUG: %s", result);

				parse_input(ses, result);
			}
		}
	}
	close_timer(TIMER_UPDATE_DELAYS);
}

void packet_update(void)
{
	char result[BUFFER_SIZE];
	struct session *ses;

	open_timer(TIMER_UPDATE_PACKETS);

	for (ses = gts->next ; ses ; ses = gtd->update)
	{
		gtd->update = ses->next;

		if (ses->check_output && gtd->time > ses->check_output)
		{
			if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
			{
				save_pos(ses);
				goto_rowcol(ses, ses->bot_row, 1);
			}

			SET_BIT(ses->flags, SES_FLAG_READMUD);

			strcpy(result, ses->more_output);

			ses->more_output[0] = 0;

			process_mud_output(ses, result, TRUE);

			DEL_BIT(ses->flags, SES_FLAG_READMUD);

			if (HAS_BIT(ses->flags, SES_FLAG_SPLIT))
			{
				restore_pos(ses);
			}
		}
	}
	close_timer(TIMER_UPDATE_PACKETS);
}

void chat_update(void)
{
	struct chat_data *buddy, *buddy_next;

	open_timer(TIMER_UPDATE_CHAT);

	if (gtd->chat)
	{
		for (buddy = gtd->chat->next ; buddy ; buddy = buddy_next)
		{
			buddy_next = buddy->next;

			if (buddy->timeout && buddy->timeout < time(NULL))
			{
				chat_socket_printf(buddy, "<CHAT> Connection timed out.");

				close_chat(buddy, TRUE);
			}
		}

		if (gtd->chat->paste_time && gtd->chat->paste_time < utime())
		{
			chat_paste(NULL);
		}
	}
	close_timer(TIMER_UPDATE_CHAT);
}

void terminal_update(void)
{
	struct session *ses;

	open_timer(TIMER_UPDATE_TERMINAL);

	for (ses = gts ; ses ; ses = ses->next)
	{
		if (HAS_BIT(ses->flags, SES_FLAG_UPDATEVTMAP))
		{
			DEL_BIT(ses->flags, SES_FLAG_UPDATEVTMAP);

			show_vtmap(ses);
		}
	}
	fflush(stdout);

	close_timer(TIMER_UPDATE_TERMINAL);
}

