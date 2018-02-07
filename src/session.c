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
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                         coded by Peter Unold 1992                           *
******************************************************************************/

#include "tintin.h"

#include <errno.h>
#include <signal.h>

/************************/
/* the #session command */
/************************/

DO_COMMAND(do_session)
{
	char temp[BUFFER_SIZE], left[BUFFER_SIZE];
	struct session *sesptr;
	int cnt;

	substitute(ses, arg, temp, SUB_VAR|SUB_FUN);

	arg = temp;

	arg = get_arg_in_braces(ses, arg, left,  FALSE);

	if (*left == 0)
	{
		tintin_puts(ses, "#THESE SESSIONS HAVE BEEN DEFINED:");

		for (sesptr = gts->next ; sesptr ; sesptr = sesptr->next)
		{
			show_session(ses, sesptr);
		}
	}
	else if (*left && *arg == 0)
	{
		if (*left == '+')
		{
			return activate_session(ses->next ? ses->next : gts->next ? gts->next : ses);
		}

		if (*left == '-')
		{
			return activate_session(ses->prev ? ses->prev : gts->prev ? gts->prev : ses);
		}

		if (is_number(left))
		{
			for (cnt = 0, sesptr = gts ; sesptr ; cnt++, sesptr = sesptr->next)
			{
				if (cnt == atoi(left))
				{
					return activate_session(sesptr);
				}
			}
		}

		tintin_puts(ses, "#THAT SESSION IS NOT DEFINED.");
	}
	else
	{
		ses = new_session(ses, left, arg, 0, 0);
	}
	return gtd->ses;
}

void show_session(struct session *ses, struct session *ptr)
{
	char temp[BUFFER_SIZE];

	sprintf(temp, "%-10s %18s:%-5s", ptr->name, ptr->session_host, ptr->session_port);

	strcat(temp, ptr == gtd->ses ? " (active)" :  "         ");

	strcat(temp, ptr->mccp ? " (mccp)" : "       ");

	strcat(temp, HAS_BIT(ptr->flags, SES_FLAG_SNOOP) ? " (snoop)" : "        ");

	strcat(temp, ptr->logfile ? " (log)" : "       ");

	strcat(temp, ptr->ssl ? " (ssl)" : "      ");

	tintin_puts2(ses, temp);
}

struct session *find_session(char *name)
{
	struct session *ses;

	for (ses = gts ; ses ; ses = ses->next)
	{
		if (!strcmp(ses->name, name))
		{
			return ses;
		}
	}
	return NULL;
}

// find a session to activate when current session is closed

struct session *newactive_session(void)
{
	push_call("newactive_session(void)");

	if (gts->next)
	{
		activate_session(gts->next);
	}
	else
	{
		activate_session(gts);
	}
	pop_call();
	return gtd->ses;
}

struct session *activate_session(struct session *ses)
{
	check_all_events(gtd->ses, SUB_ARG|SUB_SEC, 0, 1, "SESSION DEACTIVATED", gtd->ses->name);

	gtd->ses = ses;

	dirty_screen(ses);

	tintin_printf(ses, "#SESSION '%s' ACTIVATED.", ses->name);

	check_all_events(ses, SUB_ARG|SUB_SEC, 0, 1, "SESSION ACTIVATED", ses->name);

	return ses;
}

/**********************/
/* open a new session */
/**********************/

struct session *new_session(struct session *ses, char *name, char *arg, int desc, int ssl)
{
	int cnt = 0;
	char host[BUFFER_SIZE], port[BUFFER_SIZE], file[BUFFER_SIZE];
	struct session *newses;

	push_call("new_session(%p,%p,%p,%d,%d)",ses,name,arg,desc,ssl);

	if (HAS_BIT(gtd->flags, TINTIN_FLAG_TERMINATE))
	{
		pop_call();
		return ses;
	}

	arg = sub_arg_in_braces(ses, arg, host, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, port, GET_ONE, SUB_VAR|SUB_FUN);
	arg = sub_arg_in_braces(ses, arg, file, GET_ONE, SUB_VAR|SUB_FUN);

	if (desc == 0)
	{
		if (*host == 0)
		{
			tintin_puts(ses, "#HEY! SPECIFY AN ADDRESS WILL YOU?");

			pop_call();
			return ses;
		}

		if (*port == 0)
		{
			tintin_puts(ses, "#HEY! SPECIFY A PORT NUMBER WILL YOU?");

			pop_call();
			return ses;
		}
	}

	if (find_session(name))
	{
		tintin_puts(ses, "#THERE'S A SESSION WITH THAT NAME ALREADY.");

		pop_call();
		return ses;
	}

	newses                = (struct session *) calloc(1, sizeof(struct session));

	newses->name          = strdup(name);
	newses->session_host  = strdup(host);
	newses->session_ip    = strdup("");
	newses->session_port  = strdup(port);

	newses->group         = strdup(gts->group);
	newses->flags         = gts->flags;
	newses->telopts       = gts->telopts;
	newses->auto_tab      = gts->auto_tab;

	newses->cmd_color     = strdup(gts->cmd_color);

	newses->read_max      = gts->read_max;
	newses->read_buf      = (unsigned char *) calloc(1, gts->read_max);

	LINK(newses, gts->next, gts->prev);

	for (cnt = 0 ; cnt < LIST_MAX ; cnt++)
	{
		newses->list[cnt] = copy_list(newses, gts->list[cnt], cnt);
	}

	newses->rows          = gts->rows;
	newses->cols          = gts->cols;
	newses->top_row       = gts->top_row;
	newses->bot_row       = gts->bot_row;

	init_buffer(newses, gts->scroll_max);

	if (desc == 0)
	{
		tintin_printf(ses, "#TRYING TO CONNECT '%s' TO '%s' PORT '%s'.", newses->name, newses->session_host, newses->session_port);
	}
	else if (desc == -1)
	{
		// #PORT INITIALIZE {NAME} {PORT} {FILE}
	}
	else
	{
		tintin_printf(ses, "#TRYING TO LAUNCH '%s' RUNNING '%s'.", newses->name, newses->session_host);
	}

	gtd->ses = newses;

	dirty_screen(newses);

	check_all_events(newses, SUB_ARG|SUB_SEC, 0, 3, "SESSION CREATED", newses->name, newses->session_host, newses->session_port);

	if (desc == 0)
	{
		newses = connect_session(newses);
	}
	else if (desc == -1)
	{
		SET_BIT(newses->flags, SES_FLAG_PORT);

		gtd->ses = newses;
	}
	else
	{
		SET_BIT(newses->flags, SES_FLAG_CONNECTED|SES_FLAG_RUN);

		SET_BIT(newses->telopts, TELOPT_FLAG_SGA);
		DEL_BIT(newses->telopts, TELOPT_FLAG_ECHO);

		gtd->ses = newses;

		gtd->ses->socket = desc;
	}

	if (newses)
	{
#ifdef HAVE_GNUTLS_H

		if (ssl)
		{
			newses->ssl = ssl_negotiate(newses);

			if (newses->ssl == 0)
			{
				cleanup_session(newses);

				pop_call();
				return gtd->ses;
			}
		}

#endif

		if (*file)
		{
			do_read(newses, file);
		}
	}

	pop_call();
	return gtd->ses;
}

struct session *connect_session(struct session *ses)
{
	int sock;
	static struct timeval to;

	push_call("connect_session(%p)",ses);

	ses->connect_retry = utime() + gts->connect_retry;

	reconnect:

	sock = connect_mud(ses, ses->session_host, ses->session_port);

	if (sock == -1)
	{
		cleanup_session(ses);

		pop_call();
		return NULL;
	}

	if (sock)
	{
		gtd->ses    = ses;
		ses->socket = sock;

		ses->connect_retry = 0;

		SET_BIT(ses->flags, SES_FLAG_CONNECTED);

		tintin_printf2(ses, "");

		tintin_printf(ses, "#SESSION '%s' CONNECTED TO '%s' PORT '%s'", ses->name, ses->session_host, ses->session_port);

		check_all_events(ses, SUB_ARG|SUB_SEC, 0, 4, "SESSION CONNECTED", ses->name, ses->session_host, ses->session_ip, ses->session_port);

		pop_call();
		return ses;
	}

	if (ses->connect_retry > utime())
	{
		fd_set readfds;

		FD_ZERO(&readfds);
		FD_SET(0, &readfds);

		if (select(FD_SETSIZE, &readfds, NULL, NULL, &to) <= 0)
		{
			if (to.tv_sec == 0)
			{
				to.tv_sec = 1;

				tintin_printf(ses, "#SESSION '%s' FAILED TO CONNECT. RETRYING FOR %d SECONDS.", ses->name, (ses->connect_retry - utime()) / 1000000);
			}

			goto reconnect;
		}
	}

	if (ses->connect_error)
	{
		to.tv_sec = 0;

		tintin_printf(ses, "#SESSION '%s' FAILED TO CONNECT.", ses->name);
	}

	cleanup_session(ses);

	pop_call();
	return NULL;
}

/*****************************************************************************/
/* cleanup after session died. if session=gtd->ses, try find new active      */
/*****************************************************************************/

void cleanup_session(struct session *ses)
{
	push_call("cleanup_session(%p)",ses);

	if (HAS_BIT(ses->flags, SES_FLAG_CLOSED))
	{
		tintin_printf2(NULL, "\n#SESSION '%s' IS ALREADY CLOSED.", ses->name);
		dump_stack();

		pop_call();
		return;
	}

	if (ses == gtd->update)
	{
		gtd->update = ses->next;
	}

	UNLINK(ses, gts->next, gts->prev);

	if (ses->socket)
	{
		if (close(ses->socket) == -1)
		{
			syserr("close in cleanup");
		}

		// the PID is stored in the session's port.

		if (HAS_BIT(ses->flags, SES_FLAG_RUN))
		{
			kill(atoi(ses->session_port), SIGKILL);
		}

	}

	if (ses->port)
	{
		port_uninitialize(ses, "", "", "");
	}

	SET_BIT(ses->flags, SES_FLAG_CLOSED);

	if (HAS_BIT(ses->flags, SES_FLAG_CONNECTED))
	{
		DEL_BIT(ses->flags, SES_FLAG_CONNECTED);

		check_all_events(ses, SUB_ARG|SUB_SEC, 0, 4, "SESSION DISCONNECTED", ses->name, ses->session_host, ses->session_ip, ses->session_port);

		tintin_printf(gtd->ses, "#SESSION '%s' DIED.", ses->name);
	}
	else
	{
		check_all_events(ses, SUB_ARG|SUB_SEC, 0, 4, "SESSION TIMED OUT", ses->name, ses->session_host, ses->session_ip, ses->session_port);

		tintin_printf(gtd->ses, "#SESSION '%s' TIMED OUT.", ses->name);
	}

	if (ses == gtd->ses)
	{
		gtd->ses = newactive_session();
	}

	if (ses->logfile)
	{
		fclose(ses->logfile);
	}

	if (ses->logline)
	{
		fclose(ses->logline);
	}

#ifdef HAVE_GNUTLS_H

	if (ses->ssl)
	{
		gnutls_deinit(ses->ssl);
	}

#endif

	LINK(ses, gtd->dispose_next, gtd->dispose_prev);

	pop_call();
	return;
}

void dispose_session(struct session *ses)
{
	int index;

	push_call("dispose_session(%p)", ses);

	UNLINK(ses, gtd->dispose_next, gtd->dispose_prev);

	for (index = 0 ; index < LIST_MAX ; index++)
	{
		free_list(ses->list[index]);
	}

	if (ses->map)
	{
		delete_map(ses);
	}

	if (ses->mccp)
	{
		inflateEnd(ses->mccp);
		free(ses->mccp);
	}

	init_buffer(ses, 0);

	free(ses->name);
	free(ses->session_host);
	free(ses->session_ip);
	free(ses->session_port);
	free(ses->group);
	free(ses->read_buf);
	free(ses->cmd_color);

	free(ses);

	pop_call();
	return;
}
